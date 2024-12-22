/**
 * $Id: gemextra.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Lattice 3 compatability file (obsolete)
 *
 * (c) Copyright 1989, 1993 HiSoft
**/

#ifndef _GEMEXTRA_H
#define _GEMEXTRA_H

#ifndef min
int min(int,int);
int __builtin_min(int,int);
#define min(a,b)	__builtin_min(a,b)
#endif

#ifndef max
int max(int,int);
int __builtin_max(int,int);
#define max(a,b)	__builtin_max(a,b)
#endif

int rc_intersect(const GRECT *,GRECT *);
#endif
