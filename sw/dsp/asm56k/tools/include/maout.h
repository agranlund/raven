#ifndef __dsp_maout_h__
#define __dsp_maout_h__

 /*		COMMON OBJECT FILE FORMAT

 	File Organization:

 	_______________________________________________    INCLUDE FILE
 	|_______________HEADER_DATA___________________|
 	|					      |
 	|	File Header			      |    "filehdr.h"
 	|.............................................|
 	|					      |
 	|	Auxilliary Header Information	      |	   "aouthdr.h"
 	|					      |
 	|_____________________________________________|
 	|					      |
 	|	".text" section header		      |	   "scnhdr.h"
 	|					      |
 	|.............................................|
 	|					      |
 	|	".data" section header		      |	      ''
 	|					      |
 	|.............................................|
 	|					      |
 	|	".bss" section header		      |	      ''
 	|					      |
 	|_____________________________________________|
 	|______________RAW_DATA_______________________|
 	|					      |
 	|	".text" section data (rounded to 4    |
 	|				bytes)	      |
 	|.............................................|
 	|					      |
 	|	".data" section data (rounded to 4    |
 	|				bytes)	      |
 	|_____________________________________________|
 	|____________RELOCATION_DATA__________________|
 	|					      |
 	|	".text" section relocation data	      |    "reloc.h"
 	|					      |
 	|.............................................|
 	|					      |
 	|	".data" section relocation data	      |       ''
 	|					      |
 	|_____________________________________________|
 	|__________LINE_NUMBER_DATA_(SDB)_____________|
 	|					      |
 	|	".text" section line numbers	      |    "linenum.h"
 	|					      |
 	|.............................................|
 	|					      |
 	|	".data" section line numbers	      |	      ''
 	|					      |
 	|_____________________________________________|
 	|________________SYMBOL_TABLE_________________|
 	|					      |
 	|	".text", ".data" and ".bss" section   |    "syms.h"
 	|	symbols				      |	   "storclas.h"
 	|					      |
 	|_____________________________________________|
	|________________STRING_TABLE_________________|
	|					      |
	|	    long symbol names		      |
	|_____________________________________________|



 		OBJECT FILE COMPONENTS

 	HEADER FILES:
 			/usr/include/filehdr.h
			/usr/include/aouthdr.h
			/usr/include/scnhdr.h
			/usr/include/reloc.h
			/usr/include/linenum.h
			/usr/include/syms.h
			/usr/include/storclas.h

	STANDARD FILE:
			/usr/include/maout.h    "object file" 
   */
#include "filehdr.h"
#include "aouthdr.h"
#include "scnhdr.h"
#include "reloc.h"
#include "linenum.h"
#include "syms.h"
#include "storclas.h"
/*
 * Format of an maout header
 */
 

struct	exec {	/* maout header */
	int32_t		a_magic;	/* magic number */
	uint32_t a_text;		/* size of text segment */
					/* in bytes		*/
					/* padded out to next	*/
					/* page boundary with	*/
					/* binary zeros.	*/
	uint32_t a_data;		/* size of initialized data */
					/* segment in bytes	*/
					/* padded out to next	*/
					/* page boundary with	*/
					/* binary zeros.	*/
	uint32_t a_bss;		/* Actual size of	*/
					/* uninitialized data	*/
					/* segment in bytes.	*/
	uint32_t a_syms;		/* size of symbol table */
	CORE_ADDR a_entry;	/* entry point */
};

#define	A_MAGIC1	0407L		/* normal */
#define	A_MAGIC0	0401L		/* lpd (UNIX/RT) */
#define	A_MAGIC2	0410L		/* read-only text */
#define	A_MAGIC3	0411L		/* separated I&D */
#define	A_MAGIC4	0405L		/* overlay */
#define	A_MAGIC5	0437L		/* system overlay, separated I&D */


#endif
