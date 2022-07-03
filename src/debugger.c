#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/stack.h"
#include "../include/program.h"
#include "../include/asm_functions.h"

void debug_stack(stack_t *stack, inst_t *op)
{
    uint64 padding = 0;
    char n[21];
    uint64 j = 0;
    uint64 n_len = 0;
    uint64 op_arg_i = 0;
    int64 i = stack->top;

    if (!op)
        printf("\nPROGRAM BEGINS\n");
    else {
        printf("\nOP CODE : %s\nARGC : %llu\nARGV : ", OP_CODES[op->op_code], op->args_len);
        if (op->args_len == 0)
            printf("No argument found.");
        else {
            printf("%lld (%s)", op->args[op_arg_i], op->op_code == OP_PUSH ? "number" : "address");
        }
        printf("\n");
    }
    printf("|--------------------------|\n");
    for (--i; i >= 0 ; i--) {
        sprintf(n, "%lld", stack->elements[i]);
        n_len = strlen(n);
        padding = 12;
        printf("|");
        for (; j < padding - n_len / 2 - (n_len % 2 == 1) + (stack->elements[i] >= 0); j++)
            printf(" ");
        printf("%s", n);
        j = 0;
        for (; j < padding -  n_len / 2 + 1; j++)
            printf(" ");
        printf("|\n");
        j = 0;
        if (i > 0)
            printf("|--------------------------|\n");
    }
    printf("|--------------------------|\n\n");
}

void debug_memory(unsigned char *fake_mem, uint64 limit)
{
    uint64 i = 0;
    uint64 range = limit <= MEMORY_CAPACITY ? limit : MEMORY_CAPACITY;

    printf("CURRENT MEMORY : [");
    for (; i < range; i++) {
        printf("%u", fake_mem[i]);
        if (i < range - 1)
            printf(", ");
    }
    printf("]\n");
}

void debug_program(program_t *self)
{
    uint64 i = 0;
    uint64 j = 0;
    inst_t *op = 0;

    for (; i < self->instructions_len; i++) {
        op = self->instructions[i];
        printf("%llu: %s", i, OP_CODES[op->op_code]);
        if (op->args_len > 0) {
            printf(": ");
            for (; j < op->args_len; j++) {
                printf("%lld", op->args[j]);
                if (j < op->args_len - 1)
                    printf(", ");
            }
            j = 0;
        }
        if (i < self->instructions_len - 1)
            printf(",");
        printf("\n");
    }
}

void debug_jump(program_t *self, inst_t *op, uint64 op_addr, uint64 jmp_addr)
{
    printf("JUMP IS TAKEN : from %s (at %lld) to %s (at %lld)\n", OP_CODES[op->op_code], op_addr, OP_CODES[self->instructions[jmp_addr]->op_code], jmp_addr);
}

void debug_binding(inst_t *src, uint64 srci, char const *dest, uint64 desti)
{
    char const *binding_str = "BINDING : %s (at %lld) -> %s (at %llu)\n";
    printf(binding_str, OP_CODES[src->op_code], srci, dest, desti);
}
