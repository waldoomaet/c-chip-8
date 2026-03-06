#include "stack.h"

void StackInit(Stack *s)
{
    s->c = 0;
    memset(&s->m, 0, SIZE);
}

void push(Stack *s, int value)
{
    s->m[s->c] = value;
    s->c++;
}

int pop(Stack *s)
{
    s->c--;
    return s->m[s->c++];
}