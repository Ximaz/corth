#ifndef _CORTH_TOKEN_H_
    #define _CORTH_TOKEN_H_
    #include "corth_types.h"

static char *const TOKEN_TYPES[] = {
    "TOKEN_WORD",
    "TOKEN_INT",
    "TOKEN_STR",
};

typedef struct s_tok_loc {
    char const *filename;
    uint64 row;
    uint64 col;
} tok_loc_t;

typedef union u_tok_val {
    int64 integer;
    char *string;
} tok_val_t;

typedef enum e_tok_typ {
    TOKEN_WORD,
    TOKEN_INT,
    TOKEN_STR,
    COUNT_TOKENS
} tok_typ_t;

typedef struct s_token {
    tok_typ_t type;
    tok_loc_t loc;
    tok_val_t val;
} token_t;

typedef struct s_tok_lst {
    uint64 tokens_len;
    token_t **tokens;
} tok_lst_t;

token_t *new_token(tok_loc_t loc, tok_typ_t type, tok_val_t val);
void destroy_token(token_t *self);

tok_lst_t *new_tokens(void);
void push_token(tok_lst_t *self, token_t *token);
void destroy_tokens(tok_lst_t *self);

#endif