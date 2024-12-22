/**
 * $Id: float.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Floating point limit definitions required by ANSI
 *
 * (c) Copyright 1990, 1993 HiSoft
**/

#ifndef _FLOAT_H
#define _FLOAT_H

/*
 * The following symbols are specified in the ANSI C standard for the
 * floating point number system.
 */

#define FLT_RADIX	2 				/* radix of exponent					*/
#define FLT_ROUNDS	1				/* rounding mode during translation 	*/
									/*	 0 => chop							*/
									/*	 1 => round 						*/
									/*	 2 => indeterminate 				*/

#define __SINGLE_MANT_DIG	24		/* # radix digits in float mantissa 	*/
#define __DOUBLE_MANT_DIG	53 		/* # radix digits in double mantissa	*/
#define __EXTEND_MANT_DIG	53	 	/* # radix digits in long double mant.	*/

#define __SINGLE_DIG		7		/* max decimal digits for float 		*/
#define __DOUBLE_DIG		16 		/* max decimal digits for double		*/
#define __EXTEND_DIG		16 		/* max decimal digits for long double	*/

#define __SINGLE_MIN_EXP	-126	/* min radix exponent for float 		*/
#define __DOUBLE_MIN_EXP	-1022 	/* min radix exponent for double		*/
#define __EXTEND_MIN_EXP	-1022 	/* min radix exponent for long double	*/

#define __SINGLE_MIN_10_EXP	-38		/* min decimal exponent for float		*/
#define __DOUBLE_MIN_10_EXP	-308 	/* min decimal exponent for double		*/
#define __EXTEND_MIN_10_EXP	-308 	/* min decimal exponent for long double	*/

#define __SINGLE_MAX_EXP	128		/* max radix exponent for float 		*/
#define __DOUBLE_MAX_EXP	1024	/* max radix exponent for double		*/
#define __EXTEND_MAX_EXP	1024	/* max radix exponent for double long	*/

#define __SINGLE_MAX_10_EXP	38		/* max decimal exponent for float		*/
#define __DOUBLE_MAX_10_EXP	308		/* max decimal exponent for double		*/
#define __EXTEND_MAX_10_EXP	308		/* max decimal exponent for long double */

#define __SINGLE_MAX		3.40282347E+38
#define __DOUBLE_MAX		1.797693134862317E+308
#define __EXTEND_MAX		1.797693134862317E+308

#define __SINGLE_EPSILON	1.19209290E-07
#define __DOUBLE_EPSILON	2.2204460492503147E-16
#define __EXTEND_EPSILON	2.2204460492503147E-16

#define __SINGLE_MIN		3.40282347E-38
#define __DOUBLE_MIN		1.797693134862316E-308
#define __EXTEND_MIN		1.797693134862316E-308

#if defined(_MSINGLE)
#define FLT_MANT_DIG	__SINGLE_MANT_DIG
#define DBL_MANT_DIG	__SINGLE_MANT_DIG
#define LDBL_MANT_DIG	__SINGLE_MANT_DIG

#define FLT_DIG			__SINGLE_DIG
#define DBL_DIG			__SINGLE_DIG
#define LDBL_DIG		__SINGLE_DIG

#define FLT_MIN_EXP		__SINGLE_MIN_EXP
#define DBL_MIN_EXP		__SINGLE_MIN_EXP
#define LDBL_MIN_EXP	__SINGLE_MIN_EXP

#define FLT_MIN_10_EXP	__SINGLE_MIN_10_EXP
#define DBL_MIN_10_EXP	__SINGLE_MIN_10_EXP
#define LDBL_MIN_10_EXP	__SINGLE_MIN_10_EXP

#define FLT_MAX_EXP		__SINGLE_MAX_EXP
#define DBL_MAX_EXP		__SINGLE_MAX_EXP
#define LDBL_MAX_EXP	__SINGLE_MAX_EXP

#define FLT_MAX_10_EXP	__SINGLE_MAX_10_EXP
#define DBL_MAX_10_EXP	__SINGLE_MAX_10_EXP
#define LDBL_MAX_10_EXP	__SINGLE_MAX_10_EXP

#define FLT_MAX			__SINGLE_MAX
#define DBL_MAX			__SINGLE_MAX
#define LDBL_MAX		__SINGLE_MAX

#define FLT_EPSILON		__SINGLE_EPSILON
#define DBL_EPSILON		__SINGLE_EPSILON
#define LDBL_EPSILON	__SINGLE_EPSILON

