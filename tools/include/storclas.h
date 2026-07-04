#ifndef __dsp_storclas_h__
#define __dsp_storclas_h__

/*
 *   STORAGE CLASSES
 */

#define  C_EFCN          -1L    /* physical end of function */
#define  C_NULL          0L
#define  C_AUTO          1L     /* automatic variable */
#define  C_EXT           2L     /* external symbol */
#define  C_STAT          3L     /* static */
#define  C_REG           4L     /* register variable */
#define  C_EXTDEF        5L     /* external definition */
#define  C_LABEL         6L     /* label */
#define  C_ULABEL        7L     /* undefined label */
#define  C_MOS           8L     /* member of structure */
#define  C_ARG           9L     /* function argument */
#define  C_STRTAG        10L    /* structure tag */
#define  C_MOU           11L    /* member of union */
#define  C_UNTAG         12L    /* union tag */
#define  C_TPDEF         13L    /* type definition */
#define C_USTATIC	 14L    /* undefined static */
#define  C_ENTAG         15L    /* enumeration tag */
#define  C_MOE           16L    /* member of enumeration */
#define  C_REGPARM	 17L    /* register parameter */
#define  C_FIELD         18L    /* bit field */
#define C_MEMREG         19L    /* memory locations used as registers */
#define C_OPTIMIZED      20L    /* objects optimized away */

#define  C_BLOCK         100L   /* ".bb" or ".eb" */
#define  C_FCN           101L   /* ".bf" or ".ef" */
#define  C_EOS           102L   /* end of structure */
#define  C_FILE          103L   /* file name */

	/*
	 * The following storage class is a "dummy" used only by STS
	 * for line number entries reformatted as symbol table entries
	 */

#define  C_LINE		 104L
#define  C_ALIAS	 105L   /* duplicate tag */
#define  C_HIDDEN	 106L   /* special storage class for external */

	/*
	 * The following storage classes support Motorola DSP assembly
	 * language symbolic debugging.
	 */

#define  A_FILE		 200L	/* assembly language source file name */
#define	 A_SECT		 201L	/* logical section start/end symbol */
#define	 A_BLOCK	 202L	/* COFF section debug symbol */
#define	 A_MACRO	 203L	/* macro debug symbol */
#define  A_GLOBAL	 210L	/* global assembly language symbol */
#define  A_XDEF		 211L	/* XDEFed assembly language symbol */
#define  A_XREF		 212L	/* XREFed assembly language symbol */
#define  A_SLOCAL	 213L	/* section local label */
#define  A_ULOCAL	 214L	/* underscore local label */
#define  A_MLOCAL	 215L	/* macro local label */

#endif
