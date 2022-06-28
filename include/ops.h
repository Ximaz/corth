#ifndef _CORTH_OPS
#define _CORTH_OPS
#include "util.h"

static uint64 const SUPPORTED_INSTRUCTIONS = 6;

typedef enum op_code_e
{
    OP_PUSH,
    OP_PLUS,
    OP_MINUS,
    OP_DUMP,
    OP_HALT,
    OP_EQUAL,
    COUNT_OPS,
} op_code_t;

int64 *push(int64 n);
int64 *halt(void);
int64 *plus(void);
int64 *minus(void);
int64 *dump(void);
int64 *equal(void);

#endif
