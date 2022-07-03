#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/lexer.h"
#include "include/program.h"
#include "include/types.h"

static void usage(char const *binary_name)
{
    printf("Usage : %s <SUBCOMMAND> program\n", binary_name);
    printf("SUBCOMMANDS :\n");
    printf("    sim <filename>    Simulate the program from the specified filename.\n");
    printf("    com <filename>    Compile the program from the specified filename.\n");
}

static debugger_t *build_debugger(int argc, char *const *argv)
{
    int any_found = 0;
    int i = 0;
    debugger_t *debug = malloc(sizeof(debugger_t));

    if (!debug)
        return debug;
    debug->mem_lim = 15;
    for (; i < argc; i++) {
        if (strcmp(argv[i], "-dall") == 0) {
            any_found = 1;
            debug->debug_stack = 1;
            debug->debug_memory = 1;
            debug->debug_bindings = 1;
            debug->debug_jumps = 1;
            break;
        }
        if (strcmp(argv[i], "-stack") == 0) {
            any_found = 1;
            debug->debug_stack = 1;
        }
        if (strcmp(argv[i], "-mem") == 0) {
            any_found = 1;
            debug->debug_memory = 1;
        }
        if (strncmp(argv[i], "-binding", strlen("-binding")) == 0) {
            any_found = 1;
            debug->debug_bindings = 1;
        }
        if (strcmp(argv[i], "-jmp") == 0) {
            any_found = 1;
            debug->debug_jumps = 1;
        }
    }
    debug->enabled = any_found;
    return debug;
}

int main(int argc, char *const *argv)
{
    int err = 0;
    tokens_t *tokens = 0;
    debugger_t *debug = 0;
    program_t *program = 0;
    char const *filename = 0;
    char const *subcommand = 0;
    char const *binary_name = argv[0];

    if (argc == 1) {
        usage(binary_name);
        printf("ERROR: No subcommand is provided.\n");
        return 1;
    }
    if (argc == 2) {
        usage(binary_name);
        printf("ERROR: No input is provided.\n");
        return 1;
    }
    subcommand = argv[1];
    filename = argv[2];
    tokens = lex_from_file(filename);
    debug = build_debugger(argc, argv);
    program = new_program(tokens);
    if (strcmp(subcommand, "sim") == 0) {
        err = run_program(program, 1, debug, 0);
        free(debug);
    }
    else if (strcmp(subcommand, "com") == 0) {
        err = run_program(program, 0, debug, "output.asm");
        free(debug);
    }
    else {
        usage(argv[0]);
        printf("ERROR: Invalid subcommand is provided.\n");
        err = 1;
    }
    destroy_program(program);
    destroy_tokens(tokens);
    return err;
}
