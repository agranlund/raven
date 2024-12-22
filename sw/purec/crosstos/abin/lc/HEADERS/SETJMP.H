/**
 * $Id: setjmp.h,v 1.2 1993/11/14 14:43:52 AGK Rel $
 *
 * ANSI/POSIX.1 non-local jumps
 *
 * (c) Copyright 1992, 1993 HiSoft
**/
 
#ifndef _SETJMP_H
#define _SETJMP_H

#ifdef _M881
#define _JMP_BUF	37
#else
#define _JMP_BUF	13
#endif

typedef struct {long __jmp_buf[_JMP_BUF];} jmp_buf[1];

void (longjmp)(jmp_buf,int);
int (setjmp)(jmp_buf);
#define setjmp(a)	((setjmp)(a))

#if !defined(_ANSI) \
	|| defined(_POSIX_SOURCE)
#ifndef __TYPES_H
#include <__types.h>
#endif

#define _SIGJMP_BUF	_JMP_BUF + (sizeof(__sigset_t) / sizeof(long))

typedef struct {long __sigjmp_buf[_SIGJMP_BUF];} sigjmp_buf[1];

void (siglongjmp)(sigjmp_buf, int);
int (sigsetjmp)(sigjmp_buf, int);
#define sigsetjmp(a, b)	((sigsetjmp)(a, b))
#endif
#endif
