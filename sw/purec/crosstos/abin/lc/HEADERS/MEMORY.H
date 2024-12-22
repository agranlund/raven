/**
 * $Id: memory.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Memory allocation declarations
 *
 * (c) Copyright 1991, 1993 HiSoft
**/
 
#ifndef _MEMORY_H
#define	_MEMORY_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long int size_t;
#endif

void *malloc(size_t);
void *calloc(size_t,size_t);
void *realloc(void *, size_t);
void free(void *);

void *getml(size_t);
int rlsml(void *, size_t);
size_t sizmem(void);
size_t chkml(void);
int bldmem(int);

void *getmem(unsigned);
int rlsmem(void *, unsigned);

void *alloca(size_t);

extern size_t _stkdelta;

void *sbrk(unsigned);
void *lsbrk(long);
#endif
