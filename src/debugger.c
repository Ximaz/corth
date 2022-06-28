#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ops.h"
#include "../include/stack.h"

void debug_stack(stack_t *stack, op_code_t op_code)
{
    char n[21];
    uint64 n_len = 0;
    int64 i = stack->top - 1;
    uint64 j = 0;

    if (op_code == COUNT_OPS)
        printf("\nPROGRAM BEGINS\n");
    else
        printf("\nOP CODE : %s\n", op_codes[op_code]);
    printf("|------------------------|\n");
    for (; i >= 0 ; i--) {
        sprintf(n, "%lld", stack->elements[i]);
        n_len = strlen(n);
        printf("|");
        for (; j < 12 - n_len / 2 - (stack->elements[i] < 0); j++)
            printf(" ");
        printf("%s", n);
        j = 0;
        for (; j < 12 - n_len / 2 - (n_len == 1); j++)
            printf(" ");
        printf("|\n");
        j = 0;
        if (i > 0)
            printf("|------------------------|\n");
    }
    printf("|------------------------|\n\n");
}
