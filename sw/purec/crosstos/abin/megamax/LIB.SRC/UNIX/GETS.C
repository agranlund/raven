/* LINTLIBRARY */
#include <stdio.h>

char *gets(s)
register char *s;
{
	register char *result = s;
	register int c;

	while ((c = getc(stdin)) != EOF && c != '\n')
		*s++ = c;
	*s = 0;
	return (c == EOF ? 0 : result);
}
