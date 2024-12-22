/**
 * $Id: varargs.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * UNIX variable argument mechanism
 *
 * (c) Copyright 1989, 1993 HiSoft
**/

#ifndef _VARARGS_H
#define _VARARGS_H

#ifndef _VA_LIST
#define _VA_LIST
typedef char *va_list;
#endif

#define va_dcl	va_list va_alist;
#define va_start(a)	a=(va_list)&va_alist
#define va_arg(a,b)	((b *)(a+=sizeof(b)))[-1]
#define va_end(ap)
#endif
