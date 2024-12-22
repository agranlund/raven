/**
 * $Id: locale.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Localisation functions and macros
 *
 * (c) Copyright 1989, 1993 HiSoft
**/
 
#ifndef _LOCALE_H
#define _LOCALE_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned char wchar_t;
#endif

#define LC_COLLATE	0
#define LC_CTYPE	1
#define LC_NUMERIC	2
#define LC_TIME	3
#define LC_MONETARY	4
#define LC_ALL	5

struct lconv
{
	const char *decimal_point;
	const char *thousands_sep;
	const char *grouping;
#define LCONVM int_curr_symbol
	const char *int_curr_symbol;
	const char *currency_symbol;
	const char *mon_decimal_point;
	const char *mon_thousands_sep;
	const char *mon_grouping;
	const char *positive_sign;
	const char *negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
};

struct __lconvn			/* lconv from LC_NUMERIC */
{
	const char *decimal_point;
	const char *thousands_sep;
	const char *grouping;
};

struct __lconvm			/* lconv from LC_MONETARY */
{
	const char *int_curr_symbol;
	const char *currency_symbol;
	const char *mon_decimal_point;
	const char *mon_thousands_sep;
	const char *mon_grouping;
	const char *positive_sign;
	const char *negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
};

struct __locale
{
	struct __locale *nxtlc; /* next locale pointer */
	const char *lcname;		/* locale name				LC_ALL */
	const char **abswday;	/* abbreviated weekday nam	LC_TIME */
	const char **swday;		/* full weekday names		LC_TIME */
	const char **absmon;	/* abbrev. month names		LC_TIME */
	const char **smon;		/* full month names			LC_TIME */
	const char **sampm;		/* names for am. and pm.	LC_TIME */
	const char *stzone;		/* name for timezone		LC_TIME */
	const int *lccomp;		/* collating sequenc		LC_COLLATE? */
	const int *ixfrm;		/* translate table			LC_COLLATE */
	char decpt;				/* decimal point char		LC_NUMERIC */
	char mb_cur_max;		/* max bytes in wide character	LC_CTYPE */
	char mb_state;			/* flag for state-dep mappings	LC_CTYPE */
							/* mblen function			LC_CTYPE */
	int  (*mblen)(const char *, size_t);
	int  mblen_state;		/* mblen shift state		LC_CTYPE */
							/* mbtowc function			LC_CTYPE */
	int  (*mbtowc)(wchar_t *,const char *, size_t);
	int  mbtowc_state;		/* mbtowc shift state		LC_CTYPE */
							/* wctomb function			LC_CTYPE */
	int  (*wctomb)(char *, wchar_t);
	int  wctomb_state;		/* wctomb shift state		LC_CTYPE */
							/* mbstowcs function		LC_CTYPE */
	size_t	(*mbstowcs)(wchar_t *,const char *, size_t);
	int  mbstowcs_state;	/* mbstowcs shift state 	LC_CTYPE */
							/* wcstombs function		LC_CTYPE */
	size_t	(*wcstombs)(char *, const wchar_t *,size_t);
	int  wcstombs_state;	/* wcstombs shift state		LC_CTYPE */
	struct __lconvn	*lconvn; /* structure for lconv		LC_NUMERIC */
	struct __lconvm	*lconvm; /* structure for lconv		LC_MONETARY */
};

extern struct __locale *__clocale[];	/* current locale for each category */
extern struct lconv  __clconv;			/* current lconv array */
extern char __decpt;					/* current decimal point character */
#define DECPT __decpt
extern char __mb_cur_max;
extern struct __locale *__flocale;		/* first locale in list ("C") */
extern struct __locale *__llocale;		/* last locale in list */

char *setlocale(int, const char *);
struct lconv *localeconv(void);
struct __locale *readlocale(const char *);


/*
 *	  Error messages when reading a locale, stored in lclerror
 */

#define LCLNAME 	1	/* pathname for locale file too long */
#define LCLOPEN		2	/* Unable to open locale file */
#define LCLCOLL		3	/* multiple collate spec */
#define LCLCTYPE	4	/* multiple c types spec */
#define LCLNUMERIC	5	/* multiple numeric spec */
#define LCLTIME		6	/* multiple time spec */
#define LCLMONETARY	7	/* multiple monetary spec */
#define LCLREAD		8	/* Error reading locale file */
#define LCLEND		9	/* Missing END record */

extern int __lclerror;

#ifndef NULL
#define NULL	__NULL
#endif
#endif
