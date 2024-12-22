
#include <fcntl.h>
#include <osbind.h>
#include <stdio.h>

struct {
    int refnum;
    int flag;	/* bit 1=binary mode, 0=translated mode. 
		   bit 2=Append mode */
} _binary[_NFILE+3];	/* +3 for devices */

_binadd(refnum, flag)
int refnum, flag;
{
    int i;

    for (i=0; i<_NFILE+3; i++)
	if (!_binary[i].refnum) {
	    _binary[i].refnum = refnum;
	    _binary[i].flag = flag;
	    break;
	}
}

_binrem(refnum)
int refnum;
{
    int i;

    for (i=0; i<_NFILE+3; i++)
	if (_binary[i].refnum == refnum)
	    _binary[i].refnum = 0;
}

int _bintst(refnum)
register int refnum;
{
	int bin_size = sizeof _binary;

#ifndef LINT
#ifdef PCREL
    asm {
	lea _binary(A4), A0
	movea.l A0, A1
	adda.w bin_size(A6), A1
    loop:
	cmp (A0), refnum
	beq done
	addq #4, A0
	cmpa.l A0, A1
	bne loop
    done:
	move.w 2(A0), D0    ; return flag (2 into _binary struct)
    }
#else
    asm {
	lea _binary, A0
	movea.l A0, A1
	adda.w bin_size(A6), A1
    loop:
	cmp (A0), refnum
	beq done
	addq #4, A0
	cmpa.l A0, A1
	bne loop
    done:
	move.w 2(A0), D0    ; return flag (2 into _binary struct)
    }
#endif
#endif
}

int open(path, oflag, mode)
char *path;
int oflag;  /* see "fcntl.h" for flag values */
int mode;	/* unused on atari */
{
    register int result;

    if (!strcmp(path, "CON:"))
	result = STDIN;
    else if (!strcmp(path, "AUX:"))
	result = STDAUX;
    else if (!strcmp(path, "PRT:"))
	result = STDPRT;
    else 
	if (oflag & O_CREAT) {
	    if ((result = Fopen(path, oflag&3)) < 0 || (oflag&O_TRUNC)) {
		if (result >= 0) Fclose(result);
	        result = Fcreate(path, 0); /* Fcreate truncates existing file */
	    }
	}
	else 
	    if (oflag & O_TRUNC) {
	        if ((result = Fopen(path, oflag&3)) >= 0) {
		    Fclose(result);
		    result = Fcreate(path, 0);
		}
	    }
	    else
		result = Fopen(path, oflag&3);

    if (result < 0) {
	errno = result;
	return -1;
    }

    _binadd(result, oflag);
    return result;
}
