#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/util.h"
#include "../include/program.h"
#include "../include/file_handler.h"
#include "../include/asm_functions.h"


int compile(program_t *program, char const *output)
{
    FILE* fd = 0;
    uint64 i = 0;
    int64 *op = NULL;

    assert(COUNT_OPS == SUPPORTED_INSTRUCTIONS);
    if (!program)
        return 1;
    fd = open_file(output, "w");
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
