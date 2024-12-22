
#include <stdio.h>
#include <osbind.h>
#include <fcntl.h>

#define BUFFLEN 80    /* size of stdin input buffer */

/*
	3-30-87	EGP		Added stdin_read() and stdout_write() which read
					and write to fd's 0 and 1.  Needed for Beckeymeyer shell.
	7-31-87 EGP		added "bintst & O_BINARY" since bintst has all flag values
						now.
	6-17-88	EGP		_chread() lets you continue reading past ^Z (EOF)
	7-01-88 MAR		erased a character when backspace was hit
*/

static int _chread(fd)
int fd;
{
    char c;

    if (!Fread(fd, 1L, &c))
	c = 26;			/* ^Z == EOF */
    return c;
}

int getkey()
{
    int i;
    int ch;
    static char buff[BUFFLEN];	  /* input buffer for stdin */
    static int frontbuff = 0, endbuff = 0;

    if (frontbuff == endbuff) {
	frontbuff = endbuff = 0;
	while (1) {
	    ch = _chread(STDIN);
	    if (ch == '\b') {
			if (isatty(STDOUT)) {	/* erase character */
				Bconout(2, ' ');
				Bconout(2, '\b');
			}
			if (frontbuff)
				frontbuff--;
	    }
	    else {
		if (ch == '\r') {
		    buff[frontbuff++] = '\n';
		    Bconout(2, '\n');
		    break;
		}
		if (ch == 3)    /* ^C */
		    exit(2);
		if (ch == 26) {  /* ^Z, EOF */
		    buff[frontbuff++] = ch;
		    Bconout(2, '\r'); Bconout(2, '\n');
		    break;
		}
		buff[frontbuff++] = ch;
	    }
	}
    }

	if (buff[endbuff] == 26) {
		++endbuff;
		return -1;
	}
	else
		return buff[endbuff++];
}

int read(fildes, buf, nbyte)
int fildes;
char *buf;
unsigned nbyte;
{
    long count;
    register char *bp = buf;
    register char *tp;
    long i;

    count = nbyte;
    errno = 0;
    if (isatty(fildes)) {
	count = 0;
	while (nbyte--)
	    if ((*bp = getkey()) == -1)
		break;
	    else {
		bp++;
		count++;
	    }
	return count;	/* don't translate from keyboard */
    }
    else
	if ((count = Fread(fildes, count, buf)) < 0) {
		errno = count;
		return -1;
	}

    if (!(_bintst(fildes)&O_BINARY)) { /* translate CR/LF to LF */
	nbyte = count;
	tp = bp = buf;
	while (nbyte) {
	    if (bp-buf >= count) {
		if ((i = Fread(fildes, (long)nbyte, tp)) < 0) {
			errno = i;
			return -1;
		}
		if (!i) 	/* reached EOF */
		    break;
		count -= nbyte-i;
		bp = tp;
	    }

	    if (*bp == '\r')
		bp++;
	    else {
		*tp++ = *bp++;
	    	nbyte--;
	    }
	}
	count = tp-buf;
    }
    return count;
}
