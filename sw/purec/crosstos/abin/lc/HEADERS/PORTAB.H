/**
 * $Id: portab.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Various portability definitions
 *
 * (c) Copyright 1989, 1991, 1993 HiSoft
**/

#ifndef _PORTAB_H
#define _PORTAB_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#if defined(_UNSIGNEDCHAR)
#define UCHARA 1
#else
#undef UCHARA
#endif

#define PORTAB_TYPEDEFS	1

#define ANSI_C	1
#define STDARGS	__stdargs __saveds
#define HIBIT_1ST	1

typedef signed char BYTE;
typedef unsigned char UBYTE;
#ifndef _WORD_T
#define _WORD_T
typedef signed short WORD;
#endif
typedef unsigned short UWORD;
typedef signed long LONG;
typedef unsigned long ULONG;
typedef void VOID;

typedef int BOOLEAN;

typedef void *VOIDP;

#undef __PROTO
#ifndef __NOPROTO
#define __PROTO(a) a
#else
#define __PROTO(a) ()
#endif

typedef int (*CMPFUNC)(const void *,const void *);
typedef void (*VOIDFUNC)(void);

#define REG	register
#define LOCAL	auto
#define EXTERN	extern
#define MLOCAL	static
#define GLOBAL	/* */

#define	TRUE	(!0)
#define	FALSE	(0)
#define YES	TRUE
#define NO	FALSE
#define SUCCESS	TRUE
#define FAILURE	FALSE

#define EOF (-1)
#define NULLPTR	NULL
#define NULLFUNC	((void(*)(void))0)

#ifndef NULL
#define NULL	__NULL
#endif
#endif
