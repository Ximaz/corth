#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/ops.h"
#include "../include/types.h"
#include "../include/stack.h"
#include "../include/asm_functions.h"

void asm_dump(FILE *f)
{
    fprintf(f, "dump:\n");
    fprintf(f, "    mov r8, 7378697629483820647\n");
    fprintf(f, "    sub rsp, 40\n");
    fprintf(f, "    mov byte [rsp+31], 10\n");
    fprintf(f, "    lea rcx, [rsp+30]\n");
    fprintf(f, "dump_L1:\n");
    fprintf(f, "    mov rax, rdi\n");
    fprintf(f, "    mov rsi, rcx\n");
    fprintf(f, "    sub rcx, 1\n");
    fprintf(f, "    imul r8\n");
    fprintf(f, "    mov rax, rdi\n");
    fprintf(f, "    sar rax, 63\n");
    fprintf(f, "    sar rdx, 2\n");
    fprintf(f, "    sub rdx, rax\n");
    fprintf(f, "    lea rax, [rdx+rdx*4]\n");
    fprintf(f, "    add rax, rax\n");
    fprintf(f, "    sub rdi, rax\n");
    fprintf(f, "    add edi, 48\n");
    fprintf(f, "    mov byte [rcx+1], dil\n");
    fprintf(f, "    mov rdi, rdx\n");
    fprintf(f, "    test rdx, rdx\n");
    fprintf(f, "    jne dump_L1\n");
    fprintf(f, "    lea rdx, [rsp+32]\n");
    fprintf(f, "    mov edi, 1\n");
    fprintf(f, "    sub rdx, rsi\n");
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    add rsp, 40\n");
    fprintf(f, "    ret\n");
}

void asm_header(FILE *f)
{
    fprintf(f, "BITS 64\n");
    fprintf(f, "segment .text\n");
    asm_dump(f);
    fprintf(f, "global _start\n");
    fprintf(f, "_start:\n");
}

