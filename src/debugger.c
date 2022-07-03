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
    uint64 padding_i = 0;
    uint64 padding = 16;
    inst_arg_t arg;
    char buff[33];
    uint64 buff_len = 0;
    int64 stack_cursor = stack->top - 1;

    if (!op)
        return;
    printf("|");
    for (padding_i = 0; padding_i < 2 * padding + 2; padding_i++)
        printf("-");
    printf("|\n");
    for (; stack_cursor >= 0; stack_cursor--) {
        arg = stack->elements[stack_cursor];
        if (arg.word) {
            snprintf(buff, 26, "\"%s ...\"", arg.word);
        } else if (arg.integer) {
            sprintf(buff, "%lld (int)", arg.integer);
        } else if (arg.ptr) {
            sprintf(buff, "%lld (ptr)", arg.ptr);
        } else if (arg.statement) {
            sprintf(buff, "%d (stmt)", arg.statement);
        } else {
            sprintf(buff, "X (ukwn)");
        }
        buff_len = strlen(buff);
        printf("|");
        for (padding_i = 0; padding_i < padding / 2 - buff_len / 2; padding_i++)
            printf(" ");
        printf("%s", buff);
        for (padding_i = 0; padding_i < padding / 2 - buff_len / 2; padding_i++)
            printf(" ");
        printf("|\n");
        if (stack_cursor > 0) {
            printf("|");
            for (padding_i = 0; padding_i < 2 * padding + 2; padding_i++)
                printf("-");
            printf("|\n");
        }
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
    uint64 j = 0;
    inst_t *op = 0;
    inst_arg_t arg;

    for (; i < self->instructions_len; i++) {
        op = self->instructions[i];
        printf("%llu: %s", i, OP_CODES[op->op_code]);
        if (op->args_len > 0) {
            printf(": ");
            for (; j < op->args_len; j++) {
                arg = op->args[j];
                if (arg.word)
                    printf("%s (str)", arg.word);
                else if (arg.integer) {
                    printf("%lld (int)", arg.integer);
                } else if (arg.ptr) {
                    printf("%lld (ptr)", arg.ptr);
                } else if (arg.statement) {
                    printf("%d (stmt)", arg.statement);
                } else {
                    printf("X (ukwn)");
                }
                if (j < op->args_len - 1)
                    printf(", ");
            }
            j = 0;
        }
        if (i < self->instructions_len - 1)
            printf(",");
        printf("\n");
    }
}

void debug_jump(program_t *self, inst_t *op, uint64 op_addr, uint64 jmp_addr)
{
    printf("JUMP IS TAKEN : from %s (at %lld) to %s (at %lld)\n", OP_CODES[op->op_code], op_addr, OP_CODES[self->instructions[jmp_addr]->op_code], jmp_addr);
}

void debug_binding(inst_t *src, uint64 srci, char const *dest, uint64 desti)
{
    char const *binding_str = "BINDING : %s (at %lld) -> %s (at %llu)\n";
    printf(binding_str, OP_CODES[src->op_code], srci, dest, desti);
}
