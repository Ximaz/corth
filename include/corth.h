#ifndef _CORTH_H_
    #define _CORTH_H_
    #include "corth_types.h"
    #include "corth_ops.h"
    #include "corth_token.h"
    #include "corth_program.h"
    #include "corth_stack.h"
    #include "corth_asm.h"
    #include "corth_lexer.h"

int corth(char const *filename, corth_mode_t mode);
int parse_mode(char const *smode, corth_mode_t *cmode);
void simulate(char const *filename, char const *content);
void compile(char const *filename, char const *content);

#endif