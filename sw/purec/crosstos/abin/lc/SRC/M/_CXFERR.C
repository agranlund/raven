/**
 * $Header: e:/src/math/fn\_cxferr.c,v 1.2 1993/10/27 10:14:56 AGK Exp $
 *
 * Low-level floating point error trap
 *
 * (c) Copyright 1989 Lattice, Inc.
 * (c) Copyright 1989, 1993 HiSoft
**/

#include <signal.h>

extern int _FPERR;

void __stdargs
(_CXFERR)(int code)
{
	_FPERR = code;

	raise(SIGFPE);
}
