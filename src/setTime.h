#ifndef SET_TIME_H
#define SET_TIME_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

extern LiquidCrystal_I2C lcd;
extern Keypad keypad;

extern String alarmTime;
extern String newHour;
extern String newMinute;
extern String currentTime;
extern bool settingHour;
extern bool settingTime;
extern bool settingAlarm;
extern bool alarmActive;

void makeTime(String timeType, int range);
void makeTimeLoop();
void setTime();
int secToAlarm();

#endif 