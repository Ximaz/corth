#include <corth.h>
#include <string.h>

int parse_mode(char const *smode, corth_mode_t *cmode)
{
    if (strcmp(smode, "sim") == 0) {
        *cmode = SIMULATION;
        return 0;
    }
    if (strcmp(smode, "com") == 0) {
        *cmode = COMPILATION;
        return 0;
    }
    return -1;
}