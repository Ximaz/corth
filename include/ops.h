#ifndef _CORTH_OPS
#define _CORTH_OPS
#include "types.h"

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
    OP_MEM,
    OP_STORE,
    OP_LOAD,
    OP_HALT,
    COUNT_OPS,
} op_code_t;

typedef struct inst_s
{
    op_code_t op_code;
    uint64 args_len;
    int64 *args;
} inst_t;

// This struct must be used only for tokens
// which have no params.
// e.g : can't be used for `push` op.
typedef struct op_s {
    char const *sym;
    inst_t *(*func)(void);
} op_t;

static char *const OP_CODES[] = {

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
    "OP_MEM",
    "OP_STORE",
    "OP_LOAD",
    "OP_HALT",
};

inst_t *push(int64 n);
inst_t *plus(void);
inst_t *minus(void);
inst_t *dump(void);
inst_t *dup(void);
inst_t *equal(void);
inst_t *diff(void);
inst_t *gt(void);
inst_t *lt(void);
inst_t *goet(void);
inst_t *loet(void);
inst_t *iff(void);
inst_t *elsee(void);
inst_t *whilee(void);
inst_t *doo(void);
inst_t *end(void);
inst_t *mem(void);
inst_t *store(void);
inst_t *load(void);
inst_t *halt(void);

static op_t const OPS_MAP[] = {
    { "+", &plus },
    { "-", &minus },
    { "dump", &dump },
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
    { "mem", &mem },
    { "store", &store },
    { "load", &load },
    { "halt", &halt },
};

static char const COMMENT[] = {'/', '/', 0};

#endif
