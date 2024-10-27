/* Copyright (C) 1997, 1998, 1999 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Modified for MiNTLib by Guido Flohr <guido@freemint.de>.  */
/* Modified for MiNTLib by Markus F.X.J. Oberhumer <markus@oberhumer.com>.  */

/*
 *      ISO C 9X: 7.18 Integer types <stdint.h>
 */

#ifndef _STDINT_H
#define _STDINT_H       1

#ifndef _FEATURES_H
# include <features.h>
#endif

#define __need_wchar_t
#include <stddef.h>

/* Exact integral types.  */

/* Signed.  */

/* There is some amount of overlap with <sys/types.h> as known by inet code */
#ifndef __int8_t_defined
# define __int8_t_defined
typedef signed char             int8_t;
#ifdef __MSHORT__
typedef short int               int16_t;
typedef long int                int32_t;
#else
typedef short int               int16_t;
typedef int                     int32_t;
#endif
#ifndef __NO_LONGLONG
__extension__
typedef long long int           int64_t;
#endif
#endif

/* Unsigned.  */
typedef unsigned char           uint8_t;
#ifdef __MSHORT__
typedef unsigned short int      uint16_t;
typedef unsigned long int       uint32_t;
#else
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
#endif
#ifndef __NO_LONGLONG
__extension__
typedef unsigned long long int  uint64_t;
#endif


/* Small types.  */

/* Signed.  */
typedef signed char             int_least8_t;
#ifdef __MSHORT__
typedef int                     int_least16_t;
typedef long int                int_least32_t;
#else
typedef short int               int_least16_t;
typedef int                     int_least32_t;
#endif
#ifndef __NO_LONGLONG
__extension__
typedef long long int           int_least64_t;
#endif

/* Unsigned.  */
typedef unsigned char           uint_least8_t;
#ifdef __MSHORT__
typedef unsigned int            uint_least16_t;
typedef unsigned long int       uint_least32_t;
#else
typedef unsigned short int      uint_least16_t;
typedef unsigned int            uint_least32_t;
#endif
#ifndef __NO_LONGLONG
__extension__
typedef unsigned long long int  uint_least64_t;
#endif


/* Fast types.  */

/* Signed.  */
typedef signed char             int_fast8_t;
typedef int                     int_fast16_t;
#ifdef __MSHORT__
typedef long int                int_fast32_t;
#else
typedef int                     int_fast32_t;
#endif
#ifndef __NO_LONGLONG
__extension__
typedef long long int           int_fast64_t;
#endif

/* Unsigned.  */
typedef unsigned char           uint_fast8_t;
typedef unsigned int            uint_fast16_t;
#ifdef __MSHORT__
typedef unsigned long int       uint_fast32_t;
#else
typedef unsigned int            uint_fast32_t;
#endif
#ifndef __NO_LONGLONG
__extension__
typedef unsigned long long int  uint_fast64_t;
#endif


/* Types for `void *' pointers.  */
#ifndef __intptr_t_defined
# ifdef __MSHORT__
typedef long int                intptr_t;
# else
typedef int                     intptr_t;
# endif
# define __intptr_t_defined
#endif
#ifdef __MSHORT__
typedef unsigned long int       uintptr_t;
#else
typedef unsigned int            uintptr_t;
#endif


/* Largest integral types.  */
#ifndef __NO_LONGLONG
__extension__
typedef long long int           intmax_t;
__extension__
typedef unsigned long long int  uintmax_t;
#else
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
#endif


/* The ISO C 9X standard specifies that in C++ implementations these
   macros should only be defined if explicitly requested.  */
#if !defined __cplusplus || defined __STDC_LIMIT_MACROS

/* Limits of integral types.  */

/* Minimum of signed integral types.  */
# define INT8_MIN               (-128)
# define INT16_MIN              (-32767-1)
# ifdef __MSHORT__
# define INT32_MIN              (-2147483647L-1)
# else
# define INT32_MIN              (-2147483647-1)
# endif
#ifndef __NO_LONGLONG
# define INT64_MIN              (-9223372036854775807LL-1)
#endif
/* Maximum of signed integral types.  */
# define INT8_MAX               (127)
# define INT16_MAX              (32767)
# ifdef __MSHORT__
# define INT32_MAX              (2147483647L)
# else
# define INT32_MAX              (2147483647)
# endif
#ifndef __NO_LONGLONG
# define INT64_MAX              (9223372036854775807LL)
#endif

/* Maximum of unsigned integral types.  */
# define UINT8_MAX              (255U)
# define UINT16_MAX             (65535U)
# ifdef __MSHORT__
# define UINT32_MAX             (4294967295UL)
# else
# define UINT32_MAX             (4294967295U)
# endif
#ifndef __NO_LONGLONG
# define UINT64_MAX             (18446744073709551615ULL)
#endif


/* Minimum of signed integral types having a minimum size.  */
# define INT_LEAST8_MIN         (-128)
# define INT_LEAST16_MIN        (-32767-1)
# ifdef __MSHORT__
# define INT_LEAST32_MIN        (-2147483647L-1)
# else
# define INT_LEAST32_MIN        (-2147483647-1)
# endif
#ifndef __NO_LONGLONG
# define INT_LEAST64_MIN        (-9223372036854775807LL-1)
#endif
/* Maximum of signed integral types having a minimum size.  */
# define INT_LEAST8_MAX         (127)
# define INT_LEAST16_MAX        (32767)
# ifdef __MSHORT__
# define INT_LEAST32_MAX        (2147483647L)
# else
# define INT_LEAST32_MAX        (2147483647)
# endif
#ifndef __NO_LONGLONG
# define INT_LEAST64_MAX        (9223372036854775807LL)
#endif

