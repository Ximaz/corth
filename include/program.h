#ifndef _CORTH_PROGRAM
#define _CORTH_PROGRAM
#include "util.h"
#include <stdlib.h>

typedef struct program_s
{
    uint64 instructions_len;
    int64 **instructions;
} program_t;

program_t *new_program(void);
void push_instruction(program_t *self, int64 *instruction);
void destroy_program(program_t *self);

#endif