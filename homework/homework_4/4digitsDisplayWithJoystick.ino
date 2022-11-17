/* Pins */
// Joystick
const int xPin = A1;
const int yPin = A0;
const int SWPin = 2;

// Shift register
const int latchPin = 11; // STCP to 12 on Shift Register
const int clockPin = 10; // SHCP to 11 on Shift Register
const int dataPin = 12; // DS to 14 on Shift Register

// 4 digits 7seg display (each digit from right to left)
const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;


// For Joystick
bool joyMoved = false;
const int minThreshold = 400;
const int maxThreshold = 600;
byte lastSWValue = 1;
byte buttonState = 1;
unsigned long lastDebounceTime = 0;
unsigned int debounceInterval = 50;
unsigned long buttonPressedTime = 0;
unsigned long buttonReleasedTime = 0;
unsigned int longPressInterval = 500;

// For DP blinking
unsigned long previousBlinkTime = 0;
unsigned long blinkInterval = 300;


enum directions {
  up = 0,
  right = 1,
  down = 2,
  left = 3,
  neutral = 4
};


int displayDigits[] = {
  segD1, segD2, segD3, segD4
};

const int encodingNumber = 16;
byte byteEncodings[encodingNumber] = {
//A B C D E F G DP 
  B11111100, // 0 
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110  // F
};

// Current numbers on the display
int numberOnDisplay[] = {
  0,
  0,
  0,
  0
};
int digitsConfig[] = {
  B11111100,
  B11111100,
  B11111100,
  B11111100 
};
const int displayNumber = 4;

int currentState = 1;
int currentDigit = 0;

void setup() {
  // Setup joystick pins
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(SWPin, INPUT_PULLUP);

  // Setup shift pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Setup 4 digits display pins
  for (int i = 0; i < displayNumber; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  Serial.begin(9600);
}

void loop() {
  // Read potentiometers values
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  // Read the button with debounce
  int SWValue = digitalRead(SWPin);
  if (SWValue != lastSWValue) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceInterval) {

    // if the button state is different from previous one
    if (SWValue != buttonState) {
      buttonState = SWValue;

      if (!buttonState) {
        buttonPressedTime = millis();
      } else {
        buttonReleasedTime = millis();

        if (buttonReleasedTime - buttonPressedTime >= longPressInterval) { // Long press
          if (currentState == 1) {
            resetTheDisplay();
          }
        } else { // Short press
          if (currentState == 1) {
            currentState = 2;
          } else {
            currentState = 1;
          }
        }
      }
      
    }
  }

  if (currentState == 1) {
    int direction = getDirection(xValue, yValue);
    if (direction == left) {
      digitsConfig[currentDigit] = (digitsConfig[currentDigit] >> 1) << 1;
      currentDigit++;
      if (currentDigit >= displayNumber) {
        currentDigit = displayNumber - 1;
      }
    }
    if (direction == right) {
      digitsConfig[currentDigit] = (digitsConfig[currentDigit] >> 1) << 1;
      currentDigit--;
      if (currentDigit < 0) {
        currentDigit = 0;
      }
    }
  

    showDigits(digitsConfig);
    blinkDP();
  }
  if (currentState == 2) {

    int direction = getDirection(xValue, yValue);
    if (direction == up) {
      // increment the number
      numberOnDisplay[currentDigit]++;
      if (numberOnDisplay[currentDigit] >= encodingNumber) {
        numberOnDisplay[currentDigit] = encodingNumber - 1;
      }
    }
    if (direction == down) {
      // decrement the number
      numberOnDisplay[currentDigit]--;
      if (numberOnDisplay[currentDigit] < 0) {
        numberOnDisplay[currentDigit] = 0;
      }
    }
    digitsConfig[currentDigit] = byteEncodings[numberOnDisplay[currentDigit]];
    
    // DP LED is always ON
    digitsConfig[currentDigit] = ((digitsConfig[currentDigit] >> 1) << 1) ^ B1;

    showDigits(digitsConfig);
  }

  lastSWValue = SWValue;
}


int getDirection(int xValue, int yValue) {
  if (!joyMoved && yValue > maxThreshold) {
    joyMoved = true;
    return up;
  }
  if (!joyMoved && xValue > maxThreshold) {
    joyMoved = true;
    return right;
  }
  if (!joyMoved && yValue < minThreshold) {
    joyMoved = true;
    return down;
  }
  if (!joyMoved && xValue < minThreshold) {
    joyMoved = true;
    return left;
  }
  bool xIsNeutral = xValue >= minThreshold && xValue <= maxThreshold;
  bool yIsNeutral = yValue >= minThreshold && yValue <= maxThreshold;
  if (xIsNeutral && yIsNeutral) {
    joyMoved = false;
  }
  return neutral;
}

void showDigits(int digitsConfig[]) {
  for (int i = 0; i < displayNumber; i++) {
    writeReg(digitsConfig[i]);
    activateDisplay(i);
    delay(3);
  }
}

void blinkDP() {
  if (millis() - previousBlinkTime > blinkInterval) {
    digitsConfig[currentDigit] = digitsConfig[currentDigit] ^ B1;
    previousBlinkTime = millis();
  }
}

void writeReg(byte encoding) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayIndex) {
  for (int i = 0; i < displayNumber; i++) {
    if (i == displayIndex) {
      digitalWrite(displayDigits[i], LOW);
    } else {
      digitalWrite(displayDigits[i], HIGH);
    }
  }
}

void resetTheDisplay() {
  currentDigit = 0;
  for (int i = 0; i < displayNumber; i++) {
    numberOnDisplay[i] = 0;
    digitsConfig[i] = byteEncodings[0];
  }
}