void asm_footer(FILE *f)
{
    fprintf(f, "    ;; -- EXIT --\n");
    fprintf(f, "    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "segment .bss\n");
    fprintf(f, "%s: resb %llu\n", OPS_MAP[OP_MEM - 1].sym, MEMORY_CAPACITY);
}

void inst_push(FILE *f, stack_t *stack, int64 n)
{
    assert(f || stack);

    if (stack) {
        push_onto_stack(stack, n);
    }
    if (f) {
        fprintf(f, "    ;; -- PUSH --\n");
        fprintf(f, "    push %lld\n", n);
    }
}

void inst_plus(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n1 = pop_from(stack);
        n2 = pop_from(stack);
        push_onto_stack(stack, n1 + n2);
    }
    if (f) {
        fprintf(f, "    ;; -- PLUS --\n");
        fprintf(f, "    pop rax ; n1\n");
        fprintf(f, "    pop rbx ; n2\n");
        fprintf(f, "    add rax, rbx\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_minus(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n1 = pop_from(stack);
        n2 = pop_from(stack);
        push_onto_stack(stack, n2 - n1);
    }
    if (f) {
        fprintf(f, "    ;; -- MINUS --\n");
        fprintf(f, "    pop rax ; n1\n");
        fprintf(f, "    pop rbx ; n2\n");
        fprintf(f, "    sub rbx, rax\n");
        fprintf(f, "    push rbx\n");
    }
}

void inst_dump(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;

    if (stack) {
        n1 = pop_from(stack);
        printf("%lld\n", n1);
    }
    if (f) {
        fprintf(f, "    ;; -- DUMP --\n");
        fprintf(f, "    pop rdi\n");
        fprintf(f, "    call dump\n");
    }
}

void inst_dupp(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;

    if (stack) {
        n1 = pop_from(stack);
        push_onto_stack(stack, n1);
        push_onto_stack(stack, n1);
    }
    if (f) {
        fprintf(f, "    ;; -- DUP --\n");
        fprintf(f, "    pop rax\n");
        fprintf(f, "    push rax\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_equal(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n1 = pop_from(stack);
        n2 = pop_from(stack);
        push_onto_stack(stack, n1 == n2);
    }
    if (f) {
        fprintf(f, "    ;; -- EQUAL --\n");
        fprintf(f, "    pop rax      ; n1\n");
        fprintf(f, "    pop rbx      ; n2\n");
        fprintf(f, "    sub rbx, rax ; n2 - n1\n");
        fprintf(f, "    xor rax, rax\n");
        fprintf(f, "    test rbx, rbx\n");
        fprintf(f, "    sete al\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_diff(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n1 = pop_from(stack);
        n2 = pop_from(stack);
        push_onto_stack(stack, n1 != n2);
    }
    if (f) {
        fprintf(f, "    ;; -- DIFF --\n");
        fprintf(f, "    pop rax      ; n1\n");
        fprintf(f, "    pop rbx      ; n2\n");
        fprintf(f, "    sub rbx, rax ; n2 - n1\n");
        fprintf(f, "    xor rax, rax\n");
        fprintf(f, "    test rbx, rbx\n");
        fprintf(f, "    setnz al\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_gt(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n1 = pop_from(stack);
        n2 = pop_from(stack);
        push_onto_stack(stack, n2 > n1);
    }
    if (f) {
        fprintf(f, "    ;; -- GREATER THAN --\n");
        fprintf(f, "    pop rcx ; n1\n");
        fprintf(f, "    pop rbx ; n2\n");
        fprintf(f, "    xor rax, rax\n");
        fprintf(f, "    cmp rbx, rcx\n");
        fprintf(f, "    seta al\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_lt(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n1 = pop_from(stack);
        n2 = pop_from(stack);
        push_onto_stack(stack, n2 < n1);
    }
    if (f) {
        fprintf(f, "    ;; -- LESS THAN --\n");
        fprintf(f, "    pop rcx ; n1\n");
        fprintf(f, "    pop rbx ; n2\n");
        fprintf(f, "    xor rax, rax\n");
        fprintf(f, "    cmp rbx, rcx\n");
        fprintf(f, "    setb al\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_goet(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n1 = pop_from(stack);
        n2 = pop_from(stack);
        push_onto_stack(stack, n2 >= n1);
    }
    if (f) {
        fprintf(f, "    ;; -- GREATER OR EQUAL THAN --\n");
        fprintf(f, "    pop rcx ; n2\n");
        fprintf(f, "    pop rbx ; n1\n");
        fprintf(f, "    xor rax, rax\n");
        fprintf(f, "    cmp rbx, rcx\n");
        fprintf(f, "    setae al\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_loet(FILE *f, stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n1 = pop_from(stack);
        n2 = pop_from(stack);
        push_onto_stack(stack, n2 <= n1);
    }
    if (f) {
        fprintf(f, "    ;; -- LESS OR EQUAL THAN --\n");
        fprintf(f, "    pop rcx ; n1\n");
        fprintf(f, "    pop rbx ; n2\n");
        fprintf(f, "    xor rax, rax\n");
        fprintf(f, "    cmp rbx, rcx\n");
        fprintf(f, "    setbe al\n");
        fprintf(f, "    push rax\n");
    }
}

int inst_if(FILE *f, stack_t *stack, uint64 end_addr, int is_else)
{
    assert(f || stack);
    int64 n1 = 0;

    if (stack) {
        n1 = pop_from(stack);
        return (n1 == 0);
    }
    if (f) {
        fprintf(f, "    ;; -- IF --\n");
        fprintf(f, "    pop rax\n");
        fprintf(f, "    test rax, rax\n");
        fprintf(f, "    ;; -- GOTO %s --\n", is_else ? "ELSE" : "END");
        fprintf(f, "    jz addr_%llu\n", end_addr);
    }
    return 0;
}

void inst_else(FILE *f, uint64 end_addr)
{
    assert(f);
    fprintf(f, "    ;; -- GOTO END --\n");
    fprintf(f, "    jmp addr_%llu\n", end_addr);
    fprintf(f, ";; -- ELSE --\n");
}

void inst_while(FILE *f)
{
    assert(f);
    fprintf(f, "    ;; -- WHILE --\n");
}

int inst_do(FILE *f, stack_t *stack, uint64 end_addr)
{
    assert(f || stack);
    int64 n1 = 0;

    if (stack) {
        n1 = pop_from(stack);
        return (n1 == 0);
    }
    if (f) {
        fprintf(f, "    ;; -- DO --\n");
        fprintf(f, "    pop rax\n");
        fprintf(f, "    test rax, rax\n");
        fprintf(f, "    ;; -- GOTO END --\n");
        fprintf(f, "    jz addr_%llu\n", end_addr);
    }
    return 0;
}

void inst_end(FILE *f, uint64 end_addr, uint64 next_addr)
{
    assert(f);

    if (end_addr != next_addr)
        fprintf(f, "    jmp addr_%llu\n", next_addr);
    else
        fprintf(f, "    jmp addr_%llu\n", end_addr);
    fprintf(f, ";; -- END --\n");
}

void inst_mem(FILE *f, stack_t *stack)
{
    assert(f || stack);

    if (stack) {
        push_onto_stack(stack, 0);
    }
    if (f) {
        fprintf(f, "    ;; -- MEM --\n");
        fprintf(f, "    push mem\n");
    }
}

void inst_store(FILE *f, stack_t *stack, unsigned char *fake_mem)
{
    assert(f || stack);
    uint64 byte = 0;
    uint64 address = 0;


    if (stack) {
        byte = pop_from(stack);
        address = pop_from(stack);
        fake_mem[address] = byte % 0xFF; // Only pushes the lowest bytes of the value, as in assembly.
    }
    if (f) {
        fprintf(f, "    ;; -- STORE --\n");
        fprintf(f, "    pop rbx ; contains the byte to store\n");
        fprintf(f, "    pop rax ; contains the mem pointer\n");
        fprintf(f, "    mov byte [rax], bl\n");
    }
}

void inst_load(FILE *f, stack_t *stack, unsigned char *fake_mem)
{
    assert(f || stack);
    uint64 byte = 0;
    uint64 address = 0;

    if (stack) {
        address = pop_from(stack);
        byte = fake_mem[address];
        push_onto_stack(stack, byte);
    }
    if (f) {
        fprintf(f, "    ;; -- LOAD --\n");
        fprintf(f, "    pop rbx ; contains the mem pointer\n");
        fprintf(f, "    xor rax, rax\n");
        fprintf(f, "    mov al, byte [rbx]\n");
        fprintf(f, "    push rax\n");
    }
}

int inst_syscall(FILE *f, stack_t *stack, unsigned char *fake_mem, unsigned int args_len)
{
    assert(f || stack);
    assert(args_len <= 6);
    int64 rax = 0;
    int64 rdi = 0;
    int64 rsi = 0;
    int64 rdx = 0;
    // int64 r8 = 0; // Unused yet.
    // int64 r10 = 0; // Unused yet.
    // int64 r9 = 0; // Unused yet.

    if (stack) {
        if (args_len == 2) {
            rax = pop_from(stack);
            rdi = pop_from(stack);
            if (rax == 60) {
                return rdi % 256;
            }
        }
        if (args_len == 3) {
            rax = pop_from(stack);
            rdi = pop_from(stack);
            rsi = pop_from(stack);
            rdx = pop_from(stack);
            if (rax == 1) {
                write(rdi, &fake_mem[rsi], rdx);
                return 256;
            } else {
                // NOT IMPLEMENTED YET !
                assert(0);
            }
        }
        // NOT IMPLEMENTED YET !
        assert(0);
    }
    if (f) {
        fprintf(f, "    ;; -- SYSCALL --\n");
        fprintf(f, "    pop rax ; the syscall_id\n");
        if (args_len >= 1)
            fprintf(f, "    pop rdi ; 1st arg of the syscall\n");
        if (args_len >= 2)
            fprintf(f, "    pop rsi ; 2nd arg of the syscall\n");
        if (args_len >= 3)
            fprintf(f, "    pop rdx ; 3rd arg of the syscall\n");
        if (args_len >= 4)
            fprintf(f, "    pop r10 ; 4th arg of the syscall\n");
        if (args_len >= 5)
            fprintf(f, "    pop r8  ; 5th arg of the syscall\n");
        if (args_len == 6)
            fprintf(f, "    pop r9  ; 6th arg of the syscall\n");
        fprintf(f, "    syscall\n");
    }
    return 256;
}
