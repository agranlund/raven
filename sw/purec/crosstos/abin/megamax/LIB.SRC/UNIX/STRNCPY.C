
/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

char *strncpy(s1, s2, n)
register char *s1, *s2;
register int n;
{
    register char *result = s1;

    while (--n >= 0 && (*s1++ = *s2++));
	while (--n >= 0)
		*s1++ = 0;

    return result;
}
