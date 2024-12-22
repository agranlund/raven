/**
 * $Header: e:/src/lib/internal\_cxstub.c,v 1.1 1993/11/05 11:12:40 AGK Exp $
 *
 * Routine called for undefined functions
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#include <__lib/cx.h>

void __stdargs
_CXSTUB(void)
{
	_CXRTE("Undefined function call");
}
