/* LINTLIBRARY */
#include <stdio.h>
#include <fcntl.h>

extern long lseek();
extern int _bufsize;		/* default buffer size */

FILE *fdopen(fd, mode)
int fd;
register char *mode;
{
	register FILE *fp;
/*	int flags; */

	if (*mode != 'r' && *mode != 'w' && *mode != 'a')
		return 0;

	for (fp = _iob; fp<_iob+_NFILE; fp++)
		if (!(fp->_flag & (_READ | _WRITE | _RDWR)))
				break;

	if (fp >= _iob+_NFILE)
		return 0;

	fp->_flag = 0;
/*	flags = fcntl(fd, F_GETFL, 0); *** Unable to do this on ST */

	if (mode[1] == '+')
 		fp->_flag |= _RDWR;
 		fp->_flag &= ~(_WRITE | _READ);

	if (*mode == 'w') {
/*		if (!(flags & FWRITE)) {
				fp->_flag = 0;
				return 0;
		} */
		fp->_flag |= _WRITE;
	}
	else if (*mode == 'a') {
/*		if (!(flags & (FWRITE | FAPPEND))) {
				fp->_flag = 0;
				return 0;
		} */
		fp->_flag |= (_APPEND | _WRITE);
	}
	else {
/*		if (!(flags & FREAD)) {
				fp->_flag = 0;
				return 0;
		} */
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
