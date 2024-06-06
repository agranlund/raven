#include <stdio.h>

int fflush(FILE *stream)
{
	stream->__pushback = EOF;
	return 0;
}

