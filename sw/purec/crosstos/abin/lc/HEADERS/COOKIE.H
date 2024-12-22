/**
 * $Id: cookie.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * BIOS cookie interface header file
 *
 * (c) Copyright 1991, 1993 HiSoft
**/

#ifndef _COOKIE_H
#define _COOKIE_H

/* structure definition for user cookie access */
typedef struct {
	long cookie;
	long value;
} COOKIE;

#ifndef _p_cookies
#define _p_cookies	(*(COOKIE **)0x5a0)
#endif

extern long __cookie;	/* cookie number of matching cookie from getcookie */

/* standard system cookies */
#define _CPU	0x5f435055
#define _VDO	0x5f56444f
#define _SND	0x5f534e44
#define _MCH	0x5f4d4348
#define _SWI	0x5f535749
#define _FRB	0x5f465242
#define _FPU	0x5f465055
#define _FDC	0x5f464443
#define _FLK	0x5f464c4b
#define _NET	0x5f4e4554

int (getcookie)(long, long *);
int (putcookie)(long, long);
#endif
