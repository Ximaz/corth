#ifndef _CORTH_STACK
#define _CORTH_STACK
#include "types.h"

static unsigned int const corth_stack_sIZE = 4096;

corth_stack_t *new_stack(void);
void destroy_stack(corth_stack_t *self);
void push_onto_stack(corth_stack_t *self, value_t element);
value_t pop_from(corth_stack_t *self);

#endif
