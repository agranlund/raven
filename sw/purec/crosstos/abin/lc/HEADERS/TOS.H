/**
 * $Id: tos.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Turbo C compatability file
 *
 * (c) Copyright 1990, 1993 HiSoft
**/

#ifndef _TOS_H
#define _TOS_H
#include <dos.h>

typedef struct {
	char d_reserved[21];
	char d_attrib;
	short d_time;
	short d_date;
	long d_length;
	char d_fname[14];
} DTA;
#endif
