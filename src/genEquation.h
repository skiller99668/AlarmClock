#ifndef GENEQUATION_H
#define GENEQUATION_H

#include <Arduino.h>

extern String eq;
extern int ANS;
extern int difficulty;

void adjustDifficulty(float elapsed);
void getEquation();

#endif