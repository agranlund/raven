/* LINTLIBRARY */
#include <stdio.h>

#ifdef NOVOID
#define void int
#endif

extern int _bufsize;
extern char *malloc();
extern long lseek();

static char smallbuf[_NFILE];

_getbuf(fp)
register FILE *fp;
{
	if (fp == stdout && !isatty(STDOUT)) {
		fp->_flag &= ~(_UNBUF | _LINBUF);
		fp->_flag |= _BIGBUF;
		fp->_bufsize = _bufsize;
		stdin->_flag &= ~_IFLUSH;
	}
	else
	if (fp == stdin && !isatty(STDIN)) {
		fp->_flag &= ~(_UNBUF | _LINBUF | _IFLUSH);
		fp->_flag |= _BIGBUF;
		fp->_bufsize = _bufsize;
	}
		
	while (!fp->_base)
		if (fp->_flag & _UNBUF)
			fp->_base = fp->_ptr = &smallbuf[fp-_iob];
		else 
		if (!(fp->_base = fp->_ptr = malloc((unsigned) fp->_bufsize)))
			fp->_flag |= _UNBUF;
		else
		if (!(fp->_flag & _LINBUF))
			fp->_flag |= _BIGBUF;
}

