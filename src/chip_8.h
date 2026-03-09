#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "stack.h"

#define WIDTH 64
#define HEIGHT 32
#define FONT_START 0x50

#define TIMER_REFRESH 60
#define DT_TIMER_REFRESH (1.0f / 60)

typedef int (*KeyFunc)();

typedef struct
{
    uint8_t v[WIDTH][HEIGHT];
    uint8_t m[4096];
    uint8_t gp[16];
    KeyFunc keyDown;
    int pc;
    uint16_t i;
    uint8_t dt;
    uint8_t st;
    Stack s;
    int _instCount;
} Cpu;

void cpuReset(Cpu *cpu);
void CpuInit(Cpu *cpu, KeyFunc keyDown);

int doBeep(Cpu *cpu);
void tick(Cpu *cpu);
void setFont(Cpu *cpu);
void load(Cpu *cpu, unsigned char insts[], int size);

uint16_t join(uint8_t b1, uint8_t b2);
uint16_t fetch(Cpu *cpu);

int decode(Cpu *cpu, uint16_t inst);

int step(Cpu *cpu);

#endif