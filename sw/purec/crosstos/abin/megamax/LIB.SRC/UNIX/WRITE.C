
#include <stdio.h>
#include <osbind.h>
#include <fcntl.h>

/*
	3-30-87	EGP		Added call to stdout_write().  Needed for Beckeymeyer shell.
	6-11-87 EGP		Added translated mode buffering.
	7-31-87 EGP		added "bintst & O_BINARY" since bintst has all flag values
						now.
	8-20-87 RPT		removed putscr(), putaux(), putprn().
*/


int write(fildes, buf, nbyte)
int fildes;
register char *buf;
unsigned nbyte;
{
    register int i, t;
    int count;
    register char *bp = buf, *p;
    long tl;

	count = 0;
	if (fildes == STDOUT || !(_bintst(fildes)&O_BINARY)) {
		/* translated mode */
	    char tbuf[512];	/* this may not be a good idea */
		
	    p = tbuf;
	    i = bp-buf < nbyte;
	    while (i) {
			if ((*p++ = *bp++) == '\n') { /* rpt 12-28-87 should be CRLF */
				*(p-1) = '\r';
				*p++   = '\n';
			}
			i = bp-buf < nbyte;
			if (p-tbuf >= 510 || !i) {
				tl = p-tbuf;
				if ((t = Fwrite(fildes, tl, tbuf)) != tl) {
					errno = t;
					return -1;
				}
/*				count += tl; *** This returns actual # of bytes written */
				p = tbuf;
			}
	    }
		count = bp-buf;			/* EGP 8-4-87: should return asked for bytes */
	}
	else {
	    if ((tl = Fwrite(fildes, (long)nbyte, buf)) != nbyte) {
			errno = tl;
			return -1;
		}
	    count += tl;
	}
	
    return count;
}
