#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define WIDTH 64
#define HEIGHT 32
#define FONT_START 0x50

typedef int (*KeyDownFunc)();

typedef struct
{
    unsigned char v[WIDTH][HEIGHT];
    unsigned char m[4096];
    unsigned char gp[16];
    KeyDownFunc keyDown;
    int pc;
    int i;
    int dt;
    int st;
    Stack s;
} Cpu;

void CpuInit(Cpu *cpu, KeyDownFunc keyDown);

void tick(Cpu *cpu);

void setFont(Cpu *cpu);

void load(Cpu *cpu, unsigned char insts[], int size);

int fetch(Cpu *cpu);

int decode(Cpu *cpu, int inst);

int step(Cpu *cpu);

#endif