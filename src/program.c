#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/util.h"
#include "../include/stack.h"
#include "../include/token.h"
#include "../include/program.h"

program_t *preprocess_program(program_t *program)
{
    uint64 i = 0;
    int64 *op = 0;
    uint64 open_ptr = 0;
    stack_t *stack = new_stack();

    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op[0]) {
            case OP_IF:
                push_onto_stack(stack, i);
                break;
            case OP_END:
                open_ptr = pop_from(stack);
                assert(program->instructions[open_ptr][0] == OP_IF);
                program->instructions[open_ptr][2] = i;
                break;
            default:
                // Not handling other cases, it's just for blocks.
                break;
        }
    }
    destroy_stack(stack);
    return program;
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

void destroy_program(program_t *self)
{
    uint64 i = 0;

    for (; i < self->instructions_len; i++)
        free(self->instructions[i]);
    free(self->instructions);
    free(self);
}
