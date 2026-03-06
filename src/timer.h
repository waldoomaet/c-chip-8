#include "chip_8.h"

typedef void(*TickFunc)(Cpu *);
typedef struct {
    Cpu *cpu;
    TickFunc tick;
} Args;

void *threadTick(void *arg);
void startTimerThread(Args *args);