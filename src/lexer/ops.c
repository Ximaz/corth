#include <corth.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static void token_error(char const *error, token_t *token)
{
    printf("%s:%llu:%llu: %s: \"%s\"\n", token->loc.filename, token->loc.row, token->loc.col, error, token->val.string);
    exit(1);
}

static op_typ_t find_op_type(char const *word)
{
    uint64 i = 0;

    for (; i < COUNT_OPS; i++)
        if (strcmp(BUILTINS[i].word, word) == 0)
            return BUILTINS[i].type;
    return COUNT_OPS;
}

op_t *consume_token(token_t *self)
{
    assert(COUNT_TOKENS == 3);
    op_t *op = 0;

    if (!self || !(op = (op_t *) malloc(sizeof(op_t))))
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
    op->jmp = -1;  /* The block has nowhere to jump yet, -1 is set. */
    op->addr = -1; /* The address has not been pushed yet, -1 is set. */
    return op;
}