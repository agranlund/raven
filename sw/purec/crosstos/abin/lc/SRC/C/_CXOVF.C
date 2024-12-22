/**
 * $Header: e:/src/lib/internal\_cxovf.c,v 1.4 1993/11/05 11:12:10 AGK Exp $
 *
 * Function called when stack is exhausted
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#include <__lib/cx.h>

void __stdargs
_CXOVF(void)
{
	_CXRTE("Stack space exhausted");
}
