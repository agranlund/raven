/* Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
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

#if !defined _MATH_H && !defined _COMPLEX_H
# error "Never use <bits/mathdef.h> directly; include <math.h> instead"
#endif

/* The m68k FPUs evaluate all values in the 96 bit floating-point format
   which is also available for the user as `long double'.  Therefore we
   define: */
#ifndef __FLT_EVAL_METHOD__
#  if defined(__HAVE_68881__)
#    define __FLT_EVAL_METHOD__ 2
#  elif defined(__HAVE_FPU__) /* coldfire, double == long double */
#    define __FLT_EVAL_METHOD__ 1
#  else
#    define __FLT_EVAL_METHOD__ 0
#  endif
#endif

#if defined __USE_ISOC99 && defined _MATH_H && !defined _MATH_H_MATHDEF
# define _MATH_H_MATHDEF	1

#if __FLT_EVAL_METHOD__ == 0

typedef float float_t;
typedef double double_t;

#elif __FLT_EVAL_METHOD__ == 1

typedef double float_t;
typedef double double_t;

#elif __FLT_EVAL_METHOD__ == 2

typedef long double float_t;	/* `float' expressions are evaluated as
				   `long double'.  */
typedef long double double_t;	/* `double' expressions are evaluated as
				   `long double'.  */
#endif

/* The values returned by `ilogb' for 0 and NaN respectively.  */
# define FP_ILOGB0	(-__INT_MAX__ - 1)
# define FP_ILOGBNAN	(__INT_MAX__)

#endif	/* ISO C99 */
