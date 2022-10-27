/* CONSTANTS */
const int redSensorPin = A2;
const int greenSensorPin = A1;
const int blueSensorPin = A0;

const int redLedPin = 9;
const int greenLedPin = 10;
const int blueLedPin = 11;

const int minSensorValue = 0;
const int maxSensorValue = 1023;

const int minLedIntensity = 0;
const int maxLedIntensity = 255;

/* FUNCTIONS */
void setLedIntensityWithSensor(int sensorPin, int ledPin) {
  int sensorValue = analogRead(sensorPin);
  int ledIntensity = map(
    sensorValue, 
    minSensorValue, maxSensorValue, 
    minLedIntensity, maxLedIntensity
  );
  analogWrite(ledPin, ledIntensity);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(redSensorPin, INPUT);
  pinMode(greenSensorPin, INPUT);
  pinMode(blueSensorPin, INPUT);

  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
}

void loop() {
  setLedIntensityWithSensor(redSensorPin, redLedPin);
  setLedIntensityWithSensor(greenSensorPin, greenLedPin);
  setLedIntensityWithSensor(blueSensorPin, blueLedPin);
}
