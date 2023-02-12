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
    fprintf(f, "\t;; -- DUMP --\n");
    fprintf(f, "\t;; -9223372036854775807 <= rdi <= 9223372036854775807\n");
    fprintf(f, "dump:\n");
    fprintf(f, "\tpush rbx\n");
    fprintf(f, "\tmov rbx, rdi\n");
    fprintf(f, "\tsub rsp, 16\n");
    fprintf(f, "\tcmp rdi, 9\n");
    fprintf(f, "\tlea rsi, [rsp+15]\n");
    fprintf(f, "\tja .L3\n");
    fprintf(f, "\n\t.L5: ; Puts the digit into the stdout.\n");
    fprintf(f, "\tmov edi, 1\n");
    fprintf(f, "\tlea r8d, [rbx+48]\n");
    fprintf(f, "\tmov BYTE [rsp+15], r8b\n");
    fprintf(f, "\tmov eax, edi\n");
    fprintf(f, "\tmov edx, edi\n");
    fprintf(f, "\tsyscall\n");
    fprintf(f, "\tadd rsp, 16\n");
    fprintf(f, "\tpop rbx\n");
    fprintf(f, "\tret\n");
    fprintf(f, "\n\t.L3: ; Puts the '-' and absolutize the number.\n");
    fprintf(f, "\ttest rdi, rdi\n");
    fprintf(f, "\tjns .L4\n");
    fprintf(f, "\tmov edi, 1\n");
    fprintf(f, "\tmov BYTE [rsp+15], '-'\n");
    fprintf(f, "\tmov eax, edi\n");
    fprintf(f, "\tmov edx, edi\n");
    fprintf(f, "\tsyscall\n");
    fprintf(f, "\tmov rax, rbx\n");
    fprintf(f, "\tneg rax\n");
    fprintf(f, "\tcmp rbx, -9\n");
    fprintf(f, "\tmov rbx, rax\n");
    fprintf(f, "\tjge .L5\n");
    fprintf(f, "\n\t.L4: ; Do the maths to extract a digit and loop again with the rest. \n");
    fprintf(f, "\tmov rax, rbx\n");
    fprintf(f, "\tmov ecx, 10\n");
    fprintf(f, "\tcqo\n");
    fprintf(f, "\tidiv rcx\n");
    fprintf(f, "\tmov rdi, rax\n");
    fprintf(f, "\tmov rbx, rdx\n");
    fprintf(f, "\tcall dump\n");
    fprintf(f, "\tlea rsi, [rsp+15]\n");
    fprintf(f, "\tjmp .L5\n\n");
}

void asm_header(FILE *f)
{
    fprintf(f, "\tBITS 64\n\n");
    fprintf(f, "\tsection .text\n");
    fprintf(f, "\tglobal _start\n\n");
    asm_dump(f);
    fprintf(f, "_start:");
}

void asm_footer(FILE *f)
{
    fprintf(f, "\t;; -- EXIT --\n");
    fprintf(f, "\tmov rax, 60\n");
    fprintf(f, "\txor rdi, rdi\n");
    fprintf(f, "\tsyscall\n\n");
    fprintf(f, "\tsection .bss\n");
    // BUILTINS doesn't contain OP_PUSH_INT nor OP_PUSH_STR, so the actual index is
    // OP_MEM - 2.
    fprintf(f, "%s: resb %llu\n\n", BUILTINS[OP_MEM - 2].word, MEMORY_CAPACITY);
    fprintf(f, "\tsection .data\n");
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
        fprintf(f, "\t;; -- PUSH INT --\n");
        fprintf(f, "\tpush %lld\n", n);
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
        fprintf(f, "\t;; -- PUSH STR --\n");
        fprintf(f, "\tpush %lu\n", strlen(op->val.string));
        fprintf(f, "\tpush str_%llu\n", str_addr);
    }
}

