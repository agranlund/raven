/**
 * $Id: macros.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Common macros
 *
 * (c) Copyright 1992 HiSoft
**/

#ifndef _MACROS_H
#define _MACROS_H

int __builtin_abs(int);
#define abs(i) __builtin_abs(i)

int __builtin_min(int,int);
#define min(a,b)	__builtin_min(a,b)

int __builtin_max(int,int);
#define max(a,b)	__builtin_max(a,b)
#endif
