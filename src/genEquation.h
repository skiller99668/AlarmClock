#ifndef GENEQUATION_H
#define GENEQUATION_H

#include <Arduino.h>

extern String eq;
extern int ANS;

void adjustDifficulty(float elapsed);
void getEquation();

#endif