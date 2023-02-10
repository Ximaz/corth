#include <corth.h>
#include <stdio.h>
#include <stdlib.h>

static char *read_file(FILE *fp)
{
    size_t flen = 0;
    char *buffer = 0;

    fseek(fp, 0, SEEK_END);
    flen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (flen <= 0)
        return buffer;
    if (!(buffer = (char *) calloc(flen + 1, sizeof(char))))
        return 0;
    if (fread(buffer, flen, 1, fp) < 1) {
        free(buffer);
        return 0;
    }
    return buffer;
}

int corth(char const *filename, corth_mode_t mode)
{
    int status = 0;
    char *content = 0;
    tok_lst_t *tokens = 0;
    FILE *fp = fopen(filename, "r");

    if (!fp)
        return -1;
    content = read_file(fp);
    fclose(fp);
    if (!content)
        return -1;
    tokens = lex_content(filename, content);
    status = run_program(tokens, mode, "output.asm");
    destroy_tokens(tokens);
    free(content);
    return status;
}