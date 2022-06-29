#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/stack.h"
#include "../include/token.h"
#include "../include/program.h"
#include "../include/debugger.h"
#include "../include/file_handler.h"
#include "../include/asm_functions.h"

static program_t *preprocess_program(program_t *program)
{
    uint64 i = 0;
    int64 *op = 0;
    uint64 open_ptr = 0;
    stack_t *stack = new_stack();

    assert(COUNT_OPS == 14);
    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op[0]) {
            case OP_IF:
                push_onto_stack(stack, i);
                break;
            case OP_ELSE:
                open_ptr = pop_from(stack);
                // `else` can only be used in `if` blocks.
                assert(program->instructions[open_ptr][0] == OP_IF);
                program->instructions[open_ptr][2] = i + 1;
                push_onto_stack(stack, i);
                break;
            case OP_END:
                open_ptr = pop_from(stack);
                if (program->instructions[open_ptr][0] == OP_IF||
                    program->instructions[open_ptr][0] == OP_ELSE) {
                    program->instructions[open_ptr][2] = i;
                } else {
                    // Invalid `end` usage.
                    assert(0);
                }
                break;
            default:
                // Not handling other cases, it's just for blocks.
                break;
        }
    }
    destroy_stack(stack);
    return program;
}

static void show_program(program_t *self)
{
    int64 j = 0;
    uint64 i = 0;
    int64 *op = 0;

    printf("[\n");
    for (; i < self->instructions_len; i++) {
        op = self->instructions[i],
        printf("%lld    (%s, ", i, op_codes[op[0]]);
        for (; j < op[1]; j++)
            printf("%lld, ", op[j + 2]);
        printf("),\n");
        j = 0;
    }
    printf("]\n");
}

program_t *new_program(tokens_t *tokens)
{
    size_t i = 0;
    program_t *program = 0;

    if (!tokens)
        return program;
    program = (program_t *)malloc(sizeof(program_t));
    if (!program)
        return program;
    program->instructions_len = tokens->tokens_len;
    program->instructions = (int64 **) calloc(tokens->tokens_len, sizeof(int64 *));
    for (; i < tokens->tokens_len; i++)
        program->instructions[i] = tokens->tokens[i]->instruction;
    return preprocess_program(program);
}

int run_program(program_t *self, int sim, int debug, char const *output)
{
    int err = 0;
    uint64 i = 0;
    int64 *op = NULL;
    FILE* f = 0;
    stack_t *stack = 0;

    assert(COUNT_OPS == 14);
    if (!self)
        return 1;
    if (sim) {
        stack = new_stack();
        if (!stack)
            return 1;
    } else {
        // Output must be specified for compilation.
        assert(output);
        f = open_file(output, "w");
        asm_header(f);
    }
    if (sim && debug) {
        show_program(self);
        debug_stack(stack, 0);
    }
    for (; i < self->instructions_len; i++) {
        op = self->instructions[i];
        switch (op[0]) {
            case OP_PUSH:
                inst_push(f, stack, op[2]);
                break;
            case OP_PLUS:
                inst_plus(f, stack);
                break;
            case OP_MINUS:
                inst_minus(f, stack);
                break;
            case OP_EQUAL:
                inst_equal(f, stack);
                break;
            case OP_DUMP:
                inst_dump(f, stack);
                break;
            case OP_IF:
                // `preprocess_program` must be called.
                assert(op[2] >= 0);
                // Invalid `IF` end pointer.
                assert((uint64) op[2] < self->instructions_len);
                if (inst_if(f, stack, op[2], self->instructions[op[2] - 1][0] == OP_ELSE) && sim)
                    i = op[2] - 1;
                break;
            case OP_ELSE:
                // `preprocess_program` must be called.
                assert(op[2] >= 0);
                // Invalid `ELSE` end pointer.
                assert((uint64) op[2] < self->instructions_len);
                if (sim)
                    i = op[2];
                else
                    inst_else(f, i + 1, op[2]);
                break;
            case OP_END:
                if (!sim)
                    inst_end(f, i);
                break;
            case OP_DUP:
                inst_dup(f, stack);
                break;
            case OP_GT:
                inst_gt(f, stack);
                break;
            case OP_LT:
                inst_lt(f, stack);
                break;
            case OP_GOET:
                inst_goet(f, stack);
                break;
            case OP_LOET:
                inst_loet(f, stack);
                break;
            case OP_HALT:
                err = inst_halt(f, stack);
                break;
            default:
                // Unreachable.
                assert(0);
                break;
        }
        if (sim && debug)
            debug_stack(stack, op);
    }
    if (!sim)
        fclose(f);
    return err;
}

void destroy_program(program_t *self)
{
    uint64 i = 0;

    for (; i < self->instructions_len; i++)
        free(self->instructions[i]);
    free(self->instructions);
    free(self);
}
