/* LINTLIBRARY */
#include	<stdio.h>

int putw(w, stream)
int w;
FILE *stream;
{
	if (putc(w >> 8 & 0377, stream) == EOF)
		return EOF;
	if (putc(w & 0377, stream) == EOF)
		return EOF;
	return w;
}
