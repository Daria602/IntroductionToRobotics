#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h"
#include "ezBuzzer.h"

// LCD pins
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 4;
const byte d7 = 3;
const byte contrastPin = 5;
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

// Buzzer pin
const byte buzzerPin = A3;
ezBuzzer buzzer(buzzerPin);


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
  changeSound = 7,
  changeContrast = 8,
  changeLCDBrightness = 9,
  changeMatBrightness = 10,
  resetHighscores = 11,
  returnToMenu = 12
};
int currentState = initialMenuDisplay;

bool joyMoved = false;
const int minThreshold = 300;
const int maxThreshold = 700;
byte lastSWValue = 1;
byte buttonState = 1;



const unsigned int nameMaxLength = 5; 
const unsigned int highestScoreCount = 5;

const int structAddress = 0;
struct {
  int contrast = 2;
  int brightLCD = 5;
  int brightMat = 4;
  int difficulty = 1;
  byte isSoundOn = 1;
  int highestScores[highestScoreCount];
  char highestScoreNames[highestScoreCount][nameMaxLength];
} storedInMemory;

// to set the contrast
const byte minContrast = 1;
const byte maxContrast = 9;

// To set the difficulty
const byte minDifficulty = 1;
const byte maxDifficulty = 4;
enum difficulty {
  easy = 1,
  medium = 2,
  hard = 3,
  insane = 4
};

// To set the brightness of LCD
const byte minLCDBright = 1;
const byte maxLCDBright = 9;

// To set the brightness of Matrix
const byte minMatBright = 1;
const byte maxMatBright = 9;

// Strings, charachters and images to print
const byte nameLength = 5;
char playerName[nameLength] = {
  'A', 'A', 'A', 'A', 'A'
};
int playerNameAlphIndexes[nameLength] = {
  0, 0, 0, 0, 0
};

const byte initMenuLength = 5;
const String initialMenu[initMenuLength] = {
  " Start",
  " How to play",
  " Highscores",
  " Settings",
  " About"
};

const byte settingsMenuLength = 7;
const String settingsMenu[settingsMenuLength] = {
  " Difficulty",
  " Sound",
  " Contrast",
  " LCD Bright",
  " Mat Bright",
  " Reset scores",
  " Back"
};

const byte matrixSize = 8;
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

byte obstacles[matrixSize][matrixSize] = {
  {1, 1, 0, 0, 0, 0, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 0, 0, 0, 0, 1, 1}
};

const byte heart[matrixSize] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

const byte arrowUp[matrixSize] = {
  B00000,
  B00000,
  B00100,
  B01110,
  B11011,
  B10001,
  B00000,
  B00000
};

const byte arrowDown[matrixSize] = {
  B00000,
  B00000,
  B10001,
  B11011,
  B01110,
  B00100,
  B00000,
  B00000
};

const byte SleftUpPart[matrixSize] {
  B00000,
  B00001,
  B00011,
  B00110,
  B00110,
  B00110,
  B00011,
  B00001
};

const byte SrightUpPart[matrixSize] {
  B00000,
  B11000,
  B11100,
  B01100,
  B00111,
  B00000,
  B00000,
  B00000
};

const byte SleftDownPart[matrixSize] {
  B00000,
  B00000,
  B00000,
  B00000,
  B00100,
  B00111,
  B00011,
  B00000
};

const byte SrightDownPart[matrixSize] {
  B11000,
  B01100,
  B00110,
  B00110,
  B00110,
  B11110,
  B11100,
  B00000
};

const byte easyDifficulty[matrixSize] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000011,
  B00000011,
  B00000011
};

const byte mediumDifficulty[matrixSize] = {
  B00000000,
  B00000000,
  B00000000,
  B00011000,
  B00011000,
  B00011011,
  B00011011,
  B00011011
};

const byte hardDifficulty[matrixSize] = {
  B11000000,
  B11000000,
  B11000000,
  B11011000,
  B11011000,
  B11011011,
  B11011011,
  B11011011
};

const byte insaneDifficulty[matrixSize] = {
  B00000000,
  B01010101,
  B00100010,
  B01010101,
  B00000000,
  B00000000,
  B00111110,
  B00000000
};

