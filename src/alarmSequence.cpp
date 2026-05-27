#include "alarmSequence.h"
//#include "equationMaker.h"
#include "claudeEquation.h"

#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

HardwareSerial mySerial(1);
DFRobotDFPlayerMini player;

String currentAns;
bool SOLVED = false;
unsigned long startTime = millis();

void initAlarm()
{
    // make equation and display it:
    //equation();
    // change to getEquation() to use claude api instead of random generation
    getEquation();
    pinMode(BUZZER_PIN, OUTPUT);

    lcd.print(eq);

    lcd.setCursor(0,1);
    lcd.print("= ");

    tone(BUZZER_PIN, 1000); // play buzzer

    // setup for DFPlayer (to replace buzzer with music)
    //mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX, TX
    //player.begin(mySerial);
    //player.volume(10); // set volume to 10 (0-30)
    //player.loop(1); // play the first track on the SD card
}

void alarmLoop() 
{
    // key pad setup
    char key = keypad.getKey();
    int colPos = 2 + currentAns.length();
    lcd.setCursor(colPos, 1);

    // check if user input matches answer if user presses enter
    if (key == '#' && !SOLVED)
    {
        if (currentAns.toInt() == ANS)
        {
            lcd.clear();
            lcd.print("Correct!");
            SOLVED = true;
            unsigned long elapsed = (millis() - startTime)/1000;
            adjustDifficulty(elapsed);
            // Serial.println("Solved.");
        }
        else
        {
            lcd.clear();
            lcd.print("Incorrect.");
            lcd.setCursor(0,1);
            lcd.print("Try Again.");
            delay(1000);
            lcd.clear();

            currentAns = "";
            lcd.print(eq);

            lcd.setCursor(0,1);
            lcd.print("= ");
        }
    }
    // backspace
    else if (key == 'A' && !SOLVED)
    {
        currentAns.remove(currentAns.length() - 1);
        lcd.setCursor(colPos-1, 1);
        lcd.print("                ");
        lcd.setCursor(colPos-1, 1);
    }
    else if (key == '*') // maybe move to main function so that the time already starts displaying
    {
        if (SOLVED)
        {
            noTone(BUZZER_PIN);
            // will be:
            // player.stop();

            alarmActive = false;
            SOLVED = false;
            currentAns = "";
            // Serial.println("Alarm turned off.");
            lcd.clear();
            delay(200);
        }
    }
    // general key press
    else if (key && key != 'B' && key != 'C' && key != 'D' && !SOLVED)
    {
        lcd.setCursor(colPos, 1);
        lcd.print(key);
        currentAns = currentAns + key;
    }
}