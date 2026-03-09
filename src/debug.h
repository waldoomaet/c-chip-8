#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include <math.h>
#include "chip_8.h"
#include "../external/raylib/include/raylib.h"

extern int regsArea;
extern int memArea;
extern int scroll;

const char *instString(uint16_t inst);
void drawDebug(Cpu *cpu);
int debugControls(Cpu *cpu, int cpuSteps, unsigned char *insts, int size);

#endif