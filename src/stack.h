#ifndef STACK_H
#define STACK_H

#include <string.h>
#include <stdint.h>

#define SIZE 16

typedef struct {
    uint16_t m[SIZE];
    uint8_t c;
} Stack;

void StackInit(Stack *s);
void push(Stack *s, uint16_t v);
uint16_t pop(Stack *s);

#endif
