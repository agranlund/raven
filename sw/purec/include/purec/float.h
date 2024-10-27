/*      FLOAT.H

        Characteristics of floating point types

        Copyright (c) Borland International 1990
        All Rights Reserved.
*/


#if !defined( __PUREC_FLOAT_H__ )
#define __PUREC_FLOAT_H__

#ifndef	_FEATURES_H
# include <features.h>
#endif

/* Addition rounds to 0: zero, 1: nearest, 2: +inf, 3: -inf, -1: unknown.  */
/* ??? This is supposed to change with calls to fesetround in <fenv.h>.  */
#define  __FLT_ROUNDS__                  1

/* Radix of exponent representation, b. */
#define  __FLT_RADIX__                   2

/* Number of decimal digits, n, such that any floating-point number in the
   widest supported floating type with pmax radix b digits can be rounded
   to a floating-point number with n decimal digits and back again without
   change to the value,

	pmax * log10(b)			if b is a power of 10
	ceil(1 + pmax * log10(b))	otherwise
*/
#define __FLT_DECIMAL_DIG__ 9
#define __DBL_DECIMAL_DIG__ 17
#define __LDBL_DECIMAL_DIG__ 21
#define __DECIMAL_DIG__ __LDBL_DECIMAL_DIG__

/* The floating-point expression evaluation method.
        -1  indeterminate
         0  evaluate all operations and constants just to the range and
            precision of the type
         1  evaluate operations and constants of type float and double
            to the range and precision of the double type, evaluate
            long double operations and constants to the range and
            precision of the long double type
         2  evaluate all operations and constants to the range and
            precision of the long double type

   ??? This ought to change with the setting of the fp control word;
   the value provided by the compiler assumes the widest setting.  */
#ifdef __68881__
#define __FLT_EVAL_METHOD__	             2
#else
#define __FLT_EVAL_METHOD__	             0
#endif

#define  __FLT_MANT_DIG__               24
#define  __FLT_DIG__                     6	/* digits of precision of a "float" */
#define  __FLT_MIN_EXP__             (-125)
#define  __FLT_MIN_10_EXP__           (-37)
#define  __FLT_MAX_EXP__               128
#define  __FLT_MAX_10_EXP__             38
#define  __FLT_EPSILON__      1.19209289550781250000e-7F
#define  __FLT_MIN__          1.17549435082228750797e-38F	/* min decimal value of a "float" */
#define  __FLT_MAX__          3.40282346638528859812e+38F	/* max decimal value of a "float" */
#define  __FLT_DENORM_MIN__ 1.40129846432481707092e-45F
#define  __FLT_HAS_INFINITY__ 1
#define  __FLT_HAS_QUIET_NAN__ 1

#ifdef __mcoldfire__
#define  __DBL_MANT_DIG__                     53
#define  __DBL_DIG__                          15	/* digits of precision of a "double" */
#define  __DBL_MIN_EXP__                  (-1021)
#define  __DBL_MIN_10_EXP__                (-307)
#define  __DBL_MAX_EXP__                    1024
#define  __DBL_MAX_10_EXP__                  308
#define  __DBL_EPSILON__  ((double)2.2204460492503131e-16L)
#define  __DBL_MIN__      ((double)2.2250738585072014e-308L)	/* min decimal value of a "double" */
#define  __DBL_MAX__      ((double)1.7976931348623157e+308L)	/* max decimal value of a "double" */
#define  __DBL_DENORM_MIN__ ((double)4.9406564584124654e-324L)	/* Minimum positive values, including subnormals. */
#else
#define  __DBL_MANT_DIG__                     64
#define  __DBL_DIG__                          18	/* digits of precision of a "double" */
#define  __DBL_MIN_EXP__                 (-16383)
#define  __DBL_MIN_10_EXP__               (-4932)
#define  __DBL_MAX_EXP__                   16384
#define  __DBL_MAX_10_EXP__                 4932
#define  __DBL_EPSILON__  5.421010862427522170E-0020
#define  __DBL_MIN__      1.681051571556046753E-4932	/* min decimal value of a "double" */
#define  __DBL_MAX__      1.189731495357231765E+4932	/* max decimal value of a "double" */
#define  __DBL_DENORM_MIN__ ((double)1.82259976594123730126e-4951L)	/* Minimum positive values, including subnormals. */
#endif
#define  __DBL_HAS_INFINITY__ 1
#define  __DBL_HAS_QUIET_NAN__ 1

