#include <stdlib.h>
#include "../include/ops.h"
#include "../include/program.h"

int64 *push(int64 n)
{
    int64 *op = (int64 *) calloc(2, sizeof(int64));

    if (!op)
        return op;
    *op++ = OP_PUSH;
    *op-- = n;
    return op;
}

int64 *halt(void)
{
    int64 *op = (int64 *) malloc(sizeof(int64));

    if (op)
        *op = OP_HALT;
    return op;
}


int64 *plus(void)
{
    int64 *op = (int64 *) malloc(sizeof(int64));

    if (op)
        *op = OP_PLUS;
    return op;
}

int64 *minus(void)
{
    int64 *op = (int64 *) malloc(sizeof(int64));

    if (op)
        *op = OP_MINUS;
    return op;
}

int64 *dump(void)
{
    int64 *op = (int64 *) malloc(sizeof(int64));

    if (op)
        *op = OP_DUMP;
    return op;
}
