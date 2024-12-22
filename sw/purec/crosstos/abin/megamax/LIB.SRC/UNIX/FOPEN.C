/* LINTLIBRARY */
#include <stdio.h>
#ifndef ATARI
#include <file.h>
#endif
#include <fcntl.h>

#ifdef NOVOID 
#define void int
#endif

static FILE *_reopen;		/* stream to be reopened */
extern long lseek();
extern int _bufsize;		/* default buffer size */

FILE *fopen(name, mode)
char *name;
register char *mode;
{
	register int fd;
	register FILE *fp;
	register int rw, md;
    int binary = 0;    /* binary mode */

    if (*mode == 'b') {
		binary = O_BINARY; 
		mode++;
	}

	md = *mode;
	if (md != 'r' && md != 'w' && md != 'a')
		return 0;

	if ((fp = _reopen) == 0)
		for (fp = _iob; fp<_iob+_NFILE; fp++)
			if (!(fp->_flag & (_READ | _WRITE | _RDWR)))
				break;

	if (fp >= _iob+_NFILE)
		return 0;

	_reopen = 0;

	fp->_flag = 0;

	if (rw = (mode[1] == '+')) {
 		fp->_flag |= _RDWR;
 		fp->_flag &= ~(_WRITE | _READ);
	}

	if (md == 'w') {
		fd = open(name, (rw ? O_RDWR : O_WRONLY) | O_CREAT | O_TRUNC | binary,
				  0666);
 		if (!rw)
 			fp->_flag |= _WRITE;
	}
	else 
	if (md == 'a') {
		fd = open(name, O_RDWR | O_CREAT | binary, 0666);
		if (fd >= 0) {
			(void) lseek(fd, 0L, 2);    /* go to end of file */
 			fp->_flag |= _APPEND;
 			if (!rw)
 				fp->_flag |= _WRITE;
		}
	}
	else {
		fd = open(name, (rw ? O_RDWR : O_RDONLY) | binary, 0);
 		if (!rw)
 			fp->_flag |= _READ;
	}

	if (fd == -1) {
		fp->_flag = 0;
		return 0;
	}
	fp->_fd = fd;
	fp->_cnt = 0;
	fp->_base = fp->_ptr = 0;
	 
	fp->_bufsize = _bufsize;
	return fp;
}

FILE *freopen(name, mode, stream)
char *name;
char *mode;
FILE *stream;
{
	_reopen = stream;
	(void) fclose(stream);
 	return fopen(name, mode);
}
