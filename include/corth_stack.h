#ifndef _CORTH_STACK
    #define _CORTH_STACK
    #include "corth_types.h"
    #include "corth_token.h"

static unsigned int const corth_stack_size = 4096;

typedef struct s_corth_stack {
    uint64 top;
    tok_val_t *elements;
} corth_stack_t;

corth_stack_t *new_stack(void);
void destroy_stack(corth_stack_t *self);
void push_onto_stack(corth_stack_t *self, tok_val_t element);
tok_val_t pop_from(corth_stack_t *self);

#endif