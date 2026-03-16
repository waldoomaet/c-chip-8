#ifndef IO_H
#define IO_H


#define SCALE 13

#include <raylib.h>
#include <stdint.h>
#include <math.h>
#include "debug.h"

int isChip8KeyDown();
void drawBuffer(Color background, Color pixel, uint8_t buffer[][32]);
Wave GenerateBeepWave(float frequency, float duration, int sampleRate);

#endif