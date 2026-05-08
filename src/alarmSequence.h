#ifndef ALARM_SEQUENCE_H
#define ALARM_SEQUENCE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define BUZZER_PIN 12

extern LiquidCrystal_I2C lcd;
extern Keypad keypad;
extern bool alarmActive;

void initAlarm();
void alarmLoop();

#endif