/************************************************************************/
/*	PORTAB.H	Pointless redefinitions of C syntax.		*/
/*		Copyright 1985 Atari Corp.				*/
/*									*/
/*	WARNING: Use of this file may make your code incompatible with	*/
/*		 C compilers throughout the civilized world.		*/
/************************************************************************/
#ifndef DL_PORTAB
#define DL_PORTAB				/*  rpt  8-21-87	*/

#define mc68k 0

#define UCHARA 1				/* if char is unsigned     */
/*
 *	Standard type definitions
 */
#define	BYTE	char				/* Signed byte		   */
#define BOOLEAN	int				/* 2 valued (true/false)   */
#define	WORD	int  				/* Signed word (16 bits)   */
#define	UWORD	unsigned int			/* unsigned word	   */

#define	LONG	long				/* signed long (32 bits)   */
#define	ULONG	long				/* Unsigned long	   */

#define	REG	register			/* register variable	   */
#define	LOCAL	auto				/* Local var on 68000	   */
#define	EXTERN	extern				/* External variable	   */
#define	MLOCAL	static				/* Local to module	   */
#define	GLOBAL	/**/				/* Global variable	   */
#define	VOID	/**/				/* Void function return	   */

#ifndef DEFAULT			/*  This means that default is defined in obdefs.h  */
#define	DEFAULT	int				/* Default size		   */
#endif

#ifdef UCHARA
#define UBYTE	char				/* Unsigned byte 	   */
#else
#define	UBYTE	unsigned char			/* Unsigned byte	   */
#endif

#ifndef FAILURE
/****************************************************************************/
/*	Miscellaneous Definitions:					    */
/****************************************************************************/
#define	FAILURE	(-1)			/*	Function failure return val */
#define SUCCESS	(0)			/*	Function success return val */
#define	YES	1			/*	"TRUE"			    */
#define	NO	0			/*	"FALSE"			    */
#define	FOREVER	for(;;)			/*	Infinite loop declaration   */
#ifndef NULL
#define	NULL	0L			/*	Null pointer value	    */
#endif
#define NULLPTR (char *) 0		/*				    */
#ifndef EOF
#define	EOF	(-1)			/*	EOF Value		    */
#endif
#define	TRUE	(1)			/*	Function TRUE  value	    */
#define	FALSE	(0)			/*	Function FALSE value	    */
#endif

#endif DL_PORTAB
