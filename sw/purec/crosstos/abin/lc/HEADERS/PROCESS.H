/**
 * $Id: process.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Process control definitions
 *
 * (c) Copyright 1990, 1991, 1993 HiSoft
**/

#ifndef _PROCESS_H
#define	_PROCESS_H

#include <osbind.h>

int exec(const char *,const char *,const char *,int *);

#define exec(a,b,c,d)	(*(d)=(int)Pexec(0,(a),(b),(c)))

#define P_WAIT	0
#define P_NOWAIT	1
#define P_OVERLAY	2

int spawnl(int,const char *,...);
int spawnle(int,const char *,...);
int spawnlp(int,const char *,...);
int spawnlpe(int,const char *,...);
int spawnv(int,const char *,const char **);
int spawnve(int,const char *,const char **,const char **);
int spawnvp(int,const char *,const char **);
int spawnvpe(int,const char *,const char **,const char **);
int forkl(const char *,...);
int forkle(const char *,...);
int forklp(const char *,...);
int forklpe(const char *,...);
int forkv(const char *,const char **);
int forkve(const char *,const char **,const char **);
int forkvp(const char *,const char **);
int forkvpe(const char *,const char **,const char **);

int system(const char *);
#endif
