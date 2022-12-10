#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h"

// LCD pins
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 4;
const byte d7 = 3;
const int contrastPin = 5;
const byte LCDBacklightPin = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Matrix pins
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 13;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);


// Joystick pins
const byte xPin = A0;
const byte yPin = A1;
const byte swPin = A2;


// For Joystick (debounce, moving in menu and moving in game)
enum directions {
  up = 0,
  right = 1,
  down = 2,
  left = 3,
  neutral = 4
};

enum menuStates {
  initialMenuDisplay = 0,
  playGame = 1,
  seeHighscore = 2,
  displaySettings = 3,
  seeAbout = 4,
  setDificulty = 5, 
  changeContrast = 6,
  changeLCDBrightness = 7,
  changeMatBrightness = 8,
  returnToMenu = 9
};
int currentState = initialMenuDisplay;

bool joyMoved = false;
const int minThreshold = 300;
const int maxThreshold = 700;
byte lastSWValue = 1;
byte buttonState = 1;



const int nameMaxLength = 3; 
struct {
  int contrast;
  int brightLCD;
  int brightMat;
  int difficulty;
  int firstHighestScore;
  int secondHighestScore;
  int thirdHighestScore;
  char firstHighestName[nameMaxLength];
  char secondHighestName[nameMaxLength];
  char thirdHighestName[nameMaxLength];
} storedInMemory;

int structAddress = 0;

/* Game and menu mechanics */
byte foodState = HIGH;
byte blinkCharState = 0;

// to set the contrast
const int minContrast = 1;
const int maxContrast = 9;

// To set the difficulty
const int minDifficulty = 1;
const int maxDifficulty = 3;
enum difficulty {
  easy = 1,
  medium = 2,
  hard = 3
};

// To set the brightness of LCD
const int minLCDBright = 1;
const int maxLCDBright = 9;

// To set the brightness of Matrix
const int minMatBright = 1;
const int maxMatBright = 9;

int livesCount = 3;
int score = 0;

byte level = 1;

const int matrixSize = 8;

int randomFoodLocation[2] = {
  7,
  7
};
int playerLocation[2] = {
  5,
  5
};


// Strings, charachters and images to print
const String gameOverText = "Game Over :(";
const int nameLength = 3;
char playerName[nameLength] = {
  'A',
  'A',
  'A'
};
int playerNameAlphIndexes[nameLength] = {
  0,
  0,
  0
};
const unsigned int initMenuLength = 4;
String initialMenu[initMenuLength] = {
  "Start          ",
  "Highscore",
  "Settings",
  "About"
};

const unsigned int settingsMenuLength = 5;
String settingsMenu[settingsMenuLength] = {
  "Difficulty",
  "Contrast",
  "LCD Brightness",
  "Mat Brightness",
  "Back"
};
byte gameConfig[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};
byte heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte easyDifficulty[matrixSize] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000011,
  B00000011,
  B00000011
};

byte mediumDifficulty[matrixSize] = {
  B00000000,
  B00000000,
  B00000000,
  B00011000,
  B00011000,
  B00011011,
  B00011011,
  B00011011
};

byte hardDifficulty[matrixSize] = {
  B11000000,
  B11000000,
  B11000000,
  B11011000,
  B11011000,
  B11011011,
  B11011011,
  B11011011
};

const byte face[matrixSize] = {
  B00000000,
  B00100100,
  B00100100,
  B10100101,
  B10000001,
  B01000010,
  B00111100,
  B00000000,
};

const byte star[matrixSize] = {
  B00011000,
  B00011000,
  B11111111,
  B01111110,
  B00111100,
  B01111110,
  B01100110,
  B00000000,
};

const char alphabet[26] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z'
};

const char madeBy[] = "Made by Izotova Daria Github: https://github.com/Daria602/   ";


// Time and intervals
unsigned long lastDebounceTime = 0;
unsigned int debounceInterval = 50;
unsigned int foodBlinkInterval = 250; 
unsigned long long foodBlinkTimePassed = 0;
const int charBlinkInterval = 400;
unsigned long long charBlinkTimePassed = 0;
unsigned long long highscoreTimePassed = 0;
unsigned long long thankYouTimePassed = 0;
unsigned long long showTitleTime = 0;
unsigned long long scrollTime = 0;
unsigned int scrollSpeed = 250;
const byte moveInterval = 600;
unsigned long long lastMoved = 0;