const byte settings[matrixSize] = {
  B00000000,
  B01011010,
  B00100100,
  B01000010,
  B11011011,
  B01000010,
  B00100100,
  B01011010
};

const byte face[matrixSize] = {
  B00000000,
  B01000010,
  B10100101,
  B00000000,
  B00000000,
  B01000010,
  B00111100,
  B00000000
};

const byte question[matrixSize] = {
  B00111000,
  B01111100,
  B01100110,
  B00000110,
  B00001100,
  B00011000,
  B00000000,
  B00011000
};

const byte star[matrixSize] = {
  B00011000,
  B00011000,
  B11111111,
  B01111110,
  B00111100,
  B01111110,
  B01100110,
  B00000000
};

const byte soundOn[matrixSize] = {
  B00000000,
  B00100010,
  B01101001,
  B11100101,
  B11100101,
  B01101001,
  B00100010,
  B00000000
};

const byte brightness[matrixSize] = {
  B10001001,
  B01000010,
  B00111100,
  B10111100,
  B00111101,
  B00111100,
  B01000010,
  B10010001
};

const byte contrast[matrixSize] = {
  B00000000,
  B00111100,
  B01001110,
  B10001111,
  B10001111,
  B10001111,
  B01001110,
  B00111100
};

const byte house[matrixSize] = {
  B00011000,
  B00111100,
  B01111110,
  B11111111,
  B01000010,
  B01011010,
  B01011010,
  B01111110
};

const char alphabet[26] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z'
};
const int howToPlayLength = 11;
const String howToPlayTxt[howToPlayLength] = {
  "You control",
  "the snake.",
  "Eat food, grow.",
  "When snake",
  "meets itself",
  "or an obstacle,",
  "you lose a life",
  "When all your",
  "lives are gone,",
  "it's game over",
  "Have fun!"
};

const char madeBy[] = "Made by Izotova Daria Github: https://github.com/Daria602/   ";
// Snake-a-licious game name
const String gameName = "nake-a-licious";


// Time and intervals
unsigned long lastDebounceTime = 0;
const unsigned int debounceInterval = 50;
unsigned long charBlinkTimePassed = 0;
unsigned long highscoreTimePassed = 0;
unsigned long thankYouTimePassed = 0;
unsigned long introTime = 0;
unsigned long scrollTime = 0;
const unsigned int scrollSpeed = 450;
unsigned long lastMoved = 0;
const unsigned int introAnimationTime = 3000;
const unsigned int gameNameTime = 5000;
unsigned long SMoveAnimationTime;

// Indexes 
//Index of the menu shown
int index = 0;
int cursorValue = 0;
int indexInName = 0;
int positionCounter = 0;
int highscoreIndex = 0;
int animationCursor = 14;
int indexHowToPlay = 0;

// Byte flags for loops
byte goodToClearHighScore = 0;
byte nameEntered = 0;
byte isAboutPrinted = 0;
byte isInMenu = 1;
byte isGameOver = 0;
byte freeToReset = 0;
byte buzzerStartedPlaying = 0;
byte animationOver = 0;

// Whether cursor in menu reached the end of the array
byte reachedTheEnd = 0;
byte shouldClearTitle = 1;
byte skippedIntro = 0;
byte indexReset = 0;
byte changedRecently = 0;
byte hasObstacles = 0;
byte highscoreCursorLeft = 0;


/* Game and menu mechanics */
const unsigned int fastSpeed = 200;
const unsigned int slowSpeed = 300;
struct {
  int easy = 1;
  int medium = 5;
  int hard = 10;
  int insane = 15;
} difficultyBonuses;
struct {
  int row = 3;
  int column = 3;
} snakeHead;

struct {
  int row = 5;
  int column = 5;
} food;
enum soundStates {
  on = 1,
  off = 0
};
unsigned int snakeSpeed = 0;
int livesCount = 3;
int score = 0;
int headDirection = right;
int snakeLength = 3;
int difficultyBonus = 0;

unsigned long cursorBlinkTime = 0;
const unsigned int cursorBlinkInterval = 400;
byte cursorBlinkState = 0;

unsigned long foodBlinkTime = 0;
const unsigned int foodBlinkInterval = 150;
byte foodState = 1;


