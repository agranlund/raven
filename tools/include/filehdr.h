#ifndef __dsp_filehdr_h__
#define __dsp_filehdr_h__ 1

struct filehdr {
	uint32_t	f_magic;	/* magic number */
	uint32_t	f_nscns;	/* number of sections */
	uint32_t	f_timdat;	/* time & date stamp */
	int32_t		f_symptr;	/* file pointer to symbolic header */
	int32_t		f_nsyms;	/* sizeof(symbolic hdr) */
	uint32_t	f_opthdr;	/* sizeof(optional hdr) */
	uint32_t	f_flags;	/* flags */
	};

/*
 *   Bits for f_flags:
 *
 *	F_RELFLG	relocation info stripped from file
 *	F_EXEC		file is executable  (i.e. no unresolved
 *				externel references)
 *	F_LNNO		line nunbers stripped from file
 *	F_LSYMS		local symbols stripped from file
 *	F_MINMAL	this is a minimal object file (".m") output of fextract
 *	F_UPDATE	this is a fully bound update file, output of ogen
 *	F_SWABD		this file has had its bytes swabbed (in names)
 *	F_AR16WR	this file has the byte ordering of an AR16WR (e.g. 11/70) machine
 *				(it was created there, or was produced by conv)
 *	F_AR32WR	this file has the byte ordering of an AR32WR machine(e.g. vax)
 *	F_AR32W		this file has the byte ordering of an AR32W machine (e.g. 3b,maxi,MC68000)
 *	F_PATCH		file contains "patch" list in optional header
 *	F_NODF		(minimal file only) no decision functions for
 *				replaced functions
 */

#define  F_RELFLG	0000001L
#define  F_EXEC		0000002L
#define  F_LNNO		0000004L
#define  F_LSYMS	0000010L
#define  F_MINMAL	0000020L
#define  F_UPDATE	0000040L
#define  F_SWABD	0000100L
#define  F_AR16WR	0000200L
#define  F_AR32WR	0000400L
#define  F_AR32W	0001000L
#define  F_PATCH	0002000L
#define  F_NODF		0002000L


/* Motorola 56000/96000/56100/56300/56800/56600/SC400 */
#define   M56KMAGIC 709L
#define   M96KMAGIC 710L
#define   M16KMAGIC 711L
#define   M563MAGIC 712L
#define   M568MAGIC 713L
#define   M566MAGIC 714L
#define   SC1MAGIC 715L
#define   M567MAGIC 716L

#define	FILHDR	struct filehdr
#define	FILHSZ	sizeof(FILHDR)

/*
Don't use line continuation (e.g. backslash at end of line)
in the following macro.  It causes problems with the PC compilers
when newline characters are converted.
*/
#define ISCOFF(x) (((x)==M56KMAGIC) || ((x)==M96KMAGIC) || ((x)==M16KMAGIC) || ((x)==M563MAGIC) || ((x)==M568MAGIC) || ((x)==M566MAGIC) || ((x)==SC1MAGIC) || ((x)==M567MAGIC))


#endif
