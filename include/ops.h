#ifndef _CORTH_OPS
#define _CORTH_OPS
#include "util.h"

typedef enum op_code_e
{
    OP_PUSH,
    OP_PLUS,
    OP_MINUS,
    OP_DUMP,
    OP_HALT,
    COUNT_OPS,
} op_code_t;

int64 *push(int64 n);
int64 *halt(void);
int64 *plus(void);
int64 *minus(void);
int64 *dump(void);

#endif