#include <stdio.h>
#include <osbind.h>

static int (*_onexit[8])(), (**_onexitsp)() = _onexit;

onexit(f)
int (*f)();
/*
    Install exit function
*/
{
    if (_onexitsp - _onexit < 8) {
	*_onexitsp++ = f;
	return 1;
    }

    return 0;
}

exit(result)
int result;
{
    int i;
    register FILE *fp;
    extern struct {
	int refnum;
	int flag;   /* 1=binary mode, 0=translated mode. +9 for devices */
    } _binary[];

    /* Call onexit() functions */
    while (_onexitsp - _onexit)
	(**--_onexitsp)();

    for (fp = _iob; fp<_iob+_NFILE; fp++)	/* close all streams */
	if (fp->_flag & (_READ | _WRITE | _RDWR))
	    fclose(fp);

/*   for (i=0; i<12+9; i++)   /* close all open file descriptors */
/*	if (_binary[i].refnum > 0)	/* don't close devices */
/*	    Fclose(_binary[i].refnum);   /* ignore any errors */

    _exit(result);
}
