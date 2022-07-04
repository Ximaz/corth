#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ops.h"
#include "../include/token.h"
#include "../include/types.h"
#include "../include/stack.h"
#include "../include/program.h"
#include "../include/asm_functions.h"

void debug_stack(stack_t *stack, op_t *op)
{
    char buff[21];
    value_t val;
    uint64 buff_len = 0;
    uint64 display = 26;
    uint64 padding = 0;
    uint64 padding_i = 0;
    int64 stack_cursor = stack->top;

    if (!op)
        return;
    printf("|");
    for (padding_i = 0; padding_i < display; padding_i++)
        printf("-");
    printf("|\n");
    for (--stack_cursor; stack_cursor >= 0; stack_cursor--) {
        val = stack->elements[stack_cursor];
        snprintf(buff, 21, "%lld", val.integer);
        buff_len = strlen(buff);
        padding = (display - buff_len) / 2;
        printf("|");
        for (padding_i = 0; padding_i < padding; padding_i++)
            printf(" ");
        printf("%s", buff);
        for (padding_i = 0; padding_i < display - (buff_len + padding); padding_i++)
            printf(" ");
        printf("|\n");
        printf("|");
        for (padding_i = 0; padding_i < display; padding_i++)
            printf("-");
        printf("|\n");
    }
}

void debug_memory(unsigned char *fake_mem, uint64 limit)
{
    uint64 i = 0;
    uint64 range = limit <= MEMORY_CAPACITY ? limit : MEMORY_CAPACITY;

    printf("CURRENT MEMORY : [");
    for (; i < range; i++) {
        printf("%u", fake_mem[i]);
        if (i < range - 1)
            printf(", ");
    }
    printf("]\n");
}

void debug_program(program_t *self)
{
    uint64 i = 0;
    op_t *op = 0;

    for (; i < self->ops_len; i++) {
        op = self->ops[i];
        printf("%llu: %s", i, OP_CODES[op->type]);
        if (op->type == OP_PUSH_INT) {
            printf(": %lld", op->val.integer);
        } else if (op->type == OP_PUSH_STR)
            printf(": %s", op->val.string);
        if (i < self->ops_len - 1)
            printf(",");
        printf("\n");
    }
}

void debug_jump(program_t *self, op_t *op, uint64 op_addr, uint64 jmp_addr)
{
    printf("JUMP IS TAKEN : from %s (at %lld) to %s (at %lld)\n", OP_CODES[op->type], op_addr, OP_CODES[self->ops[jmp_addr]->type], jmp_addr);
}

void debug_binding(op_t *src, uint64 srci, char const *dest, uint64 desti)
{
    char const *binding_str = "BINDING : %s (at %lld) -> %s (at %llu)\n";
    printf(binding_str, OP_CODES[src->type], srci, dest, desti);
}

void debug_tokens(token_list_t *self)
{
    uint64 i = 0;
    token_t *token = 0;

    for (; i < self->tokens_len; i++) {
        token = self->tokens[i];
        printf("TOKEN : { type: \"%s\", location: \"%s:%llu:%llu\"", TOKEN_TYPES[token->type], token->loc.filename, token->loc.row, token->loc.col);
        if (token->type == TOKEN_INT) {
            printf(", .val = %lld", token->val.integer);
        } else if (token->type == TOKEN_STR || token->type == TOKEN_WORD)
            printf(", .val = %s", token->val.string);
        printf(" }\n");
    }
}
