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

static program_t *preprocess_program(program_t *program, debugger_t const *debug)
{
    uint64 i = 0;
    inst_t *op = 0;
    int64 op_pointer = 0;
    inst_t *pointed_op = 0;
    // inst_t *tmp = 0;
    // int64 tmp_pointer = 0;
    stack_t *stack = new_stack();

    assert(COUNT_OPS == 32);
    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op->op_code) {
            // `if` will have to point either to itself or an `else` address.
            case OP_IF:
                push_onto_stack(stack, i);
                break;
            // `else` will have to point the `end` address.
            case OP_ELSE:
                // The `ìf` address.
                op_pointer = pop_from(stack);
                // The `if` instruction.
                pointed_op = program->instructions[op_pointer];
                // `else` can only be used in `if` blocks.
                assert(pointed_op->op_code == OP_IF);
                // `if` will points to the next instruction after `else`.
                // if not + 1, if would point to the `else` adress which points
                // to the end address. So the `else` body would not have been
                // executed.
                if (debug->debug_bindings)
                    debug_binding(pointed_op, op_pointer, "else next instruction", i + 1);
                pointed_op->args[0] = i + 1;
                assert(program->instructions[op_pointer]->args[0] == pointed_op->args[0]);
                push_onto_stack(stack, i);
                break;
            case OP_WHILE:
                push_onto_stack(stack, i);
                break;
            // `do` will have to point the `end` address.
            case OP_DO:
                // The `while` address.
                op_pointer = pop_from(stack);
                // The `while` instruction.
                pointed_op = program->instructions[op_pointer];
                assert(pointed_op->op_code == OP_WHILE);
                // `do` now points to the `while` block.
                if (debug->debug_bindings)
                    debug_binding(op, i, OP_CODES[pointed_op->op_code], op_pointer);
                op->args[0] = op_pointer;
                assert(program->instructions[i]->args[0] == op->args[0]);
                push_onto_stack(stack, i);
                break;
            case OP_END:
                op_pointer = pop_from(stack);
                // printf("OP_POINTER : %lld\n", op_pointer);
                // if ((uint64) op_pointer >= program->instructions_len)
                    // break;
                pointed_op = program->instructions[op_pointer];
                if (pointed_op->op_code == OP_IF || pointed_op->op_code == OP_ELSE) {
                    // The block closed by `end` points to it.
                    pointed_op->args[0] = i;
                    if (debug->debug_bindings)
                        debug_binding(pointed_op, op_pointer, OP_CODES[op->op_code], i);
                    assert(program->instructions[op_pointer]->args[0] == pointed_op->args[0]);
                    // `end` points to the next instruction.
                    op->args[0] = i;
                    assert(program->instructions[i]->args[0] == op->args[0]);
                    if (debug->debug_bindings)
                        debug_binding(op, i, OP_CODES[program->instructions[op->args[0]]->op_code], op->args[0]);
                } else if (pointed_op->op_code == OP_DO) {
                    // `end` points to `while`.
                    op->args[0] = pointed_op->args[0];
                    assert(program->instructions[i]->args[0] == op->args[0]);
                    if (debug->debug_bindings)
                        debug_binding(op, i, OP_CODES[program->instructions[op->args[0]]->op_code], op->args[0]);
                    // `do` points now to the `end` block.
                    pointed_op->args[0] = i;
                    assert(program->instructions[op_pointer]->args[0] == pointed_op->args[0]);
                    if (debug->debug_bindings)
                        debug_binding(pointed_op, op_pointer, OP_CODES[op->op_code], i);
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
    return program;
}

void clear_memory(unsigned char fake_mem[])
{
    uint64 i = 0;

    for (; i < MEMORY_CAPACITY; i++)
        fake_mem[i] = 0;
}

int run_program(program_t *self, int sim, debugger_t const *debug, char const *output)
{
    int err = 0;
    FILE* f = 0;
    uint64 i = 0;
    inst_t *op = 0;
    int64 tmp_ptr = 0;
    stack_t *stack = 0;
    int64 poped = 0;
    int exit_found = 0;
    unsigned char fake_mem[MEMORY_CAPACITY];

    assert(COUNT_OPS == 32);
    if (!self)
        return 1;
    if (sim) {
        stack = new_stack();
        if (!stack)
            return 1;
    }
    else {
        // Output must be specified for compilation.
        assert(output);
        f = open_file(output, "w");
        asm_header(f);
    }
    clear_memory(fake_mem);
    self = preprocess_program(self, debug);
    debug_program(self);
    if (sim && debug->enabled && debug->debug_stack)
        debug_stack(stack, 0);
    for (; i < self->instructions_len && !exit_found; i++) {
        op = self->instructions[i];
        if (!sim)
            fprintf(f, "addr_%lld:\n", i);
        switch (op->op_code) {
            case OP_PUSH:
                inst_push(f, stack, op->args[0]);
                break;
            case OP_POP:
                // For now, poped won't do anthing.
                poped = inst_pop(f, stack);
                // Just to avoid the unused error.
                poped++; poped--;
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
                inst_dupp(f, stack);
                break;
            case OP_2DUP:
                inst_2dupp(f, stack);
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
                tmp_ptr = op->args[0];
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                if (inst_if(f, stack, tmp_ptr, self->instructions[tmp_ptr - 1]->op_code) && sim) {
                    if (debug->debug_jumps)
                        debug_jump(self, op, i, tmp_ptr);
                    i = tmp_ptr - 1; // - 1 to avoid the ++ of the for loop
                }
                break;
            case OP_ELSE:
                tmp_ptr = op->args[0];
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                assert(self->instructions[tmp_ptr]->op_code == OP_END);
                if (!sim)
                    inst_else(f, tmp_ptr);
                else
                    i = tmp_ptr - 1; // To avoid the ++ of the for loop.
                break;
            case OP_WHILE:
                if (!sim)
                    inst_while(f);
                break;
            case OP_DO:
                tmp_ptr = op->args[0];
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                if (inst_do(f, stack, tmp_ptr) && sim) {
                    if (debug->debug_jumps)
                        debug_jump(self, op, i, tmp_ptr);
                    i = tmp_ptr; // We want to jump to the `end` + 1, no need to substract because of for loop.
                }
                break;
            case OP_END:
                tmp_ptr = op->args[0];
                if (!sim)
                    inst_end(f, tmp_ptr + 1);
                else {
                    if (debug->debug_jumps)
                        debug_jump(self, op, i, tmp_ptr);
                    i = tmp_ptr; // We want to jump to the `end` + 1, no need to substract because of for loop.
                                 // When substracting : create an infinite loop.
                }
                break;
            case OP_MEM:
                inst_mem(f, stack);
                break;
            case OP_STORE:
                inst_store(f, stack, fake_mem);
                break;
            case OP_LOAD:
                inst_load(f, stack, fake_mem);
                break;
            case OP_SYSCALL0:
                inst_syscall(f, stack, fake_mem, 0);
                break;
            case OP_SYSCALL1:
                inst_syscall(f, stack, fake_mem, 1);
                break;
            case OP_SYSCALL2:
                err = inst_syscall(f, stack, fake_mem, 2);
                if (err < 256)      // Else it's not the exit syscall.
                    exit_found = 1;
                break;
            case OP_SYSCALL3:
                inst_syscall(f, stack, fake_mem, 3);
                break;
            case OP_SYSCALL4:
                inst_syscall(f, stack, fake_mem, 4);
                break;
            case OP_SYSCALL5:
                inst_syscall(f, stack, fake_mem, 5);
                break;
            case OP_SYSCALL6:
                inst_syscall(f, stack, fake_mem, 6);
                break;
            case OP_SHL:
                inst_shl(f, stack);
                break;
            case OP_SHR:
                inst_shr(f, stack);
                break;
            case OP_ORB:
                inst_orb(f, stack);
                break;
            case OP_ANDB:
                inst_andb(f, stack);
                break;
            default:
                // Unreachable.
                assert(0);
                break;
        }
        if (debug->enabled && sim) {
            if (debug->debug_stack)
                debug_stack(stack, op);
            if (debug->debug_memory)
                debug_memory(fake_mem, debug->mem_lim);
        }
    }
    if (!sim) {
        fprintf(f, "addr_%lld:\n", i);
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
