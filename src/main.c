#include <corth.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static void usage(char const *bin)
{
    printf("USAGE :\n");
    printf("\t%s <mode> <filename>\n\n", bin);
    printf("DESCRIPTION :\n");
    printf("\t<mode> : either 'sim' or 'com'\n");
    printf("\t<filename> : the entry point\n");
}

static int check_file(char const *filename)
{
    struct stat sbuf;

    if (stat(filename, &sbuf) != 0)
        return -1;
    if (!S_ISREG(sbuf.st_mode))
        return -1;
    return 0;
}

int main(int argc, char **argv)
{
    char *filename = 0;
    corth_mode_t mode;

    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }
    if (parse_mode(argv[1], &mode) == -1) {
        usage(argv[0]);
        return 1;
    }
    filename = argv[2];
    if (check_file(filename) == -1) {
        fprintf(stderr, "Invalid filename supplied : %s\n", filename);
        return 1;
    }
    return corth(filename, mode);
}