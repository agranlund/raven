/**
 * $Id: ios1.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Level 1 I/O interface file
 *
 * (c) Copyright 1989, 1993 HiSoft
**/
 
#ifndef _IOS1_H
#define _IOS1_H

/*
 * The following structure is a UNIX file block that retains information
 * about a file being accessed via the level 1 I/O functions.
 */
struct UFB {
	short ufbflg;		/* flags */
	short ufbfh;		/* file handle */
};
#define NUFBS 32	/* number of UFBs defined */

/*
 *
 * UFB.ufbflg definitions
 *
 */
#define UFB_RA 1	/* reading is allowed */
#define UFB_WA 2	/* writing is allowed */
#define UFB_NC 4	/* no close */
#define UFB_FH 16	/* file handle */

/*
 *
 * External definitions
 *
 */

extern struct UFB _ufbs[NUFBS];
extern int _iomode,_nufbs,__umask;

struct UFB *chkufb(int);
#endif
