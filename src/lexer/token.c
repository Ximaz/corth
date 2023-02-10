#include <corth.h>
#include <stdlib.h>

token_t *new_token(tok_loc_t loc, tok_typ_t type, tok_val_t val)
{
    token_t *token = (token_t *) malloc(sizeof(token_t));

    if (token) {
        token->loc = loc;
        token->type = type;
        token->val = val;
    }
    return token;
}

void destroy_token(token_t *self)
{
    if (!self)
        return;
    if (self->type == TOKEN_STR || self->type == TOKEN_WORD)
        free(self->val.string);
    free(self);
}

tok_lst_t *new_tokens(void)
{
    tok_lst_t *tokens = (tok_lst_t *)malloc(sizeof(tok_lst_t));

    if (tokens) {
        tokens->tokens_len = 0;
        tokens->tokens = (token_t **) calloc(1, sizeof(token_t *));
    }
    return tokens;
}

void push_token(tok_lst_t *self, token_t *token)
{
    if (!self || !self->tokens)
        return;
    self->tokens[self->tokens_len++] = token;
    self->tokens = (token_t **) realloc(self->tokens, (self->tokens_len + 1) * sizeof(token_t *));
}

void destroy_tokens(tok_lst_t *self)
{
    uint64 i = 0;

    if (self) {
        for (; i < self->tokens_len; i++)
            destroy_token(self->tokens[i]);
        if (self->tokens)
            free(self->tokens);
        free(self);
    }
}