/* Copyright (C) 1992-2013 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _IEEE754_H

#define _IEEE754_H 1
#include <features.h>

#include <endian.h>
#include <bits/types.h>

__BEGIN_DECLS

union ieee754_float
  {
    float f;

    /* This is the IEEE 754 single-precision format.  */
    struct
      {
#if	__BYTE_ORDER == __ORDER_BIG_ENDIAN__
	unsigned int negative:1;
	unsigned int exponent:8;
	__uint32_t   mantissa:23;
#endif				/* Big endian.  */
#if	__BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
	__uint32_t   mantissa:23;
	unsigned int exponent:8;
	unsigned int negative:1;
#endif				/* Little endian.  */
      } ieee;

    /* This format makes it easier to see if a NaN is a signalling NaN.  */
    struct
      {
#if	__BYTE_ORDER == __ORDER_BIG_ENDIAN__
	unsigned int negative:1;
	unsigned int exponent:8;
	unsigned int quiet_nan:1;
	__uint32_t   mantissa:22;
#endif				/* Big endian.  */
#if	__BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
	__uint32_t   mantissa:22;
	unsigned int quiet_nan:1;
	unsigned int exponent:8;
	unsigned int negative:1;
#endif				/* Little endian.  */
      } ieee_nan;
  };

#define IEEE754_FLOAT_BIAS	0x7f /* Added to exponent.  */


union ieee754_double
  {
    double d;

    /* This is the IEEE 754 double-precision format.  */
    struct
      {
#if	__BYTE_ORDER == __ORDER_BIG_ENDIAN__
	unsigned int negative:1;
	unsigned int exponent:11;
	/* Together these comprise the mantissa.  */
	__uint32_t    mantissa0:20;
	__uint32_t    mantissa1:32;
#endif				/* Big endian.  */
#if	__BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
# if	__FLOAT_WORD_ORDER == __ORDER_BIG_ENDIAN__
	__uint32_t   mantissa0:20;
	unsigned int exponent:11;
	unsigned int negative:1;
	__uint32_t   mantissa1:32;
# else
	/* Together these comprise the mantissa.  */
	__uint32_t   mantissa1:32;
	__uint32_t   mantissa0:20;
	unsigned int exponent:11;
	unsigned int negative:1;
# endif
#endif				/* Little endian.  */
      } ieee;

    /* This format makes it easier to see if a NaN is a signalling NaN.  */
    struct
      {
#if	__BYTE_ORDER == __ORDER_BIG_ENDIAN__
	unsigned int negative:1;
	unsigned int exponent:11;
	unsigned int quiet_nan:1;
	/* Together these comprise the mantissa.  */
	__uint32_t   mantissa0:19;
	__uint32_t   mantissa1:32;
#else
# if	__FLOAT_WORD_ORDER == __ORDER_BIG_ENDIAN__
	__uint32_t   mantissa0:19;
	unsigned int quiet_nan:1;
	unsigned int exponent:11;
	unsigned int negative:1;
	__uint32_t   mantissa1:32;
# else
	/* Together these comprise the mantissa.  */
	__uint32_t   mantissa1:32;
	__uint32_t   mantissa0:19;
	unsigned int quiet_nan:1;
	unsigned int exponent:11;
	unsigned int negative:1;
# endif
#endif
      } ieee_nan;
  };

#define IEEE754_DOUBLE_BIAS	0x3ff /* Added to exponent.  */


union ieee854_long_double
  {
    long double d;

    /* This is the IEEE 854 double-extended-precision format.  */
    struct
      {
#if	__BYTE_ORDER == __ORDER_BIG_ENDIAN__
	unsigned int negative:1;
	unsigned int exponent:15;
#if !(defined(__PUREC__) || defined(__AHCC__) || defined(__TURBOC__)) || defined(__MATH_68881__)
	unsigned int empty:16;
#endif
	__uint32_t   mantissa0:32;
	__uint32_t   mantissa1:32;
#endif
#if	__BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
# if	__FLOAT_WORD_ORDER == __ORDER_BIG_ENDIAN__
	unsigned int exponent:15;
	unsigned int negative:1;
	unsigned int empty:16;
	__uint32_t   mantissa0:32;
	__uint32_t   mantissa1:32;
# else
	__uint32_t mantissa1:32;
	__uint32_t mantissa0:32;
	unsigned int exponent:15;
	unsigned int negative:1;
	unsigned int empty:16;
# endif
#endif
      } ieee;

    /* This is for NaNs in the IEEE 854 double-extended-precision format.  */
    struct
      {
#if	__BYTE_ORDER == __ORDER_BIG_ENDIAN__
	unsigned int negative:1;
	unsigned int exponent:15;
#if !(defined(__PUREC__) || defined(__AHCC__) || defined(__TURBOC__)) || defined(__MATH_68881__)
	unsigned int empty:16;
#endif
	unsigned int one:1;
	unsigned int quiet_nan:1;
	__uint32_t   mantissa0:30;
	__uint32_t   mantissa1:32;
#endif
#if	__BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
# if	__FLOAT_WORD_ORDER == __ORDER_BIG_ENDIAN__
	unsigned int exponent:15;
	unsigned int negative:1;
	unsigned int empty:16;
	__uint32_t   mantissa0:30;
	unsigned int quiet_nan:1;
	unsigned int one:1;
	__uint32_t   mantissa1:32;
# else
	__uint32_t   mantissa1:32;
	__uint32_t   mantissa0:30;
	unsigned int quiet_nan:1;
	unsigned int one:1;
	unsigned int exponent:15;
	unsigned int negative:1;
	unsigned int empty:16;
# endif
#endif
      } ieee_nan;
  };

#define IEEE854_LONG_DOUBLE_BIAS 0x3fff

__END_DECLS

#endif /* ieee754.h */