/* Buzzer related */
const int melody[] = {
  // Melody borrowed from https://github.com/robsoncouto/arduino-songs/blob/master/keyboardcat/keyboardcat.ino
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_E4, 
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_E4
};

const int durations[] = {
  4, 4, 4, 4, 
  4, 8, 3, 4
};

const int shortToneDuration = 150;
const int snakeMoveDuration = 50;
const int menuFrequency = 800;
const int clickFrequency = 1000;
const int snakeMoveFrequency = 300;
const int snakeDirectionFrequency = 500;
const int snakeEatFrequency = 800;
const int snakeHurtFrequency = 100;


// Function to reset Arduino after the game is done
void(* resetFunc) (void) = sizeof(storedInMemory);

void setup() {
  EEPROM.get(structAddress, storedInMemory); 
  noTone(buzzerPin);
  matrixSetup(lc);
  LCDsetup(lcd);
  joystickSetup();
  introTime = millis(); 
  SMoveAnimationTime = millis();
}

void loop() {
  buzzer.loop();
  if ((millis() - introTime <= introAnimationTime + gameNameTime) && !skippedIntro) {
    if (storedInMemory.isSoundOn && !buzzerStartedPlaying) {
      buzzerStartedPlaying = 1;
      int length = sizeof(durations) / sizeof(int);
      buzzer.playMelody(melody, durations, length);
    }

    // Play animation and then show title
    if (!animationOver) {
      introAnimation();
    } else {
      showTitle();
    }
    
    // skip the intro
    if (debounceButton()) {
      skippedIntro = 1;
    }

  } else {

    if (shouldClearTitle) {
      lcd.clear();
      shouldClearTitle = 0;
      if (storedInMemory.isSoundOn) {
        buzzer.stop();
      }
      
    }

    // The main menu/game if
    if (isGameOver) {
      stopTheGame();
    } else {
      isInMenu ? inMenu() : inGame();
    }
  }
}

/* Setup functions */

void matrixSetup(LedControl &lc) {
  lc.shutdown(0, false);
  lc.setIntensity(0, map(storedInMemory.brightMat, minMatBright, maxMatBright, 0, 15));
  lc.clearDisplay(0);
}

void LCDsetup(LiquidCrystal &lcd) {
  lcd.createChar(0, heart);
  lcd.createChar(1, arrowUp);
  lcd.createChar(2, arrowDown);
  lcd.createChar(3, SleftUpPart);
  lcd.createChar(4, SrightUpPart);
  lcd.createChar(5, SleftDownPart);
  lcd.createChar(6, SrightDownPart);
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


/* Intro Animation related functions */

void introAnimation() {
  if (millis() - SMoveAnimationTime >= introAnimationTime / 15 ) {
    lcd.clear();
    SMoveAnimationTime = millis();
    animationCursor--;
    if (animationCursor < 0) {
      animationCursor = 0;
      animationOver = 1;
      lcd.clear();
    }
  }
  lcd.setCursor(animationCursor, 0);
  lcd.write(byte(3));
  lcd.setCursor(animationCursor + 1, 0);
  lcd.write(byte(4));
  lcd.setCursor(animationCursor, 1);
  lcd.write(byte(5));
  lcd.setCursor(animationCursor + 1, 1);
  lcd.write(byte(6));

}

void showTitle() {
  lcd.setCursor(0, 0);
  lcd.write(byte(3));
  lcd.setCursor(1, 0);
  lcd.write(byte(4));
  lcd.setCursor(0, 1);
  lcd.write(byte(5));
  lcd.setCursor(1, 1);
  lcd.write(byte(6));
  lcd.setCursor(2, 1);
  lcd.print(gameName);
}


/* Main functions */
void inMenu() {
  switch (currentState) {
    case initialMenuDisplay:
      showMenu(initialMenu, initMenuLength);
      displayMatrixByColumn(house);
      break;
    case playGame:
      isInMenu = 0;
      setDifficultyRelated();
      break;
    case seeHowToPlay:
      if (!indexReset) {
        indexHowToPlay = 0;
        indexReset = 1;
      }
      displayHowToPlay();
      displayMatrixByColumn(question);
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
      displayMatrixByColumn(face);
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
        case insane:
          displayMatrixByColumn(insaneDifficulty);
          break;
      }
      break;
    case changeSound:
      modifySound();
      displayMatrixByColumn(soundOn);
      break;
    case changeContrast:
      displayMatrixByColumn(contrast);
      modifyContrast(); 
      break;
    case changeLCDBrightness:
      displayMatrixByColumn(brightness);
      changeLCDBright(); 
      break;
    case changeMatBrightness:
      displayMatrixByColumn(brightness);
      changeMatBright(); 
      break;
    case resetHighscores:
      displayMatrixByColumn(star);
      clearHighscores();
      break;
  }
}

