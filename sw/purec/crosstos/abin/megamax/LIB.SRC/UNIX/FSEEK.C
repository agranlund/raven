#include <stdio.h>
extern long lseek();

int fseek(stream, offset, ptrname)
register FILE *stream;
long offset;
int ptrname;
{
	if (stream->_flag & _WRITE && fflush(stream))     /* error during flush */
		return EOF;
	else 
	if (stream->_flag & _READ) {
		if (ptrname == 1)		/* From current position */
			offset -= stream->_cnt;
		stream->_ptr = stream->_base;
	}
 	
  	stream->_flag &= ~_EOF;    /* reset eof condition */
  	if ( lseek(stream->_fd, offset, ptrname) < 0L )
		return -1L;
  	stream->_cnt = 0;
  	if (stream->_flag & _RDWR)
  		stream->_flag &= ~(_READ | _WRITE);
  	return 0;
}
