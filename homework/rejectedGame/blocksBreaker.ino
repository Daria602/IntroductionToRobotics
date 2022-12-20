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
  seeHowToPlay = 2,
  seeHighscore = 3,
  displaySettings = 4,
  seeAbout = 5,
  setDificulty = 6, 
  changeContrast = 7,
  changeLCDBrightness = 8,
  changeMatBrightness = 9,
  returnToMenu = 10
};
int currentState = initialMenuDisplay;

bool joyMoved = false;
const int minThreshold = 300;
const int maxThreshold = 700;
byte lastSWValue = 1;
byte buttonState = 1;



const unsigned int nameMaxLength = 3; 
const unsigned int highestScoreCount = 5;
struct {
  int contrast;
  int brightLCD;
  int brightMat;
  int difficulty;
  int highestScores[highestScoreCount];
  char highestScoreNames[highestScoreCount][nameMaxLength];
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
const unsigned int initMenuLength = 5;
String initialMenu[initMenuLength] = {
  " Start",
  " How to play",
  " Highscores",
  " Settings",
  " About"
};

const unsigned int settingsMenuLength = 5;
String settingsMenu[settingsMenuLength] = {
  " Difficulty",
  " Contrast",
  " LCD Bright",
  " Mat Bright",
  " Back"
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

byte level1[matrixSize][matrixSize] = {
  {0, 0, 1, 1, 1, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

byte level2[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 0, 1, 1, 1, 0},
  {0, 1, 1, 1, 0, 1, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

byte level3[matrixSize][matrixSize] = {
  {0, 0, 0, 1, 1, 0, 0, 0},
  {1, 1, 1, 0, 0, 1, 1, 1},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {1, 1, 1, 0, 0, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

byte level4[matrixSize][matrixSize] = {
  {1, 0, 0, 1, 1, 0, 0, 1},
  {0, 1, 0, 1, 1, 0, 1, 0},
  {1, 1, 1, 0, 0, 1, 1, 1},
  {1, 1, 1, 0, 0, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

byte level5[matrixSize][matrixSize] = {
  {1, 1, 0, 1, 1, 0, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 0, 1, 1, 0, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 0, 1, 1, 0, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
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

byte arrowUp[] = {
  B00000,
  B00000,
  B00100,
  B01110,
  B11011,
  B10001,
  B00000,
  B00000
};

byte arrowDown[] = {
  B00000,
  B00000,
  B10001,
  B11011,
  B01110,
  B00100,
  B00000,
  B00000
};

byte arrowLeft[] = {
  B00000,
  B00000,
  B00110,
  B01100,
  B11000,
  B01100,
  B00110,
  B00000
};

byte arrowRight[] = {
  B00000,
  B00000,
  B01100,
  B00110,
  B00011,
  B00110,
  B01100,
  B00000
};

byte humanJump[] = {
  B00000,
  B10101,
  B01110,
  B00100,
  B00100,
  B01010,
  B10001,
  B00000
};

byte humanSit[] = {
  B00000,
  B00000,
  B00100,
  B00100,
  B01110,
  B10101,
  B00100,
  B00100
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

byte settings[matrixSize] = {
  B00100100,
  B00100100,
  B00011000,
  B00011000,
  B00011000,
  B00011000,
  B00100100,
  B00100100
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

float ballMoveInterval;
int startingSpeed = 500;










void setup() {
  Serial.begin(9600);
  EEPROM.get(structAddress, storedInMemory);
  ballMoveInterval = startingSpeed*pow(0.9,storedInMemory.difficulty);
  matrixSetup();
  LCDsetup();
  joystickSetup();
  
  showTitleTime = millis(); 
}

byte shouldClearTitle = 1;
const String gameName = "*Blocks Breaker*";

void loop() {
  if (millis() - showTitleTime <= 3000) {
    lcd.setCursor(0, 0);
    lcd.print(gameName);
    lcd.setCursor(0, 1);
    lcd.print("****************");

  } else {

    if (shouldClearTitle) {
      lcd.clear();
      shouldClearTitle = 0;
    }

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
  lcd.createChar(1, arrowUp);
  lcd.createChar(2, arrowDown);
  lcd.createChar(3, arrowLeft);
  lcd.createChar(4, arrowRight);
  lcd.createChar(5, humanJump);
  lcd.createChar(6, humanSit);
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

unsigned long long timeInGame = 0;
unsigned int addRowInterval = 5000;
int platformLength = 0;
int platformStart = 3;
int indexHowToPlay = 0;
byte indexReset = 0;
/* Main functions */
void inMenu() {
  switch (currentState) {
    case initialMenuDisplay:
      showMenu(initialMenu, initMenuLength);
      displayMatrixByColumn(face);
      break;
    case playGame:
      isInMenu = 0;
      if (storedInMemory.difficulty == easy) {
        platformLength = 3;
      } else {
        platformLength = 2;
      }
      timeInGame = millis();
      break;
    case seeHowToPlay:
      if (!indexReset) {
        indexHowToPlay = 0;
        indexReset = 1;
      }
      displayHowToPlay();
      break;
    case seeHighscore:
      showHighscores();
      displayMatrixByColumn(star);
      break;
    case displaySettings:
      showMenu(settingsMenu, settingsMenuLength); 
      displayMatrixByColumn(settings);
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

const int howToPlayLength = 8;
const String howToPlayTxt[howToPlayLength] = {
  "Bounce the ball",
  "with platform",
  "and destroy",
  "the blocks.",
  "When the ball",
  "falls off",
  "you loose heart",
  "Have fun!"
};

void displayHowToPlay() {
  lcd.setCursor(1, 0);
  lcd.print(howToPlayTxt[indexHowToPlay]);
  lcd.setCursor(1, 1);
  lcd.print(howToPlayTxt[indexHowToPlay + 1]);
  if (indexHowToPlay <= howToPlayLength - 2) {
    lcd.setCursor(0, 1);
    lcd.write(byte(2));
  } else {
    lcd.print(" ");
  }
  int direction = joyMoveDirection();
  if (direction == down) {
    lcd.clear();
    indexHowToPlay++;
    if (indexHowToPlay > howToPlayLength - 2) {
      indexHowToPlay = howToPlayLength - 2;
    }
  }

  if (debounceButton()) {
    lcd.clear();
    indexReset = 0;
    currentState = initialMenuDisplay;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned long long ballLastMoved = 0;


byte cleared = 1;
byte isInfiniteMode = 0;
int currentLevel = 0;
const int maxLevel = 5;

void inGame() {
  displayLCDInGame();

  if (cleared) {
    if (isInfiniteMode) {
      addRows();
    } else {
      nextLevel();
    }
    
  } else {
    displayCurrentMatrix(gameConfig);
    displayPlatform();
    int direction = joyContinuousDirection();
    if (millis() - lastMoved >= moveInterval) {
      lastMoved = millis();
      movePlatform(direction);
    }

    if (millis() - ballLastMoved >= ballMoveInterval) {
      ballLastMoved = millis();
      moveBall();
    }

    
  }
  if(checkIfCleared()) {
    cleared = 1;
  }
}

int ballPosition[2] = { matrixSize - 2, platformStart };

// Will add that to ballPosition
// If moveRight is positive, the ballPosition[1] will increase
int moveRight = 1;
int moveDown = 1;

void moveBall() {
  

  // clear old position
  gameConfig[ballPosition[0]][ballPosition[1]] = 0;

  // check ceiling collision
  if (ballPosition[0] == 0) {
    moveDown = -moveDown;
  }
  if (ballPosition[0] == matrixSize - 1) {
    subtractLive();
    return;
  }

  // check left and right wall collisions 
  if (ballPosition[1] == 0 || ballPosition[1] == matrixSize - 1) {
    moveRight = -moveRight;
  }

  // // check paddle collision
  // for (int i = platformStart; i < platformStart + platformLength; i++) {
  //   if (ballPosition[0] + 1 == i) {
  //     moveDown = -moveDown;
  //     break;  
  //   }
  // }

  if (gameConfig[ballPosition[0] + moveDown][ballPosition[1]] == 1) {
    gameConfig[ballPosition[0] + moveDown][ballPosition[1]] = 0;
    moveDown = -moveDown;

    byte collidedWithPlarform = 0;
    if (ballPosition[0] == matrixSize - 2) {
      for (int i = platformStart; i < platformStart + platformLength; i++) {
        if (ballPosition[1] == i) {
          collidedWithPlarform = 1;
        }
      }
    }

    if (!collidedWithPlarform) {
      addToScore();
    }
  }
  
  if (gameConfig[ballPosition[0]][ballPosition[1] + moveRight] == 1) {
    gameConfig[ballPosition[0]][ballPosition[1] + moveRight] = 0;
    moveRight = -moveRight;

    byte collidedWithPlarform = 0;
    if (ballPosition[0] == matrixSize - 2) {
      for (int i = platformStart; i < platformStart + platformLength; i++) {
        if (ballPosition[1] == i) {
          collidedWithPlarform = 1;
        }
      }
    }

    if (!collidedWithPlarform) {
      addToScore();
    }

  }



  ballPosition[0] += moveDown;
  ballPosition[1] += moveRight;

  gameConfig[ballPosition[0]][ballPosition[1]] = 1;
  
}

void subtractLive() {
  lcd.clear();
  lc.clearDisplay(0);
  livesCount--;
  if (livesCount <= 0) {
    Serial.println("Got here");
    isGameOver = 1;
    highscoreTimePassed = millis();
  }
  ballPosition[0] = matrixSize - 2;
  ballPosition[1] = platformStart;
  gameConfig[ballPosition[0]][ballPosition[1]] = 1;
}

byte checkIfCleared() {
  for (int row = 0; row < matrixSize; row++) {
    for (int column = 0; column < matrixSize; column++) {
      if (gameConfig[row][column] == 1) {
        if (row == ballPosition[0] && column == ballPosition[1]) {
          continue;
        } else if (row == matrixSize - 1) {
          for (int platformPoz = platformStart; platformPoz < platformStart + platformLength; platformPoz++) {
            if (column == platformPoz) {
              continue;
            }
          }
        } else {
          return 0;
        }
      }
    }
  }

  return 1;
}


void movePlatform(int direction) {
    // The directions are reversed
    if (direction == right) {
        //  move left if possible
        platformStart--;
        if (platformStart < 0) {
            platformStart = 0;
        }
    } else if (direction == left) {
        // move right if possible
        platformStart++;
        if (platformStart + platformLength > matrixSize) {
            platformStart--;
        }
    }
}



void displayPlatform() {
    for (int i = 0; i < matrixSize; i++) {
        if (i >= platformStart && i < platformStart + platformLength) {
            gameConfig[matrixSize - 1][i] = 1;
        } else {
            gameConfig[matrixSize - 1][i] = 0;
        }
    }
}

void addRows() {
  lc.clearDisplay(0);
  // add random amount of rows between 2 and 4
  int rowsToAdd = random(2, 5);
  for (int row = 0; row < rowsToAdd; row++) {
    int newRow[matrixSize];
    for (int i = 0; i < matrixSize; i++) {
        newRow[i] = random(2);
    }

    for( int column = 0; column < matrixSize; column++) {
      gameConfig[row][column] = newRow[column];
    }
  }

  ballPosition[0] = matrixSize - 2;
  ballPosition[1] = platformStart;
  // gameConfig[ballPosition[0]][ballPosition[1]] = 1;
  moveRight = 1;
  moveDown = 1;
  cleared = 0;
  
}

void displayLevel(byte level[matrixSize][matrixSize]) {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    for (int column = 0; column < matrixSize; column++) {
      gameConfig[row][column] = level[row][column];
    }
  }
  ballPosition[0] = matrixSize - 2;
  ballPosition[1] = platformStart;
  // gameConfig[ballPosition[0]][ballPosition[1]] = 1;
  moveRight = 1;
  moveDown = 1;
  cleared = 0;
}

void nextLevel() {
  currentLevel++;
  ballMoveInterval = ballMoveInterval*0.9;
  switch (currentLevel) {
    case 1:
      displayLevel(level1);
      break;
    case 2:
      displayLevel(level2);
      break;
    case 3:
      displayLevel(level3);
      break;
    case 4:
      displayLevel(level4);
      break;
    case 5:
      displayLevel(level5);
      break;
  }

  if (currentLevel > maxLevel) {
    lcd.clear();
    highscoreTimePassed = millis();
    isGameOver = 1;
  }

}

unsigned long long humansChangeTime = 0;
unsigned int humansChangeInt = 250;
void displayLCDInGame() {
    lcd.setCursor(0,0);
    for (unsigned int i = 0; i < livesCount; i++) {
      lcd.write(byte(0));
    }

    int scoreCursor = 15;
    int temporaryScore = score;
    while (temporaryScore / 10 > 0 && scoreCursor > 11) {
        temporaryScore = temporaryScore / 10;
        scoreCursor--;
    }
    lcd.setCursor(scoreCursor, 0);
    lcd.print(score);
    if (millis() - humansChangeTime >= humansChangeInt) {
      displayHumans();
      humansChangeTime = millis();
    }
    
}

const int humansCount = 5;
void displayHumans() {
  // TODO: ADD SPARKLZZZ
    lcd.setCursor(4, 1);
    for (int i = 0; i < humansCount; i++) {
        random(2) == 0 ? lcd.write(byte(5)) : lcd.write(byte(6));
        lcd.print(" ");
    }
}

void displayScoreFromIndex(int scoreIndex) {
  lcd.print(scoreIndex + 1);
  lcd.print(".");
  for (int i = 0; i < nameMaxLength; i++) {
    lcd.print(storedInMemory.highestScoreNames[scoreIndex][i]);
  }
  lcd.print(" ");
  lcd.print(storedInMemory.highestScores[scoreIndex]);
}

void showHighscores() {
  lcd.setCursor(0, 0);
  if (highscoreIndex > 0) {
    lcd.write(byte(1));
  } else {
    lcd.print(" ");
  }
  lcd.print(" ");

  lcd.setCursor(0, 1);
  if (highscoreIndex < highestScoreCount - 2) {
    lcd.write(byte(2));
  } else {
    lcd.print(" ");
  }
  lcd.print(" ");

  lcd.setCursor(2, 0);
  displayScoreFromIndex(highscoreIndex);
  lcd.setCursor(2, 1);
  displayScoreFromIndex(highscoreIndex + 1);

  int direction = joyMoveDirection();
  
  if (direction == up) {
    highscoreIndex = highscoreIndex - 1 < 0 ? 0 : highscoreIndex - 1;
    lcd.clear();
  } else if (direction == down) {
    highscoreIndex = highscoreIndex + 1 >= highestScoreCount - 1 ? highscoreIndex : highscoreIndex + 1;
    lcd.clear();
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
  int indexIfBeatenScore = isScoreBeaten();

  if (millis() - highscoreTimePassed >= 5000) {

    if (indexIfBeatenScore != -1) {
      sayYouveBeatenTheScore(indexIfBeatenScore);
      displayMatrixByColumn(star);
    } else {
      sayThankYou();
      displayMatrixByColumn(face);
    }

  } else {
    thankYouTimePassed = millis();
    showFinalScore();
  }
  
  if (freeToReset) {
    lcd.clear();
    resetFunc();
  }
  
}

int isScoreBeaten() {
  for (int i = 0; i < highestScoreCount; i++) {
    if (score >= storedInMemory.highestScores[i]) {
      return i;
    }
  }
  return -1;
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


void sayYouveBeatenTheScore(int indexIfBeatenScore) {
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
    updateTheHighestScore(indexIfBeatenScore);
  }
}


void updateTheHighestScore(int indexOfHighscore) {

  if (nameEntered) {
    int scoreToCarry;
    char nameToCarry[nameMaxLength];
    for (unsigned int i = indexOfHighscore; i < highestScoreCount; i++) {
      // update the score and move the rest to the right
      scoreToCarry = storedInMemory.highestScores[i];
      storedInMemory.highestScores[i] = score;
      score = scoreToCarry;

      // update the name and move the rest to the right
      for (int indexInName = 0; indexInName < nameMaxLength; indexInName++) {
        nameToCarry[indexInName] = storedInMemory.highestScoreNames[i][indexInName];
        storedInMemory.highestScoreNames[i][indexInName] = playerName[indexInName];
        playerName[indexInName] = nameToCarry[indexInName];
      }

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

  lcd.setCursor(0, 0);
  lcd.print("Enter name");
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

void showMenu(String menu[], unsigned int menuLength) {
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
  showCursor();
  lcd.setCursor(0, 0);
  if (index > 0) {
    lcd.write(byte(1));
  } else {
    lcd.print(" ");
  }
  lcd.setCursor(0, 1);
  if (index < menuLength - 2) {
    lcd.write(byte(2));
  } else {
    lcd.print(" ");
  }
}

void displayMatrixByColumn(byte charachterToDisplay[matrixSize]) {
  for (int column = 0; column < matrixSize; column++) {
    lc.setColumn(0, column, charachterToDisplay[matrixSize - column - 1]);
  }
}

void changeDifficulty() {


  lcd.setCursor(3, 0);
  lcd.print("Difficulty");
  lcd.setCursor(4, 1);
  
  switch (storedInMemory.difficulty) {
    case easy:
      lcd.print(" easy");
      lcd.write(byte(4));
      break;
    case medium:
      lcd.write(byte(3));
      lcd.print("medium");
      lcd.write(byte(4));
      break;
    case hard:
      lcd.write(byte(3));
      lcd.print("hard");
      break;
  }
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    storedInMemory.difficulty = storedInMemory.difficulty - 1 < minDifficulty ? minDifficulty : storedInMemory.difficulty - 1;
    lcd.clear();
  } else if (joyDirection == right) {
    storedInMemory.difficulty = storedInMemory.difficulty + 1 > maxDifficulty ? maxDifficulty : storedInMemory.difficulty + 1;
    lcd.clear();
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
  lcd.setCursor(3, 0);
  lcd.print("Contrast");
  lcd.setCursor(6, 1);
  storedInMemory.contrast > minContrast ? lcd.write(byte(3)) : lcd.print(" ");
  lcd.print(storedInMemory.contrast);
  storedInMemory.contrast < maxContrast ? lcd.write(byte(4)) : lcd.print(" ");

  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    storedInMemory.contrast = storedInMemory.contrast - 1 < minContrast ? minContrast : storedInMemory.contrast - 1;
  } else if (joyDirection == right) {
    storedInMemory.contrast = storedInMemory.contrast + 1 > maxContrast ? maxContrast : storedInMemory.contrast + 1;
  }

  analogWrite(contrastPin, map(storedInMemory.contrast, minContrast, maxContrast, 0, 255));

  if (debounceButton()) {
    EEPROM.put(structAddress, storedInMemory);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

void changeLCDBright() {

  lcd.setCursor(1, 0);
  lcd.print("LCD Brightness");
  lcd.setCursor(6, 1);
  storedInMemory.brightLCD > minLCDBright ? lcd.write(byte(3)) : lcd.print(" ");
  lcd.print(storedInMemory.brightLCD);
  storedInMemory.brightLCD < maxLCDBright ? lcd.write(byte(4)) : lcd.print(" ");
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    storedInMemory.brightLCD = storedInMemory.brightLCD - 1 < minLCDBright ? minLCDBright : storedInMemory.brightLCD - 1;
  } else if (joyDirection == right) {
    storedInMemory.brightLCD = storedInMemory.brightLCD + 1 > maxLCDBright ? maxLCDBright : storedInMemory.brightLCD + 1;
  }

  analogWrite(LCDBacklightPin, map(storedInMemory.brightLCD, minLCDBright, maxLCDBright, 0, 255));

  if (debounceButton()) {
    EEPROM.put(structAddress, storedInMemory);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

void changeMatBright() {

  lcd.setCursor(1, 0);
  lcd.print("Mat Brightness");
  lcd.setCursor(6, 1);
  storedInMemory.brightMat > minMatBright ? lcd.write(byte(3)) : lcd.print(" ");
  lcd.print(storedInMemory.brightMat);
  storedInMemory.brightMat < maxMatBright ? lcd.write(byte(4)) : lcd.print(" ");
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    storedInMemory.brightMat = storedInMemory.brightMat - 1 < minMatBright ? minMatBright : storedInMemory.brightMat - 1;
  } else if (joyDirection == right) {
    storedInMemory.brightMat = storedInMemory.brightMat + 1 > maxMatBright ? maxMatBright : storedInMemory.brightMat + 1;
  }

  lc.setIntensity(0, map(storedInMemory.brightMat, minMatBright, maxMatBright, 0, 15));

  if (debounceButton()) {
    EEPROM.put(structAddress, storedInMemory); 
    lcd.clear();
    currentState = displaySettings;
    return;
  }
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


void addToScore() {
  score += currentLevel*storedInMemory.difficulty;
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
  lcd.setCursor(1,0);
  lcd.print(menuToDisplay[index]);
  lcd.setCursor(1, 1);
  lcd.print(menuToDisplay[index + 1]);
}

// Prints toPrint on the specified line
void lcdPrint(unsigned int line, String toPrint) {
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
void moveIndexCursor(int direction, unsigned int menuLength) {
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

unsigned long long cursorBlinkTime = 0;
unsigned int cursorBlinkInterval = 400;
byte cursorBlinkState = 0;

void showCursor() {
  lcd.setCursor(15, cursorValue);
  if (millis() - cursorBlinkTime >= cursorBlinkInterval) {
    cursorBlinkState = !cursorBlinkState;
    cursorBlinkTime = millis();
  }
  if (cursorBlinkState) {
    lcd.write(byte(3));
  } else {
    lcd.print(" ");
  }
}


// Prints a specific character (blinking or not) at the specified column and line
void printCharacter(unsigned int column, unsigned int line, char character, byte blinking) {
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


// to reset highest scores in memory
void resetMemory() {
  for (int i = 0; i < highestScoreCount; i++) {
    storedInMemory.highestScores[i] = 0;
    for (int j = 0; j < nameMaxLength; j++) {
      storedInMemory.highestScoreNames[i][j] = 'A';
    }
  }
  EEPROM.put(structAddress, storedInMemory);
}
