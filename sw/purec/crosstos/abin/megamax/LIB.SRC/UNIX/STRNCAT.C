/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>


char *strncat(s1, s2, n)
register char *s1, *s2;
register int n;
{
    char *result = s1;

    for ( ; *s1++; ) ;
    --s1;
    while ((--n >= 0) && (*s1++ = *s2++)) ;
	if (n < 0)
		*s1 = 0;
    return result;
}
