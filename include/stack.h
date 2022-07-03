#ifndef _CORTH_STACK
#define _CORTH_STACK
#include "types.h"

static unsigned int const STACK_SIZE = 4096;

typedef struct stack_s
{
    unsigned int top;
    inst_arg_t *elements;
} stack_t;

stack_t *new_stack(void);
void destroy_stack(stack_t *self);
void push_onto_stack(stack_t *self, inst_arg_t element);
inst_arg_t pop_from(stack_t *self);
#endif
