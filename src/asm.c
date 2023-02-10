#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <corth.h>

char *unescape(char *string)
{
    size_t i = 0;
    size_t j = 0;
    size_t str_len = strlen(string);
    char *escaped = (char *) calloc(str_len + 1, sizeof(char));

    if (!escaped)
        return string;
    for (; i <= str_len; i++) {
        if (string[i] == '\\') {
            switch(string[i + 1]) {
                case '\\':
                    escaped[j] = '\\';
                    break;
                case 'a':
                    escaped[j] = '\a';
                    break;
                case 'b':
                    escaped[j] = '\b';
                    break;
                case 't':
                    escaped[j] = '\t';
                    break;
                case 'n':
                    escaped[j] = '\n';
                    break;
                case 'v':
                    escaped[j] = '\v';
                    break;
                case 'f':
                    escaped[j] = '\f';
                    break;
                case 'r':
                    escaped[j] = '\r';
                    break;
            }
            i++;
        } else {
            escaped[j] = string[i];
        }
        j++;
    }
    string = strncpy(string, escaped, j);
    free(escaped);
    return string;
}

void asm_dump(FILE *f)
{
    fprintf(f, ";; -- DUMP --\n");
    fprintf(f, ";; -9223372036854775807 <= rdi <= 9223372036854775807\n");
    fprintf(f, "dump: ; Checks the number's nature. (n >= 0 ? / n <= 9 ?)\n");
    fprintf(f, "    push rbx\n");
    fprintf(f, "    mov rbx, rdi\n");
    fprintf(f, "    sub rsp, 16\n");
    fprintf(f, "    cmp rdi, 9\n");
    fprintf(f, "    lea rsi, [rsp+15]\n");
    fprintf(f, "    ja .L3\n");
    fprintf(f, ".L5: ; Puts the digit into the stdout.\n");
    fprintf(f, "    mov edi, 1\n");
    fprintf(f, "    lea r8d, [rbx+48]\n");
    fprintf(f, "    mov BYTE [rsp+15], r8b\n");
    fprintf(f, "    mov eax, edi\n");
    fprintf(f, "    mov edx, edi\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    add rsp, 16\n");
    fprintf(f, "    pop rbx\n");
    fprintf(f, "    ret\n");
    fprintf(f, ".L3: ; Puts the '-' and absolutize the number.\n");
    fprintf(f, "    test rdi, rdi\n");
    fprintf(f, "    jns .L4\n");
    fprintf(f, "    mov edi, 1\n");
    fprintf(f, "    mov BYTE [rsp+15], '-'\n");
    fprintf(f, "    mov eax, edi\n");
    fprintf(f, "    mov edx, edi\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    mov rax, rbx\n");
    fprintf(f, "    neg rax\n");
    fprintf(f, "    cmp rbx, -9\n");
    fprintf(f, "    mov rbx, rax\n");
    fprintf(f, "    jge .L5\n");
    fprintf(f, ".L4: ; Do the maths to extract a digit and loop again with the rest. \n");
    fprintf(f, "    mov rax, rbx\n");
    fprintf(f, "    mov ecx, 10\n");
    fprintf(f, "    cqo\n");
    fprintf(f, "    idiv rcx\n");
    fprintf(f, "    mov rdi, rax\n");
    fprintf(f, "    mov rbx, rdx\n");
    fprintf(f, "    call dump\n");
    fprintf(f, "    lea rsi, [rsp+15]\n");
    fprintf(f, "    jmp .L5\n");
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
    // OPS_MAP[OP_MEM - 2].word because OP_PUSH and OP_STRING are not supported.
    fprintf(f, "%s: resb %llu\n", BUILTINS[OP_MEM - 2].word, MEMORY_CAPACITY);
    fprintf(f, "segment .data\n");
}

void inst_push(FILE *f, corth_stack_t *stack, int64 n)
{
    assert(f || stack);
    tok_val_t val;

    if (stack) {
        val.integer = n;
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- PUSH INT --\n");
        fprintf(f, "    push %lld\n", n);
    }
}

void inst_string(FILE *f, corth_stack_t *stack, uint64 str_addr, op_t *op)
{
    assert(f || stack);
    tok_val_t val;

    if (stack) {
        val.integer = strlen(op->val.string);
        push_onto_stack(stack, val);
        if (op->addr == -1)
            op->addr = str_addr;
        val.integer = op->addr;
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- PUSH STR --\n");
        fprintf(f, "    push %llu\n", strlen(op->val.string));
        fprintf(f, "    push str_%llu\n", str_addr);
    }
}

tok_val_t inst_pop(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val;

    if (stack)
        return pop_from(stack);
    if (f) {
        fprintf(f, "    ;; -- POP --\n");
        fprintf(f, "    pop rdi\n");
    }
    val.integer = 0;
    return val;
}

