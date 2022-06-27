#include <stdio.h>
#include "../include/asm_functions.h"

void asm_dump(FILE *fd)
{
    fprintf(fd, "_print_int:\n");
    fprintf(fd, "    mov r8, 7378697629483820647\n");
    fprintf(fd, "    sub rsp, 40\n");
    fprintf(fd, "    mov byte [rsp+31], 10\n");
    fprintf(fd, "    lea rcx, [rsp+30]\n");
    fprintf(fd, "_print_int_L1:\n");
    fprintf(fd, "    mov rax, rdi\n");
    fprintf(fd, "    mov rsi, rcx\n");
    fprintf(fd, "    sub rcx, 1\n");
    fprintf(fd, "    imul r8\n");
    fprintf(fd, "    mov rax, rdi\n");
    fprintf(fd, "    sar rax, 63\n");
    fprintf(fd, "    sar rdx, 2\n");
    fprintf(fd, "    sub rdx, rax\n");
    fprintf(fd, "    lea rax, [rdx+rdx*4]\n");
    fprintf(fd, "    add rax, rax\n");
    fprintf(fd, "    sub rdi, rax\n");
    fprintf(fd, "    add edi, 48\n");
    fprintf(fd, "    mov byte [rcx+1], dil\n");
    fprintf(fd, "    mov rdi, rdx\n");
    fprintf(fd, "    test rdx, rdx\n");
    fprintf(fd, "    jne _print_int_L1\n");
    fprintf(fd, "    lea rdx, [rsp+32]\n");
    fprintf(fd, "    mov edi, 1\n");
    fprintf(fd, "    sub rdx, rsi\n");
    fprintf(fd, "    mov rax, 1\n");
    fprintf(fd, "    syscall\n");
    fprintf(fd, "    add rsp, 40\n");
    fprintf(fd, "    ret\n");
}
