#include <LiquidCrystal.h>
#include <Keypad.h>
#include <string.h>



//GLOBAL VARIABLES
int buttonPin = 13;
int rstPin = 6;
int countDownStart = 30;
int countDownEnd = 0;
int startMillis = 0;
int timeArmed = 0;
int timeRemaining = 0;
int keyPressedTime = 0;

bool exploded = false;
bool defused = false;

char countdown[10];

//LIQUID CRYSTAL
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//-------------

//KEYPAD
const byte numRows = 3;
const byte numCols = 1;
char keymap[numRows][numCols] = {
  {'1'},
  {'2'},
  {'3'}
};
byte rowPins[numRows] = {A1, A2, A3};
byte colPins[numCols]= {A0};
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
//-----

void setup() {

  startMillis = getTime();

  pinMode(buttonPin, INPUT);
  pinMode(rstPin, INPUT);

  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.print("AIRSOC REPLICA");

}


int getTime() {

  int time = millis() / 1000;
  return time;
  
}


void armedScreen(char currentKey, String keysPressed, int timeRemaining) {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(timeRemaining);
  lcd.setCursor(15, 0);
  lcd.print(currentKey);
  lcd.setCursor(0, 1);
  lcd.print(keysPressed);
  delay(100);
  return;

}


char* generateKeys() {
  
  static char keysGenerated[8];
  for (int i=0;i<=7;i++) {
    randomSeed(analogRead(0));
    char key = random(49, 52);
    keysGenerated[i] = key;
  }
  return keysGenerated;

}


void loop() {

  bool armed = digitalRead(buttonPin);
  bool reset = digitalRead(rstPin);
  bool cleared = false;

  timeArmed = getTime();
  char keysPressed[] = "00000000";
  char* keysNeeded = generateKeys();
  char keyNeeded;
  int currentDigit = 0;

  while (armed and not exploded and not defused) {
    armed = digitalRead(buttonPin);

    if (not cleared) {
      lcd.clear();
      cleared = true;
    }

    if (not exploded and not defused) {
      armed = digitalRead(buttonPin);
      timeRemaining = (countDownStart - (getTime() - timeArmed));
      
      char keyPressed = myKeypad.getKey();
      keyNeeded = keysNeeded[currentDigit];

      if (keyPressed) {

        keyPressedTime = getTime();

        if (keyPressed == keyNeeded) {
          keysPressed[currentDigit] = keyPressed;
          currentDigit ++;
        }
        else if ((keyPressed != keyNeeded)) {
          memcpy(keysPressed, "00000000", 8);
          currentDigit = 0;
        }
      }

      bool continuityCondition = (getTime() - keyPressedTime) <= 1;

      if (not continuityCondition) {
        memcpy(keysPressed, "00000000", 8);
        currentDigit = 0;
      }

      armedScreen(keyNeeded, keysPressed, timeRemaining);

      int i;
      int count = 0;

      for (i=0; i<=7; i++) {
        if (keysPressed[i] == keysNeeded[i]) {
          count ++;
        }
      }

      if (count == 8) {
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("DEFUSED");
        delay(2000);
        defused = true;
      }

      if (timeRemaining == countDownEnd) {
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.print("BOOM");
        delay(2000);
        exploded = true;
      }
    }

  }

  if (reset) {
    lcd.clear();
    exploded = false;
    defused = false;
  }

  lcd.setCursor(0, 0);
  lcd.print("AIRSOC REPLICA");
  lcd.setCursor(0, 1);
  lcd.print("UNARMED");

}
