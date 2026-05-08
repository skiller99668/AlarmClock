#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#include "alarmSequence.h"
// #include "equationMaker.h"

//#define BUZZER_PIN 12

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

bool alarmActive = true; // should be false but testing rn

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(BUZZER_PIN, OUTPUT);
    
    // make display:
    lcd.init();
    lcd.backlight();
    
    // equation();

    // lcd.print(eq);

    // lcd.setCursor(0,1);
    // lcd.print("= ");
}

void loop() {

    // if time for alarm:
    // if current time == alarm time
    // alarmActive = true;
//  =========================================================================
// if alarm time is right now:

// move equation stuff to here

// loop through this:
    if (alarmActive)
    {
        initAlarm();
        while (alarmActive)
        {
            alarmLoop();
        }
    }
}

// next steps:
// 1. get the current time system working on the lcd
// 2. get the set alarm system working on the lcd 
// 3. if current time == alarm time, then alarmActive = true
// then do the led stuff and then done!!!