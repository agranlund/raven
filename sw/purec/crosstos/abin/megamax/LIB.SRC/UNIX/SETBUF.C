/* LINTLIBRARY */
#include <stdio.h>

setbuf(stream, buf)
register FILE *stream;
char *buf;
{
	if (buf == NULL) {
		stream->_flag |= _UNBUF;
		stream->_flag &= ~(_LINBUF | _BIGBUF);
		stream->_bufsize = 0;
	}
	else {
		stream->_flag |= _BIGBUF;
		stream->_flag &= ~(_LINBUF | _UNBUF);
		stream->_base = stream->_ptr = buf;
		stream->_bufsize = BUFSIZ;
		if (stream == stdout || stream == stdin)
			stdin->_flag &= ~_IFLUSH;
	}
	stream->_flag &= ~_DIRTY;
	stream->_cnt = 0;
}
