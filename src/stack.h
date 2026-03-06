#ifndef STACK_H
#define STACK_H

#include <string.h>

#define SIZE 16

typedef struct {
    int m[SIZE];
    int c;
} Stack;

void StackInit(Stack *s);
void push(Stack *s, int v);
int pop(Stack *s);

#endif
