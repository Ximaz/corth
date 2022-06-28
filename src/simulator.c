#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/util.h"
#include "../include/stack.h"
#include "../include/program.h"

int simulate(program_t *program)
{
    int err = 0;
    uint64 i = 0;
    int64 n1 = 0;
    int64 n2 = 0;
    int64 *op = NULL;
    stack_t *stack;

    assert(COUNT_OPS == SUPPORTED_INSTRUCTIONS);
    if (!program)
        return 1;
    stack = new_stack();
    if (!stack)
        return 1;
    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op[0]) {
            case OP_PUSH:
                push_onto_stack(stack, op[1]);
                break;
            case OP_PLUS:
                n1 = pop_from(stack);
                n2 = pop_from(stack);
                push_onto_stack(stack, n1 + n2);
                break;
            case OP_MINUS:
                n1 = pop_from(stack);
                n2 = pop_from(stack);
                push_onto_stack(stack, n2 - n1);
                break;
            case OP_DUMP:
                n1 = pop_from(stack);
                printf("%lld\n", n1);
                break;
            case OP_HALT:
                err = pop_from(stack);
                break;
            case OP_NOP:
                break;
            default:
                printf("Unreachable\n");
                break;
        }
    }
    destroy_stack(stack);
    return err;
}
