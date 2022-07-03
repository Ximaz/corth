#ifndef _CORTH_DEBUGGER
#define _CORTH_DEBUGGER
#include "ops.h"
#include "stack.h"
#include "program.h"

void debug_stack(stack_t *stack, inst_t *op);
void debug_memory(unsigned char *fake_mem, uint64 limit);
void debug_program(program_t *self);
void debug_jump(program_t *self, inst_t *op, uint64 op_addr, uint64 jmp_addr);
void debug_binding(inst_t *src, uint64 srci, char const *dest, uint64 desti);
#endif
