/**
 * $Id: utime.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * POSIX file access times
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _UTIME_H
#define _UTIME_H

#ifndef __TYPES_H
#include <__types.h>
#endif

struct utimbuf {
	__time_t actime;
	__time_t modtime;
};

int (utime)(const char *, const struct utimbuf *);
#endif
