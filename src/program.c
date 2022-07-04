#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/stack.h"
#include "../include/token.h"
#include "../include/program.h"
#include "../include/debugger.h"
#include "../include/file_handler.h"
#include "../include/asm_functions.h"

static program_t *compile_program(token_list_t *tokens)
{
    uint64 cursor = 0;
    token_t *token = 0;
    program_t *program = (program_t *) malloc(sizeof(program_t));

    if (!program)
        return program;
    program->ops_len = tokens->tokens_len;
    program->ops = (op_t **)calloc(program->ops_len, sizeof(op_t *));
    for (; cursor < program->ops_len; cursor++) {
        token = tokens->tokens[cursor];
        program->ops[cursor] = compile_token_to_op(token);
        if (token->type == TOKEN_STR) {
            program->ops[cursor]->val.string = strdup(token->val.string);
        } else if (token->type == TOKEN_INT)
            program->ops[cursor]->val.integer = token->val.integer;
    }
    destroy_tokens(tokens);
    return program;
}

static program_t *preprocess_program(program_t *self, debugger_t const *debug)
{
    uint64 i = 0;
    op_t *op = 0;
    int64 op_pointer = 0;
    op_t *pointed_op = 0;
    stack_t *stack = new_stack();
    value_t tmp_value;

    if (!stack)
        return self;
    assert(COUNT_OPS == 35);
    for (; i < self->ops_len; i++) {
        op = self->ops[i];
        switch (op->type) {
            // `if` will have to point either to itself or an `else` address.
            case OP_IF:
                op->jmp = i;
                tmp_value.integer = op->jmp;
                push_onto_stack(stack, tmp_value);
                break;
            // `else` will have to point the `end` address.
            case OP_ELSE:
                // The `ìf` address.
                op_pointer = pop_from(stack).integer;
                // The `if` instruction.
                pointed_op = self->ops[op_pointer];
                // `else` can only be used in `if` blocks.
                assert(pointed_op->type == OP_IF);
                // `if` will points to the next instruction after `else`.
                // if not + 1, if would point to the `else` adress which points
                // to the end address. So the `else` body would not have been
                // executed.
                if (debug->debug_bindings)
                    debug_binding(pointed_op, op_pointer, "else next instruction", i + 1);
                pointed_op->jmp = i + 1;
                assert(self->ops[op_pointer]->jmp == pointed_op->jmp);
                // The `else` instruction points to itself for now.
                op->jmp = i;
                tmp_value.integer = op->jmp;
                push_onto_stack(stack, tmp_value);
                break;
            case OP_WHILE:
                tmp_value.integer = i;
                push_onto_stack(stack, tmp_value);
                break;
            // `do` will have to point the `end` address.
            case OP_DO:
                // The `while` address.
                op_pointer = pop_from(stack).integer;
                // The `while` instruction.
                pointed_op = self->ops[op_pointer];
                assert(pointed_op->type == OP_WHILE);
                // `do` now points to the `while` block.
                if (debug->debug_bindings)
                    debug_binding(op, i, OP_CODES[pointed_op->type], op_pointer);
                op->jmp = op_pointer;
                assert(self->ops[i]->jmp == op->jmp);
                tmp_value.integer = i;
                push_onto_stack(stack, tmp_value);
                break;
            case OP_END:
                op_pointer = pop_from(stack).integer;
                // printf("OP_POINTER : %lld\n", op_pointer);
                // if ((uint64) op_pointer >= self->ops_len)
                    // break;
                pointed_op = self->ops[op_pointer];
                if (pointed_op->type == OP_IF || pointed_op->type == OP_ELSE) {
                    // The block closed by `end` points to it.
                    pointed_op->jmp = i;
                    if (debug->debug_bindings)
                        debug_binding(pointed_op, op_pointer, OP_CODES[op->type], i);
                    assert(self->ops[op_pointer]->jmp == pointed_op->jmp);
                    // `end` points to the next instruction.
                    op->jmp = i;
                    assert(self->ops[i]->jmp == op->jmp);
                    if (debug->debug_bindings)
                        debug_binding(op, i, OP_CODES[self->ops[op->jmp]->type], op->jmp);
                } else if (pointed_op->type == OP_DO) {
                    // `end` points to `while`.
                    op->jmp = pointed_op->jmp;
                    assert(self->ops[i]->jmp == op->jmp);
                    if (debug->debug_bindings)
                        debug_binding(op, i, OP_CODES[self->ops[op->jmp]->type], op->jmp);
                    // `do` points now to the `end` block.
                    pointed_op->jmp = i;
                    assert(self->ops[op_pointer]->jmp == pointed_op->jmp);
                    if (debug->debug_bindings)
                        debug_binding(pointed_op, op_pointer, OP_CODES[op->type], i);
                } else {
                    printf("POINTED OP : %s\n", OP_CODES[pointed_op->type]);
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
    return self;
}

void clear_memory(char fake_mem[])
{
    uint64 i = 0;

    for (; i < MEMORY_CAPACITY; i++)
        fake_mem[i] = 0;
}

int run_program(token_list_t *self, int sim, debugger_t const *debug, char const *output)
{
    int err = 0;
    FILE* f = 0;
    uint64 i = 0;
    op_t *op = 0;
    int64 tmp_ptr = 0;
    int exit_found = 0;
    stack_t *stack = 0;
    program_t *program = 0;
    uint64 str_len = 0;
    uint64 str_size = 0;
    char fake_mem[MEMORY_CAPACITY];

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
    clear_memory(fake_mem);
    if (debug->enabled && debug->debug_tokens)
        debug_tokens(self);
    program = preprocess_program(compile_program(self), debug);
    for (; i < program->ops_len && !exit_found; i++) {
        op = program->ops[i];
        if (!sim)
            fprintf(f, "addr_%lld:\n", i);
        switch (op->type) {
            case OP_PUSH_INT:
                inst_push(f, stack, op->val.integer);
                break;
            case OP_PUSH_STR:
                str_len = strlen(op->val.string);
                inst_string(f, stack, str_size, op);
                strncpy(fake_mem + str_size, op->val.string, str_len);
                str_size += str_len;
                if (str_size > STRING_CAPACITY) {
                    printf("String buffer overflow !\n");
                    exit(1);
                }
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
                tmp_ptr = op->jmp;
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                if (inst_if(f, stack, tmp_ptr, program->ops[tmp_ptr - 1]->type) && sim) {
                    if (debug->debug_jumps)
                        debug_jump(program, op, i, tmp_ptr);
                    i = tmp_ptr - 1; // - 1 to avoid the ++ of the for loop
                }
                break;
            case OP_ELSE:
                tmp_ptr = op->jmp;
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                assert(program->ops[tmp_ptr]->type == OP_END);
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
                tmp_ptr = op->jmp;
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                if (inst_do(f, stack, tmp_ptr) && sim) {
                    if (debug->debug_jumps)
                        debug_jump(program, op, i, tmp_ptr);
                    i = tmp_ptr; // We want to jump to the `end` + 1, no need to substract because of for loop.
                }
                break;
            case OP_END:
                tmp_ptr = op->jmp;
                if (!sim)
                    inst_end(f, tmp_ptr + 1);
                else {
                    if (debug->debug_jumps)
                        debug_jump(program, op, i, tmp_ptr);
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
    if (sim)
        destroy_stack(stack);
    return err;
}
