/*      MATH.H

        Mathematical Definition Includes

        Copyright (c) Borland International 1990
        All Rights Reserved.
*/


#ifndef _MATH_H
# error "Never use <purec/mathinl.h> directly; include <math.h> instead."
#endif

/* MC68881 extensions */
/* Nearest integer, absolute value, and remainder functions.  */
#define trunc( x )        __FINTRZ__( x )
#define fabs( x )         __FABS__( x )
#define rint( x )         __FINT__( x )
#define ldexp( x, y )     __FSCALE__( y, x )
#define fmod( x, y )      __FMOD__( y, x )
#define remainder( x, y ) __FREM__( y, x )

/* Exponential and logarithmic functions.  */
#define exp( x )          __FETOX__( x )
#define expm1( x )        __FETOXM1__( x )
#define log( x )          __FLOGN__( x )
#define log2( x )         __FLOG2__( x )
#define log10( x )        __FLOG10__( x )
#define log1p( x )        __FLOGNP1__( x )
#define sqrt( x )         __FSQRT__( x )
#define pow2( x )         __FTWOTOX__( x )
#define pow10( x )        __FTENTOX__( x )
#define exp2( x )         __FTWOTOX__( x )
#define exp10( x )        __FTENTOX__( x )
#define logb( x )         __FGETEXP__( x )
#define significand( x )  __FGETMAN__( x )

/* Trigonometric functions.  */
#define acos( x )         __FACOS__( x )
#define asin( x )         __FASIN__( x )
#define atan( x )         __FATAN__( x )
#define cos( x )          __FCOS__( x )
#define sin( x )          __FSIN__( x )
#define tan( x )          __FTAN__( x )

/* Hyperbolic functions.  */
#define atanh( x )        __FATANH__( x )
#define cosh( x )         __FCOSH__( x )
#define sinh( x )         __FSINH__( x )
#define tanh( x )         __FTANH__( x )

/* PureC compatible functions */
#define fgetexp( x )      __FGETEXP__( x )
#define fgetman( x )      __FGETMAN__( x )
#define fint( x )         __FINT__( x )
#define fintrz( x )       __FINTRZ__( x )
#define frem( x, y )      __FREM__( y, x )
#define fsgldiv( x, y )   __FSGLDIV__( y, x )
#define fsglmul( x, y )   __FSGLMUL__( y, x )
#define fetoxm1( x )      __FETOXM1__( x )
#define flognp1( x )      __FLOGNP1__( x )

/* Conversion functions only for PC881LIB.LIB */
long double x80x96cnv( const void *rep10bytes );
void   x96x80cnv( long double rep12bytes, void *rep10bytes );

/*
 * Other (undocumented) inline functions:
 * __FSETCONTROL__
 * __FGETCONTROL__
 * __FMOVECR__
 */


/* ISO C99 defines some macros to perform unordered comparisons.  The
   m68k FPU supports this with special opcodes and we should use them.
   These must not be inline functions since we have to be able to handle
   all floating-point types.  */
#  undef isgreater
#  undef isgreaterequal
#  undef isless
#  undef islessequal
#  undef islessgreater
#  undef isunordered

static double __m81_fcmp1(double x, double y) 0xf200;
static double __m81_fcmp2(double) 0x0438;

#define __m81_fcmp(x, y) __m81_fcmp2(__m81_fcmp1(x, y))

static double __m81_fsogt1(double x) 0xf240;
static signed char __m81_fsogt2(double x) 0x0002;

#define __m81_fsogt(x) __m81_fsogt2(__m81_fsogt1(x))

static double __m81_fsoge1(double x) 0xf240;
static signed char __m81_fsoge2(double x) 0x0003;

#define __m81_fsoge(x) __m81_fsoge2(__m81_fsoge1(x))

static double __m81_fsolt1(double x) 0xf240;
static signed char __m81_fsolt2(double x) 0x0004;

#define __m81_fsolt(x) __m81_fsolt2(__m81_fsolt1(x))

static double __m81_fsole1(double x) 0xf240;
static signed char __m81_fsole2(double x) 0x0005;

#define __m81_fsole(x) __m81_fsole2(__m81_fsole1(x))

static double __m81_fsogl1(double x) 0xf240;
static signed char __m81_fsogl2(double x) 0x0006;

#define __m81_fsogl(x) __m81_fsogl2(__m81_fsogl1(x))

static double __m81_fsun1(double x) 0xf240;
static signed char __m81_fsun2(double x) 0x0008;

#define __m81_fsun(x) __m81_fsun2(__m81_fsun1(x))


#define isunordered(x, y) (-__m81_fsun(__m81_fcmp(x, y)))
#define isgreater(x, y) (-__m81_fsogt(__m81_fcmp(x, y)))
#define isgreaterequal(x, y) (-__m81_fsoge(__m81_fcmp(x, y)))
#define isless(x, y) (-__m81_fsolt(__m81_fcmp(x, y)))
#define islessequal(x, y) (-__m81_fsole(__m81_fcmp(x, y)))
#define islessgreater(x, y) (-__m81_fsogl(__m81_fcmp(x, y)))
