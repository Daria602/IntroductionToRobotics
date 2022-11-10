/* Pins */
// Joystick
const int xPin = A1;
const int yPin = A0;
const int SWPin = 2;

// 7 segment display
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

const int segSize = 8;
const int segments[segSize] = {
  pinA,
  pinB,
  pinC,
  pinD,
  pinE,
  pinF,
  pinG,
  pinDP,
};
const int DPposition = 7;

// The current state of each led in 7 seg display
byte isLit[segSize] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// Current position of the LED
int currentPosition = 0;
unsigned int previousBlinkTime = 0;
unsigned int blinkInterval = 400;
byte currentLedState = HIGH;

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


enum directions {
  up = 0,
  right = 1,
  down = 2,
  left = 3,
  neutral = 4
};
const int nrDirections = 5;
// the directions by index are up(0), right(1), down(2), left(3)
// later we move the cursor by this number in array
int movesMapped[segSize][nrDirections - 1] = {
  { 0,  1,  6,  5},
  {-1,  0,  1,  4},
  {-1,  5,  1,  2},
  { 3, -1,  0,  1},
  { 1, -2, -1,  0},
  {-5, -4, -1,  0},
  {-6, -5, -3, -1},
  {0,  0,  0, -5}
};

int currentState = 1;

void setup() {
  // Setup joystick pins
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(SWPin, INPUT_PULLUP);

  // Setup 7seg display pins
  for (int i = 0; i < segSize; i++) {
  	pinMode(segments[i], OUTPUT);
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
            resetDisplay();
          }
        } else { // Short press
          if (currentState == 1) {
            currentState = 2;
          } else {
            isLit[currentPosition] = currentLedState; 
            currentState = 1;
          }
        }
      }
      
    }
  }

  if (currentState == 1) {
    movingPositionsState(xValue, yValue);
  }
  if (currentState == 2) {
    selectLedState(xValue);
  }

  lastSWValue = SWValue;
}

void movingPositionsState(int xValue, int yValue) {
  int direction = getDirection(xValue, yValue);
  if (direction != neutral) {
    move(direction);
  }
  displayLitLeds(isLit);
  blinkCurrentPos();  
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

void move(int direction) {
  currentPosition += movesMapped[currentPosition][direction];
}

void displayLitLeds(byte isLit[segSize]) {
  for (int index = 0; index < segSize; index++ ) {
    digitalWrite(segments[index], isLit[index]);
  }
}

void blinkCurrentPos() {
  if (millis() - previousBlinkTime >= blinkInterval) {
    currentLedState = !currentLedState;
    previousBlinkTime = millis();
  }
  digitalWrite(segments[currentPosition], currentLedState);
}

void selectLedState(int xValue) {
  int direction = getDirection(xValue, xValue);
  if (direction != 4 ) {
    currentLedState = !currentLedState;
  }
  digitalWrite(segments[currentPosition], currentLedState);
}

void resetDisplay() {
  for (int index = 0; index < segSize; index++) {
    isLit[index] = 0;
  }
  currentPosition = DPposition;
}
