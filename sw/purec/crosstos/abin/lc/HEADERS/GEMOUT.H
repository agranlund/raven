/**
 * $Id: gemout.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * GEMDOS file format definitions (aka a.out.h)
 *
 * (c) Copyright 1991, 1993 HiSoft
**/

#ifndef _GEMOUT_H
#define _GEMOUT_H

/*
 * GEMDOS object header
 */
struct gemohdr {
#define GEMOMAGIC	0x601a		/* executable magic number */
	short g_magic;				/* magic number */
#define GO_TEXT		0			/* text section */
#define GO_DATA		1			/* data section */
#define GO_BSS		2			/* BSS */
#define GO_SYM		3			/* symbol table */
#define GEMNSEG		4			/* number of sections */
	long g_ssize[GEMNSEG];		/* size of sections */
	long g_reserved;			/* zero */
#define GF_FAST		0x01		/* fastload bit */
#define GF_TTMALLOC	0x02		/* Malloc from TT RAM */
#define GF_LOAD		0x04		/* load into TT RAM */
#define GF_PROT_P	0x00
#define GF_PROT_G	0x10
#define GF_PROT_S	0x20
#define GF_PROT_PR	0x30
#define GF_SHTEXT	0x800
#define GF_TPASHIFT	28			/* bits to shift for TPA size field */
	long g_flags;				/* loadbits */
	short g_relflag;			/* non-zero if no relocation */
};

/*
 * GEMDOS symbol
 */
struct gemsym {
	char gs_name[8];			/* name */
#define GS_HISOFT	0x0048		/* HiSoft extended */
#define	GS_ARCHIVE	0x0040		/* archive */
#define	GS_FILE	0x0080			/* file */
#define	GS_BSS	0x0100			/* bss */
#define	GS_TEXT	0x0200			/* text */
#define	GS_DATA	0x0400			/* data */
#define	GS_EXT	0x0800			/* external */
#define GS_EQREG	0x1000		/* equated register */
#define GS_GLOBL	0x2000		/* global */
#define GS_EQU	0x4000			/* equated */
#define GS_DEF	0x8000			/* defined */
	unsigned short gs_type;		/* type */
	long gs_value;				/* value */
};

/*
 * GEMDOS executable relocation types
 */
#define GR_ABS	0				/* absolute */
#define GR_DATA	1				/* data relative */
#define GR_TEXT	2				/* text relative */
#define GR_BSS	3				/* BSS relative */
#define GR_SYM	4				/* symbol */
#define GR_LONG	5				/* first word of longword fixup */
#define GR_PC	6				/* pc relative */
#define GR_INST	7				/* first word of instruction */
#define GR_MASK	7				/* mask of relocation types */
#define GR_SHIFT	3			/* amount to shift */

/*
 * GEMDOS archive format
 */
#define GEMAMAGIC	0xff65

struct gemahdr {
	char ga_name[14];			/* file name */
	long ga_date;				/* last modified time */
	char ga_uid;				/* not used */
	char ga_gid;				/* not used */
	short ga_mode;				/* mode word */
	long ga_size;				/* size in bytes */
	short ga_fill;				/* zero */
};

#endif