void inGame() {
  displayLCDInGame();
  if (hasObstacles) {
    addObstacles();
  }
  
  int direction = joyMoveDirection();
  if (direction != neutral && direction != headDirection && !changedRecently) {
    changeSnakeDirection(direction);
  }
  if (millis() - lastMoved >= snakeSpeed) {
    lastMoved = millis();
    moveSnake();
    
    changedRecently = 0;
  }
  addFood();
  displayCurrentMatrix(gameConfig);
}

void stopTheGame() {
  // show players scores
  // check if highscore was beaten
  // if yes, ask player to enter name
  // if no, say thank you for playing -> turn off
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


/* Game related*/

// At the start of the game, calculates the stuff related to difficulty
void setDifficultyRelated() {
  switch (storedInMemory.difficulty) {
    case easy:
      snakeSpeed = slowSpeed;
      hasObstacles = 0;
      difficultyBonus = difficultyBonuses.easy;
      break;
    case medium:
      snakeSpeed = slowSpeed;
      hasObstacles = 1;
      difficultyBonus = difficultyBonuses.medium;
      break;
    case hard:
      snakeSpeed = fastSpeed;
      hasObstacles = 0;
      difficultyBonus = difficultyBonuses.hard;
      break;
    case insane:
      snakeSpeed = fastSpeed;
      hasObstacles = 1;
      difficultyBonus = difficultyBonuses.insane;
      break;
  }
}

// Adds obstacles to gameConfig
void addObstacles() {
  for (int row = 0; row < matrixSize; row++) {
    for (int column = 0; column < matrixSize; column++) {
      if (obstacles[row][column] == 1) {
        gameConfig[row][column] = 2;
      }
    }
  }
}

// Adds food to gameConfig
void addFood() {
  if (food.row == -1 || food.column == -1) {
    recalculateFood();
  }
  if (millis() - foodBlinkTime >= foodBlinkInterval) {
    foodState = !foodState;
    foodBlinkTime = millis();
  }
  gameConfig[food.row][food.column] = foodState;
}

// Changes the direction of snake accordingly
void changeSnakeDirection(int direction) {
  switch (direction) {
    case up:
      if (headDirection != down) {
        headDirection = up;
        changedRecently = 1;
      } 
      break;
    case down:
      if (headDirection != up) {
        headDirection = down;
        changedRecently = 1;
      } 
      break;
    case left:
      if (headDirection != right) {
        headDirection = left;
        changedRecently = 1;
      } 
      break;
    case right:
      if (headDirection != left) {
        headDirection = right;
        changedRecently = 1;
      } 
      break;
  }
}

// Function for moving the snake
// Reversed because of joyMoveDirection
void moveSnake() {
  switch (headDirection) {
    case down:
      snakeHead.row++;
      if (snakeHead.row == matrixSize) {
        snakeHead.row = 0;
      }
      break;
    case up:
      snakeHead.row--;
      if (snakeHead.row < 0) {
        snakeHead.row = matrixSize - 1;
      }
      break;
    case left:
      snakeHead.column++;
      if (snakeHead.column == matrixSize) {
        snakeHead.column = 0;
      }
      break;
    case right:
      snakeHead.column--;
      if (snakeHead.column < 0) {
        snakeHead.column = matrixSize - 1;
      }
      
      break;
  }

  // check if food was eaten
  if (snakeHead.row == food.row && snakeHead.column == food.column) {
    playSound(snakeEatFrequency, shortToneDuration);
    snakeLength++;
    for (int row = 0; row < matrixSize; row++) {
      for (int column = 0; column < matrixSize; column++) {
        if (gameConfig[row][column] > 0) {
          gameConfig[row][column]++;
        }
      }
    }
    food.row = -1;
    food.column = -1;
    addToScore();
  } else if (gameConfig[snakeHead.row][snakeHead.column] > 0 && food.row > -1 && food.column > -1) { // in the snake had met a body segment, end the game
    playSound(snakeHurtFrequency, shortToneDuration);
    livesCount--;
    if (livesCount == 0) {
      lcd.clear();
      isGameOver = 1;
      highscoreTimePassed = millis();
      return;
    } else {
      resetLevel();
    }
    
  } else if (!changedRecently) {
    playSound(snakeMoveFrequency, snakeMoveDuration); 
  } else {
    playSound(snakeDirectionFrequency, snakeMoveDuration); 
  }

  gameConfig[snakeHead.row][snakeHead.column] = snakeLength + 1;

  for (int row = 0; row < matrixSize; row++) {
    for (int column = 0; column < matrixSize; column++) {
      if (gameConfig[row][column] > 0) {
        gameConfig[row][column]--;
      }
    }
  }
}

// Resets the level if snake was hurt
void resetLevel() {
  lc.clearDisplay(0);
  lcd.clear();
  for (int row = 0; row < matrixSize; row++) {
    for (int column = 0; column < matrixSize; column++) {
      gameConfig[row][column] = 0;
    }
  }
  snakeHead.row = 5;
  snakeHead.column = 5;
  snakeLength = 3;
  food.row = -1;
  food.column = -1;
}

// If food was eaten, recalculates the food position
void recalculateFood() {
  food.row = random(0, matrixSize);
  food.column = random(0, matrixSize);
  byte isSnake = gameConfig[food.row][food.column] > 0;
  byte isAnObstacle = hasObstacles && obstacles[food.row][food.column] == 1;
  if (isSnake || isAnObstacle) {
    recalculateFood();
  }
}

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
    
}

