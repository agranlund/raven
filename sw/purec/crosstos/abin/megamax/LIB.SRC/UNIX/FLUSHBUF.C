
/* LINTLIBRARY */
#include <stdio.h>

#ifdef NOVOID
#define void int
#endif

extern int _bufsize;
extern char *malloc();
extern long lseek();

_flushbuf(c, fp)
char c;
register FILE *fp;
{
 	if (fp->_flag & _RDWR && !(fp->_flag & _READ))
 		fp->_flag |= _WRITE;
 
 	if (!(fp->_flag & _WRITE))
 		fp->_flag |= _ERR;
 
	fp->_cnt = 0;

	if (fp->_flag & _ERR)
		return EOF;

	if (!fp->_base)
		_getbuf(fp);

	if (fp->_flag & _UNBUF) {
		*fp->_ptr++ = c;
		fp->_flag |= _DIRTY;
	} 
	else
	if (fp->_flag & _LINBUF) { 
		fp->_flag |= _DIRTY;
		*fp->_ptr++ = c;
		if (c != '\n' && fp->_ptr - fp->_base < fp->_bufsize)
			return c & 0377;
	}

	if (fflush(fp)) {
		fp->_flag |= _ERR;
		return EOF;
	}

	if (fp->_flag & (_UNBUF|_LINBUF)) {
		fp->_cnt = 0;
		return c & 0377;
	}
	else {
		fp->_cnt = fp->_bufsize-1;
		fp->_flag |= _DIRTY;
		return (*fp->_ptr++ = c) & 0377;
	}
}
