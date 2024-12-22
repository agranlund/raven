
#include <stdio.h>
#include <fcntl.h>
#include <osbind.h>

long lseek(fildes, offset, whence)
int fildes;
long offset;
int whence;
{
    long retval, fpos, leof;
    int temp;
	
    if (fildes < 0) {	/* can't seek on a device */	/* ST Specific */
		errno = -37;			/* Bad handle number */
		return -1L;
	}
	
    if ((retval = Fseek(offset, fildes, whence)) >= 0)
		return retval;	/* normal seek */

    /* Trying to seek past EOF, must extend file */
    if ( (fpos = Fseek(0L, fildes, 1)) < 0) {
		errno = fpos;
		return -1L;
	}
    leof = Fseek(0L, fildes, 2);
    if (whence == 1)
		offset = fpos+offset;
    else if (whence == 2)
		offset = leof+offset;
    else if (whence)
		return -1L;

    if (offset > leof)	/* extend file with random garbage from stack */
		if ((_bintst(fildes)&~O_BINARY) == O_RDONLY) {
			errno	= retval;
			return -1L;
			}
		else
			Fwrite(fildes, offset-leof, &temp);
    if (temp = Fseek(offset, fildes, 0) < 0) {
		errno = temp;
		return -1L;
	}
    else
		return offset;
}
