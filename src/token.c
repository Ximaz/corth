#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ops.h"
#include "../include/util.h"
#include "../include/token.h"

void token_error(char const *error, token_t *token)
{
    printf("%s", error);
    printf(" \"%s\" in %s:%llu:%llu.\n", token->token, token->filename, token->row, token->col);
    exit(1);
}

int64 *parse_token(token_t *token)
{
    uint64 i = 0;
    int64 n = 0;
    char *tvalue = token->token;

    if (strcmp(tvalue, "+") == 0)
        return plus();
    if (strcmp(tvalue, "-") == 0)
        return minus();
    if (strcmp(tvalue, ".") == 0)
        return dump();
    if (strcmp(tvalue, "halt") == 0)
        return halt();
    while (tvalue[i] >= '0' && tvalue[i] <= '9') i++;
    if (tvalue[i] == 0) {
        if (i <= 19) {
            n = strtoll(tvalue, NULL, 10);
            return push(n);
        }
        if (errno == ERANGE)
            token_error("ERROR: The number does not fit max value of 'long long'.", token);
        token_error("ERROR: The number does not fit into a 'long long'.", token);
    }
    token_error("ERROR: Unknowned token.", token);
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
    if (self != NULL)
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
    if (!self->tokens)
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
