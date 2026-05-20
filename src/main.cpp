#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <RTClib.h>
#include <FastLED.h>

#include "alarmSequence.h"
#include "setTime.h"

// make rtc:
RTC_DS1307 rtc;

// make keypad:
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

// make lcd:
LiquidCrystal_I2C lcd(0x27, 16, 2);

// set up led strip:
#define LED_PIN 15
#define NUM_LEDS 8 // change this to the number of LEDs on the strip i buy
CRGB leds[NUM_LEDS];


bool alarmActive = false; 
bool settingTime = false;
bool settingAlarm = false;

String currentTime;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    
    // make display:
    lcd.init();
    lcd.backlight();

    // initalize time:
    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // initialize led:
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
    DateTime now = rtc.now();
    char key = keypad.getKey();
    
    // Display current time onto lcd:
    // =================================================
    lcd.setCursor(0, 0);
    if (now.hour() < 10) 
    { 
        lcd.print("0");
        currentTime += "0";
    }
    lcd.print(now.hour());
    currentTime += String(now.hour()) + ":";
    lcd.print(":");
    if (now.minute() < 10) 
    { 
        lcd.print("0");
        currentTime += "0"; 
    }
    lcd.print(now.minute());
    currentTime += String(now.minute()) + ":";
    lcd.print(":");
    if (now.second() < 10) 
    { 
        lcd.print("0");
        currentTime += "0"; 
    }   
    lcd.print(now.second());
    currentTime += String(now.second());    
    // =================================================

    // set alarm:
    if (key == 'A')
    {
        if (!alarmActive)
        {
            settingAlarm = true;
            settingHour = true;
            alarmActive = true;
            makeTime("Hour", 23);
            while (settingAlarm)
            {
                makeTimeLoop();
            }
            newHour = "";
            newMinute = "";
            
        }
        else 
        {
            alarmActive = false;
            alarmTime = "";
            lcd.clear();
            lcd.print("Alarm Cancelled");
            delay(1000);
            lcd.clear();
            FastLED.setBrightness(0);
            FastLED.show();
        }
    }

    // check alarm:
    if (key == 'B')
    {
        lcd.clear();
        if (alarmActive)
        {
            lcd.print("Alarm Set For:");
            lcd.setCursor(0, 1);
            lcd.print(alarmTime);
            delay(2000);
            lcd.clear();
        }
        else
        {
            lcd.print("No Alarm Set.");
            delay(2000);
            lcd.clear();
        }

    }

    // reset time:
    if (key == 'D')
    {
        settingTime = true;
        settingHour = true;
        makeTime("Hour", 23);
        
        while (settingTime)
        {
            makeTimeLoop();
        }
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), newHour.toInt(), newMinute.toInt(), 1));
        newHour = "";
        newMinute = "";
    }

    
    // alarm sequence:
    if (alarmTime == currentTime) // and currentTime == alarmTime
    {
        lcd.clear();
        initAlarm();
        while (alarmActive)
        {
            alarmLoop();
        }
        alarmTime = "";
    }
    int secondsToAlarm = secToAlarm();
    if (secondsToAlarm <= 900 && alarmActive)
    {
        // Start at 100 brightness at 15 minutes and gradually increase to 255 brightness at alarm time (per second)
        fill_solid(leds, NUM_LEDS, CRGB(255, 147, 41)); // warm white color
        FastLED.setBrightness(map(secondsToAlarm, 900, 0, 100, 255));
        FastLED.show();
        
    }
    
    // reset current time string:
    currentTime = "";
}
