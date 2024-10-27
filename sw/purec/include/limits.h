/* Copyright (C) 1991, 1992, 1996, 1997, 1998, 1999, 2000, 2005
   Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/*
 *	ISO C99 Standard: 7.10/5.2.4.2.1 Sizes of integer types	<limits.h>
 */

/* Modified for MiNTLib by Guido Flohr <guido@freemint.de>.  */

#ifndef _LIBC_LIMITS_H_
#define _LIBC_LIMITS_H_	1

#ifndef _FEATURES_H
#include <features.h>
#endif


/* Maximum length of any multibyte character in any locale.
   We define this value here since the gcc header does not define
   the correct value.  */
#define MB_LEN_MAX	16


/* If we are not using GNU CC we have to define all the symbols ourself.
   Otherwise use gcc's definitions (see below).  */
#if !defined __GNUC__ || __GNUC__ < 2

/* We only protect from multiple inclusion here, because all the other
   #include's protect themselves, and in GCC 2 we may #include_next through
   multiple copies of this file before we get to GCC's.  */
# ifndef _LIMITS_H
#  define _LIMITS_H	1

#ifndef __WORDSIZE
#include <bits/wordsize.h>
#endif

/* We don't have #include_next.
   Define ANSI <limits.h> for standard 32-bit words.  */

/* These assume 8-bit `char's, 16-bit `short int's,
   and 32-bit `int's and `long int's.  */

/* Number of bits in a `char'.	*/
#  ifndef __CHAR_BIT__
#   define __CHAR_BIT__ 8
#  endif
#  define CHAR_BIT	__CHAR_BIT__

/* Minimum and maximum values a `signed char' can hold.  */
#  ifndef __SCHAR_MAX__
#    define __SCHAR_MAX__ 127
#  endif
#  define SCHAR_MIN	(-128)
#  define SCHAR_MAX	__SCHAR_MAX__

/* Maximum value an `unsigned char' can hold.  (Minimum is 0.)  */
#  define UCHAR_MAX	255U

/* Minimum and maximum values a `char' can hold.  */
#  ifndef __CHAR_UNSIGNED__
#   if !('\x80' < 0)
#    define __CHAR_UNSIGNED__
#   endif
#  endif
#  ifdef __CHAR_UNSIGNED__
#   define CHAR_MIN	0
#   define CHAR_MAX	UCHAR_MAX
#  else
#   define CHAR_MIN	SCHAR_MIN
#   define CHAR_MAX	SCHAR_MAX
#  endif

/* Minimum and maximum values a `signed short int' can hold.  */
#  ifndef __SHRT_MAX__
#    define __SHRT_MAX__ 32767
#  endif
#  define SHRT_MIN	(-32767-1)
#  define SHRT_MAX	__SHRT_MAX__

/* Maximum value an `unsigned short int' can hold.  (Minimum is 0.)  */
#  define USHRT_MAX	65535U

/* Minimum and maximum values a `signed int' can hold.  */
#  ifndef __INT_MAX__
#    ifdef __MSHORT__
#      define __INT_MAX__         32767
#    else
#      define __INT_MAX__	2147483647
#    endif
#  endif
#  define INT_MAX __INT_MAX__
#  define INT_MIN	(-INT_MAX - 1)

/* Maximum value an `unsigned int' can hold.  (Minimum is 0.)  */
#  ifdef __MSHORT__
#    define UINT_MAX	65535U
#  else
#    define UINT_MAX	4294967295U
#  endif

/* Minimum and maximum values a `signed long int' can hold.  */
#  if __WORDSIZE == 64
#   define LONG_MAX	9223372036854775807L
#   define ULONG_MAX	18446744073709551615UL
#  else
#   define LONG_MAX	2147483647L
#   define ULONG_MAX	4294967295UL
#  endif
#  define LONG_MIN	(-LONG_MAX - 1L)

#  if defined(__USE_ISOC99) && !defined(__NO_LONGLONG)

/* Minimum and maximum values a `signed long long int' can hold.  */
#   define LLONG_MAX	__LONG_LONG_MAX__
#   define LLONG_MIN	(-LLONG_MAX - 1LL)

/* Maximum value an `unsigned long long int' can hold.  (Minimum is 0.)  */
#   define ULLONG_MAX	18446744073709551615ULL

#  endif /* ISO C99 */

# endif	/* limits.h  */
#endif	/* GCC 2.  */

#endif	/* !_LIBC_LIMITS_H_ */

 /* Get the compiler's limits.h, which defines almost all the ISO constants.

    We put this #include_next outside the double inclusion check because
    it should be possible to include this file more than once and still get
    the definitions from gcc's header.  */
#if defined __GNUC__ && !defined _GCC_LIMITS_H_
/* `_GCC_LIMITS_H_' is what GCC's file defines.  */
#if (defined __STDC_WANT_IEC_60559_BFP_EXT__ || (defined (__STDC_VERSION__) && __STDC_VERSION__ > 201710L))
/* TS 18661-1 / C2X widths of integer types.  */
# undef CHAR_WIDTH
# define CHAR_WIDTH __SCHAR_WIDTH__
# undef SCHAR_WIDTH
# define SCHAR_WIDTH __SCHAR_WIDTH__
# undef UCHAR_WIDTH
# define UCHAR_WIDTH __SCHAR_WIDTH__
# undef SHRT_WIDTH
# define SHRT_WIDTH __SHRT_WIDTH__
# undef USHRT_WIDTH
# define USHRT_WIDTH __SHRT_WIDTH__
# undef INT_WIDTH
# define INT_WIDTH __INT_WIDTH__
# undef UINT_WIDTH
# define UINT_WIDTH __INT_WIDTH__
# undef LONG_WIDTH
# define LONG_WIDTH __LONG_WIDTH__
# undef ULONG_WIDTH
# define ULONG_WIDTH __LONG_WIDTH__
# undef LLONG_WIDTH
# define LLONG_WIDTH __LONG_LONG_WIDTH__
# undef ULLONG_WIDTH
# define ULLONG_WIDTH __LONG_LONG_WIDTH__
#endif
#endif

/* The <limits.h> files in some gcc versions don't define LLONG_MIN,
   LLONG_MAX, and ULLONG_MAX.  Instead only the values gcc defined for
   ages are available.  */
#if defined __USE_ISOC99 && !defined(__NO_LONGLONG)
# if !defined(LLONG_MAX) && defined(__LONG_LONG_MAX__)
#  define LLONG_MAX	__LONG_LONG_MAX__
# endif
# ifndef LLONG_MIN
#  define LLONG_MIN	(-LLONG_MAX-1)
# endif
# ifndef ULLONG_MAX
#  define ULLONG_MAX	(LLONG_MAX * 2ULL + 1)
# endif
#endif

#ifdef	__USE_POSIX
/* POSIX adds things to <limits.h>.  */
# include <bits/posix1lm.h>
#endif

#ifdef	__USE_POSIX2
# include <bits/posix2lm.h>
#endif

#ifdef	__USE_XOPEN
# include <bits/xopenlim.h>
#endif
