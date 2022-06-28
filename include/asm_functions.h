#ifndef _CORTH_ASM_FUNCTIONS
#define _CORTH_ASM_FUNCTIONS
#include "util.h"
#include "stack.h"
#include <stdio.h>

void asm_dump(FILE *f);
void asm_header(FILE *f);
void inst_dump(FILE *f, stack_t *stack);
void inst_push(FILE *f, stack_t *stack, int64 n);
void inst_plus(FILE *f, stack_t *stack);
void inst_minus(FILE *f, stack_t *stack);
void inst_equal(FILE *f, stack_t *stack);
int inst_halt(FILE *f, stack_t *stack);
int inst_if(FILE *f, stack_t *stack);

#endif
