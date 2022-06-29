#ifndef _CORTH_ASM_FUNCTIONS
#define _CORTH_ASM_FUNCTIONS
#include "types.h"
#include "stack.h"
#include <stdio.h>

void asm_dump(FILE *f);
void asm_header(FILE *f);
void inst_push(FILE *f, stack_t *stack, int64 n);
void inst_plus(FILE *f, stack_t *stack);
void inst_minus(FILE *f, stack_t *stack);
void inst_dump(FILE *f, stack_t *stack);
void inst_dup(FILE *f, stack_t *stack);
void inst_equal(FILE *f, stack_t *stack);
void inst_gt(FILE *f, stack_t *stack);
void inst_lt(FILE *f, stack_t *stack);
void inst_goet(FILE *f, stack_t *stack);
void inst_loet(FILE *f, stack_t *stack);
int inst_if(FILE *f, stack_t *stack, uint64 end_addr, int is_else);
void inst_else(FILE *f, uint64 end_addr);
void inst_while(FILE *f);
int inst_do(FILE *f, stack_t *stack, uint64 end_addr);
void inst_end(FILE *f, uint64 end_addr, uint64 next_addr);
int inst_halt(FILE *f, stack_t *stack);
#endif
