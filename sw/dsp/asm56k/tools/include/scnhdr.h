#ifndef __dsp_scnhdr_h__
#define __dsp_scnhdr_h__ 1

struct scnhdr {
	char		s_name[8];	/* section name */
	CORE_ADDR	s_paddr;	/* physical address, aliased s_nlib */
	CORE_ADDR	s_vaddr;	/* virtual address */
	int32_t		s_size;		/* section size */
	int32_t		s_scnptr;	/* file ptr to raw data for section */
	int32_t		s_relptr;	/* file ptr to relocation */
	int32_t		s_lnnoptr;	/* file ptr to gp histogram */
	uint32_t	s_nreloc;	/* number of relocation entries */
	uint32_t	s_nlnno;	/* number of gp histogram entries */
	int32_t		s_flags;	/* flags */
	};

/* the number of shared libraries in a .lib section in an absolute output file
 * is put in the s_paddr field of the .lib section header, the following define
 * allows it to be referenced as s_nlib
 */

#define s_nlib	s_paddr
#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)


/*
 * Define constants for names of "special" sections
 */

#define	_TEXT	".text"
#define	_DATA	".data"
#define	_BSS	".bss"
#define	_TV	".tv"
#define _INIT ".init"
#define _FINI ".fini"
#define _LIB ".lib"

/*
 * The low 4 bits of s_flags is used as a section "type"
 */

#define STYP_REG	0x00L		/* "regular" section:
						allocated, relocated, loaded */
#define STYP_DSECT	0x01L		/* "dummy" section:
						not allocated, relocated,
						not loaded */
#define STYP_NOLOAD	0x02L		/* "noload" section:
						allocated, relocated,
						 not loaded */
#define STYP_GROUP	0x04L		/* "grouped" section:
						formed of input sections */
#define STYP_PAD	0x08L		/* "padding" section:
						not allocated, not relocated,
						 loaded */
#define STYP_COPY	0x10L		/* "copy" section:
						for decision function used
						by field update;  not
						allocated, not relocated,
						loaded;  reloc & lineno
						entries processed normally */
#define	STYP_TEXT	0x20L		/* section contains text only */
#define STYP_DATA	0x40L		/* section contains data only */
#define STYP_BSS	0x80L		/* section contains bss only */
#define STYP_DEBUG 0x0100L /* DWARF debug section */

/*
 *  In a minimal file or an update file, a new function
 *  (as compared with a replaced function) is indicated by S_NEWFCN
 */

#define S_NEWFCN  0x100L

/*
 * In 3b Update Files (output of ogen), sections which appear in SHARED
 * segments of the Pfile will have the S_SHRSEG flag set by ogen, to inform
 * dufr that updating 1 copy of the proc. will update all process invocations.
 */
#define S_SHRSEG	0x20L

/*
 * Motorola DSP section header flags
 */
#define STYP_BLOCK	0x400L
#define STYP_OVERLAY	0x800L
#define STYP_MACRO	0x1000L
#define STYP_BW		0x2000L
#define STYP_OVERLAYP   0x4000L

/*
 * Flags added for block define pseudo instructions to identify run-time space.
 */
#define STYP_P_RUNSPACE		0x10000L
#define STYP_X_RUNSPACE		0x20000L
#define STYP_Y_RUNSPACE		0x40000L
#define STYP_EMI_RUNSPACE	0x80000L

#endif
