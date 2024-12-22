/**
 * $Header: e:/src/math/fn\_cxfpe.c,v 1.2 1993/10/27 10:14:56 AGK Exp $
 *
 * Low-level floating point error handler
 *
 * (c) Copyright 1989 Lattice, Inc.
 * (c) Copyright 1989, 1993 HiSoft
**/

#include <signal.h>
#include <math.h>
#include <errno.h>

extern int _FPERR;

void
(_CXFPE)(int sig)
{
	switch (_FPERR) {
		case FPEUND:
		case FPEOVF:
		case FPEZDV:
			errno = ERANGE;
			break;

		case FPENAN:
		case FPECOM:
			errno = EDOM;
			break;
	}
}
