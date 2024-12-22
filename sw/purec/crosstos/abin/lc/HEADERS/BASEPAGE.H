/**
 * $Id: basepage.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * GEMDOS base page structure
 *
 * (c) Copyright 1989, 1990, 1993 HiSoft
**/

#ifndef _BASEPAGE_H
#define _BASEPAGE_H

typedef struct _base BASEPAGE;
struct _base {
	void *p_lowtpa;				/* pointer to bottom of TPA */
	void *p_hitpa;				/* pointer to top of TPA + 1 */
	void *p_tbase;				/* base of text segment */
	long p_tlen;				/* length of text segment */
	void *p_dbase;				/* base of data segment */
	long p_dlen;				/* length of data segment */
	void *p_bbase;				/* base of BSS segment */
	long p_blen;				/* length of BSS segment */
	void *p_dta;				/* pointer to current DTA */
	BASEPAGE *p_parent;			/* pointer to parent's basepage */
	long p_flags;				/* memory usage flags */
	char *p_env;				/* pointer to environment strings */
	long p_undef[20];
	char p_cmdlin[128];			/* command line image */
};

extern BASEPAGE *_pbase, *_PBASE;
#endif
