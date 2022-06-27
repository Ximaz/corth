#ifndef _CORTH_STACK
#define _CORTH_STACK
#include "util.h"

static unsigned int const STACK_SIZE = 4096;

typedef struct stack_s
{
    int top;
    int64 *elements;
} stack_t;

stack_t *new_stack(void);
void destroy_stack(stack_t *self);
void push_onto_stack(stack_t *self, int64 element);
int64 pop_from(stack_t *self);

#endif