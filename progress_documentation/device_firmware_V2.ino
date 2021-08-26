#include <LiquidCrystal.h>
#include <Keypad.h>
#include <string.h>
#include <RTClib.h>


//GLOBAL VARIABLES

const int digitCount = 5;
const int countDownTime = 30;
char emptyHashString[digitCount + 1];
char keysPressedString[digitCount +1];
const int screenRefreshTime = 150;
char keyNeeded;
char keyPressed;
int currentKey;
int keyPressedTime;
int timeArmed;

bool primed = false;
bool armed = false;
bool reset = false;

//PINS
// SPEAKER      D8
// REDLED       D9
// GREEN LED    D10
// PRIMER       D13
const int speaker = 7;
const int redLED = 9;
const int greenLED = 10;
const int primer = 13;

const int inputPins[] = {primer};
const int outputPins[] = {speaker, redLED, greenLED};

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
// ROW1     A1
// ROW2     A2
// ROW3     A3
// COLUMN1  A0
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


//--------------------------------PERSONAL FUNCTIONS------------------------------------
int getTime() {
    int time = millis() / 1000;
    return time;
}


void beep(int beeps, int beepLowDelay = 100, int beepHighDelay = 100) {
  for (int i=0; i<beeps; i++) {
    digitalWrite(speaker, HIGH);
    delay(beepHighDelay);
    digitalWrite(speaker, LOW);
    delay(beepLowDelay);
  }
  return;
}


void createEmptyHashString() {
    char hashString[digitCount + 1];
    for (int i=0; i<digitCount; i++) {
        emptyHashString[i] = '#';
    }
    emptyHashString[digitCount + 1] = '\0';
    return;
    }


void initializePins() {
    for (int i=0; i < sizeof(outputPins)/sizeof(outputPins[0]); i++) {
        pinMode(outputPins[i], OUTPUT);
    }
    for (int i=0; i < sizeof(inputPins)/sizeof(inputPins[0]); i++) {
        pinMode(inputPins[i], INPUT);
    }
    return;
}


void mainScreen() {
    char title[] = "AIRSOC REPLICA";
    char state[] = "unprimed";
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(title);
    lcd.setCursor(0,1);
    lcd.print(state);
    return;
}


void primedScreen() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(keyNeeded);
    lcd.setCursor(0,1);
    lcd.print(keysPressedString);
    return;
}


void armedScreen(int timeRemaining) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(keyNeeded);
    lcd.setCursor(13, 0);
    lcd.print(timeRemaining);
    lcd.setCursor(0,1);
    lcd.print(keysPressedString);
    return;
}


void resetKeys() {
    memcpy(keysPressedString, emptyHashString, digitCount);
    currentKey = 0;
    return;
}


bool checkKeys(char keysNeeded[digitCount]) {
    int count = 0;
    for (int key=0; key<digitCount; key++) {
        if (keysPressedString[key] == keysNeeded[key]) {
            count++;
        }
    }
    return (count == digitCount);
}


char *generateKeys() {
  static char keysGenerated[digitCount];
  for (int i=0; i<digitCount; i++) {
    randomSeed(analogRead(0));
    char key = random(49, 52);
    keysGenerated[i] = key;
  }
  return keysGenerated;
}
//--------------------------------------------------------------------------------------


//--------------------------------------SETUP-------------------------------------------
void setup() {

    int startMillis = getTime();
    lcd.begin(16, 2);
    createEmptyHashString();
    resetKeys();
    initializePins();
    beep(3);

}
//--------------------------------------------------------------------------------------


//---------------------------------------MAIN-------------------------------------------
void loop() {
primed = digitalRead(primer);
mainScreen();

char* keysNeeded = generateKeys();

while (primed and not armed) {
    primed = digitalRead(primer);

    keyPressed = myKeypad.getKey();
    keyNeeded = keysNeeded[currentKey];

    if (keyPressed) {
        beep(1);
        keyPressedTime = getTime();

        if (keyPressed == keyNeeded) {
            keysPressedString[currentKey] = keyPressed;
            currentKey ++;
        }
        else if (keyPressed != keyNeeded) {
            resetKeys();
        }
    }

    bool continuityCondition = (getTime() - keyPressedTime) <= 2;

    if (not continuityCondition) {
        resetKeys();
    }

    bool keyCheck = checkKeys(keysNeeded);
    if (keyCheck) {
        resetKeys();
        timeArmed = getTime();
        armed = true;
    }

    primedScreen();

    delay(screenRefreshTime);
}

while (armed and primed) {
    int timeRemaining = (countDownTime - getTime() + timeArmed);
    armedScreen(timeRemaining);
    keyPressed = myKeypad.getKey();
    keyNeeded = keysNeeded[currentKey];

    if (keyPressed) {
        beep(1);
        keyPressedTime = getTime();

        if (keyPressed == keyNeeded) {
            keysPressedString[currentKey] = keyPressed;
            currentKey ++;
        }
        else if (keyPressed != keyNeeded) {
            resetKeys();
        }
    }

    bool continuityCondition = (getTime() - keyPressedTime) <= 2;

    if (not continuityCondition) {
        resetKeys();
    }

    bool keyCheck = checkKeys(keysNeeded);
    if (keyCheck) {
        lcd.clear();
        lcd.setCursor(0,0);
        char defused[] = "defused";
        lcd.print(defused);
        delay(60000);
    }
    else if (timeRemaining == 0) {
        lcd.clear();
        lcd.setCursor(0,0);
        char boom[] = "boom";
        lcd.print(boom);
        delay(60000);
    }
    delay(screenRefreshTime);
}

delay(screenRefreshTime);
}
//--------------------------------------------------------------------------------------