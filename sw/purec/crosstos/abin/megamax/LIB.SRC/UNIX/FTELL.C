
#include <stdio.h>
extern long lseek();

long ftell(stream)
register FILE *stream;
{
  	register int cnt, flag, tmp;
	register long rv;
 	
  	flag = stream->_flag;
  	if (stream->_cnt < 0)
  		stream->_cnt = 0;
  	
  	cnt = stream->_cnt;
  	if (flag & (_READ | _WRITE | _RDWR)) {
  		if ((rv = lseek(stream->_fd, 0L, 1)) == -1)
  			return EOF;
 		
  		if (flag & _READ)
  			return rv - cnt;
  		else
 			if (!(flag & _WRITE) || !stream->_ptr || flag & _UNBUF)
 				return rv;
 			else
 				return rv + (stream->_ptr - stream->_base);
  	}
  	else
  		return EOF;
}
