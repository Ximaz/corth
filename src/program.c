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

static tokens_t *preprocess_program(tokens_t *program, debugger_t const *debug)
{
    uint64 i = 0;
    inst_t *op = 0;
    int64 op_pointer = 0;
    inst_t *pointed_op = 0;
    inst_arg_t tmp_arg;
    // inst_t *tmp = 0;
    // int64 tmp_pointer = 0;
    stack_t *stack = new_stack();

    assert(COUNT_OPS == 35);
    for (; i < program->tokens_len; i++) {
        op = program->tokens[i]->instruction;
        switch (op->op_code) {
            // `if` will have to point either to itself or an `else` address.
            case OP_IF:
                op->args[0].ptr = i;
                push_onto_stack(stack, op->args[0]);
                break;
            // `else` will have to point the `end` address.
            case OP_ELSE:
                // The `ìf` address.
                op_pointer = pop_from(stack).ptr;
                // The `if` instruction.
                pointed_op = program->tokens[op_pointer]->instruction;
                // `else` can only be used in `if` blocks.
                assert(pointed_op->op_code == OP_IF);
                // `if` will points to the next instruction after `else`.
                // if not + 1, if would point to the `else` adress which points
                // to the end address. So the `else` body would not have been
                // executed.
                if (debug->debug_bindings)
                    debug_binding(pointed_op, op_pointer, "else next instruction", i + 1);
                pointed_op->args[0].ptr = i + 1;
                assert(program->tokens[op_pointer]->instruction->args[0].ptr == pointed_op->args[0].ptr);
                // The `else` instruction points to itself for now.
                op->args[0].ptr = i;
                push_onto_stack(stack, op->args[0]);
                break;
            case OP_WHILE:
                clear_arg(&tmp_arg);
                tmp_arg.ptr = i;
                push_onto_stack(stack, tmp_arg);
                break;
            // `do` will have to point the `end` address.
            case OP_DO:
                // The `while` address.
                op_pointer = pop_from(stack).ptr;
                // The `while` instruction.
                pointed_op = program->tokens[op_pointer]->instruction;
                assert(pointed_op->op_code == OP_WHILE);
                // `do` now points to the `while` block.
                if (debug->debug_bindings)
                    debug_binding(op, i, OP_CODES[pointed_op->op_code], op_pointer);
                op->args[0].ptr = op_pointer;
                assert(program->tokens[i]->instruction->args[0].ptr == op->args[0].ptr);
                clear_arg(&tmp_arg);
                tmp_arg.ptr = i;
                push_onto_stack(stack, tmp_arg);
                break;
            case OP_END:
                op_pointer = pop_from(stack).ptr;
                // printf("OP_POINTER : %lld\n", op_pointer);
                // if ((uint64) op_pointer >= program->tokens_len)
                    // break;
                pointed_op = program->tokens[op_pointer]->instruction;
                if (pointed_op->op_code == OP_IF || pointed_op->op_code == OP_ELSE) {
                    // The block closed by `end` points to it.
                    pointed_op->args[0].ptr = i;
                    if (debug->debug_bindings)
                        debug_binding(pointed_op, op_pointer, OP_CODES[op->op_code], i);
                    assert(program->tokens[op_pointer]->instruction->args[0].ptr == pointed_op->args[0].ptr);
                    // `end` points to the next instruction.
                    op->args[0].ptr = i;
                    assert(program->tokens[i]->instruction->args[0].ptr == op->args[0].ptr);
                    if (debug->debug_bindings)
                        debug_binding(op, i, OP_CODES[program->tokens[op->args[0].ptr]->instruction->op_code], op->args[0].ptr);
                } else if (pointed_op->op_code == OP_DO) {
                    // `end` points to `while`.
                    op->args[0].ptr = pointed_op->args[0].ptr;
                    assert(program->tokens[i]->instruction->args[0].ptr == op->args[0].ptr);
                    if (debug->debug_bindings)
                        debug_binding(op, i, OP_CODES[program->tokens[op->args[0].ptr]->instruction->op_code], op->args[0].ptr);
                    // `do` points now to the `end` block.
                    pointed_op->args[0].ptr = i;
                    assert(program->tokens[op_pointer]->instruction->args[0].ptr == pointed_op->args[0].ptr);
                    if (debug->debug_bindings)
                        debug_binding(pointed_op, op_pointer, OP_CODES[op->op_code], i);
                } else {
                    printf("POINTED OP : %s\n", OP_CODES[pointed_op->op_code]);
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

void clear_memory(unsigned char fake_mem[])
{
    uint64 i = 0;

    for (; i < MEMORY_CAPACITY; i++)
        fake_mem[i] = 0;
}

int run_program(tokens_t *self, int sim, debugger_t const *debug, char const *output)
{
    int err = 0;
    FILE* f = 0;
    uint64 i = 0;
    inst_t *op = 0;
    int64 tmp_ptr = 0;
    stack_t *stack = 0;
    // inst_arg_t poped;
    int exit_found = 0;
    unsigned char fake_mem[MEMORY_CAPACITY];

    assert(COUNT_OPS == 35);
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
    if (debug->debug_tokens)
        debug_tokens(self);
    clear_memory(fake_mem);
    self = preprocess_program(self, debug);
    // debug_program(self);
    if (sim && debug->enabled && debug->debug_stack)
        debug_stack(stack, 0);
    for (; i < self->tokens_len && !exit_found; i++) {
        op = self->tokens[i]->instruction;
        if (!sim)
            fprintf(f, "addr_%lld:\n", i);
        switch (op->op_code) {
            case OP_PUSH:
                inst_push(f, stack, op->args[0].integer);
                break;
            case OP_STRING:
                inst_string(f, stack, op->args[0].word);
                break;
            case OP_POP:
                // For now, poped won't do anthing else than poping.
                inst_pop(f, stack);
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
                tmp_ptr = op->args[0].ptr;
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                if (inst_if(f, stack, tmp_ptr, self->tokens[tmp_ptr - 1]->instruction->op_code) && sim) {
                    if (debug->debug_jumps)
                        debug_jump(self, op, i, tmp_ptr);
                    i = tmp_ptr - 1; // - 1 to avoid the ++ of the for loop
                }
                break;
            case OP_ELSE:
                tmp_ptr = op->args[0].ptr;
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                assert(self->tokens[tmp_ptr]->instruction->op_code == OP_END);
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
                tmp_ptr = op->args[0].ptr;
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                if (inst_do(f, stack, tmp_ptr) && sim) {
                    if (debug->debug_jumps)
                        debug_jump(self, op, i, tmp_ptr);
                    i = tmp_ptr; // We want to jump to the `end` + 1, no need to substract because of for loop.
                }
                break;
            case OP_END:
                tmp_ptr = op->args[0].ptr;
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
            case OP_SWAP:
                inst_swap(f, stack);
                break;
            case OP_OVER:
                inst_over(f, stack);
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
