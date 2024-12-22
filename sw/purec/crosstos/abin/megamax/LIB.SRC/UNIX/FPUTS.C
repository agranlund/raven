/* LINTLIBRARY */
#include <stdio.h>

#ifdef NOVOID
#define void int
#endif

fputs(s, stream)
register char *s;
FILE *stream;
{
	if (s && *s)
		(void) fwrite(s, strlen(s), 1, stream);
}

