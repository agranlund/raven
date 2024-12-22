/* LINTLIBRARY */
#include	<stdio.h>

int fputc(c, stream)
char c;
FILE *stream;
{
	return putc(c, stream);
}
