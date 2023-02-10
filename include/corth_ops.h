#ifndef _CORTH_OPS_H_
    #define _CORTH_OPS_H_
    #include "corth_types.h"
    #include "corth_token.h"

typedef enum e_op_typ {
    OP_PUSH_INT,
    OP_PUSH_STR,
    OP_POP,
    OP_PLUS,
    OP_MINUS,
    OP_DUMP,
    OP_DUP,
    OP_2DUP,
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
    OP_SYSCALL0,
    OP_SYSCALL1,
    OP_SYSCALL2,
    OP_SYSCALL3,
    OP_SYSCALL4,
    OP_SYSCALL5,
    OP_SYSCALL6,
    OP_SHL,
    OP_SHR,
    OP_ORB,
    OP_ANDB,
    OP_SWAP,
    OP_OVER,
    COUNT_OPS,
} op_typ_t;

typedef struct s_op {
    op_typ_t type;
    tok_loc_t loc;
    /*
        Exists only for type OP_PUSH_INT or OP_PUSH_STR.
        Contains the value that needs to be pushed onto
        the stack.
    */
    tok_val_t val;
    /* Required when it time to free the op. */
    int is_val_str;
    /*
        Exists only for ops like `if`, `else`, `while`, etc.
        Contains an index of an op within the program that the
        execution has to jump to depending on the circumstantces.
        In case of `if` it's the place of else branch, in case of
        `else` it's the end of the construction, etc. Default value
        will be -1, which marks it as unset.
    */
    int64 jmp;
    /*
        Exists only for the op OP_PUSH_STR. It contains the address
        where the string was pushed in the memory.
    */
    int64 addr;
} op_t;

/* Binding a word to an actual op_code */
typedef struct s_builtins_map {
    char const *word;
    op_typ_t type;
} builtins_map_t;

static char *const OP_CODES[] = {
    "OP_PUSH_INT",
    "OP_PUSH_STR",
    "OP_POP",
    "OP_PLUS",
    "OP_MINUS",
    "OP_DUMP",
    "OP_DUP",
    "OP_2DUP",
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
    "OP_SYSCALL0",
    "OP_SYSCALL1",
    "OP_SYSCALL2",
    "OP_SYSCALL3",
    "OP_SYSCALL4",
    "OP_SYSCALL5",
    "OP_SYSCALL6",
    "OP_SHL",
    "OP_SHR",
    "OP_ORB",
    "OP_ANDB",
    "OP_SWAP",
    "OP_OVER"
};

static builtins_map_t const BUILTINS[] = {
    { "pop", OP_POP },
    { "+", OP_PLUS },
    { "-", OP_MINUS },
    { "dump", OP_DUMP },
    { "dup", OP_DUP },
    { "2dup", OP_2DUP },
    { "=", OP_EQUAL },
    { "!=", OP_DIFF },
    { ">", OP_GT },
    { "<", OP_LT },
    { ">=", OP_GOET },
    { "<=", OP_LOET },
    { "if", OP_IF },
    { "else", OP_ELSE },
    { "while", OP_WHILE },
    { "do", OP_DO },
    { "end", OP_END },
    { "mem", OP_MEM },
    { "store", OP_STORE },
    { "load", OP_LOAD },
    { "syscall0", OP_SYSCALL0 },
    { "syscall1", OP_SYSCALL1 },
    { "syscall2", OP_SYSCALL2 },
    { "syscall3", OP_SYSCALL3 },
    { "syscall4", OP_SYSCALL4 },
    { "syscall5", OP_SYSCALL5 },
    { "syscall6", OP_SYSCALL6 },
    { "shl", OP_SHL },
    { "shr", OP_SHR },
    { "orb", OP_ORB },
    { "andb", OP_ANDB },
    { "swap", OP_SWAP },
    { "over", OP_OVER },
};

static char const COMMENT[] = {'/', '/', 0};

op_t *consume_token(token_t *self);

#endif