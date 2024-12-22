/**
 * $Id: acc.h,v 1.1 1993/11/14 14:42:42 AGK Rel $
 *
 * Desk Accessory support definitions
 *
 * (c) Copyright 1991, 1993 HiSoft
**/

#ifndef _ACC_H
#define _ACC_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long int size_t;
#endif

/*
 * STACK macro - define a size and storage area for a DA's stack; must be used
 * globally (it will fail if a local attempt is made)
 */
#define STACK(x)	size_t __far _STACK_HEAP[(x + sizeof(size_t) - 1) / sizeof(size_t)]; \
	extern size_t _STACK = sizeof(_STACK_HEAP)

void _addheap(void *,size_t);
#endif
