#include <stdio.h>
#include <stdlib.h>
#include "../include/util.h"
#include "../include/program.h"

program_t *new_program(void)
{
    program_t *program = (program_t *)malloc(sizeof(program_t));
    if (!program)
        return program;
    program->instructions_len = 0;
    program->instructions = (int64 **) calloc(1, sizeof(int64 *));
    return program;
}

void push_instruction(program_t *self, int64 *instruction)
{
    if (!self->instructions)
        return;
    self->instructions[self->instructions_len++] = instruction;
    self->instructions = (int64 **) realloc(self->instructions, (self->instructions_len + 1) * sizeof(int64 *));
}

void destroy_program(program_t *self)
{
    uint64 i = 0;

    for (; i < self->instructions_len; i++)
        free(self->instructions[i]);
    free(self->instructions);
    free(self);
}
