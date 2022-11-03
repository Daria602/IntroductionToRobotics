/* Pins */
const int buttonPin = 2;
const int buzzerPin = 4;
const int carsRedLedPin = 12;
const int carsYellowLedPin = 10;
const int carsGreenLedPin = 9;
const int peopleRedLedPin = 7;
const int peopleGreenLedPin = 6;

/* For button */
byte buttonState = 1;
byte lastButtonState = 1;
unsigned long lastButtonTime = 0;  
unsigned long lastDebounceTime = 0;
unsigned int debounceInterval = 50;

/* For buzzer */
byte isBuzzerActive = 0;
int buzzerTone = 1000;
unsigned long previousBuzzerTime = 0;
unsigned int buzzerBeepInterval;
unsigned int slowBeep = 500;
unsigned int fastBeep = 250;

/* Intervals for states */
unsigned long previousStateTime = 0;   
unsigned int waitAfterDefault = 8000;
unsigned int attentionForDrivers = 3000;
unsigned int pedestriansWalk = 8000;
unsigned int hurryBeforeRed = 4000;

/* For blinking LED */
unsigned long previousBlinkTime = 0;
unsigned int ledBlinkingInterval = fastBeep;
byte blinkingLedState = 0;

int currentState = 1;
byte stateHasSound = 0;
byte changeHasStarted = 0;


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(carsRedLedPin, OUTPUT);
  pinMode(carsYellowLedPin, OUTPUT);
  pinMode(carsGreenLedPin, OUTPUT);
  pinMode(peopleRedLedPin, OUTPUT);
  pinMode(peopleGreenLedPin, OUTPUT);
  Serial.begin(9600);
  
}

void loop() {
  // read the state of the button
  int reading = digitalRead(buttonPin);

  // if the button state changed, reset the debounce timer
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // work with button if enough time has passed since it was pressed
  if ((millis() - lastDebounceTime) > debounceInterval) {

    // if the button state is different from previous one
    if (reading != buttonState) {
      buttonState = reading;

      // if the button is pressed
      if (!buttonState) {
        if (currentState == 1) {
          changeHasStarted = 1;
          lastButtonTime = millis();
          previousStateTime = millis();
        }
      }
    }
  }

  // if the button was pushed, the change has started
  if (changeHasStarted) {
    if (millis() - previousStateTime >= waitAfterDefault) {
      currentState = 2;
      changeHasStarted = 0;
      previousStateTime = millis();
    }
  }

  switch(currentState) {
    case 1:
      defaultState();
      break;
    case 2:
      if (millis() - previousStateTime >= attentionForDrivers) {
        currentState = 3;
        previousStateTime = millis();
      } else {
        yellowForDrivers();
      }
      break;
    case 3:
      if (millis() - previousStateTime >= pedestriansWalk) {
        currentState = 4;
        previousStateTime = millis();
      } else {
        freeForPedestrians();
      }
      break;
    case 4:
      if (millis() - previousStateTime >= hurryBeforeRed) {
        currentState = 1;
        previousStateTime = millis();
      } else {
        pedestriansHurry();
      }
      break;
  }

  // If the state has sound, play the buzzer at interval
  if (stateHasSound) {
    if (millis() - previousBuzzerTime >= buzzerBeepInterval) {
      previousBuzzerTime = millis();
      if (isBuzzerActive) {
        isBuzzerActive = 0;
      } else {
        isBuzzerActive = 1;
      }
    }
  }
  playBuzzer(isBuzzerActive);

  // Save the current button reading for the next loop
  lastButtonState = reading;
}

void playBuzzer(byte isBuzzerActive) {
  if (isBuzzerActive) {
    tone(buzzerPin, buzzerTone);
  } else {
    noTone(buzzerPin);
  }
}
void defaultState() {
  stateHasSound = 0;
  digitalWrite(carsRedLedPin, LOW);
  digitalWrite(carsGreenLedPin, HIGH);
  digitalWrite(peopleGreenLedPin, LOW);
  digitalWrite(peopleRedLedPin, HIGH);
  isBuzzerActive = LOW;
}

void yellowForDrivers() {
  digitalWrite(carsGreenLedPin, LOW);
  digitalWrite(carsYellowLedPin, HIGH);
  isBuzzerActive = LOW;
}

void freeForPedestrians() {
  stateHasSound = 1;
  digitalWrite(carsYellowLedPin, LOW);
  digitalWrite(carsRedLedPin, HIGH);
  digitalWrite(peopleRedLedPin, LOW);
  digitalWrite(peopleGreenLedPin, HIGH);
  buzzerBeepInterval = slowBeep;
}

void pedestriansHurry() {
  digitalWrite(carsYellowLedPin, LOW);
  digitalWrite(carsRedLedPin, HIGH);
  digitalWrite(peopleRedLedPin, LOW);
  buzzerBeepInterval = fastBeep;
  if (millis() - previousBlinkTime >= ledBlinkingInterval) {
      previousBlinkTime = millis();
      if (blinkingLedState) {
        blinkingLedState = 0;
      } else {
        blinkingLedState = 1;
      }
  }
  digitalWrite(peopleGreenLedPin, blinkingLedState);
}