// Indexes 
//Index of the menu shown
int index = 0;
int cursorValue = 0;
int indexInName = 0;
int enterNameIndex = 0;
int positionCounter = 0;
int highscoreIndex = 0;

// Byte flags for loops
byte goodToClearHighScore = 0;
byte nameEntered = 0;
byte isAboutPrinted = 0;
byte isInMenu = 1;
byte isGameOver = 0;
byte freeToReset = 0;
// Whether cursor in menu reached the end of the array
byte reachedTheEnd = 0;
byte bestFirst = 0;
byte bestSecond = 0;
byte bestThird = 0;
byte notTheBest = 0;












void setup() {
  Serial.begin(9600);
  EEPROM.get(structAddress, storedInMemory);
  matrixSetup();
  LCDsetup();
  joystickSetup();
  
  showTitleTime = millis(); 
}


void loop() {
  if (millis() - showTitleTime <= 3000) {
    lcd.setCursor(0, 0);
    lcd.print("Game name here!");
  } else {
    if (isGameOver) {
      stopTheGame();
    } else {
      isInMenu ? inMenu() : inGame();
    }
  }
}


/* Setup functions */
void(* resetFunc) (void) = sizeof(storedInMemory);


void matrixSetup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, map(storedInMemory.brightMat, minMatBright, maxMatBright, 0, 15));
  lc.clearDisplay(0);
}

void LCDsetup() {
  lcd.createChar(0, heart);
  lcd.begin(16, 2);
  pinMode(contrastPin, OUTPUT);
  pinMode(LCDBacklightPin, OUTPUT);
  analogWrite(contrastPin, map(storedInMemory.contrast, minContrast, maxContrast, 0, 255));
  analogWrite(LCDBacklightPin, map(storedInMemory.brightLCD, minLCDBright, maxLCDBright, 0, 255));
}

void joystickSetup() {
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
}


/* Main functions */
void inMenu() {
  switch (currentState) {
    case initialMenuDisplay:
      showMenu(initialMenu, initMenuLength);
      displayMatrixByColumn(face);
      break;
    case playGame:
      isInMenu = 0;
      break;
    case seeHighscore:
      showHighscores();
      displayMatrixByColumn(star);
      break;
    case displaySettings:
      showMenu(settingsMenu, settingsMenuLength); 
      break;
    case seeAbout:
      showAbout();
      break;
    case setDificulty:
      changeDifficulty(); 
      switch(storedInMemory.difficulty) {
        case easy:
          displayMatrixByColumn(easyDifficulty);
          break;
        case medium:
          displayMatrixByColumn(mediumDifficulty);
          break;
        case hard:
          displayMatrixByColumn(hardDifficulty);
          break;
      }
      break;
    case changeContrast:
      displayMatrixByColumn(star);
      modifyContrast(); 
      break;
    case changeLCDBrightness:
      displayMatrixByColumn(star);
      changeLCDBright(); 
      break;
    case changeMatBrightness:
      displayMatrixByColumn(star);
      changeMatBright(); 
      break;
  }
}


