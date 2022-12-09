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
bool joyMoved = false;
const byte moveInterval = 600;
unsigned long long lastMoved = 0;
const int minThreshold = 300;
const int maxThreshold = 700;
byte lastSWValue = 1;
byte buttonState = 1;
unsigned long lastDebounceTime = 0;
unsigned int debounceInterval = 50;


// Addresses for EEPROM to read from
int contrastAddress = 0;
int brightLCDAddress = 1;
int brightMatAddress = 2;
int difficultyAddress = 3;
int firstHighestScoreAdd = 4;
int secondHighestScoreAdd = 5;
int thirdHighestScoreAdd = 6;
int firstHighestNameAdd = 7;
int secondHighestNameAdd = 8;
int thirdHighestNameAdd = 9;

String firstHighestName = "AAA";
String secondHighestName = "AAA";
String thirdHighestName = "AAA";
int highscoreIndex = 0;
int highscoreCursorValue = 0;
int firstHighestHighscore = 0;
int secondHighestHighscore = 0;
int thirdHighestHighscore = 0;

/* Game and menu mechanics */
// Blinking food on Matrix
byte foodState = HIGH;
unsigned int foodBlinkInterval = 250; 
unsigned long foodBlinkTimePassed = 0;

// Blinking character on LCD
const int charBlinkInterval = 400;
unsigned long charBlinkTimePassed = 0;
byte blinkCharState = 0;

// to set the contrast
int contrast;
const int minContrast = 1;
const int maxContrast = 9;

// To set the difficulty
int currentDifficulty;
const int minDifficulty = 1;
const int maxDifficulty = 3;
enum difficulty {
  easy = 1,
  medium = 2,
  hard = 3
};

// To set the brightness of LCD
int currentLCDBright;
const int minLCDBright = 1;
const int maxLCDBright = 9;

// To set the brightness of Matrix
int currentMatBright;
const int minMatBright = 1;
const int maxMatBright = 9;


int livesCount = 3;
int score = 0;
const String gameOverText = "Game Over :(";
byte level = 1;
byte isGameOver = 0;
byte freeToReset = 0;

const unsigned int initMenuLength = 4;
String initialMenu[initMenuLength] = {
  "Start",
  "Highscore",
  "Difficulty",
  "Settings"
};

const unsigned int settingsMenuLength = 4;
String settingsMenu[settingsMenuLength] = {
  "Contrast",
  "LCD Brightness",
  "Mat Brightness",
  "Back"
};

int index = 0;
int cursorValue = 0;
// Whether cursor in menu reached the end of the array
byte reachedTheEnd = 0;

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
int indexInName = 0;


enum menuStates {
  initialMenuDisplay = 0,
  playGame = 1,
  seeHighscore = 2,
  setDificulty = 3, // +
  displaySettings = 4,
  changeContrast = 5,
  changeLCDBrightness = 6, // display -> 0-7; matrix -> 0-7
  changeMatBrightness = 7,
  returnToMenu = 8
};

int currentState = initialMenuDisplay;
byte isInMenu = 1;


const int matrixSize = 8;
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

int randomFoodLocation[2] = {
  0,
  0
};
int playerLocation[2] = {
  5,
  5
};

unsigned long long highscoreTimePassed = 0;
byte bestFirst = 0;
byte bestSecond = 0;
byte bestThird = 0;
byte notTheBest = 0;



/* Custom character library*/
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

// TODO: make face blinking like :) -> ;)
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

int enterNameIndex = 0;
const char alphabet[26] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z'
};

unsigned long long thankYouTimePassed = 0;

void setup() {
  readValuesFromMemory();
  matrixSetup();
  LCDsetup();
  joystickSetup();
  Serial.begin(9600);
}


void loop() {
  if (isGameOver) {
    stopTheGame();
  } else {
    isInMenu ? inMenu() : inGame();
  }
  Serial.println(firstHighestName);
}


/* Setup functions */
void(* resetFunc) (void) = 10;

void readValuesFromMemory() {
  contrast = EEPROM.read(contrastAddress);
  currentLCDBright = EEPROM.read(brightLCDAddress);
  currentMatBright = EEPROM.read(brightMatAddress); 
  currentDifficulty = EEPROM.read(difficultyAddress);

  // TODO: READ THE VALUES FROM EEPROM
  //int fAddress = firstHighestNameAdd + sizeof(firstHighestName);
  //int sAddress = secondHighestNameAdd + sizeof(secondHighestName);
  //int thAddress = thirdHighestNameAdd + sizeof(thirdHighestNameAdd);
  //EEPROM.get(firstHighestNameAdd, firstHighestName);
  //EEPROM.get(secondHighestNameAdd, firstHighestName);
  //EEPROM.get(thirdHighestNameAdd, firstHighestName);
  //firstHighestName = EEPROM.get(firstHighestNameAdd); //!= 0 ? EEPROM.get(firstHighestNameAdd) : "AAA";
  //secondHighestName = EEPROM.get(secondHighestNameAdd); //!= 0 ? EEPROM.get(secondHighestNameAdd) : "AAA";
  //thirdHighestName = EEPROM.get(thirdHighestNameAdd); //!= 0 ? EEPROM.get(thirdHighestNameAdd) : "AAA";

  firstHighestName = "JEB";
  secondHighestName = "POP";
  thirdHighestName = "SAM";

}


