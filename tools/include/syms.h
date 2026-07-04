#ifndef __dsp_syms_h__
#define __dsp_syms_h__ 1

/*	Defines for "special" symbols   */

#define _ETEXT	"etext"
#define _EDATA	"edata"
#define _END	"end"

#define _START	"__start"

/*		Storage Classes are defined in storclas.h  */
#include "storclas.h"

/*		Number of characters in a symbol name */
#define  SYMNMLEN	8
/*		Number of characters in a file name */
#define  FILNMLEN	16
/*		Number of array dimensions in auxiliary entry */
#define  DIMNUM		4


struct syment
{
	union
	{
		char		_n_name[SYMNMLEN];	/* old COFF version */
		struct
		{
			int32_t	_n_zeroes;	/* new == 0 */
			int32_t	_n_offset;	/* offset into string table */
		} _n_n;
		/* char		*_n_nptr[2]; */	/* allows for overlaying */
	} _n;
	union {
		CORE_ADDR _n_address; /* when an address */
		uint32_t _n_val[2]; /* _n_val[0] is least significant */
	} _n_value;	/* value of symbol */
	int32_t			n_scnum;	/* section number */
	uint32_t		n_type;		/* type and derived type */
	int32_t			n_sclass;	/* storage class */
	int32_t			n_numaux;	/* number of aux. entries */
};

#define n_name		_n._n_name
#define n_nptr		_n._n_nptr[1]
#define n_zeroes	_n._n_n._n_zeroes
#define n_offset	_n._n_n._n_offset
#define n_value _n_value._n_address
/*
   Relocatable symbols have a section number of the
   section in which they are defined.  Otherwise, section
   numbers have the following meanings:
*/
        /* undefined symbol */
#define  N_UNDEF	0L
        /* value of symbol is absolute */
#define  N_ABS		-1L
        /* special debugging symbol -- value of symbol is meaningless */
#define  N_DEBUG	-2L

/*
   The fundamental type of a symbol packed into the low 
   4 bits of the word.
*/

#define  _EF	".ef"

#define  T_NULL     0L
#define  T_ARG      1L          /* function argument (only used by compiler) */
#define  T_VOID     1L          /* void */
#define  T_CHAR     2L          /* character */
#define  T_SHORT    3L          /* short integer */
#define  T_INT      4L          /* integer */
#define  T_LONG     5L          /* long integer */
#define  T_FLOAT    6L          /* floating point */
#define  T_DOUBLE   7L          /* double word */
#define  T_STRUCT   8L          /* structure  */
#define  T_UNION    9L          /* union  */
#define  T_ENUM     10L         /* enumeration  */
#define  T_MOE      11L         /* member of enumeration */
#define  T_UCHAR    12L         /* unsigned character */
#define  T_USHORT   13L         /* unsigned short */
#define  T_UINT     14L         /* unsigned integer */
#define  T_ULONG    15L         /* unsigned long */

#define  T_FRAC     0x10000L         /* fractional */
#define  T_UFRAC    0x10001L         /* unsigned fractional */
#define  T_LFRAC    0x10002L         /* long fractional */
#define  T_ULFRAC   0x10003L         /* unsigned long fractional */
#define  T_ACCUM    0x10004L         /* accumulator ext:msb*/
#define  T_LACCUM   0x10005L         /* long accumulator ext:msb:lsb */

/*
 * derived types are:
 */

#define  DT_NON      0L          /* no derived type */
#define  DT_PTR      1L          /* pointer */
#define  DT_FCN      2L          /* function */
#define  DT_ARY      3L          /* array */

/*
 *   type packing constants
 */

#define  N_BTMASK     0x1000fL
#define  N_TMASK      0x30L
#define  N_BTSHFT     4L
#define  N_TSHIFT     2L
#define  N_BTMASK_LOW 0xfL
#define  N_BTMASK_HI  0x10000L
#define  N_BTSHFT_HI  16L

/*
 *   MACROS
 */

	/*   Basic Type of  x   */

#define  BTYPE(x)  ((x) & N_BTMASK)

       /* index for extended basic types - used for table lookup */
#define  BTYPE_INDEX(x)  (((x) & N_BTMASK_LOW) | (((x) & N_BTMASK_HI) >> (N_BTSHFT_HI-N_BTSHFT)))

	/*   Is  x  a  pointer ?   */

#define  ISPTR(x)  (((x) & N_TMASK) == (DT_PTR << N_BTSHFT))

	/*   Is  x  a  function ?  */

#define  ISFCN(x)  (((x) & N_TMASK) == (DT_FCN << N_BTSHFT))

	/*   Is  x  an  array ?   */

#define  ISARY(x)  (((x) & N_TMASK) == (DT_ARY << N_BTSHFT))

	/* Is x a structure, union, or enumeration TAG? */

#define ISTAG(x)  ((x)==C_STRTAG || (x)==C_UNTAG || (x)==C_ENTAG)

#define  INCREF(x) ((((x)&~N_BTMASK)<<N_TSHIFT)|(DT_PTR<<N_BTSHFT)|(x&N_BTMASK))

#define  DECREF(x) (((((x)&~N_BTMASK)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

/*
 *	AUXILIARY ENTRY FORMAT
 */

union auxent
{
	struct
	{
		int32_t		x_tagndx;	/* str, un, or enum tag indx */
		union
		{
			struct
			{
				uint32_t	x_lnno;	/* declaration line number */
				uint32_t	x_size;	/* str, union, array size */
			} x_lnsz;
			int32_t	x_fsize;	/* size of function */
		} x_misc;
		union
		{
			struct			/* if ISFCN, tag, or .bb */
			{
				int32_t	x_lnnoptr;	/* ptr to fcn line # */
				int32_t	x_endndx;	/* entry ndx past block end */
				uint32_t x_type; /* function prologue/epilogue type index */
			} x_fcn;
			struct			/* if ISARY, up to 4 dimen. */
			{
				uint32_t	x_dimen[DIMNUM];
			} 	x_ary;
		} x_fcnary;
		uint32_t  x_tvndx;		/* tv index */
	} x_sym;
	struct
	{
		char	x_fname[FILNMLEN]; /* filename here if x_foff==0 */
		uint32_t x_foff; /* if !0 then x_fname in string table */
	    uint32_t x_ftype; /* index indicating memory space used by file for stack */
	} x_file;
    struct
    {
                int32_t    x_scnlen;          /* section length */
                uint32_t  x_nreloc;  /* number of relocation entries */
                uint32_t  x_nlinno;  /* number of line numbers */
               uint32_t x_soff; /* section name offset in string table (for named sections) */
    } x_scn;

	struct
	{
		int32_t		x_tvfill;	/* tv fill value */
		uint32_t	x_tvlen;	/* length of .tv */
		uint32_t	x_tvran[2];	/* tv range */
	}	x_tv;	/* info about .tv section (in auxent of symbol .tv)) */
	struct syment filler; /* to fill out to size of syment */
};

#define	SYMENT	struct syment
#define	SYMESZ	(sizeof(SYMENT))

#define	AUXENT	union auxent
#define	AUXESZ	(sizeof(AUXENT)) 

#endif
