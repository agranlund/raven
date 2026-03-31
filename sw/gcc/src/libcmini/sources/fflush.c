#include <stdio.h>

int fflush(FILE *stream)
{
	stream->__pushback = EOF;
	stream->__last_char = EOF;
	return 0;
}