#define  __LDBL_MANT_DIG__                    64
#define  __LDBL_DIG__                         18
#define  __LDBL_MIN_EXP__                (-16382)
#define  __LDBL_MIN_10_EXP__              (-4931)
#define  __LDBL_MAX_EXP__                  16384
#define  __LDBL_MAX_10_EXP__                4932
#define  __LDBL_EPSILON__ 5.421010862427522170E-0020L
#define  __LDBL_MIN__     1.681051571556046753E-4932L
#define  __LDBL_MAX__     1.1897314953572317649999999999E+4932L
#define  __LDBL_DENORM_MIN__ 1.82259976594123730126e-4951L
#define  __LDBL_HAS_INFINITY__ 1
#define  __LDBL_HAS_QUIET_NAN__ 1

/* Whether types support subnormal numbers.  */
#define __FLT_HAS_SUBNORM__		1
#define __DBL_HAS_SUBNORM__		1
#define __LDBL_HAS_SUBNORM__	1

#define __FLT_HAS_DENORM__		1
#define __DBL_HAS_DENORM__		1
#define __LDBL_HAS_DENORM__		1


#define FLT_ROUNDS      __FLT_ROUNDS__

#define FLT_RADIX       __FLT_RADIX__

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define FLT_EVAL_METHOD	__FLT_EVAL_METHOD__
#define DECIMAL_DIG __DECIMAL_DIG__
#endif

#define FLT_MANT_DIG    __FLT_MANT_DIG__
#define FLT_DIG         __FLT_DIG__
#define FLT_MIN_EXP     __FLT_MIN_EXP__
#define FLT_MIN_10_EXP  __FLT_MIN_10_EXP__
#define FLT_MAX_EXP     __FLT_MAX_EXP__
#define FLT_MAX_10_EXP  __FLT_MAX_10_EXP__
#define FLT_EPSILON     __FLT_EPSILON__
#define FLT_MIN         __FLT_MIN__
#define FLT_MAX         __FLT_MAX__
 
#define DBL_MANT_DIG    __DBL_MANT_DIG__
#define DBL_DIG         __DBL_DIG__
#define DBL_MIN_EXP     __DBL_MIN_EXP__
#define DBL_MIN_10_EXP  __DBL_MIN_10_EXP__
#define DBL_MAX_EXP     __DBL_MAX_EXP__
#define DBL_MAX_10_EXP  __DBL_MAX_10_EXP__
#define DBL_EPSILON     __DBL_EPSILON__
#define DBL_MIN         __DBL_MIN__
#define DBL_MAX         __DBL_MAX__

#define LDBL_MANT_DIG   __LDBL_MANT_DIG__
#define LDBL_DIG        __LDBL_DIG__
#define LDBL_MIN_EXP    __LDBL_MIN_EXP__
#define LDBL_MIN_10_EXP __LDBL_MIN_10_EXP__
#define LDBL_MAX_EXP    __LDBL_MAX_EXP__
#define LDBL_MAX_10_EXP __LDBL_MAX_10_EXP__
#define LDBL_EPSILON    __LDBL_EPSILON__
#define LDBL_MIN        __LDBL_MIN__
#define LDBL_MAX        __LDBL_MAX__

#undef FLT_DECIMAL_DIG
#undef DBL_DECIMAL_DIG
#undef LDBL_DECIMAL_DIG
#define FLT_DECIMAL_DIG		__FLT_DECIMAL_DIG__
#define DBL_DECIMAL_DIG		__DBL_DECIMAL_DIG__
#define LDBL_DECIMAL_DIG	__LDBL_DECIMAL_DIG__

#undef FLT_HAS_SUBNORM
#undef DBL_HAS_SUBNORM
#undef LDBL_HAS_SUBNORM
#define FLT_HAS_SUBNORM		__FLT_HAS_DENORM__
#define DBL_HAS_SUBNORM		__DBL_HAS_DENORM__
#define LDBL_HAS_SUBNORM	__LDBL_HAS_DENORM__


/* Minimum positive values, including subnormals.  */
#undef FLT_TRUE_MIN
#undef DBL_TRUE_MIN
#undef LDBL_TRUE_MIN
#if __FLT_HAS_DENORM__
#define FLT_TRUE_MIN	__FLT_DENORM_MIN__
#else
#define FLT_TRUE_MIN	__FLT_MIN__
#endif
#if __DBL_HAS_DENORM__
#define DBL_TRUE_MIN	__DBL_DENORM_MIN__
#else
#define DBL_TRUE_MIN	__DBL_MIN__
#endif
#if __LDBL_HAS_DENORM__
#define LDBL_TRUE_MIN	__LDBL_DENORM_MIN__
#else
#define LDBL_TRUE_MIN	__LDBL_MIN__
#endif

#ifdef __STDC_WANT_DEC_FP__
/* Draft Technical Report 24732, extension for decimal floating-point
   arithmetic: Characteristic of decimal floating types <float.h>.  */

/* currently not supported */

#endif

#endif /* __PUREC_FLOAT_H__ */
/************************************************************************/
