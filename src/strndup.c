#include <stdlib.h>
#include <string.h>

char *strndup(const char *s, size_t n)
{
    char *p = 0;
    size_t n1 = 0;

    while (n1 < n && s[n1++]);
    if ((p = (char *) calloc(n + 1, sizeof(char)))) {
        memcpy(p, s, n1);
        p[n1] = 0;
    }
    return p;
}