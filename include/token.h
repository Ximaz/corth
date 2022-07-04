#ifndef _CORTH_TOKEN
#define _CORTH_TOKEN
#include "types.h"

static char *const TOKEN_TYPES[] = {
    "TOKEN_WORD",
    "TOKEN_INT",
    "TOKEN_STR",
};

token_t *new_token(location_t token_loc, token_type_t token_type, value_t val);
op_t *compile_token_to_op(token_t *self);
void destroy_token(token_t *self);

token_list_t *new_tokens(void);
void push_token(token_list_t *self, token_t *token);
void destroy_tokens(token_list_t *self);

#endif
