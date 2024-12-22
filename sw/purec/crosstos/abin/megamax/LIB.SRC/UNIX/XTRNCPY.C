
/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

char *xtrncpy(s1, s2, n)
register char *s1, *s2;
register int n;
{
    while (n-- && (*s1++ = *s2++));

    return n == -1 ? s1 : s1 - 1;
}

