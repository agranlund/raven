/**
 * $Header: e:/src/lib/internal\_cxrte.c,v 1.1 1993/11/05 11:12:26 AGK Exp $
 *
 * Run time error handler (TOS version)
 *
 * (c) Copyright 1993 HiSoft
**/

#include <__lib/cx.h>
#include <mintbind.h>

void __regargs
(_CXRTE)(const char *s)
{
	extern int __mint;

	if (__mint >= 98)
		Salert(s);
	else {
		while (*s)
			Bconout(2, *s++);
	}
	if (__mint)
		Pkill(Pgetpid(), 6);
	Pterm(-39);
}
