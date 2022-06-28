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
    int err = 0;
    FILE* f = 0;
    uint64 i = 0;
    int64 *op = NULL;

    assert(COUNT_OPS == SUPPORTED_INSTRUCTIONS);
    if (!program)
        return 1;
    f = open_file(output, "w");
    asm_header(f);
    for (; i < program->instructions_len; i++) {
        op = program->instructions[i];
        switch (op[0]) {
            case OP_PUSH:
                inst_push(f, 0, op[1]);
                break;
            case OP_PLUS:
                inst_plus(f, 0);
                break;
            case OP_MINUS:
                inst_minus(f, 0);
                break;
            case OP_EQUAL:
                inst_equal(f, 0);
                break;
            case OP_DUMP:
                inst_dump(f, 0);
                break;
            case OP_HALT:
                err = inst_halt(f, 0);
                break;
            default:
                printf("Unreachable.\n");
                break;
        }
    }
    fclose(f);
    return err;
}
