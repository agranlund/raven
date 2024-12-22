/* LINTLIBRARY */
#include <stdio.h>

#ifdef NOVOID
#define void int
#endif

puts(s)
char *s;
{
    fputs(s, stdout);
    putc('\n', stdout);
}