// Checks if one of the highscores was beaten
int isScoreBeaten() {
  for (int i = 0; i < highestScoreCount; i++) {
    if (score >= storedInMemory.highestScores[i] && score != 0) {
      return i;
    }
  }
  return -1;
}

// Thanks player for playing if the score wasn't beaten
void sayThankYou() {
  if (millis() - thankYouTimePassed >= 3000) {
    freeToReset = 1;
  }
  lcd.setCursor(0,0);
  lcd.print("Thank you       ");
  lcd.setCursor(0,1);
  lcd.print("for playing!");
}

// Displays "Youve beaten the highest score" for 3 seconds
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

// Updates the highest scores after the name was entered
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

// Function to enter the name if the highscore was beaten
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

// Displays the score after the game is over
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

// Adds difficulty bonus to the score
void addToScore() {
  score += difficultyBonus;
}


/* Menu related */

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
    lcd.print("S");
    lcd.print(gameName);
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

void displayHowToPlay() {
  lcd.setCursor(1, 0);
  lcd.print(howToPlayTxt[indexHowToPlay]);
  lcd.setCursor(1, 1);
  lcd.print(howToPlayTxt[indexHowToPlay + 1]);
  if (indexHowToPlay < howToPlayLength - 2) {
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
    } else {
      playSound(menuFrequency, shortToneDuration);
    }
  }

  if (debounceButton()) {
    lcd.clear();
    indexReset = 0;
    currentState = initialMenuDisplay;
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
    if (highscoreIndex > 0) {
      playSound(menuFrequency, shortToneDuration);
      highscoreIndex--;

    }
    lcd.clear();
  } else if (direction == down) {
    if (highscoreIndex + 1 < highestScoreCount - 1) {
      playSound(menuFrequency, shortToneDuration);
      highscoreIndex++;

    }
    lcd.clear();
  }

  if (debounceButton()) {
    lcd.clear();
    currentState = initialMenuDisplay;
  }
}

