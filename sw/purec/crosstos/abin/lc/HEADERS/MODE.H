/**
 * $Id: mode.h,v 1.3 1993/11/14 14:43:52 AGK Rel $
 *
 * Interface to Falcon video hardware
 *
 * (c) Copyright 1992 Atari Corp.
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _MODE_H
#define _MODE_H

/**
 * Vsetmode() flags
**/
#define	VERTFLAG	0x100	/* double-line/interlace */
#define	STMODES		0x80
#define	OVERSCAN	0x40
#define	PAL			0x20	/* PAL if set, else NTSC */
#define	VGA			0x10	/* VGA if set, else TV mode */
#define	COL80		0x8
#define	NUMCOLS		7
#define	BPS16		4
#define	BPS8		3
#define	BPS4		2
#define	BPS2		1
#define	BPS1		0

/**
 * Montype() values
**/
#define STmono	0
#define	STcolor	1
#define	VGAcolor	2
#define	TVcolor	3

/**
 * VsetSync() flags
**/
#define	VID_CLOCK	1
#define	VID_VSYNC	2
#define	VID_HSYNC	4
#endif
