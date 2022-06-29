#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ops.h"
#include "../include/util.h"
#include "../include/stack.h"

void debug_stack(stack_t *stack, int64 *op)
{
    char n[21];
    uint64 j = 0;
    uint64 n_len = 0;
    int64 op_arg_i = 0;
    int64 i = stack->top - 1;

    if (!op)
        printf("\nPROGRAM BEGINS\n");
    else {
        printf("\nOP CODE : %s\nARGC : %lld\nARGV : ", op_codes[op[0]], op[1]);
        if (op[1] == 0)
            printf("No argument found.");
        else {
            for (; op_arg_i < op[1]; op_arg_i++) {
                printf("%lld", op[op_arg_i + 2]);
                if (op_arg_i < op[1] - 1)
                    printf(" ");
            }
        }
        printf("\n");
    }
    printf("|------------------------|\n");
    for (; i >= 0 ; i--) {
        sprintf(n, "%lld", stack->elements[i]);
        n_len = strlen(n);
        printf("|");
        for (; j < 12 - n_len / 2 - (stack->elements[i] < 0); j++)
            printf(" ");
        printf("%s", n);
        j = 0;
            for (; j < 12 - n_len / 2 - (n_len % 2 == 1); j++)
            printf(" ");
        printf("|\n");
        j = 0;
        if (i > 0)
            printf("|------------------------|\n");
    }
    printf("|------------------------|\n\n");
}