void showAbout() {
  if (isAboutPrinted) {
    if (millis() - scrollTime >= scrollSpeed) {
      positionCounter++;
      if (positionCounter >= strlen(madeBy) - 1 - 16) {
        lcd.clear();
        isAboutPrinted = 0;
      }
      scrollTime = millis();
    }
    for (int i = 0; i < 16; i++) {
      lcd.setCursor(i, 1);
      lcd.print(madeBy[i + positionCounter]);
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Game name here... ");
    isAboutPrinted = 1;
    positionCounter = 0;
    scrollTime = millis();
  }

  if (debounceButton()) {
    lcd.clear();
    currentState = initialMenuDisplay;
    isAboutPrinted = 0;
  }
  
}

void inGame() {
  displayLCDInGame();
  putPlayer();
  putFood();
  displayCurrentMatrix(gameConfig);
  int direction = joyContinuousDirection();
  if (millis() - lastMoved >= moveInterval / storedInMemory.difficulty) {
    lastMoved = millis();
    movePlayer(direction);
  }
}


void showHighscores() {
  lcd.setCursor(0, 0);
  lcd.print(highscoreIndex + 1);
  lcd.print(".");
  lcd.setCursor(0, 1);
  lcd.print(highscoreIndex + 2);
  lcd.print(".");
  if (highscoreIndex == 0) {
    lcd.setCursor(2, 0);
    for (int i = 0; i < nameLength; i++) {
        lcd.print(storedInMemory.firstHighestName[i]);
    }
    lcd.print(" ");
    lcd.print(storedInMemory.firstHighestScore);
    lcd.print("   ");
    lcd.setCursor(2, 1);
    for (int i = 0; i < nameLength; i++) {
        lcd.print(storedInMemory.secondHighestName[i]);
    }
    lcd.print(" ");
    lcd.print(storedInMemory.secondHighestScore);
    lcd.print("   ");
  } else {
    lcd.setCursor(2, 0);
    for (int i = 0; i < nameLength; i++) {
        lcd.print(storedInMemory.secondHighestName[i]);
    }
    lcd.print(" ");
    lcd.print(storedInMemory.secondHighestScore);
    lcd.print("   ");
    lcd.setCursor(2, 1);
    for (int i = 0; i < nameLength; i++) {
        lcd.print(storedInMemory.thirdHighestName[i]);
    }
    lcd.print(" ");
    lcd.print(storedInMemory.thirdHighestScore);
    lcd.print("   ");
  }

  int direction = joyMoveDirection();
  
  if (direction == up) {
    highscoreIndex = highscoreIndex - 1 < 0 ? 0 : highscoreIndex - 1;
  } else if (direction == down) {
    highscoreIndex = highscoreIndex + 1 >= 2 ? highscoreIndex : highscoreIndex + 1;
  }

  if (debounceButton()) {
    lcd.clear();
    currentState = initialMenuDisplay;
  }
}


void stopTheGame() {
  // show players scores
  // check if highscore was beaten
  // if yes, ask player to enter name
  // if no, say thank you for playing and see you next time -> turn off
  if (millis() - highscoreTimePassed >= 5000) {

    if (bestFirst || bestSecond || bestThird) {

      sayYouveBeatenTheScore();
      
    } else {
      sayThankYou();
    }

  } else {
    thankYouTimePassed = millis();
    showFinalScore();
  }

  if (score > storedInMemory.firstHighestScore) {
    bestFirst = 1;
  } else if (score > storedInMemory.secondHighestScore) {
    bestSecond = 1;
  } else if (score > storedInMemory.thirdHighestScore) {
    bestThird = 1;
  } else {
    notTheBest = 1;
  }
  
  if (freeToReset) {
    resetFunc();
  }
  
}


void sayThankYou() {
  if (millis() - thankYouTimePassed >= 5000) {
    freeToReset = 1;
  }
  lcd.setCursor(0,0);
  lcd.print("Thank you       ");
  lcd.setCursor(0,1);
  lcd.print("for playing!");
}


void sayYouveBeatenTheScore() {
  if (millis() - thankYouTimePassed <= 3000) {
    lcd.setCursor(0,0);
    lcd.print("Youve beaten the");
    lcd.setCursor(0,1);
    lcd.print("highest score!  ");

    goodToClearHighScore = 1;

  } else {
    if (goodToClearHighScore) {
      lcd.clear();
      goodToClearHighScore = 0;
    }
    updateTheHighestScore();
  }
}


void updateTheHighestScore() {
  if (nameEntered) {    
    if (bestFirst) {
      storedInMemory.firstHighestName[0] = playerName[0];
      storedInMemory.firstHighestName[1] = playerName[1];
      storedInMemory.firstHighestName[2] = playerName[2];
      storedInMemory.firstHighestScore = score;
      
    } else if (bestSecond) {
      storedInMemory.secondHighestName[0] = playerName[0];
      storedInMemory.secondHighestName[1] = playerName[1];
      storedInMemory.secondHighestName[2] = playerName[2];
      storedInMemory.secondHighestScore = score;
    } else {
      storedInMemory.thirdHighestName[0] = playerName[0];
      storedInMemory.thirdHighestName[1] = playerName[1];
      storedInMemory.thirdHighestName[2] = playerName[2];
      storedInMemory.thirdHighestScore = score;
    }
    EEPROM.put(structAddress, storedInMemory);
    freeToReset = 1;
  } else {
    enterTheName();
  }
}


void enterTheName() {
  int direction = joyMoveDirection();
  if (direction == up) {
    playerNameAlphIndexes[indexInName] = playerNameAlphIndexes[indexInName] - 1 < 0 ?  0 : playerNameAlphIndexes[indexInName] - 1;
  } else if (direction == down) {
    playerNameAlphIndexes[indexInName] = playerNameAlphIndexes[indexInName] + 1 >= 26 ? playerNameAlphIndexes[indexInName] : playerNameAlphIndexes[indexInName] + 1;
  } else if (direction == left) {
    indexInName = indexInName - 1 < 0 ? 0 : indexInName - 1;
  } else if (direction == right) {
    indexInName = indexInName + 1 >= nameLength ? indexInName : indexInName + 1;
  }

  playerName[indexInName] = alphabet[playerNameAlphIndexes[indexInName]];

  lcdPrint(0, "Enter your name:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < nameLength; i++) {
    lcd.print(playerName[i]);
  }

  if (debounceButton()) {
    nameEntered = 1;
  }
}

void showFinalScore() {
  lcd.setCursor(0, 0);
  lcd.print("Your score is:  ");
  if (score/1000 >= 1) {
    lcd.setCursor(6,1);
  } else if (score/100) {
    lcd.setCursor(7,1);
  } else {
    lcd.setCursor(8,1);
  }
  lcd.print(score);
}

void showMenu(String menu[], uint8_t menuLength) {
  int joyDirection = joyMoveDirection();
  
  if (joyDirection == up || joyDirection == down) {
    moveIndexCursor(joyDirection, menuLength);
  }
  byte buttonPushed = debounceButton();
  if (buttonPushed) {
    selectOption();
    return;
  }
  displayCurrentItems(menu);
  printCharacter(15, cursorValue, '<', 1);
}

void displayMatrixByColumn(byte charachterToDisplay[matrixSize]) {
  for (int column = 0; column < matrixSize; column++) {
    lc.setColumn(0, column, charachterToDisplay[matrixSize - column - 1]);
  }
}

void changeDifficulty() {

  lcdPrint(0, "Difficulty:");
  storedInMemory.difficulty > minDifficulty ? printCharacter(6, 1, '<', 1) : printCharacter(6, 1, ' ', 0);
  lcd.setCursor(7, 2);
  lcd.print(storedInMemory.difficulty);
  storedInMemory.difficulty < maxDifficulty ? printCharacter(8, 1, '>', 1) : printCharacter(8, 1, ' ', 0);
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    storedInMemory.difficulty = storedInMemory.difficulty - 1 < minDifficulty ? minDifficulty : storedInMemory.difficulty - 1;
  } else if (joyDirection == right) {
    storedInMemory.difficulty = storedInMemory.difficulty + 1 > maxDifficulty ? maxDifficulty : storedInMemory.difficulty + 1;
  }
  byte buttonPushed = debounceButton();
  if (buttonPushed) {
    EEPROM.put(structAddress, storedInMemory);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

void modifyContrast() {
  lcdPrint(0, "Contrast:");
  storedInMemory.contrast > minContrast ? printCharacter(6, 1, '<', 1) : printCharacter(6, 1, ' ', 0);
  lcd.setCursor(7, 2);
  lcd.print(storedInMemory.contrast);
  storedInMemory.contrast < maxContrast ? printCharacter(8, 1, '>', 1) : printCharacter(8, 1, ' ', 0);
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    storedInMemory.contrast = storedInMemory.contrast - 1 < minContrast ? minContrast : storedInMemory.contrast - 1;
  } else if (joyDirection == right) {
    storedInMemory.contrast = storedInMemory.contrast + 1 > maxContrast ? maxContrast : storedInMemory.contrast + 1;
  }
  analogWrite(contrastPin, map(storedInMemory.contrast, minContrast, maxContrast, 0, 255));
  byte buttonPushed = debounceButton();
  if (buttonPushed) {
    EEPROM.put(structAddress, storedInMemory);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

void changeLCDBright() {
  
  lcdPrint(0, "LCD Brightness:");
  storedInMemory.brightLCD > minLCDBright ? printCharacter(6, 1, '<', 1) : printCharacter(6, 1, ' ', 0);
  lcd.setCursor(7, 2);
  lcd.print(storedInMemory.brightLCD);
  storedInMemory.brightLCD < maxLCDBright ? printCharacter(8, 1, '>', 1) : printCharacter(8, 1, ' ', 0);
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    storedInMemory.brightLCD = storedInMemory.brightLCD - 1 < minLCDBright ? minLCDBright : storedInMemory.brightLCD - 1;
  } else if (joyDirection == right) {
    storedInMemory.brightLCD = storedInMemory.brightLCD + 1 > maxLCDBright ? maxLCDBright : storedInMemory.brightLCD + 1;
  }
  analogWrite(LCDBacklightPin, map(storedInMemory.brightLCD, minLCDBright, maxLCDBright, 0, 255));
  byte buttonPushed = debounceButton();
  if (buttonPushed) {
    EEPROM.put(structAddress, storedInMemory);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

void changeMatBright() {
  lcdPrint(0, "Mat Brightness:");
  storedInMemory.brightMat > minMatBright ? printCharacter(6, 1, '<', 1) : printCharacter(6, 1, ' ', 0);
  lcd.setCursor(7, 2);
  lcd.print(storedInMemory.brightMat);
  storedInMemory.brightMat < maxMatBright ? printCharacter(8, 1, '>', 1) : printCharacter(8, 1, ' ', 0);
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    storedInMemory.brightMat = storedInMemory.brightMat - 1 < minMatBright ? minMatBright : storedInMemory.brightMat - 1;
  } else if (joyDirection == right) {
    storedInMemory.brightMat = storedInMemory.brightMat + 1 > maxMatBright ? maxMatBright : storedInMemory.brightMat + 1;
  }
  lc.setIntensity(0, map(storedInMemory.brightMat, minMatBright, maxMatBright, 0, 15));
  byte buttonPushed = debounceButton();
  if (buttonPushed) {
    EEPROM.put(structAddress, storedInMemory); 
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

void displayLCDInGame() {
  if (livesCount > 0) {
    lcd.setCursor(0,0);
    lcd.print("HP: ");
    for (uint8_t i = 0; i < livesCount; i++) {
      lcd.write(byte(0));
    }
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);
  }
  
}

void putPlayer() {
  gameConfig[playerLocation[0]][playerLocation[1]] = HIGH;
}

void putFood() {
  if (millis() - foodBlinkTimePassed >= foodBlinkInterval) {
    foodState = !foodState;
    foodBlinkTimePassed = millis();
  }
  gameConfig[randomFoodLocation[0]][randomFoodLocation[1]] = foodState;
}

void displayCurrentMatrix(byte matrixConfig[matrixSize][matrixSize]) {
  for (int row = 0; row < matrixSize; row++ ) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrixConfig[matrixSize - col - 1][matrixSize - row - 1]);
    }
  }
}

int joyContinuousDirection() {
  // Read potentiometers values
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  if (yValue > maxThreshold) {
    return left;
  } else if (xValue > maxThreshold) {
    return down;
  } else if (yValue < minThreshold) {
    return right;
  } else if (xValue < minThreshold) {
    return up;
  }
  return neutral;
}

// Returns the direction of the joystick movement
// Check the directions before calling, might be reversed
int joyMoveDirection() {
  // Read potentiometers values
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  if (!joyMoved && yValue > maxThreshold) {
    joyMoved = true;
    return left;
  } else if (!joyMoved && xValue > maxThreshold) {
    joyMoved = true;
    return down;
  } else if (!joyMoved && yValue < minThreshold) {
    joyMoved = true;
    return right;
  } else if (!joyMoved && xValue < minThreshold) {
    joyMoved = true;
    return up;
  }
  bool xIsNeutral = xValue >= minThreshold && xValue <= maxThreshold;
  bool yIsNeutral = yValue >= minThreshold && yValue <= maxThreshold;
  if (xIsNeutral && yIsNeutral) {
    joyMoved = false;
  }
  return neutral;
}

void movePlayer(int direction) {
  gameConfig[playerLocation[0]][playerLocation[1]] = LOW;
  switch(direction) {
    case right:
      playerLocation[1]--;
      if (playerLocation[1] < 0) {
        playerLocation[1] = 0;
        lcd.clear();
        livesCount--;
      }
      break;
    case left:
      playerLocation[1]++;
      if (playerLocation[1] >= matrixSize) {
        playerLocation[1] = matrixSize - 1;
        lcd.clear();
        livesCount--;
      }
      break;
    case up:
      playerLocation[0]--;
      if (playerLocation[0] < 0) {
        playerLocation[0] = 0;
        lcd.clear();
        livesCount--;
      }
      break;
    case down:
      playerLocation[0]++;
      if (playerLocation[0] >= matrixSize) {
        playerLocation[0] = matrixSize - 1;
        lcd.clear();
        livesCount--;
      }
      break;
  }

  if (!isGameOver && livesCount <= 0) {
    isGameOver = 1;
    highscoreTimePassed = millis();
    
  }

  if (playerLocation[0] == randomFoodLocation[0] && playerLocation[1] == randomFoodLocation[1]) {
    addToScore();
    generateNewFoodLocation();
  }
}

// Modifies the current state based on the option selected
void selectOption() {
  // if it's the top option, just grab whatever is under the index
  // else it's index + 1
  int optionSelected = !cursorValue ? index : index + 1;
  if (currentState == initialMenuDisplay) {
    currentState = optionSelected + 1;
  } else {
    currentState = optionSelected + initMenuLength + 1;
  }
  if (currentState == returnToMenu) {
    currentState = initialMenuDisplay;
  }
  if (currentState == initialMenuDisplay || currentState == displaySettings) {
    index = 0;
    cursorValue = 0;
  }
  lcd.clear();
}

void gameOver() {
  lcd.setCursor(0, 0);
  lcd.print(gameOverText);
}

void addToScore() {
  score += level*storedInMemory.difficulty;
}

void generateNewFoodLocation() {
  randomFoodLocation[0] = random(matrixSize);
  randomFoodLocation[1] = random(matrixSize);
  if (playerLocation[0] == randomFoodLocation[0] && playerLocation[1] == randomFoodLocation[1]) {
    generateNewFoodLocation();
  }
}


// Displays item from index and index + 1
void displayCurrentItems(String menuToDisplay[]) {
  lcd.setCursor(0,0);
  lcd.print(menuToDisplay[index]);
  lcd.setCursor(0, 1);
  lcd.print(menuToDisplay[index + 1]);
}

// Prints toPrint on the specified line
void lcdPrint(uint8_t line, String toPrint) {
  lcd.setCursor(0, line);
  lcd.print(toPrint);
}

// Should be put everywhere!
// Returns 1 if button was pushed and 0 otherwise
byte debounceButton() {
  byte buttonWasPushed = 0;
  // Read the button value
  int SWValue = digitalRead(swPin);
  if (SWValue != lastSWValue) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceInterval) {

    // if the button state is different from previous one
    if (SWValue != buttonState) {
      buttonState = SWValue;

      if (!buttonState) {
        buttonWasPushed = 1;
      }
      
    }
  }
  lastSWValue = SWValue;
  if (buttonWasPushed) {
    buttonWasPushed = 0;
    return 1;
  }
  return 0;
}

// Moves index and cursorValue
void moveIndexCursor(int direction, uint8_t menuLength) {
  if (direction == up) {
    if (reachedTheEnd) {
      cursorValue = 0;
      reachedTheEnd = 0;
    } else {
      index--;
    }
    lcd.clear();
  } else if (direction == down) {
    index++;
    lcd.clear();
    
  } 
  if (index <= 0) {
    index = 0;
  } else if (index >= menuLength - 1) {
    index = menuLength - 2;
    cursorValue = 1;
    reachedTheEnd = 1;
  }
}

// Prints a specific character (blinking or not) at the specified column and line
void printCharacter(uint8_t column, uint8_t line, char character, byte blinking) {
  lcd.setCursor(column, line);
  if (!blinking) {
    lcd.print(character);
    return;
  }
  if (millis() - charBlinkTimePassed >= charBlinkInterval) {
    blinkCharState = !blinkCharState;
    charBlinkTimePassed = millis();
  }
  lcd.print(blinkCharState ? character : ' ');
}
