/**
 * $Id: m68881.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Inline 68881 interface file
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _M68881_H
#define _M68881_H
#ifdef _M881
double __builtin_fpc(int, double);
#define acos(d) 	__builtin_fpc(0x001C,d)
#define asin(d) 	__builtin_fpc(0x000C,d)
#define atan(d) 	__builtin_fpc(0x000A,d)
#define cos(d)		__builtin_fpc(0x001D,d)
#define cosh(d) 	__builtin_fpc(0x0019,d)
#define exp(d)		__builtin_fpc(0x0010,d)
#define fabs(d) 	__builtin_fpc(0x0018,d)
#define log(d)		__builtin_fpc(0x0014,d)
#define log10(d)	__builtin_fpc(0x0015,d)
#define sin(d)		__builtin_fpc(0x000E,d)
#define sinh(d) 	__builtin_fpc(0x0002,d)
#define sqrt(d) 	__builtin_fpc(0x0004,d)
#define tan(d)		__builtin_fpc(0x000F,d)
#define tanh(d) 	__builtin_fpc(0x0009,d)

#ifndef _ANSI
#define pow2(d)		__builtin_fpc(0x0011,d)

#define fintrz(d)	__builtin_fpc(0x0003,d)
#define flognp1(d)	__builtin_fpc(0x0006,d)
#define fetoxm1(d)	__builtin_fpc(0x0008,d)
#define fatanh(d)	__builtin_fpc(0x000D,d)
#define fgetexp(d)	__builtin_fpc(0x001E,d)
#define ftentox(d)	__builtin_fpc(0x0012,d)
#define flog2(d)	__builtin_fpc(0x0016,d)
#define fneg(d) 	__builtin_fpc(0x001A,d)
#define fgetman(d)	__builtin_fpc(0x001F,d)
#endif
#endif
#endif
