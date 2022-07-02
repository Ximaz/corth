#ifndef _CORTH_DEBUGGER
#define _CORTH_DEBUGGER
#include "ops.h"
#include "stack.h"

typedef struct debugger_s
{
    int enabled;
    int mem_lim;
    int debug_stack;
    int debug_memory;
} debugger_t;

void debug_stack(stack_t *stack, inst_t *op);
void debug_memory(unsigned char *fake_mem, uint64 limit);

#endif
