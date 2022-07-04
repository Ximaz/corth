#ifndef _CORTH_UTIL
#define _CORTH_UTIL

// Types for integers :

typedef long long int64;
typedef unsigned long long uint64;

// Types for debugger :

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

// Type for locations :
// Can be used into op_code_t and token_t.
typedef struct location_s
{
    char const *filename;
    uint64 row;
    uint64 col;
} location_t;

// Types for op codes :

typedef enum op_type_e
{
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
} op_type_t;

// Types for builtins :
typedef struct builtins_map_s
{
    char const *word;
    op_type_t type;
} builtins_map_t;

typedef union value_u
{
    int64 integer;
    char *string;
} value_t;

typedef struct op_s
{
    op_type_t type;
    location_t loc;
    // Exists only for type OP_PUSH_INT or OP_PUSH_STR.
    // Contains the value // that needs to be pushed onto
    // the stack.
    value_t val;
    // Exists only for ops like `if`, `else`, `while`, etc.
    // Contains an index of an op within the program that the
    // execution has to jump to depending on the circumstantces.
    // In case of `if` it's the place of else branch, in case of
    // `else` it's the end of the construction, etc. Default value
    // will be -1, which marks it as unset.
    uint64 jmp;
} op_t;

// Types for tokens :

typedef enum token_type_e
{
    TOKEN_WORD,
    TOKEN_INT,
    TOKEN_STR,
    COUNT_TOKENS
} token_type_t;

typedef struct token_s
{
    token_type_t type;
    location_t loc;
    value_t val;
} token_t;

typedef struct token_list_s
{
    uint64 tokens_len;
    token_t **tokens;
} token_list_t;

// Types for programs :

typedef struct program_s
{
    uint64 ops_len;
    op_t **ops;
} program_t;

// Types for stacks :

typedef struct stack_s
{
    unsigned int top;
    value_t *elements;
} stack_t;

#endif
