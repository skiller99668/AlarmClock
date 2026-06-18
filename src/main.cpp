#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <FastLED.h>

#include "alarmSequence.h"
#include "setTime.h"

// make rtc:
ThreeWire myWire(27, 14, 26); // IO, SCLK, CE
RtcDS1302<ThreeWire> rtc(myWire);

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
    Serial.println("Starting up...");
    
    // make display:
    lcd.init();
    lcd.backlight();

    // custom √ glyph (char code 1) used by toDisplay() in genEquation.cpp
    byte sqrtGlyph[8] = {
        0b00011,
        0b00010,
        0b00010,
        0b00010,
        0b10010,
        0b01010,
        0b00110,
        0b00010
    };
    lcd.createChar(1, sqrtGlyph);

    // initalize time:
    rtc.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    rtc.SetDateTime(compiled);

    // initialize led:
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
    RtcDateTime now = rtc.GetDateTime();
    char key = keypad.getKey();
    
    // Display current time onto lcd:
    // =================================================
    lcd.setCursor(0, 0);
    if (now.Hour() < 10) 
    { 
        lcd.print("0");
        currentTime += "0";
    }
    lcd.print(now.Hour());
    currentTime += String(now.Hour()) + ":";
    lcd.print(":");
    if (now.Minute() < 10) 
    { 
        lcd.print("0");
        currentTime += "0"; 
    }
    lcd.print(now.Minute());
    currentTime += String(now.Minute()) + ":";
    lcd.print(":");
    if (now.Second() < 10) 
    { 
        lcd.print("0");
        currentTime += "0"; 
    }   
    lcd.print(now.Second());
    currentTime += String(now.Second());    
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
            FastLED.clear();
            FastLED.show();
            
        }
        else 
        {
            alarmActive = false;
            alarmTime = "";
            lcd.clear();
            lcd.print("Alarm Cancelled");
            delay(1000);
            lcd.clear();
            FastLED.clear();
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
        rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), newHour.toInt(), newMinute.toInt(), 1));
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
        FastLED.clear();
        FastLED.show();
    }
    int secondsToAlarm = secToAlarm();
    if (secondsToAlarm <= 900 && alarmActive)
    {
        // Start at 0 brightness at 15 minutes and gradually increase to 255 brightness at alarm time (per second)
        fill_solid(leds, NUM_LEDS, CRGB(255, 147, 41)); // warm white color
        FastLED.setBrightness(map(secondsToAlarm, 900, 0, 0, 255));
        FastLED.show();
        
    }
    
    // reset current time string:
    currentTime = "";
}
