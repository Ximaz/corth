#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/stack.h"

void debug_stack(stack_t *stack, inst_t *op)
{
    uint64 padding = 0;
    char n[21];
    uint64 j = 0;
    uint64 n_len = 0;
    uint64 op_arg_i = 0;
    long i = stack->top - 1;

    if (!op)
        printf("\nPROGRAM BEGINS\n");
    else {
        printf("\nOP CODE : %s\nARGC : %llu\nARGV : ", op_codes[op->op_code], op->args_len);
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
    for (; i >= 0 ; i--) {
        sprintf(n, "%lld", stack->elements[i]);
        n_len = strlen(n);
        padding = 12;
        printf("|");
        for (; j < padding - n_len / 2 - (n_len % 2 == 1) + (stack->elements[i] >= 0); j++)
            printf(" ");
        printf("%s", n);
        j = 0;
            for (; j < padding + n_len / 2 + (n_len % 2 == 1 ? 1 : -1); j++)
            printf(" ");
        printf("|\n");
        j = 0;
        if (i > 0)
            printf("|--------------------------|\n");
    }
    printf("|--------------------------|\n\n");
}
