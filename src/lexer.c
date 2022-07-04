#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/ops.h"
#include "../include/token.h"
#include "../include/lexer.h"
#include "../include/file_handler.h"

static size_t run_to_get(char *line, size_t start, char predicated)
{
    while (start < strlen(line) && line[start] != predicated)
        start++;
    return start;
}

static size_t run_to_avoid(char *line, size_t start, char predicated)
{
    while (start < strlen(line) && line[start] == predicated)
        start++;
    return start;
}

static char *strip_end_of_line(char *line)
{
    size_t line_len = strlen(line);

    if (line[line_len - 2] == '\r' && line[line_len - 1] == '\n')
        line[line_len - 2] = 0;
    if (line[line_len - 1] == '\n')
        line[line_len - 1] = 0;
    return line;
}

static token_list_t *lex_line(char const *filename, char *line, uint64 line_i, token_list_t *tokens)
{
    token_t *token = 0;
    location_t token_loc = {
        .row = line_i,
        .filename = filename,
        .col = 0
    };
    size_t col_end = 0;
    size_t col = run_to_avoid(line, 0, ' ');
    size_t line_len = strlen(line);
    char *word = (char *) calloc(line_len, sizeof(char));
    char *num_checkpoint = 0;
    int64 num = 0;
    value_t val;

    while (col < line_len) {
        token_loc.col = col;
        if (strncmp(&line[col], COMMENT, 2) == 0)
            break;
        if (line[col] == '"') {
            col_end = run_to_get(line, col + 1, '"');
            assert(line[col_end] == '"');
            word = strncpy(word, &line[col + 1], col_end - 1);
            val.string = strdup(word);
            token = new_token(token_loc, TOKEN_STR, val);
            col = run_to_avoid(line, col_end + 1, ' ');
        } else {
            col_end = run_to_get(line, col, ' ');
            word = strncpy(word, &line[col], col_end - col);
            word[col_end - col] = 0;
            num_checkpoint = word;
            num = strtoll(word, &num_checkpoint, 10);
            // Unable to convert the number, it's not a number.
            if (num_checkpoint == word || *num_checkpoint != '\0' ||
                ((num == LLONG_MIN || num == LLONG_MAX) && errno == ERANGE)) {
                val.string = strdup(word);
                token = new_token(token_loc, TOKEN_WORD, val);
            }
            else {
                val.integer = num;
                token = new_token(token_loc, TOKEN_INT, val);
            }
            col = run_to_avoid(line, col_end, ' ');
        }
        push_token(tokens, token);
    }
    free(word);
    return tokens;
}

token_list_t *lex_file(char const *filename)
{
    char *line = 0;
    size_t line_len = 0;
    uint64 line_number = 1;
    FILE *f = open_file(filename, "r");
    token_list_t *tokens = new_tokens();

    while (getline(&line, &line_len, f) > 0) {
        line = strip_end_of_line(line);
        tokens = lex_line(filename, line, line_number++, tokens);
    }
    free(line);
    fclose(f);
    return tokens;
}
