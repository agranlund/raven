/**
 * $Id: ftw.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * File tree walking definitions
 *
 * (c) Copyright 1991, 1993 HiSoft
**/

#ifndef _FTW_H
#define _FTW_H

#include <sys/types.h>
#include <sys/stat.h>

#define	FTW_F		0	/* normal file */
#define	FTW_D		1	/* directory */
#define	FTW_DNR		2	/* unreadable directory */
#define	FTW_NS		3	/* unreadable file info */

int ftw(const char *,int (*)(const char *,struct stat *,int),int);
#endif
