#include <stdio.h>
#include <stdlib.h>

FILE *open_file(char const *filename, char const *mode)
{
    FILE *f = fopen(filename, mode);
    if (!f) {
        perror("Error: ");
        exit(1);
    }
    return f;
}

size_t flen(FILE *f)
{
    size_t l = 0;

    fseek(f, 0, SEEK_END);
    l = ftell(f);
    fseek(f, 0, SEEK_SET);
    return l;
}

char *read_file(FILE *f)
{
    size_t len = flen(f);
    char *buf = (char *)calloc(len + 1, sizeof(char));

    if (!buf) {
        perror("Error: ");
        exit(1);
    }
    if (fread(buf, len, 1, f) < 1) {
        free(buf);
        fclose(f);
        perror("Error: ");
        exit(1);
    }
    return buf;
}