// Shows the given menu of given menuLength
void showMenu(String menu[], unsigned int menuLength) {
  int joyDirection = joyMoveDirection();
  
  if (joyDirection == up || joyDirection == down) {
    moveIndexCursor(joyDirection, menuLength);
  }

  if (debounceButton()) {
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

// Displays item from index and index + 1
void displayCurrentItems(String menuToDisplay[]) {
  lcd.setCursor(1,0);
  lcd.print(menuToDisplay[index]);
  lcd.setCursor(1, 1);
  lcd.print(menuToDisplay[index + 1]);
}

// Shows cursor in menu at column 15
void showCursor() {
  lcd.setCursor(15, cursorValue);
  if (millis() - cursorBlinkTime >= cursorBlinkInterval) {
    cursorBlinkState = !cursorBlinkState;
    cursorBlinkTime = millis();
  }
  if (cursorBlinkState) {
    lcd.print('<');
  } else {
    lcd.print(" ");
  }
}

// Moves index and cursorValue
void moveIndexCursor(int direction, unsigned int menuLength) {
  if (direction == up) {
    if (reachedTheEnd) {
      playSound(menuFrequency, shortToneDuration);
      
      cursorValue = 0;
      reachedTheEnd = 0;
    } else {
      
      index--;
      if (index < 0) {
        index = 0;
      } else {
        playSound(menuFrequency, shortToneDuration);
      }
    }
    lcd.clear();

  } else if (direction == down) {
    index++;
    if (!reachedTheEnd) {
      playSound(menuFrequency, shortToneDuration);
    }
    if (index > menuLength - 2) {
      index = menuLength - 2;
      cursorValue = 1;
      reachedTheEnd = 1;
    }
    lcd.clear();
  } 
}


/* Changing settings with EEPROM */
// change difficulty
void changeDifficulty() {

  lcd.setCursor(3, 0);
  lcd.print("Difficulty");
  lcd.setCursor(4, 1);
  
  switch (storedInMemory.difficulty) {
    case easy:
      lcd.print(" easy");
      lcd.print('>');
      break;
    case medium:
      lcd.print('<');
      lcd.print("medium");
      lcd.print('>');
      break;
    case hard:
      lcd.print('<');
      lcd.print("hard");
      lcd.print('>');
      break;
    case insane:
      lcd.print('<');
      lcd.print("insane");
      break;
  }
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    if (!(storedInMemory.difficulty - 1 < minDifficulty)) {
      playSound(menuFrequency, shortToneDuration);
      storedInMemory.difficulty--;
    }
    lcd.clear();
  } else if (joyDirection == right) {
    if (!(storedInMemory.difficulty + 1 > maxDifficulty)) {
      playSound(menuFrequency, shortToneDuration);
      storedInMemory.difficulty++;
    }
    lcd.clear();
  }
  if (debounceButton()) {
    EEPROM.put(structAddress, storedInMemory);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

// change contrast
void modifyContrast() {
  lcd.setCursor(3, 0);
  lcd.print("Contrast");
  lcd.setCursor(6, 1);
  storedInMemory.contrast > minContrast ? lcd.print('<') : lcd.print(" ");
  lcd.print(storedInMemory.contrast);
  storedInMemory.contrast < maxContrast ? lcd.print('>') : lcd.print(" ");

  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    if (!(storedInMemory.contrast - 1 < minContrast)) {
      playSound(menuFrequency, shortToneDuration);
      storedInMemory.contrast--;
    }
  } else if (joyDirection == right) {
    if (!(storedInMemory.contrast + 1 > maxContrast)) {
      playSound(menuFrequency, shortToneDuration);
      storedInMemory.contrast++;
    }
  }

  analogWrite(contrastPin, map(storedInMemory.contrast, minContrast, maxContrast, 0, 255));

  if (debounceButton()) {
    EEPROM.put(structAddress, storedInMemory);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

// change LCD brightness
void changeLCDBright() {

  lcd.setCursor(1, 0);
  lcd.print("LCD Brightness");
  lcd.setCursor(6, 1);
  storedInMemory.brightLCD > minLCDBright ? lcd.print('<') : lcd.print(" ");
  lcd.print(storedInMemory.brightLCD);
  storedInMemory.brightLCD < maxLCDBright ? lcd.print('>') : lcd.print(" ");
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    if (!(storedInMemory.brightLCD - 1 < minLCDBright)) {
      playSound(menuFrequency, shortToneDuration);
      storedInMemory.brightLCD--;
    }
  } else if (joyDirection == right) {
    if (!(storedInMemory.brightLCD + 1 > maxLCDBright)) {
      playSound(menuFrequency, shortToneDuration);
      storedInMemory.brightLCD++;
    }
  }

  analogWrite(LCDBacklightPin, map(storedInMemory.brightLCD, minLCDBright, maxLCDBright, 0, 255));

  if (debounceButton()) {
    EEPROM.put(structAddress, storedInMemory);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

// change matrix brightness
void changeMatBright() {

  lcd.setCursor(1, 0);
  lcd.print("Mat Brightness");
  lcd.setCursor(6, 1);
  storedInMemory.brightMat > minMatBright ? lcd.print('<') : lcd.print(" ");
  lcd.print(storedInMemory.brightMat);
  storedInMemory.brightMat < maxMatBright ? lcd.print('>') : lcd.print(" ");
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    if (!(storedInMemory.brightMat - 1 < minMatBright)) {
      playSound(menuFrequency, shortToneDuration);
      storedInMemory.brightMat--;
    }
  } else if (joyDirection == right) {
    if (!(storedInMemory.brightMat + 1 > maxMatBright)) {
      playSound(menuFrequency, shortToneDuration);
      storedInMemory.brightMat++;
    }
  }

  lc.setIntensity(0, map(storedInMemory.brightMat, minMatBright, maxMatBright, 0, 15));

  if (debounceButton()) {
    EEPROM.put(structAddress, storedInMemory); 
    lcd.clear();
    currentState = displaySettings;
    return;
  }
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

// To set sound ON or OFF
void modifySound() {
  lcd.setCursor(5, 0);
  lcd.print("Sound");
  lcd.setCursor(5, 1);
  
  switch (storedInMemory.isSoundOn) {
    case on:
      lcd.print(" ON");
      lcd.print('>');
      break;
    case off:
      lcd.print('<');
      lcd.print("OFF");
      break;
  }
  
  int joyDirection = joyMoveDirection();
  if (joyDirection == left) {
    if (storedInMemory.isSoundOn == off) {
      storedInMemory.isSoundOn = on;
      playSound(menuFrequency, shortToneDuration);
    }
    lcd.clear();
  } else if (joyDirection == right) {
    if (storedInMemory.isSoundOn == on) {
      storedInMemory.isSoundOn = off;
    }
    lcd.clear();
  }
  if (debounceButton()) {
    EEPROM.put(structAddress, storedInMemory);
    lcd.clear();
    currentState = displaySettings;
    return;
  }
}

void clearHighscores() {
  lcd.setCursor(0, 0);
  lcd.print("Reset all?");
  lcd.setCursor(3, 1);
  lcd.print("Yes");
  lcd.setCursor(11, 1);
  lcd.print("No");

  lcd.setCursor(2, 1);
  highscoreCursorLeft ? lcd.print('>') : lcd.print(" ");
  lcd.setCursor(10, 1);
  !highscoreCursorLeft ? lcd.print('>') : lcd.print(" ");
  
  int direction = joyMoveDirection();
  if (direction == left) {
    if (highscoreCursorLeft == 0) {
      playSound(menuFrequency, shortToneDuration);
    }
    highscoreCursorLeft = 1;
  } else if (direction == right) {
    if (highscoreCursorLeft == 1) {
      playSound(menuFrequency, shortToneDuration);
    }
    highscoreCursorLeft = 0;
  }

  if (debounceButton()) {
    if (highscoreCursorLeft) {
      resetMemory();
    } 
    lcd.clear();
    currentState = displaySettings;
  }
}


/* Utils */

// Displays matrix by array of bytes (for images)
void displayMatrixByColumn(byte charachterToDisplay[matrixSize]) {
  for (int column = 0; column < matrixSize; column++) {
    lc.setColumn(0, column, charachterToDisplay[matrixSize - column - 1]);
  }
}

// Displays the given matrix, the positions are modified due to matrix alignment on the board
void displayCurrentMatrix(byte matrixConfig[matrixSize][matrixSize]) {
  for (int row = 0; row < matrixSize; row++ ) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrixConfig[matrixSize - col - 1][matrixSize - row - 1] == 0 ? 0 : 1);
    }
  }
}

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
    playSound(clickFrequency, shortToneDuration);
    return 1;
  }
  return 0;
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

void playSound(int frequency, int duration) {
  if (storedInMemory.isSoundOn) {
    tone(buzzerPin, frequency, duration);
  } else {
    noTone(buzzerPin);
  }
}