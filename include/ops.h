#ifndef _CORTH_OPS
#define _CORTH_OPS
#include "types.h"

// This struct must be used only for tokens
// which have no params.
// e.g : can't be used for `push` op.
typedef struct op_s {
    char const *sym;
    int64 *(*func)(void);
} op_t;

typedef enum op_code_e
{
    OP_PUSH,
    OP_PLUS,
    OP_MINUS,
    OP_DUMP,
    OP_DUP,
    OP_EQUAL,
    OP_DIFF,
    OP_GT,
    OP_LT,
    OP_GOET,
    OP_LOET,
    OP_IF,
    OP_ELSE,
    OP_WHILE,
    OP_DO,
    OP_END,
    OP_HALT,
    COUNT_OPS,
} op_code_t;

static char *const op_codes[] = {
    "OP_PUSH",
    "OP_PLUS",
    "OP_MINUS",
    "OP_DUMP",
    "OP_DUP",
    "OP_EQUAL",
    "OP_DIFF",
    "OP_GT",
    "OP_LT",
    "OP_GOET",
    "OP_LOET",
    "OP_IF",
    "OP_ELSE",
    "OP_WHILE",
    "OP_DO",
    "OP_END",
    "OP_HALT",
};

int64 *push(int64 n);
int64 *plus(void);
int64 *minus(void);
int64 *dump(void);
int64 *dup(void);
int64 *equal(void);
int64 *diff(void);
int64 *gt(void);
int64 *lt(void);
int64 *goet(void);
int64 *loet(void);
int64 *iff(void);
int64 *elsee(void);
int64 *whilee(void);
int64 *doo(void);
int64 *end(void);
int64 *halt(void);

static op_t const ops_map[] = {
    { "+", &plus },
    { "-", &minus },
    { ".", &dump },
    { "dup", &dup },
    { "=", &equal },
    { "!=", &diff },
    { ">", &gt },
    { "<", &lt },
    { ">=", &goet },
    { "<=", &loet },
    { "if", &iff },
    { "else", &elsee },
    { "while", &whilee },
    { "do", &doo },
    { "end", &end },
    { "halt", &halt },
};

static char const comment[] = {'/', '/', 0};

#endif
