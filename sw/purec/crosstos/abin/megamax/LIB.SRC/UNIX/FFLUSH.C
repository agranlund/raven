/* LINTLIBRARY */
#include	<stdio.h>

#ifdef NOVOID
#define void int
#endif

int fflush(stream)
register FILE *stream;
{
	register int len;
	long lseek();

	if (!(stream->_flag & (_READ | _WRITE | _RDWR)))
		return EOF;

	len = stream->_ptr - stream->_base;

	if (stream->_flag & _DIRTY) {
		if (!(stream->_flag & _WRITE))
			return EOF;

		if (stream->_flag & _APPEND)
			(void) lseek(stream->_fd, 0L, 2); /* go to end for append */

		if (write(stream->_fd, stream->_base, len) == -1) {
			stream->_flag |= _ERR;
			return EOF;
		}
		stream->_flag &= ~_DIRTY;	/* no longer dirty */
	}
					
	stream->_ptr = stream->_base;	/* empty the buffer */

	stream->_cnt = 0; 	/* Forces _flushbuf() to be called which sets _DIRTY */
						/*   in _WRITE mode. */
	return 0;
}
