#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include <math.h>
#include <raylib.h>
#include "chip_8.h"

#define REGS_ARES 300
#define MEM_AREA 350

extern int regsArea;
extern int memArea;
extern int scroll;

void enableDebug();
void disableDebug();
const char *instString(uint16_t inst);
void drawDebug(Cpu *cpu);
int debugControls(Cpu *cpu, int cpuSteps, unsigned char *insts, int size);

#endif