#ifndef _CORTH_PROGRAM
#define _CORTH_PROGRAM
#include "ops.h"
#include "types.h"
#include "token.h"
#include <stdlib.h>

typedef struct program_s
{
    uint64 instructions_len;
    inst_t **instructions;
} program_t;

program_t *new_program(tokens_t *tokens);
void destroy_program(program_t *self);
int run_program(program_t *self, int sim, int debug, char const *output);

#endif
