#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "include/ops.h"
#include "include/program.h"
#include "include/compiler.h"
#include "include/simulator.h"
#include "include/file_handler.h"

char *trim_line(char *token)
{
    while (*token != 0 && *token <= ' ')
        token++;
    return token;
}

int64 *parse_token_to_op_code(char *token)
{
    size_t i = 0;
    long long n = 0;

    if (strcmp(token, "+") == 0)
        return plus();
    if (strcmp(token, "-") == 0)
        return minus();
    if (strcmp(token, ".") == 0)
        return dump();
    if (strcmp(token, "halt") == 0)
        return halt();
    while (token[i] >= '0' && token[i] <= '9') i++;
    if (i == 0)
        return nop();
    if (token[i] == 0) {
        if (i <= 19) {
            n = strtoll(token, NULL, 10);
            return push(n);
        }
        if (errno == ERANGE) {
            printf("ERROR: The number does not fit max value of 'long long'.\n");
            exit(1);
        }
        printf("ERROR: The number does not fit into a 'long long'.\n");
        exit(1);
    } else {
        printf("ERROR: bad token is provided : %s.\n", token);
        exit(1);
    }
}

void parse_instruction(program_t *program, char *instruction)
{
    size_t i = 0;
    char *token = 0;

    instruction = trim_line(instruction);
    if (!instruction[i])
        return;
    while (i < strlen(instruction)) {
        token = strtok(&instruction[i], " ");
        if (token) {
            push_instruction(program, parse_token_to_op_code(token));
            i += strlen(token);
        }
        i++;
    }
}

program_t *get_program(char const *input_filename)
{
    size_t i = 0;
    char *instruction = 0;
    program_t *program;
    FILE *f = open_file(input_filename, "r");
    char *buf = read_file(f);
    size_t buf_sz = strlen(buf);

    fclose(f);
    program = new_program();
    // TODO : Handle "\r\n" the same way as "\n" since VSC puts
    //        "\r\n" on Windows. If the code is then executed
    //        by a docker or the WSL, it needs to behave the same.
    while (i < buf_sz) {
        instruction = strtok(&buf[i], "\n");
        if (instruction) {
            parse_instruction(program, instruction);
            i += strlen(instruction);
        }
        i++;
    }
    free(buf);
    return program;
}

void usage(char const *binary_name)
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
        program_t *program = get_program(filename);
        err = simulate(program);
        destroy_program(program);
    } else if (strcmp(subcommand, "com") == 0) {
        filename = argv[2];
        program_t *program = get_program(filename);
        err = compile(program, "output.asm");
        destroy_program(program);
    } else {
        usage(argv[0]);
        printf("Invalid subcommand is provided.\n");
        return 1;
    }
    return err;
}
