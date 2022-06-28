#include <stdlib.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/program.h"

static int64 *new_op(int64 op_code, uint64 argc)
{
    int64 *op;

    op = (int64 *) calloc(argc + 1, sizeof(int64));
    if (!op)
        return op;
    op[0] = op_code;
    return op;
}

int64 *push(int64 n)
{
    int64 *op = new_op(OP_PUSH, 1);

    if (!op)
        return op;
    op[1] = n;
    return op;
}

int64 *halt(void)
{
    return new_op(OP_HALT, 0);
}

int64 *plus(void)
{
    return new_op(OP_PLUS, 0);
}

int64 *minus(void)
{
    return new_op(OP_MINUS, 0);
}

int64 *equal(void)
{
    return new_op(OP_EQUAL, 0);
}

int64 *dump(void)
{
    return new_op(OP_DUMP, 0);
}
