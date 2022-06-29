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

// static void show_op(int64 *op)
// {
//     printf("(%s, %lld", op_codes[op[0]], op[1]);
//     if (op[1] == 1)
//         printf(", %lld", op[2]);
//     printf(")\n");
// }

static program_t *preprocess_program(program_t *program)
{
    uint64 i = 0;
    int64 *op = 0;
    int64 *tmp_op = 0;
    uint64 open_ptr = 0;
    stack_t *stack = new_stack();

    assert(COUNT_OPS == 16);
    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op[0]) {
            case OP_IF:
                program->instructions[i][2] = i;
                push_onto_stack(stack, i);
                break;
            case OP_ELSE:
                open_ptr = pop_from(stack);
                // `else` can only be used in `if` blocks.
                assert(program->instructions[open_ptr][0] == OP_IF);
                program->instructions[open_ptr][2] = i + 1; // `if` will points to the next instruction
                program->instructions[i][2] = i;
                push_onto_stack(stack, i);
                break;
            case OP_WHILE:
                push_onto_stack(stack, i);
                break;
            case OP_DO:
                open_ptr = pop_from(stack);
                program->instructions[i][2] = open_ptr;
                push_onto_stack(stack, i);
                break;
            case OP_END:
                open_ptr = pop_from(stack);
                tmp_op = program->instructions[open_ptr];
                // Invalid `end` usage, no open block encountered.
                if(tmp_op[2] < 0) {
                    printf("ERROR: `end` : No block opened.\n");
                    exit(1);
                }
                if (tmp_op[0] == OP_IF || tmp_op[0] == OP_ELSE) {
                    program->instructions[open_ptr][2] = i;
                    program->instructions[i][0] = OP_END;
                    program->instructions[i][2] = i + 1;
                } else if (tmp_op[0] == OP_DO) {
                    program->instructions[i][2] = tmp_op[2]; // store the `while` pointer.
                    program->instructions[open_ptr][2] = i;  // sets the `do` end pointer there.
                } else {
                    // ERROR: `end` : block was not opened neither by `if` nor `else` nor `while`.
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

    assert(COUNT_OPS == 16);
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
    if (debug) {
        show_program(self);
        if (sim)
            debug_stack(stack, 0);
    }
    for (; i < self->instructions_len; i++) {
        op = self->instructions[i];
        if (f)
            fprintf(f, "addr_%lld:\n", i);
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
                // Invalid `if` end pointer.
                assert((uint64) op[2] < self->instructions_len);
                if (inst_if(f, stack, op[2], self->instructions[op[2] - 1][0] == OP_ELSE) && sim)
                    i = op[2] - 1;
                break;
            case OP_ELSE:
                // `preprocess_program` must be called.
                assert(op[2] >= 0);
                // Invalid `else` end pointer.
                assert((uint64) op[2] < self->instructions_len);
                if (!sim)
                    inst_else(f, op[2]);
                else
                    i = op[2];
                break;
            case OP_WHILE:
                if (!sim)
                    inst_while(f);
                break;
            case OP_DO:
                // `preprocess_program` must be called.
                assert(op[2] >= 0);
                // Invalid `end` pointer.
                assert((uint64) op[2] < self->instructions_len);
                if (inst_do(f, stack, op[2] + 1) && sim)
                    i = op[2];
                break;
            case OP_END:
                // `preprocess_program` must be called. No address to the next instruction found.
                assert(op[2] >= 0);
                // Invalid `end` pointer.
                assert((uint64) op[2] < self->instructions_len);
                if (!sim)
                    inst_end(f, i, op[2]);
                else
                    i = op[2];
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
        if (debug && sim)
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
