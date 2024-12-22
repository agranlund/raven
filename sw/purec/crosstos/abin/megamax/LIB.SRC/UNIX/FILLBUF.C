/* LINTLIBRARY */
#include <stdio.h>

#ifdef NOVOID
#define void int
#endif

extern int _bufsize;
extern char *malloc();
extern long lseek();

_loadbuf(fp)
register FILE *fp;
{
 	if (fp->_flag & _RDWR && !(fp->_flag & _WRITE))
 		fp->_flag |= _READ;

	if (!(fp->_flag & _READ))
		fp->_flag |= _ERR;

	if ((fp->_flag & (_EOF | _ERR))) {
		fp->_ptr = fp->_base;
		fp->_cnt = 0;
		return EOF;
	}

	if (!fp->_base)
		_getbuf(fp);

	fp->_ptr = fp->_base;

	if (fp->_flag & _IFLUSH) /* flush standard out */
		fflush(stdout);

	fp->_cnt = read(fp->_fd, fp->_ptr, 
		fp->_flag & (_UNBUF|_LINBUF) ? 1 : fp->_bufsize);

	return 0;
}

_fillbuf(fp)
register FILE *fp;
{
	if (_loadbuf(fp))
		return EOF;

	if (--fp->_cnt < 0) {
		if (fp->_cnt == -1)
			fp->_flag |= _EOF;
		else
			fp->_flag |= _ERR;
		fp->_cnt = 0;
		return EOF;
	}
	return (*fp->_ptr++ & 0377);
}

