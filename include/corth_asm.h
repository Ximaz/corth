#ifndef _CORTH_ASM_FUNCTIONS_H_
    #define _CORTH_ASM_FUNCTIONS_H_
    #include <stdio.h>
    #include "corth_types.h"
    #include "corth_stack.h"
    #include "corth_ops.h"

    #define STRING_CAPACITY 640000ULL
    #define MEMORY_CAPACITY 640000ULL

char *unescape(char *string);
void asm_dump(FILE *f);
void asm_header(FILE *f);
void asm_footer(FILE *f);
void inst_push(FILE *f, corth_stack_t *stack, int64 n);
void inst_string(FILE *f, corth_stack_t *stack, uint64 str_addr, op_t *op);
tok_val_t inst_pop(FILE *f, corth_stack_t *stack);
void inst_plus(FILE *f, corth_stack_t *stack);
void inst_minus(FILE *f, corth_stack_t *stack);
void inst_dump(FILE *f, corth_stack_t *stack);
void inst_dupp(FILE *f, corth_stack_t *stack);
void inst_2dupp(FILE *f, corth_stack_t *stack);
void inst_equal(FILE *f, corth_stack_t *stack);
void inst_diff(FILE *f, corth_stack_t *stack);
void inst_gt(FILE *f, corth_stack_t *stack);
void inst_lt(FILE *f, corth_stack_t *stack);
void inst_goet(FILE *f, corth_stack_t *stack);
void inst_loet(FILE *f, corth_stack_t *stack);
int inst_if(FILE *f, corth_stack_t *stack, uint64 end_addr, op_typ_t op_type);
void inst_else(FILE *f, uint64 end_addr);
void inst_while(FILE *f);
int inst_do(FILE *f, corth_stack_t *stack, uint64 end_addr);
void inst_end(FILE *f, uint64 next_addr);
void inst_mem(FILE *f, corth_stack_t *stack);
void inst_store(FILE *f, corth_stack_t *stack, char *fake_mem);
void inst_load(FILE *f, corth_stack_t *stack, char *fake_mem);
int inst_syscall(FILE *f, corth_stack_t *stack, char *fake_mem, unsigned int args_len);
void inst_shl(FILE *f, corth_stack_t *stack);
void inst_shr(FILE *f, corth_stack_t *stack);
void inst_orb(FILE *f, corth_stack_t *stack);
void inst_andb(FILE *f, corth_stack_t *stack);
void inst_swap(FILE *f, corth_stack_t *stack);
void inst_over(FILE *f, corth_stack_t *stack);

#endif