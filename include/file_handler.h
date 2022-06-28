#ifndef _CORTH_FILE_HANDLER
#define _CORTH_FILE_HANDLER
#include <stdio.h>

FILE *open_file(char const *filename, char const *mode);
char *read_file(FILE *f);

#endif
