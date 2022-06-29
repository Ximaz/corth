#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/util.h"
#include "../include/stack.h"
#include "../include/program.h"
#include "../include/debugger.h"
#include "../include/asm_functions.h"

int simulate(program_t *program)
{
    int err = 0;
    uint64 i = 0;
    int64 *op = NULL;
    stack_t *stack;

    assert(COUNT_OPS == SUPPORTED_INSTRUCTIONS);
    if (!program)
        return 1;
    stack = new_stack();
    if (!stack)
        return 1;
    debug_stack(stack, 0);
    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op[0]) {
            case OP_PUSH:
                inst_push(0, stack, op[2]);
                break;
            case OP_PLUS:
                inst_plus(0, stack);
                break;
            case OP_MINUS:
                inst_minus(0, stack);
                break;
            case OP_EQUAL:
                inst_equal(0, stack);
                break;
            case OP_DUMP:
                inst_dump(0, stack);
                break;
            case OP_IF:
                if (inst_if(0, stack)) {
                    printf("%lld\n", op[2]);
                    // `preprocess_program` must be called.
                    assert(op[2] >= 0);
                    i = op[2];
                }
                break;
            case OP_END:
                break;
            case OP_HALT:
                err = inst_halt(0, stack);
                break;
            default:
                printf("Unreachable.\n");
                break;
        }
        debug_stack(stack, op);
    }
    return err;
}
