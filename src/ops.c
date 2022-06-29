#include <stdlib.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/program.h"

/**
 * The OP int64 array will be stored the following way:
 * OP[0]   = (int64)  OP_CODE
 * OP[1]   = (uint64) aguments taken
 * OP[...] = The necessary allocated memory according to OP[1]
 */
static int64 *new_op(int64 op_code, uint64 argc)
{
    int64 *op;

    op = (int64 *) calloc(argc + 2, sizeof(int64));
    if (!op)
        return op;
    op[0] = op_code;
    op[1] = argc;
    return op;
}

int64 *push(int64 n)
{
    int64 *op = new_op(OP_PUSH, 1);

    if (op)
        op[2] = n;
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

int64 *iff(void)
{
    int64 *op = new_op(OP_IF, 1);

    if (op)
        op[2] = -1; // Invalid pointer to `end`.
    return op;
}

int64 *elsee(void)
{
    int64 *op = new_op(OP_ELSE, 1);

    if (op)
        op[2] = -1; // Invalid pointer to `end`.
    return op;
}

int64 *end(void)
{
    return new_op(OP_END, 0);
}

int64 *dup(void)
{
    return new_op(OP_DUP, 0);
}
