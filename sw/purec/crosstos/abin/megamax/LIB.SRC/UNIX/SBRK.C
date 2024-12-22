
#include <osbind.h>
extern int errno;

char *lsbrk(incr)
unsigned long incr;
{
    long retval;

    retval = (long)Malloc(incr);
	if (!retval) {
		retval = -1;
		errno = -39;	/* Insufficient memory */
	}
	return (char *)retval;
}

char *sbrk(incr)
unsigned incr;
{
	return lsbrk((unsigned long)incr);
}
