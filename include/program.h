#ifndef _CORTH_PROGRAM
#define _CORTH_PROGRAM
#include "ops.h"
#include "token.h"
#include "types.h"

int run_program(tokens_t *self, int sim, debugger_t const *debug, char const *output);

#endif
