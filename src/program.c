#include <stdio.h>
#include <corth.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

extern char *strndup(const char *s, size_t n);

static void destroy_program(program_t *self)
{
    uint64 i = 0;

    if (!self)
        return;
    if (self->ops) {
        for (; i < self->ops_len; i++)
            free(self->ops[i]);
        free(self->ops);
    }
    free(self);
}

static program_t *compile_program(tok_lst_t *tokens)
{
    op_t *op = 0;
    uint64 cursor = 0;
    token_t *token = 0;
    program_t *program = (program_t *) malloc(sizeof(program_t));

    if (!program)
        return program;
    program->ops_len = tokens->tokens_len;
    program->ops = (op_t **) calloc(program->ops_len, sizeof(op_t *));
    for (; cursor < program->ops_len; cursor++) {
        token = tokens->tokens[cursor];
        op = consume_token(token);
        if (!op)
            break;
        if (token->type == TOKEN_STR) {
            op->val.string = token->val.string;
            op->is_val_str = 1;
        } else if (token->type == TOKEN_INT) {
            op->val.integer = token->val.integer;
            op->is_val_str = 0;
        }
        program->ops[cursor] = op;
    }
    return program;
}

static program_t *preprocess_program(program_t *self)
{
    uint64 i = 0;
    op_t *op = 0;
    int64 op_pointer = 0;
    op_t *pointed_op = 0;
    corth_stack_t *stack = new_stack();
    tok_val_t tmp_value;

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
                    assert(self->ops[op_pointer]->jmp == pointed_op->jmp);
                    // `end` points to the next instruction.
                    op->jmp = i;
                    assert(self->ops[i]->jmp == op->jmp);
                } else if (pointed_op->type == OP_DO) {
                    // `end` points to `while`.
                    op->jmp = pointed_op->jmp;
                    assert(self->ops[i]->jmp == op->jmp);
                    // `do` points now to the `end` block.
                    pointed_op->jmp = i;
                    assert(self->ops[op_pointer]->jmp == pointed_op->jmp);
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

int run_program(tok_lst_t *self, corth_mode_t mode, char const *output)
{
    int err = 0;
    FILE* f = 0;
    uint64 i = 0;
    op_t *op = 0;
    int64 tmp_ptr = 0;
    int exit_found = 0;
    corth_stack_t *stack = 0;
    program_t *program = 0;
    uint64 fake_str_i = 0;
    uint64 fake_str_len = 0;
    char fake_mem[STRING_CAPACITY + MEMORY_CAPACITY];
    int64 str_index = 0;
    char **fake_strings = 0;

    assert(COUNT_OPS == 35);
    if (!self)
        return 1;
    if (mode == SIMULATION && !(stack = new_stack()))
        return 1;
    if (mode == COMPILATION) {
        // Output must be specified for compilation.
        assert(output);
        f = fopen(output, "w");
        asm_header(f);
    }
    clear_memory(fake_mem);
    program = preprocess_program(compile_program(self));
    for (; i < program->ops_len && !exit_found; i++) {
        op = program->ops[i];
        if (mode == COMPILATION)
            fprintf(f, "\n\t.addr_%lld:\n", i);
        switch (op->type) {
            case OP_PUSH_INT:
                inst_push(f, stack, op->val.integer);
                break;
            case OP_PUSH_STR:
                inst_string(f, stack, str_index, op);
                fake_str_len = strlen(op->val.string);
                op->val.string = unescape(op->val.string);
                if (mode == SIMULATION) {
                    strncpy(fake_mem + fake_str_i, op->val.string, fake_str_len);
                    fake_str_i += fake_str_len;
                } else {
                    fake_strings = (char **) realloc(fake_strings, (str_index + 1) * sizeof(char *));
                    fake_strings[str_index++] = strndup(op->val.string, fake_str_len);
                }
                if (fake_str_i > STRING_CAPACITY && mode == SIMULATION) {
                    free(fake_strings);
                    free(stack);
                    fclose(f);
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
                if (inst_if(f, stack, tmp_ptr, program->ops[tmp_ptr - 1]->type) && mode == SIMULATION)
                    i = tmp_ptr - 1; // - 1 to avoid the ++ of the for loop
                break;
            case OP_ELSE:
                tmp_ptr = op->jmp;
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                assert(program->ops[tmp_ptr]->type == OP_END);
                if (mode == COMPILATION)
                    inst_else(f, tmp_ptr);
                else
                    i = tmp_ptr - 1; // To avoid the ++ of the for loop.
                break;
            case OP_WHILE:
                if (mode == COMPILATION)
                    inst_while(f);
                break;
            case OP_DO:
                tmp_ptr = op->jmp;
                // `preprocess_program` must be called.
                assert(tmp_ptr > 0);
                if (inst_do(f, stack, tmp_ptr) && mode == SIMULATION)
                    i = tmp_ptr; // We want to jump to the `end` + 1, no need to substract because of for loop.
                break;
            case OP_END:
                tmp_ptr = op->jmp;
                if (mode == COMPILATION)
                    inst_end(f, tmp_ptr + 1);
                else {
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
    }
    if (mode == COMPILATION) {
        fprintf(f, "\n\t.addr_%lld:\n", i);
        asm_footer(f);
        for (--str_index; str_index >= 0; str_index--) {
            fprintf(f, "str_%llu: db ", str_index); // %s, 0\n", str_index, serialized);
            for (int i = 0; fake_strings[str_index][i]; i++)
                fprintf(f, "%d, ", fake_strings[str_index][i]);
            fprintf(f, "0\n");
            free(fake_strings[str_index]);
        }
        free(fake_strings);
        fclose(f);
    }
    if (mode == SIMULATION)
        destroy_stack(stack);
    destroy_program(program);
    return err;
}