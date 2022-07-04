#include <stdio.h>
#include <stdlib.h>
#include "../include/stack.h"
#include "../include/types.h"

stack_t *new_stack(void)
{
    stack_t *stack = (stack_t *) malloc(sizeof(stack_t));

    if (stack) {
        stack->top = 0;
        stack->elements = (value_t *) calloc(STACK_SIZE, sizeof(value_t));
    }
    return stack;
}

void destroy_stack(stack_t *self)
{
    free(self->elements);
    free(self);
}

void push_onto_stack(stack_t *self, value_t element)
{
    self->elements[self->top++] = element;
}

value_t pop_from(stack_t *self)
{
    if (self->top == 0) {
        printf("Stack is empty. Use debug_stack to see what happened.\n");
        exit(0);
    }
    return self->elements[--self->top];
}
