#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "include/ops.h"
#include "include/util.h"
#include "include/stack.h"
#include "include/program.h"
#include "include/asm_functions.h"


static uint64 const SUPPORTED_INSTRUCTIONS = 5;


int simulate(program_t *program)
{
    int err = 0;
    uint64 i = 0;
    int64 n1 = 0;
    int64 n2 = 0;
    int64 *op = NULL;
    stack_t *stack;

    assert(COUNT_OPS == SUPPORTED_INSTRUCTIONS);
    if (!program)
        return 1;
    stack = new_stack();
    if (!stack)
        return 1;
    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op[0]) {
            case OP_PUSH:
                push_onto_stack(stack, op[1]);
                break;
            case OP_PLUS:
                n1 = pop_from(stack);
                n2 = pop_from(stack);
                push_onto_stack(stack, n1 + n2);
                break;
            case OP_MINUS:
                n1 = pop_from(stack);
                n2 = pop_from(stack);
                push_onto_stack(stack, n2 - n1);
                break;
            case OP_DUMP:
                n1 = pop_from(stack);
                printf("%lld\n", n1);
                break;
            case OP_HALT:
                err = pop_from(stack);
                break;
            default:
                printf("Unreachable\n");
                break;
        }
    }
    destroy_stack(stack);
    return err;
}

int compile(program_t *program, char const *output)
{
    FILE* fd = 0;
    uint64 i = 0;
    int64 *op = NULL;

    assert(COUNT_OPS == SUPPORTED_INSTRUCTIONS);
    if (!program)
        return 1;
    fd = fopen(output, "w");
    if (!fd)
        return 1;
    fprintf(fd, "BITS 64\n");
    fprintf(fd, "segment .text\n");
    asm_dump(fd);
    fprintf(fd, "global _start\n");
    fprintf(fd, "_start:\n");
    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op[0]) {
            case OP_PUSH:
                fprintf(fd, "    ;; -- PUSH --\n");
                fprintf(fd, "    push %lld\n", op[1]);
                break;
            case OP_PLUS:
                fprintf(fd, "    ;; -- PLUS --\n");
                fprintf(fd, "    pop rax\n");
                fprintf(fd, "    pop rbx\n");
                fprintf(fd, "    add rax, rbx\n");
                fprintf(fd, "    push rax\n");
                break;
            case OP_MINUS:
                fprintf(fd, "    ;; -- MINUS --\n");
                fprintf(fd, "    pop rax\n");
                fprintf(fd, "    pop rbx\n");
                fprintf(fd, "    sub rbx, rax\n");
                fprintf(fd, "    push rbx\n");
                break;
            case OP_DUMP:
                fprintf(fd, "    ;; -- DUMP --\n");
                fprintf(fd, "    pop rdi\n");
                fprintf(fd, "    call _print_int\n");
                break;
            case OP_HALT:
                fprintf(fd, "    ;; -- HALT --\n");
                fprintf(fd, "    mov rax, 60\n");
                fprintf(fd, "    pop rdi\n");
                fprintf(fd, "    syscall\n");
                fprintf(fd, "    retn\n");
                break;
            default:
                printf("Unreachable\n");
                break;
        }
    }
    fclose(fd);
    return 0;
}


program_t *hardcoded_program(void)
{
    program_t *program = new_program();

    push_instruction(program, push(34));
    push_instruction(program, push(35));
    push_instruction(program, plus());
    push_instruction(program, dump());
    push_instruction(program, push(500));
    push_instruction(program, push(80));
    push_instruction(program, minus());
    push_instruction(program, dump());
    push_instruction(program, push(0));
    push_instruction(program, halt());
    return program;
}

void usage(char const *binary_name)
{
    printf("Usage : %s <SUBCOMMAND> program\n", binary_name);
    printf("SUBCOMMANDS :\n");
    printf("    sim        Simulate the program\n");
    printf("    com        Compile the program\n");
}

int main(int argc, char *const *argv)
{
    int err = 0;
    char *subcommand = 0;

    if (argc < 2) {
        usage(argv[0]);
        printf("ERROR: No subcommand is provided.\n");
        return 1;
    }
    subcommand = argv[1];
    if (strcmp(subcommand, "sim") == 0) {
        program_t *program = hardcoded_program();
        err = simulate(program);
        destroy_program(program);
    } else if (strcmp(subcommand, "com") == 0) {
        program_t *program = hardcoded_program();
        err = compile(program, "output.asm");
        destroy_program(program);
    } else {
        usage(argv[0]);
        printf("Invalid subcommand is provided.\n");
        return 1;
    }
    return err;
}
