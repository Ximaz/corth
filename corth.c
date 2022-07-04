#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/lexer.h"
#include "include/token.h"
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
    debug->debug_stack = 0;
    debug->debug_memory = 0;
    debug->debug_bindings = 0;
    debug->debug_jumps = 0;
    debug->debug_tokens = 0;
    debug->mem_lim = 15;
    for (; i < argc; i++) {
        if (strcmp(argv[i], "-dall") == 0) {
            any_found = 1;
            debug->debug_stack = 1;
            debug->debug_memory = 1;
            debug->debug_bindings = 1;
            debug->debug_jumps = 1;
            debug->debug_tokens = 1;
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
        if (strncmp(argv[i], "-token", strlen("-token")) == 0) {
            any_found = 1;
            debug->debug_tokens = 1;
        }
    }
    debug->enabled = any_found;
    return debug;
}

int main(int argc, char *const *argv)
{
    int err = 0;
    token_list_t *tokens = 0;
    debugger_t *debug = 0;
    char *output = "output.asm";
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
    if (argc > 3)
        output = argv[3];
    tokens = lex_file(filename);
    debug = build_debugger(argc, argv);
    if (strcmp(subcommand, "sim") == 0) {
        err = run_program(tokens, 1, debug, 0);
        free(debug);
    }
    else if (strcmp(subcommand, "com") == 0) {
        err = run_program(tokens, 0, debug, output);
        free(debug);
    }
    else {
        usage(argv[0]);
        printf("ERROR: Invalid subcommand is provided.\n");
        err = 1;
    }
    return err;
}
