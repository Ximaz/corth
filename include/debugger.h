#ifndef _CORTH_DEBUGGER
#define _CORTH_DEBUGGER
#include "ops.h"
#include "stack.h"

void debug_stack(stack_t *stack, inst_t *op);
void debug_memory(unsigned char *fake_mem, uint64 limit);

#endif
