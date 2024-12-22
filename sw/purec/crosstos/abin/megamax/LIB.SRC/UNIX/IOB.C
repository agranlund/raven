#include <stdio.h>

FILE _iob[_NFILE] = {
	{NULL, 0, NULL, _READ | _UNBUF | _IFLUSH, STDIN, 0},    /* stdin */
	{NULL, 0, NULL, _WRITE | _LINBUF, STDOUT, _BUFSIZE},   /* stdout */
	{NULL, 0, NULL, _WRITE | _LINBUF, STDERR, _BUFSIZE}    /* stderr */
};
int _bufsize = _BUFSIZE;	/* default buffer size */

