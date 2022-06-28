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
    if (strcmp(subcommand, "sim") == 0) {
        filename = argv[2];
        tokens_t *tokens = lex_from_file(filename);
        program_t *program = new_program(tokens);
        err = simulate(program);
        destroy_program(program);
        destroy_tokens(tokens);
    } else if (strcmp(subcommand, "com") == 0) {
        filename = argv[2];
        tokens_t *tokens = lex_from_file(filename);
        program_t *program = new_program(tokens);
        err = compile(program, "output.asm");
        destroy_program(program);
        destroy_tokens(tokens);
    } else {
        usage(argv[0]);
        printf("Invalid subcommand is provided.\n");
        return 1;
    }
    return err;
}
