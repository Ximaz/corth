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
    printf("%s:%llu:%llu: %s: \"%s\"\n", token->loc.filename, token->loc.row, token->loc.col, error, token->val.string);
    exit(1);
}

static op_type_t find_op_type(char *word)
{
    uint64 i = 0;

    for (; i < COUNT_OPS; i++)
        if (strcmp(BUILTINS[i].word, word) == 0)
            return BUILTINS[i].type;
    return COUNT_OPS;
}

op_t *compile_token_to_op(token_t *self)
{
    assert(COUNT_TOKENS == 3);
    op_t *op = malloc(sizeof(op_t));

    if (!op)
        return op;
    if (self->type == TOKEN_WORD) {
        op->type = find_op_type(self->val.string);
        if (op->type == COUNT_OPS) {
            free(op);
            token_error("unknowned word", self);
        }
    } else if (self->type == TOKEN_INT)
        op->type = OP_PUSH_INT;
    else if(self->type == TOKEN_STR)
        op->type = OP_PUSH_STR;
    else {
        free(op);
        token_error("unexpected error", self);
    }
    op->val = self->val;
    op->loc = self->loc;
    op->jmp = -1;
    return op;
}

token_t *new_token(location_t token_loc, token_type_t token_type, value_t val)
{
    token_t *token = 0;

    token = (token_t *) malloc(sizeof(token_t));
    if (!token)
        return token;
    token->loc = token_loc;
    token->type = token_type;
    token->val = val;
    return token;
}

void destroy_token(token_t *self)
{
    if (self) {
        if (self->type == TOKEN_STR)
            free(self->val.string);
        free(self);
    }
}

token_list_t *new_tokens(void)
{
    token_list_t *tokens = (token_list_t *)malloc(sizeof(token_list_t));

    if (!tokens)
        return tokens;
    tokens->tokens_len = 0;
    tokens->tokens = (token_t **) calloc(1, sizeof(token_t *));
    return tokens;
}

void push_token(token_list_t *self, token_t *token)
{
    if (!self || !self->tokens)
        return;
    self->tokens[self->tokens_len++] = token;
    self->tokens = (token_t **) realloc(self->tokens, (self->tokens_len + 1) * sizeof(token_t *));
}

void destroy_tokens(token_list_t *self)
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
