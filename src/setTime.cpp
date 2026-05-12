#include <Arduino.h>

#include "setTime.h"

bool settingHour;
int colPos;
String alarmTime;
String newHour;
String newMinute;

void makeTime(String timeType, int range)
{
    char key = keypad.getKey();
    

    lcd.clear();
    lcd.print("Set Time:");
    lcd.setCursor(0, 1);
    lcd.print(timeType + " (0-" + range + "): ");
    lcd.setCursor(13, 1);
}

void makeTimeLoop()
{
    char key = keypad.getKey();
    // enter key:
    if (key == '#')
    {
        setTime();
    }
    // general key press:
    else if (key && isDigit(key))
    {
        lcd.print(key);
        if (settingHour)
        {
            newHour += key;
            // Serial.println("Hour: " + newHour);
            // Serial.println("Minute: " + newMinute);
        }
        else
        {
            newMinute += key;
            // Serial.println("Hour: " + newHour);
            // Serial.println("Minute: " + newMinute);
        }
    }
    // backspace key:
    else if (key == 'A')
    {
        if (settingHour && newHour.length() > 0)
        {
            newHour.remove(newHour.length() - 1);
            colPos = 13 + newHour.length();
            lcd.setCursor(colPos, 1);
            lcd.print("   ");
            lcd.setCursor(colPos, 1);
            lcd.setCursor(colPos, 1);
            // Serial.println("Hour: " + newHour);
            // Serial.println("Minute: " + newMinute);
        }
        else if (!settingHour && newMinute.length() > 0)
        {
            newMinute.remove(newMinute.length() - 1);
            colPos = 13 + newMinute.length();
            lcd.setCursor(colPos, 1);
            lcd.print("   ");
            lcd.setCursor(colPos, 1);
            lcd.setCursor(colPos, 1);
            // Serial.println("Hour: " + newHour);
            // Serial.println("Minute: " + newMinute);
        }
    }
    // cancel key:
    else if(key == 'D')
    {
        newHour = "";
        newMinute = "";
        if (settingAlarm)
        {
            alarmTime = "";
        }
        lcd.clear();
        lcd.print("Cancelled.");
        delay(1500);
        lcd.clear();
        settingTime = false;
        settingAlarm = false;
    }
}

void setTime()
{
    if (settingHour)
    {
        if (newHour.length() <= 0 || newHour.toInt() >= 24)
        {
            lcd.clear();
            lcd.print("Invalid Hour.");
            delay(1000);
            newHour = "";
            makeTime("Hour", 23);
        }
        else 
        {
            if (newHour.length() == 1)
            {
            newHour = "0" + newHour;
            }
            makeTime("Mins", 59);
            settingHour = false;
            if (settingAlarm)
            {
                alarmTime += newHour + ":";
            }
        }
        
    }
    else
    {
        if (newMinute.length() <= 0 || newMinute.toInt() >= 60)
        {
            lcd.clear();
            lcd.print("Invalid Minute.");
            delay(1000);
            newMinute = "";
            makeTime("Mins", 59);
        }
        else
        {
            if (newMinute.length() == 1)
            {
            newMinute = "0" + newMinute;
            }
            lcd.clear();
            if (settingAlarm)
            {
                alarmTime += newMinute + ":00";
                lcd.print("Alarm Set For:");
                // Serial.println("Alarm Time: " + alarmTime);
                lcd.setCursor(0, 1);
                lcd.print(newHour + ":" + newMinute);
            }
            else if(settingTime)
            {
                lcd.print("Time Set For:");
                lcd.setCursor(0, 1);
                lcd.print(newHour + ":" + newMinute);
            }
            delay(1000);
            lcd.clear();
            settingAlarm = false;
            settingTime = false;
        }
    }
    
}