/* Maximum of unsigned integral types having a minimum size.  */
# define UINT_LEAST8_MAX        (255U)
# define UINT_LEAST16_MAX       (65535U)
# ifdef __MSHORT__
# define UINT_LEAST32_MAX       (4294967295UL)
# else
# define UINT_LEAST32_MAX       (4294967295U)
# endif
#ifndef __NO_LONGLONG
# define UINT_LEAST64_MAX       (18446744073709551615ULL)
#endif


/* Minimum of fast signed integral types having a minimum size.  */
# define INT_FAST8_MIN          (-128)
# ifdef __MSHORT__
# define INT_FAST16_MIN         (-32767-1)
# define INT_FAST32_MIN         (-2147483647L-1)
# else
# define INT_FAST16_MIN         (-2147483647-1)
# define INT_FAST32_MIN         (-2147483647-1)
# endif
#ifndef __NO_LONGLONG
# define INT_FAST64_MIN         (-9223372036854775807LL-1)
#endif
/* Maximum of fast signed integral types having a minimum size.  */
# define INT_FAST8_MAX          (127)
# ifdef __MSHORT__
# define INT_FAST16_MAX         (32767)
# define INT_FAST32_MAX         (2147483647L)
# else
# define INT_FAST16_MAX         (2147483647)
# define INT_FAST32_MAX         (2147483647)
# endif
#ifndef __NO_LONGLONG
# define INT_FAST64_MAX         (9223372036854775807LL)
#endif

/* Maximum of fast unsigned integral types having a minimum size.  */
# define UINT_FAST8_MAX         (255U)
# ifdef __MSHORT__
# define UINT_FAST16_MAX        (65535U)
# define UINT_FAST32_MAX        (4294967295UL)
# else
# define UINT_FAST16_MAX        (4294967295U)
# define UINT_FAST32_MAX        (4294967295U)
# endif
#ifndef __NO_LONGLONG
# define UINT_FAST64_MAX        (18446744073709551615ULL)
#endif


/* Values to test for integral types holding `void *' pointer.  */
# ifdef __MSHORT__
# define INTPTR_MIN             (-2147483647L-1)
# define INTPTR_MAX             (2147483647L)
# define UINTPTR_MAX            (4294967295UL)
# else
# define INTPTR_MIN             (-2147483647-1)
# define INTPTR_MAX             (2147483647)
# define UINTPTR_MAX            (4294967295U)
# endif


/* Minimum for largest signed integral type.  */
#ifndef __NO_LONGLONG
# define INTMAX_MIN             (-9223372036854775807LL-1)
/* Maximum for largest signed integral type.  */
# define INTMAX_MAX             (9223372036854775807LL)
/* Maximum for largest unsigned integral type.  */
# define UINTMAX_MAX            (18446744073709551615ULL)
#else
# define INTMAX_MIN INT32_MIN
# define INTMAX_MAX INT32_MAX
# define UINTMAX_MAX UINT32_MAX
#endif


/* Limits of other integer types.  */

/* Limits of `ptrdiff_t' type.  */
# ifdef __MSHORT__
# define PTRDIFF_MIN            (-2147483647L-1)
# define PTRDIFF_MAX            (2147483647L)
# else
# define PTRDIFF_MIN            (-2147483647-1)
# define PTRDIFF_MAX            (2147483647)
# endif

/* Limits of `sig_atomic_t'.  */
# ifdef __MSHORT__
# define SIG_ATOMIC_MIN         (-2147483647L-1)
# define SIG_ATOMIC_MAX         (2147483647L)
# else
# define SIG_ATOMIC_MIN         (-2147483647-1)
# define SIG_ATOMIC_MAX         (2147483647)
# endif

/* Limit of `size_t' type.  */
# ifdef __MSHORT__
# define SIZE_MAX               (4294967295UL)
# else
# define SIZE_MAX               (4294967295U)
# endif

/* Limits of `wchar_t'.  */
# ifndef WCHAR_MIN
/* These constants might also be defined in <wchar.h>.  */
#  define WCHAR_MIN             (__WCHAR_MIN__)
#  define WCHAR_MAX             (__WCHAR_MAX__)
# endif

/* Limits of `wint_t'.  */
# define WINT_MIN               (__WINT_MIN__)
# define WINT_MAX               (__WINT_MAX__)

#endif  /* C++ && limit macros */


/* The ISO C 9X standard specifies that in C++ implementations these
   should only be defined if explicitly requested.  */
#if !defined __cplusplus || defined __STDC_CONSTANT_MACROS

/* Signed.  */
# define INT8_C(c)      c
# define INT16_C(c)     c
# ifdef __MSHORT__
# define INT32_C(c)     c ## L
# else
# define INT32_C(c)     c
# endif
#ifndef __NO_LONGLONG
# define INT64_C(c)     c ## LL
#endif

/* Unsigned.  */
# define UINT8_C(c)     c ## U
# define UINT16_C(c)    c ## U
# ifdef __MSHORT__
# define UINT32_C(c)    c ## UL
# else
# define UINT32_C(c)    c ## U
# endif
#ifndef __NO_LONGLONG
# define UINT64_C(c)    c ## ULL
#endif

/* Maximal type.  */
#ifndef __NO_LONGLONG
# define INTMAX_C(c)    c ## LL
# define UINTMAX_C(c)   c ## ULL
#else
# define INTMAX_C(c) c ## L
# define UINTMAC_C(c) c ## UL
#endif

#endif  /* C++ && constant macros */

#endif /* stdint.h */
