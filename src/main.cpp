#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#include "equationMaker.h"

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

byte rowPins[ROWS] = {19, 18, 5, 17}; // connect to keypad row pins
byte colPins[COLS] = {16, 4, 0, 2};      // connect to keypad col pins

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);

String currentAns;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    
    // make display:
    lcd.init();
    lcd.backlight();
    
    equation();

    lcd.print(eq);

    lcd.setCursor(0,1);
    lcd.print("= ");
}

void loop() {

// KEYPAD LOGIC
// =========================
    // key pad setup
    char key = keypad.getKey();
    int colPos = 2 + currentAns.length();
    lcd.setCursor(colPos, 1);

    // check if user input matches answer if user presses enter
    if (key == '#')
    {
        if (currentAns.toInt() == ANS)
        {
            lcd.clear();
            lcd.print("Correct!");
        }
        else
        {
            lcd.clear();
            lcd.print("Incorrect.");
            lcd.setCursor(0,1);
            lcd.print("Try Again.");
            delay(2000);
            lcd.clear();

            currentAns = "";
            lcd.print(eq);

            lcd.setCursor(0,1);
            lcd.print("= ");
        }
    }
    // backspace
    else if (key == 'A')
    {
        currentAns.remove(currentAns.length() - 1);
        lcd.setCursor(colPos-1, 1);
        lcd.print("                ");
        lcd.setCursor(colPos-1, 1);
    }
    // general key press
    else if (key)
    {
        lcd.setCursor(colPos, 1);
        lcd.print(key);
        currentAns = currentAns + key;

    }
// ==========================


}
