/**
 * $Id: stdarg.h,v 1.2 1993/11/14 14:43:52 AGK Rel $
 *
 * ANSI variable argument mechanism
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _STDARG_H
#define _STDARG_H

#ifndef __TYPES_H
#include <__types.h>
#endif

typedef __va_list va_list;

#define __va(a)	((sizeof(a) + 1) & ~1ul)
#define va_start(a,b)	((void)((a) = (va_list)&(b) + __va(b)))
#define va_arg(a,b)	(*(b *)(((a) += __va(b)) - __va(b)))
#define va_end(a)	((void)0)
#endif
