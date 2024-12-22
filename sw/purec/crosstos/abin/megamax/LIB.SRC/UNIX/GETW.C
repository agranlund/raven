/* LINTLIBRARY */
#include	<stdio.h>

int getw(stream)
FILE *stream;
{
	register int lo, hi;

	if ((hi = fgetc(stream)) == EOF)
		return EOF;
	if ((lo = fgetc(stream)) == EOF)
		return EOF;
	return (hi << 8) + lo;
}