#define FLT_MIN			__SINGLE_MIN
#define DBL_MIN			__SINGLE_MIN
#define LDBL_MIN		__SINGLE_MIN
#elif defined(_MDOUBL)
#define FLT_MANT_DIG	__DOUBLE_MANT_DIG
#define DBL_MANT_DIG	__DOUBLE_MANT_DIG
#define LDBL_MANT_DIG	__DOUBLE_MANT_DIG

#define FLT_DIG			__DOUBLE_DIG
#define DBL_DIG			__DOUBLE_DIG
#define LDBL_DIG		__DOUBLE_DIG

#define FLT_MIN_EXP		__DOUBLE_MIN_EXP
#define DBL_MIN_EXP		__DOUBLE_MIN_EXP
#define LDBL_MIN_EXP	__DOUBLE_MIN_EXP

#define FLT_MIN_10_EXP	__DOUBLE_MIN_10_EXP
#define DBL_MIN_10_EXP	__DOUBLE_MIN_10_EXP
#define LDBL_MIN_10_EXP	__DOUBLE_MIN_10_EXP

#define FLT_MAX_EXP		__DOUBLE_MAX_EXP
#define DBL_MAX_EXP		__DOUBLE_MAX_EXP
#define LDBL_MAX_EXP	__DOUBLE_MAX_EXP

#define FLT_MAX_10_EXP	__DOUBLE_MAX_10_EXP
#define DBL_MAX_10_EXP	__DOUBLE_MAX_10_EXP
#define LDBL_MAX_10_EXP	__DOUBLE_MAX_10_EXP

#define FLT_MAX			__DOUBLE_MAX
#define DBL_MAX			__DOUBLE_MAX
#define LDBL_MAX		__DOUBLE_MAX

#define FLT_EPSILON		__DOUBLE_EPSILON
#define DBL_EPSILON		__DOUBLE_EPSILON
#define LDBL_EPSILON	__DOUBLE_EPSILON

#define FLT_MIN			__DOUBLE_MIN
#define DBL_MIN			__DOUBLE_MIN
#define LDBL_MIN		__DOUBLE_MIN
#else
#define FLT_MANT_DIG	__SINGLE_MANT_DIG
#define DBL_MANT_DIG	__DOUBLE_MANT_DIG
#define LDBL_MANT_DIG	__EXTEND_MANT_DIG

#define FLT_DIG			__SINGLE_DIG
#define DBL_DIG			__DOUBLE_DIG
#define LDBL_DIG		__EXTEND_DIG

#define FLT_MIN_EXP		__SINGLE_MIN_EXP
#define DBL_MIN_EXP		__DOUBLE_MIN_EXP
#define LDBL_MIN_EXP	__EXTEND_MIN_EXP

#define FLT_MIN_10_EXP	__SINGLE_MIN_10_EXP
#define DBL_MIN_10_EXP	__DOUBLE_MIN_10_EXP
#define LDBL_MIN_10_EXP	__EXTEND_MIN_10_EXP

#define FLT_MAX_EXP		__SINGLE_MAX_EXP
#define DBL_MAX_EXP		__DOUBLE_MAX_EXP
#define LDBL_MAX_EXP	__EXTEND_MAX_EXP

#define FLT_MAX_10_EXP	__SINGLE_MAX_10_EXP
#define DBL_MAX_10_EXP	__DOUBLE_MAX_10_EXP
#define LDBL_MAX_10_EXP	__EXTEND_MAX_10_EXP

#define FLT_MAX			__SINGLE_MAX
#define DBL_MAX			__DOUBLE_MAX
#define LDBL_MAX		__EXTEND_MAX

#define FLT_EPSILON		__SINGLE_EPSILON
#define DBL_EPSILON		__DOUBLE_EPSILON
#define LDBL_EPSILON	__EXTEND_EPSILON

#define FLT_MIN			__SINGLE_MIN
#define DBL_MIN			__DOUBLE_MIN
#define LDBL_MIN		__EXTEND_MIN
#endif

#ifndef _ANSI
#define FLT_GUARD	0 				/* guard digits during multiplication	*/
									/*	 0 => No							*/
									/*	 1 => Yes							*/
#define FLT_NORMALIZE	1 			/* normalization required				*/
									/*	 0 => No							*/
									/*	 1 => Yes							*/
#ifndef HUGE_VAL
#define HUGE_VAL	1.797693134862317E+308	/* huge double value	*/
#endif
#endif
#endif
