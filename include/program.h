#ifndef _CORTH_PROGRAM
#define _CORTH_PROGRAM
#include "util.h"
#include "token.h"
#include <stdlib.h>

typedef struct program_s
{
    uint64 instructions_len;
    int64 **instructions;
} program_t;

program_t *new_program(tokens_t *tokens);
void destroy_program(program_t *self);

#endif
