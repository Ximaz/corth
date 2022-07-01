#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/stack.h"
#include "../include/token.h"
#include "../include/program.h"
#include "../include/debugger.h"
#include "../include/file_handler.h"
#include "../include/asm_functions.h"

static program_t *preprocess_program(program_t *program)
{
    uint64 i = 0;
    inst_t *op = 0;
    inst_t *tmp_op = 0;
    uint64 open_ptr = 0;
    stack_t *stack = new_stack();

    assert(COUNT_OPS == 27);
    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op->op_code) {
            case OP_IF:
                program->instructions[i]->args[0] = i;
                push_onto_stack(stack, i);
                break;
            case OP_ELSE:
                open_ptr = pop_from(stack);
                // `else` can only be used in `if` blocks.
                assert(program->instructions[open_ptr]->op_code == OP_IF);
                // `if` will points to the next instruction
                program->instructions[open_ptr]->args[0] = i + 1;
                program->instructions[i]->args[0] = i;
                push_onto_stack(stack, i);
                break;
            case OP_WHILE:
                push_onto_stack(stack, i);
                break;
            case OP_DO:
                open_ptr = pop_from(stack);
                program->instructions[i]->args[0] = open_ptr;
                push_onto_stack(stack, i);
                break;
            case OP_END:
                open_ptr = pop_from(stack);
                tmp_op = program->instructions[open_ptr];
                // Invalid `end` usage, no open block encountered.
                if(tmp_op->args[0] < 0) {
                    printf("ERROR: `end` : No block opened.\n");
                    exit(1);
                }
                if (tmp_op->op_code == OP_IF || tmp_op->op_code == OP_ELSE) {
                    program->instructions[open_ptr]->args[0] = i;
                    program->instructions[i]->args[0] = i;
                } else if (tmp_op->op_code == OP_DO) {
                    // store the `while` pointer.
                    program->instructions[i]->args[0] = tmp_op->args[0];
                      // sets the `do` end pointer there.
                    program->instructions[open_ptr]->args[0] = i;
                } else {
                    printf("ERROR: `end` : block was not opened neither by `if` nor `else` nor `while`.\n");
                    exit(1);
                }
                break;
            default:
                // Not handling other cases, it's just for blocks.
                break;
        }
    }
    destroy_stack(stack);
    return program;
}

program_t *new_program(tokens_t *tokens)
{
    size_t i = 0;
    program_t *program = 0;

    if (!tokens)
        return program;
    program = (program_t *)malloc(sizeof(program_t));
    if (!program)
        return program;
    program->instructions_len = tokens->tokens_len;
    program->instructions = (inst_t **) calloc(tokens->tokens_len, sizeof(inst_t *));
    for (; i < tokens->tokens_len; i++)
        program->instructions[i] = tokens->tokens[i]->instruction;
    return preprocess_program(program);
}

int run_program(program_t *self, int sim, int debug, char const *output)
{
    int err = 0;
    FILE* f = 0;
    uint64 i = 0;
    inst_t *op = 0;
    stack_t *stack = 0;
    int halt_found = 0;
    unsigned char fake_memory[MEMORY_CAPACITY];

    assert(COUNT_OPS == 27);
    if (!self)
        return 1;
    if (sim) {
        stack = new_stack();
        if (!stack)
            return 1;
    } else {
        // Output must be specified for compilation.
        assert(output);
        f = open_file(output, "w");
        asm_header(f);
    }
    if (debug && sim)
        debug_stack(stack, 0);
    for (; i < self->instructions_len && !halt_found; i++) {
        op = self->instructions[i];
        if (!sim)
            fprintf(f, "addr_%lld:\n", i);
        switch (op->op_code) {
            case OP_PUSH:
                inst_push(f, stack, op->args[0]);
                break;
            case OP_PLUS:
                inst_plus(f, stack);
                break;
            case OP_MINUS:
                inst_minus(f, stack);
                break;
            case OP_DUMP:
                inst_dump(f, stack);
                break;
            case OP_DUP:
                inst_dup(f, stack);
                break;
            case OP_EQUAL:
                inst_equal(f, stack);
                break;
             case OP_DIFF:
                inst_diff(f, stack);
                break;
            case OP_GT:
                inst_gt(f, stack);
                break;
            case OP_LT:
                inst_lt(f, stack);
                break;
            case OP_GOET:
                inst_goet(f, stack);
                break;
            case OP_LOET:
                inst_loet(f, stack);
                break;
            case OP_IF:
                // `preprocess_program` must be called.
                assert(op->args[0] >= 0);
                // Invalid `if` end pointer.
                assert((uint64) op->args[0] < self->instructions_len);
                if (inst_if(f, stack, op->args[0], self->instructions[op->args[0] - 1]->op_code == OP_ELSE) && sim)
                    i = op->args[0] - 1;
                break;
            case OP_ELSE:
                // `preprocess_program` must be called.
                assert(op->args[0] >= 0);
                // Invalid `else` end pointer.
                assert((uint64) op->args[0] < self->instructions_len);
                if (!sim)
                    inst_else(f, op->args[0]);
                else
                    i = op->args[0];
                break;
            case OP_WHILE:
                if (!sim)
                    inst_while(f);
                break;
            case OP_DO:
                // `preprocess_program` must be called.
                assert(op->args[0] >= 0);
                // Invalid `end` pointer.
                assert((uint64) op->args[0] < self->instructions_len);
                if (inst_do(f, stack, op->args[0] + 1) && sim)
                    i = op->args[0];
                break;
            case OP_END:
                // `preprocess_program` must be called. No address to the next instruction found.
                assert(op->args[0] >= 0);
                // Invalid `end` pointer.
                assert((uint64) op->args[0] < self->instructions_len);
                if (!sim)
                    inst_end(f, i, op->args[0] + 1);
                else
                    i = op->args[0];
                break;
            case OP_MEM:
                inst_mem(f, stack);
                break;
            case OP_STORE:
                inst_store(f, stack, fake_memory);
                break;
            case OP_LOAD:
                inst_load(f, stack, fake_memory);
                break;
            case OP_SYSCALL0:
                inst_syscall(f, stack, 0);
                break;
            case OP_SYSCALL1:
                inst_syscall(f, stack, 1);
                break;
            case OP_SYSCALL2:
                inst_syscall(f, stack, 2);
                break;
            case OP_SYSCALL3:
                inst_syscall(f, stack, 3);
                break;
            case OP_SYSCALL4:
                inst_syscall(f, stack, 4);
                break;
            case OP_SYSCALL5:
                inst_syscall(f, stack, 5);
                break;
            case OP_SYSCALL6:
                inst_syscall(f, stack, 6);
                break;
            case OP_HALT:
                err = inst_halt(f, stack);
                halt_found = 1;
                break;
            default:
                // Unreachable.
                assert(0);
                break;
        }
        if (debug && sim) {
            debug_stack(stack, op);
            debug_memory(fake_memory, 3);
        }
    }
    if (!halt_found) {
        inst_push(f, stack, 0);
        err = inst_halt(f, stack);
    }
    if (!sim) {
        asm_footer(f);
        fclose(f);
    }
    return err;
}

void destroy_program(program_t *self)
{
    uint64 i = 0;

    for (; i < self->instructions_len; i++)
        free(self->instructions[i]);
    free(self->instructions);
    free(self);
}
