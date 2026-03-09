#include "stack.h"

void StackInit(Stack *s)
{
    s->c = 0;   
    memset(s->m, 0, sizeof(s->m));
}

void push(Stack *s, uint16_t value)
{
    s->m[s->c] = value;
    s->c++;
}

uint16_t pop(Stack *s)
{
    uint16_t value = s->m[s->c - 1];
    s->m[s->c - 1] = 0;
    s->c--;
    return value;
}