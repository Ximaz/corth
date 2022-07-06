#include <stdio.h>
#include <stdlib.h>
#include "../include/stack.h"
#include "../include/types.h"

corth_stack_t *new_stack(void)
{
    corth_stack_t *stack = (corth_stack_t *) malloc(sizeof(corth_stack_t));

    if (stack) {
        stack->top = 0;
        stack->elements = (value_t *) calloc(corth_stack_sIZE, sizeof(value_t));
    }
    return stack;
}

void destroy_stack(corth_stack_t *self)
{
    free(self->elements);
    free(self);
}

void push_onto_stack(corth_stack_t *self, value_t element)
{
    self->elements[self->top++] = element;
}

value_t pop_from(corth_stack_t *self)
{
    if (self->top == 0) {
        printf("Stack is empty. Use debug_stack to see what happened.\n");
        exit(0);
    }
    return self->elements[--self->top];
}
