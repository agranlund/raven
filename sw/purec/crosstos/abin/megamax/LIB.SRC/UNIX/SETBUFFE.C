/* LINTLIBRARY */
#include <stdio.h>


setbuffer(stream, buf, size)
FILE *stream;
char *buf;
int size;
{
	setbuf(stream, buf);
	if (buf)
		stream->_bufsize = size;
}
