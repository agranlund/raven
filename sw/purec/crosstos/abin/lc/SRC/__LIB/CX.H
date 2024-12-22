/**
 * $Header: e:/src/include/__lib\cx.h,v 1.1 1993/11/09 10:14:22 AGK Exp $
 *
 * Private CX error definitions for runtime library
 *
 * (c) Copyright 1993 HiSoft
**/

#ifndef ___CX_H
#define ___CX_H
void __regargs _CXRTE(const char *);
void __stdargs _CXOVF(void);
void __stdargs _CXSTUB(void);
#endif
