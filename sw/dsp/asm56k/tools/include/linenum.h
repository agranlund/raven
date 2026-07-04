#ifndef __dsp_linenum_h__
#define __dsp_linenum_h__ 1

/*  There is one line number entry for every 
    "breakpointable" source line in a section.
    Line numbers are grouped on a per function
    basis; the first entry in a function grouping
    will have l_lnno = 0 and in place of physical
    address will be the symbol table index of
    the function name.
*/
struct lineno
{
	union
	{
		int32_t	l_symndx ;	/* sym. table index of function name
						iff l_lnno == 0      */
		CORE_ADDR l_paddr ;	/* (physical) address of line number */
	}		l_addr ;
	uint32_t	l_lnno ;	/* line number */
};

#define	LINENO	struct lineno
#define	LINESZ	(sizeof(LINENO)) 

#endif
