/* LINTLIBRARY */
#include	<stdio.h>

int ungetc(c, stream)
char c;
register FILE *stream;
{
	if (stream->_flag&_DIRTY || (stream->_ptr == stream->_base))
		return EOF;
	stream->_cnt++;
	return *--stream->_ptr = c;
}
