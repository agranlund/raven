/*	atst.out.h	.1	24Dec85	*/
/*
 * Header prepended to each Atari ST .prg file
 */
#ifndef _STAOUT_H
#define _STAOUT_H

#ifdef __cplusplus
extern "C" {
#endif

struct aexec {
	 short	a_magic;	/* magic number */
unsigned long	a_text;		/* size of text segment */
unsigned long	a_data;		/* size of initialized data */
unsigned long	a_bss;		/* size of uninitialized data */
unsigned long	a_syms;		/* size of symbol table */
unsigned long	a_AZero1;	/* always zero */
unsigned long	a_ldflgs;	/* program load flags */
#define a_AZero2 a_ldflgs	/* compatibility */
unsigned short	a_isreloc;	/* is reloc info present */
};

/*
 * ldflgs - stolen from MiNT mem.h
 */

#define F_FASTLOAD	0x01	/* don't clear heap */
#define F_ALTLOAD	0x02	/* OK to load in alternate RAM */
#define F_ALTALLOC	0x04	/* OK to malloc from alt. RAM */
/* #define F_RESERVED	0x08	* reserved for future use */
#define F_SMALLTPA	0x08	/* Used in Magic; TPA can be allocated
				   as specified in the program header
				   rather than the biggest free memory
				   block*/
#define F_MEMFLAGS	0xf0	/* reserved for future use */
#define F_SHTEXT	0x800	/* program's text may be shared */

#define BASE_OFFSET	(-32768)

#define	CMAGIC	0x601A		/* contiguous text */
#define	ISRELOCINFO	0	/* relocation information is present */
				/*  any other value - no reloc info  */
/*
 * Macros which take exec structures as arguments and tell whether
 * the file has a reasonable magic number or offsets to text|symbols|strings.
 */
#define	A_BADMAG(x) 	(((x).a_magic)!=CMAGIC)
/* this is busted...
#define	A_TXTOFF(x)	((x).a_magic==ZMAGIC ? 1024 : sizeof (struct exec))
*/
#define	A_TXTOFF(x)	(sizeof (struct aexec))
#define A_DATOFF(x)	(A_TXTOFF(x) + (x).a_text)
#define A_SYMOFF(x)	(A_TXTOFF(x) + (x).a_text+(x).a_data)
#define	A_STROFF(x)	(A_SYMOFF(x) + (x).a_syms)

/*
 * Format of a symbol table entry
 */
struct	asym 
	{
	char		a_name[8];	/* symbol name */
	unsigned short	a_type;		/* type flag, i.e. N_TEXT etc; see below */
	unsigned long	a_value;	/* value of this symbol (or sdb offset) */
	};

/*
 * Simple values for a_type.
 */
#define A_UNDF	0		/* ?? for gas */
#define	A_BSS	0x0100		/* bss */
#define	A_TEXT	0x0200		/* text */
#define	A_DATA	0x0400		/* data */
#define	A_EXT	0x0800		/* external */
#define A_EQREG	0x1000		/* equated register */
#define A_GLOBL	0x2000		/* global */
#define A_EQU	0x4000		/* equated */
#define A_DEF	0x8000		/* defined */
#define A_LNAM  0x0048		/* gst compat long name */
                                /* file symbols ala aln */
#define A_TFILE 0x0280          /* text file corresponding to object module */
#define A_TFARC 0x02C0          /* text file archive */

#ifdef __cplusplus
}
#endif

#endif /* _STAOUT_H */
