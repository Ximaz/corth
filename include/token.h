#ifndef _CORTH_TOKEN
#define _CORTH_TOKEN
#include "ops.h"
#include "types.h"


typedef struct token_s
{
    // Where does the token come from ?
    uint64 row;
    uint64 col;
    char const *filename;
    // The token properties :
    inst_t *instruction;
} token_t;

typedef struct tokens_s
{
    uint64 tokens_len;
    token_t **tokens;
} tokens_t;

token_t *new_token(char const *filename, uint64 row, uint64 col, char *word);
void destroy_token(token_t *self);

tokens_t *new_tokens(void);
void push_token(tokens_t *self, token_t *token);
void destroy_tokens(tokens_t *self);

#endif
