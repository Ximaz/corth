#ifndef _CORTH_TOKEN
#define _CORTH_TOKEN
#include "util.h"

typedef struct token_s
{
    char const *filename;
    char *token;
    int64 *instruction;
    uint64 row;
    uint64 col;
} token_t;

typedef struct tokens_s
{
    uint64 tokens_len;
    token_t **tokens;
} tokens_t;

token_t *new_token(char const *filename, char *line, uint64 line_i, uint64 col);
void destroy_token(token_t *self);

tokens_t *new_tokens(void);
void push_token(tokens_t *self, token_t *token);
void destroy_tokens(tokens_t *self);

#endif
