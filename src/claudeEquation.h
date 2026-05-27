#ifndef CLAUDE_EQUATION_H
#define CLAUDE_EQUATION_H

#include <Arduino.h>

extern String eq;
extern int ANS;

void adjustDifficulty(float elapsed);
void getEquation();

#endif