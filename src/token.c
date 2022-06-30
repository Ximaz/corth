#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/token.h"

static void token_error(char const *error, token_t *token)
{
    printf("%s:%llu:%llu: %s: \"%s\"\n", token->filename, token->row, token->col, error, token->token);
    exit(1);
}

static inst_t *find_inst(char *sym)
{
    uint64 i = 0;

    for (; i < COUNT_OPS - 1; i++)
        if (strcmp(ops_map[i].sym, sym) == 0)
            return ops_map[i].func();
    return 0;
}

static inst_t *parse_token(token_t *token)
{
    int64 n = 0;
    uint64 i = 0;
    char *tvalue = token->token;
    inst_t *inst = find_inst(tvalue);

    if (inst)
        return inst;
    while (tvalue[i] >= '0' && tvalue[i] <= '9') i++;
    if (tvalue[i] == 0) {
        if (i <= 19) {
            n = strtoll(tvalue, 0, 10);
            return push(n);
        }
        if (errno == ERANGE)
            token_error("number wont fit into int64 bits", token);
        token_error("number wont fit into int64 bits (might be a uint64 error)", token);
    }
    token_error("unexpected token", token);
    return 0;
}

token_t *new_token(char const *filename, char *tvalue, uint64 row, uint64 col)
{
    token_t *token = 0;

    if (!tvalue)
        return token;
    token = (token_t *) malloc(sizeof(token_t));
    if (!token)
        return token;
    token->filename = filename;
    token->token = tvalue;
    token->row = row;
    token->col = col;
    token->instruction = parse_token(token);
    return token;
}

void destroy_token(token_t *self)
{
    if (self)
        free(self);
}

tokens_t *new_tokens(void)
{
    tokens_t *tokens = (tokens_t *)malloc(sizeof(tokens_t));

    if (!tokens)
        return tokens;
    tokens->tokens_len = 0;
    tokens->tokens = (token_t **) calloc(1, sizeof(token_t *));
    return tokens;
}

void push_token(tokens_t *self, token_t *token)
{
    if (!self || !self->tokens)
        return;
    self->tokens[self->tokens_len++] = token;
    self->tokens = (token_t **) realloc(self->tokens, (self->tokens_len + 1) * sizeof(token_t *));
}

void destroy_tokens(tokens_t *self)
{
    uint64 i = 0;

    if (!self)
        return;
    for (; i < self->tokens_len; i++)
        destroy_token(self->tokens[i]);
    if (self->tokens)
        free(self->tokens);
    free(self);
}
