#ifndef _CORTH_H_
    #define _CORTH_H_
    #include "corth_types.h"

int corth(char const *filename, corth_mode_t mode);
int parse_mode(char const *smode, corth_mode_t *cmode);
void simulate(char const *content);
void compile(char const *content);

#endif