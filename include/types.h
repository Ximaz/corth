#ifndef _CORTH_UTIL
#define _CORTH_UTIL

typedef long long int64;
typedef unsigned long long uint64;

typedef struct debugger_s
{
    int enabled;
    int mem_lim;
    int debug_stack;
    int debug_memory;
    int debug_bindings;
    int debug_jumps;
} debugger_t;

#endif
