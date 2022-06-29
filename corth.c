#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/lexer.h"
#include "include/program.h"
#include "include/compiler.h"
#include "include/simulator.h"

static void usage(char const *binary_name)
{
    printf("Usage : %s <SUBCOMMAND> program\n", binary_name);
    printf("SUBCOMMANDS :\n");
    printf("    sim <filename>    Simulate the program from the specified filename.\n");
    printf("    com <filename>    Compile the program from the specified filename.\n");
}

int main(int argc, char *const *argv)
{
    int err = 0;
    int use_debugger = 0;
    tokens_t *tokens = 0;
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
    use_debugger = argc > 3 ? strcmp(argv[3], "-d") == 0 : 0;
    program = new_program(tokens);
    if (strcmp(subcommand, "sim") == 0)
        err = run_program(program, 1, use_debugger, 0);
    else if (strcmp(subcommand, "com") == 0)
        err = run_program(program, 0, 0, "output.asm");
    else {
        usage(argv[0]);
        printf("ERROR: Invalid subcommand is provided.\n");
        err = 1;
    }
    destroy_program(program);
    destroy_tokens(tokens);
    return err;
}
