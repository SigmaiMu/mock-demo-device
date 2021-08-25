#include <LiquidCrystal.h>
#include <Keypad.h>

const byte numRows = 3;
const byte numCols = 1;

char keymap[numRows][numCols] = {
  {'1'},
  {'2'},
  {'3'}
};

byte rowPins[numRows] = {A1, A2, A3};
byte colPins[numCols]= {A0};

int buttonPin = 13;
int rstPin = 6;
int countDownStart = 30;
int countDownEnd = 0;
int startMillis = 0;
int timeArmed = 0;
int timeRemaining = 0;

bool exploded = false;

char countdown[10];

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

int getTime() {

  int time = millis() / 1000;
  return time;
  
}


void setup() {

  startMillis = getTime();

  pinMode(buttonPin, INPUT);
  pinMode(rstPin, INPUT);

  lcd.begin(16, 2);
  lcd.print("AIRSOC REPLICA");

}


void loop() {

  bool armed = digitalRead(buttonPin);
  bool reset = digitalRead(rstPin);
  bool cleared = false;

  timeArmed = getTime();

  while (armed and not exploded) {

    if (not cleared) {
      lcd.clear();
      cleared = true;
    }

    armed = digitalRead(buttonPin);

    if (not exploded) {
      armed = digitalRead(buttonPin);
      timeRemaining = (countDownStart - (getTime() - timeArmed));
      sprintf(countdown, "%d", timeRemaining);
      lcd.setCursor(0, 1);
      lcd.print(countdown);
      if (timeRemaining == countDownEnd) {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("BOOM");
        delay(2000);
        exploded = true;
      }
    }
  }

  if (reset) {
    lcd.clear();
    exploded = false;
  }

  lcd.setCursor(0, 0);
  lcd.print("AIRSOC REPLICA");
  lcd.setCursor(0, 1);
  lcd.print("UNARMED");
  char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY) {
    lcd.clear();
    lcd.print(keypressed);
    delay(1000);
    lcd.clear();
  }

}
