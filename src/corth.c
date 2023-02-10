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
    FILE *fp = fopen(filename, "r");
    char *content = 0;

    if (!fp)
        return -1;
    if (!(content = read_file(fp))) {
        fclose(fp);
        return -1;
    }
    if (mode == SIMULATION)
        simulate(content);
    if (mode == COMPILATION)
        compile(content);
    free(content);
    fclose(fp);
    return 0;
}