void inst_plus(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;
    tok_val_t val;

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n1 + n2;
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- PLUS --\n");
        fprintf(f, "    pop rax ; n1\n");
        fprintf(f, "    pop rbx ; n2\n");
        fprintf(f, "    add rax, rbx\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_minus(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;
    tok_val_t val;

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n2 - n1;
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- MINUS --\n");
        fprintf(f, "    pop rax ; n1\n");
        fprintf(f, "    pop rbx ; n2\n");
        fprintf(f, "    sub rbx, rax\n");
        fprintf(f, "    push rbx\n");
    }
}

void inst_dump(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;

    if (stack) {
        n1 = pop_from(stack).integer;
        printf("%lld\n", n1);
    }
    if (f) {
        fprintf(f, "    ;; -- DUMP --\n");
        fprintf(f, "    pop rdi\n");
        fprintf(f, "    call dump\n");
    }
}

void inst_dupp(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val;

    if (stack) {
        val = pop_from(stack);
        push_onto_stack(stack, val);
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- DUP --\n");
        fprintf(f, "    pop rax\n");
        fprintf(f, "    push rax\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_2dupp(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val1;
    tok_val_t val2;

    if (stack) {
        val1 = pop_from(stack);
        val2 = pop_from(stack);
        push_onto_stack(stack, val2);
        push_onto_stack(stack, val1);
        push_onto_stack(stack, val2);
        push_onto_stack(stack, val1);
    }
    if (f) {
        fprintf(f, "    ;; -- 2DUP --\n");
        fprintf(f, "    pop rax\n");
        fprintf(f, "    pop rbx\n");
        fprintf(f, "    push rbx\n");
        fprintf(f, "    push rax\n");
        fprintf(f, "    push rbx\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_equal(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;
    tok_val_t val;

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n1 == n2;
        push_onto_stack(stack, val);
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

void inst_diff(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;
    tok_val_t val;

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n1 != n2;
        push_onto_stack(stack, val);
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

void inst_gt(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;
    tok_val_t val;

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n2 > n1;
        push_onto_stack(stack, val);
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

void inst_lt(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;
    tok_val_t val;

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n2 < n1;
        push_onto_stack(stack, val);
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

void inst_goet(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;
    tok_val_t val;

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n2 >= n1;
        push_onto_stack(stack, val);
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

void inst_loet(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    int64 n1 = 0;
    int64 n2 = 0;
    tok_val_t val;

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n2 <= n1;
        push_onto_stack(stack, val);
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

int inst_if(FILE *f, corth_stack_t *stack, uint64 end_addr, op_typ_t op_type)
{
    assert(f || stack);
    assert(op_type == OP_ELSE || op_type == OP_END);

    if (stack)
        return (pop_from(stack).integer == 0);
    if (f) {
        fprintf(f, "    ;; -- IF --\n");
        fprintf(f, "    pop rax\n");
        fprintf(f, "    test al, al\n");
        fprintf(f, "    ;; -- GOTO %s --\n", OP_CODES[op_type]);
        fprintf(f, "    jz addr_%llu\n", end_addr);
    }
    return 0;
}

void inst_else(FILE *f, uint64 end_addr)
{
    assert(f);
    fprintf(f, "    ;; -- GOTO END --\n");
    fprintf(f, "    jmp addr_%llu\n", end_addr + 1); // + 1 to go after the `end` block.
    fprintf(f, ";; -- ELSE --\n");
}

void inst_while(FILE *f)
{
    assert(f);
    fprintf(f, "    ;; -- WHILE --\n");
}

int inst_do(FILE *f, corth_stack_t *stack, uint64 end_addr)
{
    assert(f || stack);

    if (stack)
        return (pop_from(stack).integer == 0);
    if (f) {
        fprintf(f, "    ;; -- DO --\n");
        fprintf(f, "    pop rax\n");
        fprintf(f, "    test al, al\n");
        fprintf(f, "    ;; -- GOTO END --\n");
        fprintf(f, "    jz addr_%llu\n", end_addr + 1); // + 1 to go to the `end` + 1 instruction.
    }
    return 0;
}

void inst_end(FILE *f, uint64 next_addr)
{
    assert(f);

    fprintf(f, "    jmp addr_%llu\n", next_addr);
    fprintf(f, ";; -- END --\n");
}

void inst_mem(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val;

    if (stack) {
        val.integer = STRING_CAPACITY; // Get the pointer to the beginning of the memory.
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- MEM --\n");
        fprintf(f, "    push mem\n");
    }
}

void inst_store(FILE *f, corth_stack_t *stack, char *fake_mem)
{
    assert(f || stack);
    uint64 byte = 0;
    int64 address = 0;

    if (stack) {
        byte = pop_from(stack).integer;
        address = pop_from(stack).integer;
        fake_mem[address] = byte % 0xFF; // Only pushes the lowest bytes of the value, as in assembly.
    }
    if (f) {
        fprintf(f, "    ;; -- STORE --\n");
        fprintf(f, "    pop rbx ; contains the byte to store\n");
        fprintf(f, "    pop rax ; contains the mem pointer\n");
        fprintf(f, "    mov byte [rax], bl\n");
    }
}

void inst_load(FILE *f, corth_stack_t *stack, char *fake_mem)
{
    assert(f || stack);
    tok_val_t val;
    uint64 byte = 0;
    int64 address = 0;

    if (stack) {
        address = pop_from(stack).integer;
        byte = fake_mem[address];
        val.integer = byte;
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- LOAD --\n");
        fprintf(f, "    pop rbx ; contains the mem pointer\n");
        fprintf(f, "    xor rax, rax\n");
        fprintf(f, "    mov al, byte [rbx]\n");
        fprintf(f, "    push rax\n");
    }
}

int inst_syscall(FILE *f, corth_stack_t *stack, char *fake_mem, unsigned int vals_len)
{
    assert(f || stack);
    assert(vals_len <= 6);
    int64 rax = 0;
    int64 rdi = 0;
    int64 rsi = 0;
    int64 rdx = 0;
    // int64 r8 = 0; // Unused yet.
    // int64 r10 = 0; // Unused yet.
    // int64 r9 = 0; // Unused yet.

    if (stack) {
        if (vals_len == 2) {
            rax = pop_from(stack).integer;
            rdi = pop_from(stack).integer;
            if (rax == 60) {
                return rdi % 256;
            }
        }
        if (vals_len == 3) {
            rax = pop_from(stack).integer;
            rdi = pop_from(stack).integer;
            rsi = pop_from(stack).integer;
            rdx = pop_from(stack).integer;
            if (rax == 1) {
                write(rdi, unescape(&fake_mem[rsi]), rdx);
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
        if (vals_len >= 1)
            fprintf(f, "    pop rdi ; 1st val of the syscall\n");
        if (vals_len >= 2)
            fprintf(f, "    pop rsi ; 2nd val of the syscall\n");
        if (vals_len >= 3)
            fprintf(f, "    pop rdx ; 3rd val of the syscall\n");
        if (vals_len >= 4)
            fprintf(f, "    pop r10 ; 4th val of the syscall\n");
        if (vals_len >= 5)
            fprintf(f, "    pop r8  ; 5th val of the syscall\n");
        if (vals_len == 6)
            fprintf(f, "    pop r9  ; 6th val of the syscall\n");
        fprintf(f, "    syscall\n");
    }
    return 256;
}

void inst_shl(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val;
    int64 n1 = 0; // shifter
    int64 n2 = 0; // shifted

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n2 << n1;
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- SHL --\n");
        fprintf(f, "    pop rcx ; shifter\n");
        fprintf(f, "    pop rax ; shifted\n");
        fprintf(f, "    shl rax, cl\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_shr(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val;
    int64 n1 = 0; // shifter
    int64 n2 = 0; // shifted

    if (stack) {
        n1 = pop_from(stack).integer;
        n2 = pop_from(stack).integer;
        val.integer = n2 >> n1;
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- SHR --\n");
        fprintf(f, "    pop rcx ; shifter\n");
        fprintf(f, "    pop rax ; shifted\n");
        fprintf(f, "    shr rax, cl\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_orb(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val;
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n2 = pop_from(stack).integer;
        n1 = pop_from(stack).integer;
        val.integer = n1 | n2;
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- OR --\n");
        fprintf(f, "    pop rax ; n1\n");
        fprintf(f, "    pop rbx ; n1\n");
        fprintf(f, "    or rax, rbx\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_andb(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val;
    int64 n1 = 0;
    int64 n2 = 0;

    if (stack) {
        n2 = pop_from(stack).integer;
        n1 = pop_from(stack).integer;
        val.integer = n1 & n2;
        push_onto_stack(stack, val);
    }
    if (f) {
        fprintf(f, "    ;; -- AND --\n");
        fprintf(f, "    pop rax ; n1\n");
        fprintf(f, "    pop rbx ; n1\n");
        fprintf(f, "    and rax, rbx\n");
        fprintf(f, "    push rax\n");
    }
}

void inst_swap(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val1;
    tok_val_t val2;

    if (stack) {
        val1 = pop_from(stack);
        val2 = pop_from(stack);
        push_onto_stack(stack, val1);
        push_onto_stack(stack, val2);
    }
    if (f) {
        fprintf(f, "    ;; -- SWAP --\n");
        fprintf(f, "    pop rax\n");
        fprintf(f, "    pop rbx\n");
        fprintf(f, "    push rax\n");
        fprintf(f, "    push rbx\n");
    }
}

void inst_over(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val1;
    tok_val_t val2;

    if (stack) {
        val2 = pop_from(stack);
        val1 = pop_from(stack);
        push_onto_stack(stack, val1);
        push_onto_stack(stack, val2);
        push_onto_stack(stack, val1);
    }
    if (f) {
        fprintf(f, "    ;; -- OVER --\n");
        fprintf(f, "    pop rbx\n");
        fprintf(f, "    pop rax\n");
        fprintf(f, "    push rax\n");
        fprintf(f, "    push rbx\n");
        fprintf(f, "    push rax\n");
    }
}
