/* LINTLIBRARY */
#include <stdio.h>


char *fgets(s, n, iop)
register char *s;
int n;
FILE *iop;
{
	int c;
	register char *cs;

	cs = s;
	while (--n > 0 && (c = getc(iop)) != EOF)
		if ((*cs++ = c) == '\n')
			break;
	*cs = '\0';
	return ((c == EOF && cs == s) ? 0 : s);
}
