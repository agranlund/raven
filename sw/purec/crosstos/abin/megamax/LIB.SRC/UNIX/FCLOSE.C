/* LINTLIBRARY */
#include <stdio.h>

int fclose(stream)
register FILE *stream;
{
	if ((stream->_flag & _WRITE)) {
		if (fflush(stream))
			return EOF;
	}

	if (stream->_flag & _BIGBUF)
		free(stream->_base);

	stream->_flag = 0;
	if (close(stream->_fd))	/* ST specific */
		return EOF;
	return 0;
}

