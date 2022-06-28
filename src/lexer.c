#include "ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/token.h"
#include "../include/lexer.h"
#include "../include/file_handler.h"

size_t trim_line(char *token, size_t col)
{
    while (col < strlen(token) && token[col] <= ' ')
        col++;
    return col;
}

int64 find_col(char *buf, uint64 col, char match)
{
    while (col < strlen(buf) && buf[col] != match && buf[col])
        col++;
    if (col == strlen(buf))
        return -1;
    return col;
}

tokens_t *lex_from_line(char const *filename, char *line, uint64 line_i, tokens_t *tokens)
{
    size_t i = 0;
    size_t line_len = strlen(line);
    char *tvalue = 0;
    token_t *token = 0;

    if (line[line_len - 1] == '\n')
        line[line_len - 1] = 0;
    i = trim_line(line, 0);
    while (i < line_len && (tvalue = strtok(&line[i], " "))) {
        token = new_token(filename, tvalue, line_i, i);
        i += strlen(tvalue) + 1;
        push_token(tokens, token);
    }
    return tokens;
}

tokens_t *lex_from_file(char const *filename)
{
    char *line = 0;
    size_t line_len = 0;
    uint64 line_i = 0;
    FILE *f = open_file(filename, "r");
    tokens_t *tokens = new_tokens();

    while (getline(&line, &line_len, f) > 0)
        tokens = lex_from_line(filename, line, ++line_i, tokens);
    free(line);
    fclose(f);
    return tokens;
}
