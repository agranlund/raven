/* LINTLIBRARY */
#include <stdio.h>

setlinebuf(stream)
register FILE *stream;
{
	if (!stream->_base) {
		_getbuf(stream);
		stream->_ptr = stream->_base;
		stream->_cnt = 0;
	}
	
	if (stream->_base) {
		stream->_flag |= _LINBUF;
		stream->_flag &= ~(_UNBUF | _BIGBUF);
	}
}
