#ifndef _CORTH_UTIL
#define _CORTH_UTIL

typedef long long int64;
typedef unsigned long long uint64;

typedef struct debugger_s
{
    int enabled;
    int mem_lim;
    int debug_stack;
    int debug_memory;
    int debug_bindings;
    int debug_jumps;
    int debug_tokens;
} debugger_t;

typedef union inst_arg_u
{
    int64 integer; // For integer tokens.
    int64 ptr;     // For pointer tokens : `if`, `else`, `do`, `end`, ...
    char *word;    // For string  tokens.
    int statement; // For boolean tokens.
} inst_arg_t;

typedef enum token_type_e
{
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_WORD,
    COUNT_TYPES
} token_type_t;

static char *const TOKEN_TYPES[] = {
    "TOKEN_ID",
    "TOKEN_INT",
    "TOKEN_WORD",
};

#endif
