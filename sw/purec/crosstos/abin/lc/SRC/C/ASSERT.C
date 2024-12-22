/**
 * $Header: e:/src/lib/assert\assert.c,v 1.2 1993/08/17 11:56:26 AGK Exp $
 *
 * Print assertion failure message
 *
 * (c) Copyright 1993 HiSoft
**/

#include <assert.h>
#include <__lib/cx.h>

void
(__assert)(const char *s)
{
	_CXRTE(s);
}
