#include <stdlib.h>
#include "../include/stack.h"

stack_t *new_stack(void)
{
    stack_t *stack = (stack_t *) malloc(sizeof(stack_t));

    if (stack) {
        stack->top = 0;
        stack->elements = (int64 *) calloc(STACK_SIZE, sizeof(int64));
    }
    return stack;
}

void destroy_stack(stack_t *self)
{
    free(self->elements);
    free(self);
}

void push_onto_stack(stack_t *self, int64 element)
{
    self->elements[self->top++] = element;
}

int64 pop_from(stack_t *self)
{
    return self->elements[--self->top];
}
