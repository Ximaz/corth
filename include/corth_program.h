#ifndef _CORTH_PROGRAM_H_
    #define _CORTH_PROGRAM_H_
    #include "corth_types.h"
    #include "corth_ops.h"

typedef struct s_program {
    uint64 ops_len;
    op_t **ops;
} program_t;

int run_program(tok_lst_t *self, corth_mode_t mode, char const *output);

#endif