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
    OP_EQUAL,
    OP_IF,
    OP_ELSE,
    OP_END,
    COUNT_OPS,
} op_code_t;

static char *const op_codes[] = {
    "OP_PUSH",
    "OP_PLUS",
    "OP_MINUS",
    "OP_DUMP",
    "OP_HALT",
    "OP_EQUAL",
    "OP_IF",
    "OP_ELSE",
    "OP_END",
};

int64 *push(int64 n);
int64 *halt(void);
int64 *plus(void);
int64 *minus(void);
int64 *dump(void);
int64 *equal(void);
int64 *iff(void);
int64 *elsee(void);
int64 *end(void);

#endif
