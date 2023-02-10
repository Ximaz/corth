#include <corth.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static uint64 run_until_predicated(char *line, uint64 start, char predicated)
{
    uint64 len = strlen(line);

    while (start < len && line[start] != predicated)
        start++;
    return start;
}

static uint64 run_while_predicated(char *line, uint64 start, char predicated)
{
    uint64 len = strlen(line);

    while (start < len && line[start] == predicated)
        start++;
    return start;
}

static tok_lst_t *lex_line(char const *filename, char *line, uint64 line_i, tok_lst_t *tokens)
{
    token_t *token = 0;
    tok_loc_t loc = {
        .filename = filename,
        .row = line_i,
        .col = 0
    };
    tok_val_t val;
    uint64 col_end = 0;
    uint64 col = run_while_predicated(line, 0, ' ');
    uint64 line_len = strlen(line);
    int64 num = 0;
    char *word = 0;
    char *num_checkpoint = 0;

    if (!line || !(word = (char *) calloc(line_len + 1, sizeof(char))))
        return tokens;
    while (col < line_len) {
        loc.col = col;
        if (strncmp(&line[col], COMMENT, 2) == 0)
            break;
        if (line[col] == '"') {
            col_end = run_until_predicated(line, col + 1, '"');
            assert(line[col_end] == '"');
            word = strncpy(word, &line[col + 1], col_end - 1);
            val.string = strdup(word);
            token = new_token(loc, TOKEN_STR, val);
            col = run_while_predicated(line, col_end + 1, ' ');
        } else {
            col_end = run_until_predicated(line, col, ' ');
            word = strncpy(word, &line[col], col_end - col);
            word[col_end - col] = 0;
            num_checkpoint = word;
            num = strtoll(word, &num_checkpoint, 10);
            // Unable to convert the number, it's not a number.
            if (num_checkpoint == word || *num_checkpoint != '\0' ||
                ((num == LLONG_MIN || num == LLONG_MAX) && errno == ERANGE)) {
                val.string = strdup(word);
                token = new_token(loc, TOKEN_WORD, val);
            } else {
                val.integer = num;
                token = new_token(loc, TOKEN_INT, val);
            }
            col = run_while_predicated(line, col_end, ' ');
        }
        push_token(tokens, token);
    }
    free(word);
    return tokens;
}

tok_lst_t *lex_content(char const *filename, char *content)
{
    char *line = strtok(content, "\n");
    uint64 line_number = 1;
    tok_lst_t *tokens = new_tokens();

    while (line) {
        tokens = lex_line(filename, line, line_number++, tokens);
        line = strtok(0, "\n");
    }
    return tokens;
}