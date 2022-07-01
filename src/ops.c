#include <stdlib.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/program.h"

static inst_t *new_op(op_code_t op_code, uint64 argc)
{
    inst_t *inst = (inst_t *) malloc(sizeof(inst_t));

    if (!inst)
        return inst;
    inst->op_code = op_code;
    inst->args_len = argc;
    inst->args = argc == 0 ? 0 : (int64 *) calloc(argc, sizeof(int64));
    return inst;
}

inst_t *push(int64 n)
{
    inst_t *inst = new_op(OP_PUSH, 1);

    if (inst)
        inst->args[0] = n;
    return inst;
}

inst_t *plus(void)
{
    return new_op(OP_PLUS, 0);
}

inst_t *minus(void)
{
    return new_op(OP_MINUS, 0);
}

inst_t *dump(void)
{
    return new_op(OP_DUMP, 0);
}

inst_t *dup(void)
{
    return new_op(OP_DUP, 0);
}

inst_t *equal(void)
{
    return new_op(OP_EQUAL, 0);
}

inst_t *diff(void)
{
    return new_op(OP_DIFF, 0);
}

inst_t *gt(void)
{
    return new_op(OP_GT, 0);
}

inst_t *lt(void)
{
    return new_op(OP_LT, 0);
}

inst_t *goet(void)
{
    return new_op(OP_GOET, 0);
}

inst_t *loet(void)
{
    return new_op(OP_LOET, 0);
}
inst_t *iff(void)
{
    inst_t *inst = new_op(OP_IF, 1);

    if (inst)
        inst->args[0] = -1; // Invalid pointer to `end`.
    return inst;
}

inst_t *elsee(void)
{
    inst_t *inst = new_op(OP_ELSE, 1);

    if (inst)
        inst->args[0] = -1; // Invalid pointer to `end`.
    return inst;
}

inst_t *whilee(void)
{
    return new_op(OP_WHILE, 0);
}

inst_t *doo(void)
{
    inst_t *inst = new_op(OP_DO, 1);

    if (inst)
        inst->args[0] = -1; // Invalid pointer to either `end` or `end` + 1.
    return inst;
}

inst_t *end(void)
{
    inst_t *inst = new_op(OP_END, 1);

    if (inst)
        inst->args[0] = -1; // Invalid pointer to `while`.
    return inst;
}

inst_t *mem(void)
{
    return new_op(OP_MEM, 0);
}

inst_t *store(void)
{
    return new_op(OP_STORE, 0);
}

inst_t *load(void)
{
    return new_op(OP_LOAD, 0);
}

inst_t *syscall0(void)
{
    return new_op(OP_SYSCALL0, 0);
}

inst_t *syscall1(void)
{
    return new_op(OP_SYSCALL1, 0);
}

inst_t *syscall2(void)
{
    return new_op(OP_SYSCALL2, 0);
}

inst_t *syscall3(void)
{
    return new_op(OP_SYSCALL3, 0);
}

inst_t *syscall4(void)
{
    return new_op(OP_SYSCALL4, 0);
}

inst_t *syscall5(void)
{
    return new_op(OP_SYSCALL5, 0);
}

inst_t *syscall6(void)
{
    return new_op(OP_SYSCALL6, 0);
}

inst_t *halt(void)
{
    return new_op(OP_HALT, 0);
}
