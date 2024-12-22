/**
 * $Id: stddef.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * ANSI common definitions
 *
 * (c) Copyright 1993 HiSoft
**/

#ifndef _STDDEF_H
#define _STDDEF_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef NULL
#define NULL	__NULL
#endif

#define offsetof(type, memb)	((size_t)&((type *)0)->memb)

typedef __ptrdiff_t ptrdiff_t;

#ifndef _SIZE_T
#define _SIZE_T
typedef __size_t size_t;
#endif

#ifndef _WCHAR_T
#define _WCHAR_T
typedef __wchar_t wchar_t;
#endif
#endif