void matrixSetup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, map(currentMatBright, minMatBright, maxMatBright, 0, 15));
  lc.clearDisplay(0);
}

void LCDsetup() {
  lcd.createChar(0, heart);
  lcd.begin(16, 2);
  pinMode(contrastPin, OUTPUT);
  pinMode(LCDBacklightPin, OUTPUT);
  analogWrite(contrastPin, map(contrast, minContrast, maxContrast, 0, 255));
  analogWrite(LCDBacklightPin, map(currentLCDBright, minLCDBright, maxLCDBright, 0, 255));
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
    case setDificulty:
      changeDifficulty(); 
      switch(currentDifficulty) {
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
    case displaySettings:
      showMenu(settingsMenu, settingsMenuLength); 
      break;
    case changeContrast:
      modifyContrast(); 
      break;
    case changeLCDBrightness:
      changeLCDBright(); 
      break;
    case changeMatBrightness:
      changeMatBright(); 
      break;
  }
}

void inGame() {
  displayLCDInGame();
  putPlayer();
  putFood();
  displayCurrentMatrix(gameConfig);
  int direction = joyContinuousDirection();
  if (millis() - lastMoved >= moveInterval / currentDifficulty) {
    lastMoved = millis();
    movePlayer(direction);
  }
}


void showHighscores() {
  String highscoreList[3] = {
    firstHighestName,
    secondHighestName,
    thirdHighestName
  };

  
  lcd.setCursor(0,0);
  lcd.print(highscoreList[highscoreIndex]);
  lcd.setCursor(0,1);
  lcd.print(highscoreList[highscoreIndex+1]);
  highscoreIndex > 0 ? printCharacter(15, 0, '^', 1) : printCharacter(15, 0, ' ', 0);
  highscoreIndex < 1 ? printCharacter(15, 1, 'v', 1) : printCharacter(15, 1, ' ', 0);
  // TODO: GET THE HIGHEST SCORES SOMEHOW
  EEPROM.get(firstHighestScoreAdd, firstHighestHighscore);
  EEPROM.get(secondHighestScoreAdd, secondHighestHighscore);
  EEPROM.get(thirdHighestScoreAdd, thirdHighestHighscore);
  if (highscoreIndex == 0) {
    lcd.setCursor(5, 0);
    lcd.print(firstHighestHighscore);
    lcd.setCursor(5, 1);
    lcd.print(secondHighestHighscore);
  } else if (highscoreIndex == 1) {
    lcd.setCursor(5, 0);
    lcd.print(secondHighestHighscore);
    lcd.setCursor(5, 1);
    lcd.print(thirdHighestHighscore);
  }
  int direction = joyMoveDirection();
  if (direction == up) {
    highscoreIndex = highscoreIndex - 1 < 0 ? 0 : highscoreIndex - 1;
    lcd.clear();
  } else if (direction == down) {
    highscoreIndex = highscoreIndex + 1 >= 2 ? highscoreIndex : highscoreIndex + 1;
    lcd.clear();
  }
  if (debounceButton()) {
    currentState = initialMenuDisplay;
    lcd.clear();
    lc.clearDisplay(0);
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

  if (score > EEPROM.read(firstHighestScoreAdd)) {
    bestFirst = 1;
  } else if (score > EEPROM.read(secondHighestScoreAdd)) {
    bestSecond = 1;
  } else if (score > EEPROM.read(thirdHighestScoreAdd)) {
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

byte goodToClear = 0;
void sayYouveBeatenTheScore() {
  if (millis() - thankYouTimePassed <= 3000) {
    lcd.setCursor(0,0);
    lcd.print("Youve beaten the");
    lcd.setCursor(0,1);
    lcd.print("highest score!  ");

    goodToClear = 1;

  } else {
    if (goodToClear) {
      lcd.clear();
      goodToClear = 0;
    }
    updateTheHighestScore();
  }
}

byte nameEntered = 0;
void updateTheHighestScore() {
  if (nameEntered) {    
    String name(playerName);
    if (bestFirst) {
      firstHighestName = name;
      EEPROM.put(firstHighestNameAdd, firstHighestName);
    } else if (bestSecond) {
      secondHighestName = name;
      EEPROM.put(secondHighestNameAdd, secondHighestName);
    } else {
      thirdHighestName = name;
      EEPROM.put(thirdHighestNameAdd, thirdHighestName);
    }
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
  currentDifficulty > minDifficulty ? printCharacter(6, 1, '<', 1) : printCharacter(6, 1, ' ', 0);
  lcd.setCursor(7, 2);
  lcd.print(currentDifficulty);
  currentDifficulty < maxDifficulty ? printCharacter(8, 1, '>', 1) : printCharacter(8, 1, ' ', 0);
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    currentDifficulty = currentDifficulty - 1 < minDifficulty ? minDifficulty : currentDifficulty - 1;
  } else if (joyDirection == right) {
    currentDifficulty = currentDifficulty + 1 > maxDifficulty ? maxDifficulty : currentDifficulty + 1;
  }
  byte buttonPushed = debounceButton();
  if (buttonPushed) {
    EEPROM.update(difficultyAddress, currentDifficulty);
    lcd.clear();
    currentState = initialMenuDisplay;
    return;
  }
}

void modifyContrast() {

  lcdPrint(0, "Contrast:");
  contrast > minContrast ? printCharacter(6, 1, '<', 1) : printCharacter(6, 1, ' ', 0);
  lcd.setCursor(7, 2);
  lcd.print(contrast);
  contrast < maxContrast ? printCharacter(8, 1, '>', 1) : printCharacter(8, 1, ' ', 0);
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    contrast = contrast - 1 < minContrast ? minContrast : contrast - 1;
  } else if (joyDirection == right) {
    contrast = contrast + 1 > maxContrast ? maxContrast : contrast + 1;
  }
  analogWrite(contrastPin, map(contrast, minContrast, maxContrast, 0, 255));
  byte buttonPushed = debounceButton();
  if (buttonPushed) {
    EEPROM.update(contrastAddress, contrast);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

void changeLCDBright() {
  
  lcdPrint(0, "LCD Brightness:");
  currentLCDBright > minLCDBright ? printCharacter(6, 1, '<', 1) : printCharacter(6, 1, ' ', 0);
  lcd.setCursor(7, 2);
  lcd.print(currentLCDBright);
  currentLCDBright < maxLCDBright ? printCharacter(8, 1, '>', 1) : printCharacter(8, 1, ' ', 0);
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    currentLCDBright = currentLCDBright - 1 < minLCDBright ? minLCDBright : currentLCDBright - 1;
  } else if (joyDirection == right) {
    currentLCDBright = currentLCDBright + 1 > maxLCDBright ? maxLCDBright : currentLCDBright + 1;
  }
  analogWrite(LCDBacklightPin, map(currentLCDBright, minLCDBright, maxLCDBright, 0, 255));
  byte buttonPushed = debounceButton();
  if (buttonPushed) {
    EEPROM.update(brightLCDAddress, currentLCDBright);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

void changeMatBright() {
  lcdPrint(0, "Mat Brightness:");
  currentMatBright > minMatBright ? printCharacter(6, 1, '<', 1) : printCharacter(6, 1, ' ', 0);
  lcd.setCursor(7, 2);
  lcd.print(currentMatBright);
  currentMatBright < maxMatBright ? printCharacter(8, 1, '>', 1) : printCharacter(8, 1, ' ', 0);
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    currentMatBright = currentMatBright - 1 < minMatBright ? minMatBright : currentMatBright - 1;
  } else if (joyDirection == right) {
    currentMatBright = currentMatBright + 1 > maxMatBright ? maxMatBright : currentMatBright + 1;
  }
  lc.setIntensity(0, map(currentMatBright, minMatBright, maxMatBright, 0, 15));
  byte buttonPushed = debounceButton();
  if (buttonPushed) {
    EEPROM.update(brightMatAddress, currentMatBright);
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
  score += level*currentDifficulty;
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

/*
// In case memory is empty, uncomment these lines and call WriteValuesToMemory from setup()
int contrast = 4;
int currentDifficulty = 1;
int currentLCDBright = 7;
int currentMatBright = 6;
*/

// Writes contrast, LCDBrightness, MatrixBrightness and Difficulty to EEPROM
void WriteValuesToMemory() {
  EEPROM.put(contrastAddress, contrast);
  EEPROM.put(brightLCDAddress, currentLCDBright);
  EEPROM.put(brightMatAddress, currentMatBright); 
  EEPROM.put(difficultyAddress, currentDifficulty);
}