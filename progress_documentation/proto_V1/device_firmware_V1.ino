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
int screenRefreshDelay = 100;
const int numberOfKeys = 4;

bool exploded = false;
bool defused = false;

char countdown[5];

//LIQUID CRYSTAL
// VSS  GND
// VDD  +5V
// V0   varResistor
// RS   D12
// RW   GND
// E    D11
// D0   None
// D1   None
// D2   None
// D3   None
// D4   D5
// D5   D4
// D6   D3
// D7   D2
// A    None
// K    GND
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

//LEDs
int greenLED = 10;
int redLED = 9;
//----

//SPEAKER
int speaker = 8;
//-------


//--------------------------------PERSONAL FUNCTIONS--------------------------------------

int getTime() {

  int time = millis() / 1000;
  return time;
  
}


void beep(int beeps, int beepLowDelay = 500, int beepHighDelay = 500) {
  
  for (int i=0; i<beeps; i++) {
    digitalWrite(speaker, HIGH);
    delay(beepHighDelay);
    digitalWrite(speaker, LOW);
    delay(beepLowDelay);
  }
  return;

}


void armedScreen(char currentKey, String keysPressed, int timeRemaining) {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(timeRemaining);
  lcd.setCursor(15, 0);
  lcd.print(currentKey);
  lcd.setCursor(0, 1);
  lcd.print(keysPressed);
  delay(screenRefreshDelay);
  return;

}


char* generateKeys() {
  
  static char keysGenerated[numberOfKeys];
  for (int i=0; i<numberOfKeys; i++) {
    randomSeed(analogRead(0));
    char key = random(49, 52);
    keysGenerated[i] = key;
  }
  return keysGenerated;

}
//--------------------------------------------------------------------------------------


//--------------------------------------SETUP-------------------------------------------
void setup() {

  startMillis = getTime();

  pinMode(buttonPin, INPUT);
  pinMode(rstPin, INPUT);

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(speaker, OUTPUT);

  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED,  LOW);
  digitalWrite(speaker, LOW);

  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.print("AIRSOC REPLICA");

  char emptyKeys[numberOfKeys];
  char keysPressed[numberOfKeys];

  for (int i=0; i<numberOfKeys; i++) {
    emptyKeys[i] = '#';
    keysPressed[i] = '#';
  }

}
//--------------------------------------------------------------------------------------


//---------------------------------------MAIN-------------------------------------------
void loop() {

  bool armed = digitalRead(buttonPin);
  bool reset = digitalRead(rstPin);
  bool cleared = false;

  char keyNeeded;
  int currentDigit = 0;

  timeArmed = getTime();

  char* keysNeeded = generateKeys();

  digitalWrite(redLED, LOW);

  while (armed and not exploded and not defused) {

    armed = digitalRead(buttonPin);

    if (not cleared) {
      lcd.clear();
      digitalWrite(redLED, HIGH);
      cleared = true;
    }

    if (not exploded and not defused) {
      armed = digitalRead(buttonPin);
      timeRemaining = (countDownStart - (getTime() - timeArmed));
      //beep(1, 1000);
      char keyPressed = myKeypad.getKey();
      keyNeeded = keysNeeded[currentDigit];

      if (keyPressed) {

        keyPressedTime = getTime();

        if (keyPressed == keyNeeded) {
          keysPressed[currentDigit] = keyPressed;
          currentDigit ++;
        }
        else if (keyPressed != keyNeeded) {
          memcpy(keysPressed, emptyKeys, numberOfKeys);
          currentDigit = 0;
        }
      }

      bool continuityCondition = (getTime() - keyPressedTime) <= 2;

      if (not continuityCondition) {
        memcpy(keysPressed, emptyKeys, numberOfKeys);
        currentDigit = 0;
      }

      armedScreen(keyNeeded, keysPressed, timeRemaining);

      int i;
      int count = 0;

      for (i=0; i<numberOfKeys; i++) {
        if (keysPressed[i] == keysNeeded[i]) {
          count ++;
        }
      }

      if (count == numberOfKeys) {
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("DEFUSED");
        beep(3, 100, 100);
        delay(screenRefreshDelay);
        defused = true;
      }

      if (timeRemaining == countDownEnd) {
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.print("BOOM");
        beep(10, 100, 400);
        delay(screenRefreshDelay);
        exploded = true;
      }
    }

  }

  if (reset) {
    lcd.clear();
    exploded = false;
    defused = false;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AIRSOC REPLICA");
  lcd.setCursor(0, 1);
  lcd.print("UNARMED");
  delay(screenRefreshDelay);

}