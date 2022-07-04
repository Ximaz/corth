#ifndef _CORTH_STACK
#define _CORTH_STACK
#include "types.h"

static unsigned int const STACK_SIZE = 4096;

stack_t *new_stack(void);
void destroy_stack(stack_t *self);
void push_onto_stack(stack_t *self, value_t element);
value_t pop_from(stack_t *self);

#endif
