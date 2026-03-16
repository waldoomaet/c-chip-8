#ifndef ARGS_PARSE_H
#define ARGS_PARSE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    char *romPath;
    int debug;
    int cpuSteps;
} Args;

int parseArgs(int argc, char **argv, Args *args);
#endif