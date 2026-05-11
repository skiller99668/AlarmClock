#include <Arduino.h>

#include "alarmSequence.h"
#include "equationMaker.h"


String currentAns;
bool SOLVED = false;

void initAlarm()
{
    // make display:
    equation();

    lcd.print(eq);

    lcd.setCursor(0,1);
    lcd.print("= ");

    tone(BUZZER_PIN, 1000); // play buzzer

}

void alarmLoop() 
{
    // KEYPAD LOGIC
// =========================
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
            // Serial.println("Solved.");
            delay(2000);
            lcd.clear();

            // display current time
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
    else if (key == 'A' && !SOLVED)
    {
        currentAns.remove(currentAns.length() - 1);
        lcd.setCursor(colPos-1, 1);
        lcd.print("                ");
        lcd.setCursor(colPos-1, 1);
    }
    else if (key == '*')
    {
        if (SOLVED)
        {
            noTone(BUZZER_PIN);
            alarmActive = false;
            // Serial.println("Alarm turned off.");
        }
    }
    // general key press
    else if (key && key != 'B' && key != 'C' && key != 'D' && !SOLVED)
    {
        lcd.setCursor(colPos, 1);
        lcd.print(key);
        currentAns = currentAns + key;
    }
// ==========================
}