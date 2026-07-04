#ifndef __dsp_aouthdr_h__
#define __dsp_aouthdr_h__ 1

/* Values for the magic field in aouthdr
 */
#define	OMAGIC	0407L
#define	NMAGIC	0410L
#define	ZMAGIC	0413L
#define	LIBMAGIC	0443L
#define	N_BADMAG(x) (((x).magic)!=OMAGIC && ((x).magic)!=NMAGIC && ((x).magic)!=ZMAGIC && ((x).magic)!=LIBMAGIC)

typedef	struct aouthdr {
	int32_t	magic;		/* see above				*/
	int32_t	vstamp;		/* version stamp			*/
	int32_t	tsize;		/* text size in words */
	int32_t	dsize;		/* initialized data "  "		*/
	int32_t	bsize;		/* uninitialized data "   "		*/

	CORE_ADDR entry;		/* entry pt.				*/
	CORE_ADDR text_start;	/* base of text used for this file	*/
	CORE_ADDR data_start;	/* base of data used for this file	*/
	CORE_ADDR text_end;	/* end address of text used for this file	*/
	CORE_ADDR data_end;	/* end address of data used for this file	*/

} AOUTHDR;
#define AOUTHSZ sizeof(AOUTHDR)

#endif
