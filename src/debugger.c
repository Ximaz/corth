#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/stack.h"
#include "../include/asm_functions.h"

void debug_stack(stack_t *stack, inst_t *op)
{
    uint64 padding = 0;
    char n[21];
    uint64 j = 0;
    uint64 n_len = 0;
    uint64 op_arg_i = 0;
    uint64 i = stack->top;

    if (!op)
        printf("\nPROGRAM BEGINS\n");
    else {
        printf("\nOP CODE : %s\nARGC : %llu\nARGV : ", OP_CODES[op->op_code], op->args_len);
        if (op->args_len == 0)
            printf("No argument found.");
        else {
            for (; op_arg_i < op->args_len; op_arg_i++) {
                printf("%lld", op->args[op_arg_i]);
                if (op_arg_i < op->args_len - 1)
                    printf(" ");
            }
        }
        printf("\n");
    }
    printf("|--------------------------|\n");
    for (; i > 0 ; i--) {
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
        if (i - 1 > 0)
            printf("|--------------------------|\n");
    }
    printf("|--------------------------|\n\n");
}

void debug_memory(unsigned char *fake_memory, uint64 limit)
{
    uint64 i = 0;
    uint64 range = limit <= MEMORY_CAPACITY ? limit : MEMORY_CAPACITY;

    printf("CURRENT MEMORY : [");
    for (; i < range; i++) {
        printf("%u", fake_memory[i]);
        if (i < range - 1)
            printf(", ");
    }
    printf("]\n");
}
