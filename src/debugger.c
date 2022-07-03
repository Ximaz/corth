#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/stack.h"
#include "../include/program.h"
#include "../include/asm_functions.h"

void debug_stack(stack_t *stack, inst_t *op)
{
    char buff[21];
    inst_arg_t arg;
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
        arg = stack->elements[stack_cursor];
        snprintf(buff, 21, "%lld", arg.integer);
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

void debug_program(tokens_t *self)
{
    uint64 i = 0;
    uint64 j = 0;
    inst_t *op = 0;
    inst_arg_t arg;

    for (; i < self->tokens_len; i++) {
        op = self->tokens[i]->instruction;
        printf("%llu: %s", i, OP_CODES[op->op_code]);
        if (op->args_len > 0) {
            printf(": ");
            for (; j < op->args_len; j++) {
                arg = op->args[j];
                printf("%lld", arg.integer);
                if (j < op->args_len - 1)
                    printf(", ");
            }
            j = 0;
        }
        if (i < self->tokens_len - 1)
            printf(",");
        printf("\n");
    }
}

void debug_jump(tokens_t *self, inst_t *op, uint64 op_addr, uint64 jmp_addr)
{
    printf("JUMP IS TAKEN : from %s (at %lld) to %s (at %lld)\n", OP_CODES[op->op_code], op_addr, OP_CODES[self->tokens[jmp_addr]->instruction->op_code], jmp_addr);
}

void debug_binding(inst_t *src, uint64 srci, char const *dest, uint64 desti)
{
    char const *binding_str = "BINDING : %s (at %lld) -> %s (at %llu)\n";
    printf(binding_str, OP_CODES[src->op_code], srci, dest, desti);
}

void debug_tokens(tokens_t *self)
{
    uint64 inst_i = 0;
    token_t *token = 0;
    inst_t *inst = 0;

    for (; inst_i < self->tokens_len; inst_i++) {
        token = self->tokens[inst_i];
        inst = token->instruction;
        printf("TOKEN : { type: \"%s\", location: \"%s:%llu:%llu\", instruction: \"%s\"}\n", TOKEN_TYPES[token->type], token->filename, token->row, token->col, OP_CODES[inst->op_code]);
    }
}