tok_val_t inst_pop(FILE *f, corth_stack_t *stack)
{
    assert(f || stack);
    tok_val_t val;

    if (stack)
        return pop_from(stack);
    if (f) {
        fprintf(f, "\t;; -- POP --\n");
        fprintf(f, "\tpop rdi\n");
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
        fprintf(f, "\t;; -- PLUS --\n");
        fprintf(f, "\tpop rax ; n1\n");
        fprintf(f, "\tpop rbx ; n2\n");
        fprintf(f, "\tadd rax, rbx\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- MINUS --\n");
        fprintf(f, "\tpop rax ; n1\n");
        fprintf(f, "\tpop rbx ; n2\n");
        fprintf(f, "\tsub rbx, rax\n");
        fprintf(f, "\tpush rbx\n");
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
        fprintf(f, "\t;; -- DUMP --\n");
        fprintf(f, "\tpop rdi\n");
        fprintf(f, "\tcall dump\n");
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
        fprintf(f, "\t;; -- DUP --\n");
        fprintf(f, "\tpop rax\n");
        fprintf(f, "\tpush rax\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- 2DUP --\n");
        fprintf(f, "\tpop rax\n");
        fprintf(f, "\tpop rbx\n");
        fprintf(f, "\tpush rbx\n");
        fprintf(f, "\tpush rax\n");
        fprintf(f, "\tpush rbx\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- EQUAL --\n");
        fprintf(f, "\tpop rax      ; n1\n");
        fprintf(f, "\tpop rbx      ; n2\n");
        fprintf(f, "\tsub rbx, rax ; n2 - n1\n");
        fprintf(f, "\txor rax, rax\n");
        fprintf(f, "\ttest rbx, rbx\n");
        fprintf(f, "\tsete al\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- DIFF --\n");
        fprintf(f, "\tpop rax      ; n1\n");
        fprintf(f, "\tpop rbx      ; n2\n");
        fprintf(f, "\tsub rbx, rax ; n2 - n1\n");
        fprintf(f, "\txor rax, rax\n");
        fprintf(f, "\ttest rbx, rbx\n");
        fprintf(f, "\tsetnz al\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- GREATER THAN --\n");
        fprintf(f, "\tpop rcx ; n1\n");
        fprintf(f, "\tpop rbx ; n2\n");
        fprintf(f, "\txor rax, rax\n");
        fprintf(f, "\tcmp rbx, rcx\n");
        fprintf(f, "\tseta al\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- LESS THAN --\n");
        fprintf(f, "\tpop rcx ; n1\n");
        fprintf(f, "\tpop rbx ; n2\n");
        fprintf(f, "\txor rax, rax\n");
        fprintf(f, "\tcmp rbx, rcx\n");
        fprintf(f, "\tsetb al\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- GREATER OR EQUAL THAN --\n");
        fprintf(f, "\tpop rcx ; n2\n");
        fprintf(f, "\tpop rbx ; n1\n");
        fprintf(f, "\txor rax, rax\n");
        fprintf(f, "\tcmp rbx, rcx\n");
        fprintf(f, "\tsetae al\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- LESS OR EQUAL THAN --\n");
        fprintf(f, "\tpop rcx ; n1\n");
        fprintf(f, "\tpop rbx ; n2\n");
        fprintf(f, "\txor rax, rax\n");
        fprintf(f, "\tcmp rbx, rcx\n");
        fprintf(f, "\tsetbe al\n");
        fprintf(f, "\tpush rax\n");
    }
}

int inst_if(FILE *f, corth_stack_t *stack, uint64 end_addr, op_typ_t op_type)
{
    assert(f || stack);
    assert(op_type == OP_ELSE || op_type == OP_END);

    if (stack)
        return (pop_from(stack).integer == 0);
    if (f) {
        fprintf(f, "\t;; -- IF --\n");
        fprintf(f, "\tpop rax\n");
        fprintf(f, "\ttest al, al\n");
        fprintf(f, "\t;; -- GOTO %s --\n", OP_CODES[op_type]);
        fprintf(f, "\tjz .addr_%llu\n", end_addr);
    }
    return 0;
}

