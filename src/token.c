#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/token.h"

static void token_error(char const *error, token_t *token, char *word)
{
    printf("%s:%llu:%llu: %s: \"%s\"\n", token->filename, token->row, token->col, error, word);
    exit(1);
}

static inst_t *find_inst(char *word)
{
    uint64 i = 0;

    // COUNT_OPS - 2 : The OPS_MAP doesn't contain all ops, especially
    // those which take params such as `OP_STRING` and `OP_PUSH`.
    for (; i < COUNT_OPS - 2; i++)
        if (strcmp(OPS_MAP[i].word, word) == 0)
            return OPS_MAP[i].func();
    return 0;
}

static inst_t *parse_token(token_t *token, char *word)
{
    int64 n = 0;
    uint64 i = 0;
    inst_t *inst = find_inst(word);

    if (inst)
        return inst;
    while (word[i] >= '0' && word[i] <= '9') i++;
    if (word[i] == 0) {
        if (i <= 19) {
            n = strtoll(word, 0, 10);
            token->type = TOKEN_INT;
            return push(n);
        }
        if (errno == ERANGE)
            token_error("number wont fit into int64 bits", token, word);
        token_error("number wont fit into int64 bits (might be a uint64 error)", token, word);
    }
    token->type = TOKEN_WORD;
    return string(word);
}

token_t *new_token(char const *filename, uint64 row, uint64 col, char *word)
{
    token_t *token = 0;

    if (!word)
        return token;
    token = (token_t *) malloc(sizeof(token_t));
    if (!token)
        return token;
    token->row = row;
    token->col = col;
    token->filename = filename;
    token->type = TOKEN_ID;
    token->instruction = parse_token(token, word);
    return token;
}

void destroy_token(token_t *self)
{
    if (self) {
        if (self->instruction) {
            if (self->instruction->args)
                free(self->instruction->args);
            free(self->instruction);
        }
        free(self);
    }
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