void inst_else(FILE *f, uint64 end_addr)
{
    assert(f);
    fprintf(f, "\t;; -- GOTO END --\n");
    fprintf(f, "\tjmp .addr_%llu\n", end_addr + 1); // + 1 to go after the `end` block.
    fprintf(f, "\n\t;; -- ELSE --\n");
}

void inst_while(FILE *f)
{
    assert(f);
    fprintf(f, "\t;; -- WHILE --\n");
}

int inst_do(FILE *f, corth_stack_t *stack, uint64 end_addr)
{
    assert(f || stack);

    if (stack)
        return (pop_from(stack).integer == 0);
    if (f) {
        fprintf(f, "\t;; -- DO --\n");
        fprintf(f, "\tpop rax\n");
        fprintf(f, "\ttest al, al\n");
        fprintf(f, "\t;; -- GOTO END --\n");
        fprintf(f, "\tjz .addr_%llu\n", end_addr + 1); // + 1 to go to the `end` + 1 instruction.
    }
    return 0;
}

void inst_end(FILE *f, uint64 next_addr)
{
    assert(f);

    fprintf(f, "\tjmp .addr_%llu\n", next_addr);
    fprintf(f, "\t;; -- END --\n");
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
        fprintf(f, "\t;; -- MEM --\n");
        fprintf(f, "\tpush mem\n");
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
        fprintf(f, "\t;; -- STORE --\n");
        fprintf(f, "\tpop rbx ; contains the byte to store\n");
        fprintf(f, "\tpop rax ; contains the mem pointer\n");
        fprintf(f, "\tmov byte [rax], bl\n");
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
        fprintf(f, "\t;; -- LOAD --\n");
        fprintf(f, "\tpop rbx ; contains the mem pointer\n");
        fprintf(f, "\txor rax, rax\n");
        fprintf(f, "\tmov al, byte [rbx]\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- SYSCALL --\n");
        fprintf(f, "\tpop rax ; the syscall_id\n");
        if (vals_len >= 1)
            fprintf(f, "\tpop rdi ; 1st val of the syscall\n");
        if (vals_len >= 2)
            fprintf(f, "\tpop rsi ; 2nd val of the syscall\n");
        if (vals_len >= 3)
            fprintf(f, "\tpop rdx ; 3rd val of the syscall\n");
        if (vals_len >= 4)
            fprintf(f, "\tpop r10 ; 4th val of the syscall\n");
        if (vals_len >= 5)
            fprintf(f, "\tpop r8  ; 5th val of the syscall\n");
        if (vals_len == 6)
            fprintf(f, "\tpop r9  ; 6th val of the syscall\n");
        fprintf(f, "\tsyscall\n");
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
        fprintf(f, "\t;; -- SHL --\n");
        fprintf(f, "\tpop rcx ; shifter\n");
        fprintf(f, "\tpop rax ; shifted\n");
        fprintf(f, "\tshl rax, cl\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- SHR --\n");
        fprintf(f, "\tpop rcx ; shifter\n");
        fprintf(f, "\tpop rax ; shifted\n");
        fprintf(f, "\tshr rax, cl\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- OR --\n");
        fprintf(f, "\tpop rax ; n1\n");
        fprintf(f, "\tpop rbx ; n1\n");
        fprintf(f, "\tor rax, rbx\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- AND --\n");
        fprintf(f, "\tpop rax ; n1\n");
        fprintf(f, "\tpop rbx ; n1\n");
        fprintf(f, "\tand rax, rbx\n");
        fprintf(f, "\tpush rax\n");
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
        fprintf(f, "\t;; -- SWAP --\n");
        fprintf(f, "\tpop rax\n");
        fprintf(f, "\tpop rbx\n");
        fprintf(f, "\tpush rax\n");
        fprintf(f, "\tpush rbx\n");
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
        fprintf(f, "\t;; -- OVER --\n");
        fprintf(f, "\tpop rbx\n");
        fprintf(f, "\tpop rax\n");
        fprintf(f, "\tpush rax\n");
        fprintf(f, "\tpush rbx\n");
        fprintf(f, "\tpush rax\n");
    }
}
