__BEGIN_NAMESPACE_STD

/* Arc cosine of X.  */
double acos(double __x);
/* Arc sine of X.  */
double asin(double __x);
/* Arc tangent of X.  */
double atan(double __x);
/* Arc tangent of Y/X.  */
double atan2(double __y, double __x);

/* Cosine of X.  */
double cos(double __x);
/* Sine of X.  */
double sin(double __x);
/* Tangent of X.  */
double tan(double __x);

/* Hyperbolic functions.  */

/* Hyperbolic cosine of X.  */
double cosh(double __x);
/* Hyperbolic sine of X.  */
double sinh(double __x);
/* Hyperbolic tangent of X.  */
double tanh(double __x);
__END_NAMESPACE_STD

#if defined __USE_GNU || defined __USE_PUREC
/* Cosine and sine of X.  */
void sincos(double __x, double *__sinx, double *__cosx);
#endif

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99 || defined __USE_PUREC
__BEGIN_NAMESPACE_C99
/* Hyperbolic arc cosine of X.  */
double acosh(double __x);
/* Hyperbolic arc sine of X.  */
double asinh(double __x);
/* Hyperbolic arc tangent of X.  */
double atanh(double __x);
__END_NAMESPACE_C99
#endif

/* Exponential and logarithmic functions.  */

__BEGIN_NAMESPACE_STD
/* Exponential function of X.  */
double exp(double __x);

/* Break VALUE into a normalized fraction and an integral power of 2.  */
double frexp(double __x, int *__exponent);

/* X times (two to the EXP power).  */
double ldexp(double __x, int __exponent);

/* Natural logarithm of X.  */
double log(double __x);

/* Base-ten logarithm of X.  */
double log10(double __x);

/* Break VALUE into integral and fractional parts.  */
double modf(double __x, double *__iptr) __nonnull ((2));
__END_NAMESPACE_STD

#if defined __USE_GNU || defined __USE_PUREC
/* A function missing in all standards: compute exponent to base ten.  */
double exp10(double __x);
/* Another name occasionally used.  */
double pow10(double __x);
#endif

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99 || defined __USE_PUREC
__BEGIN_NAMESPACE_C99
/* Return exp(X) - 1.  */
double expm1(double __x);

/* Return log(1 + X).  */
double log1p(double __x);

/* Return the base 2 signed integral exponent of X.  */
double logb(double __x);
__END_NAMESPACE_C99
#endif

#if defined __USE_ISOC99 || defined __USE_PUREC
__BEGIN_NAMESPACE_C99
/* Compute base-2 exponential of X.  */
double exp2(double __x);
/* Another name occasionally used.  */
double pow2(double __x);

/* Compute base-2 logarithm of X.  */
double log2(double __x);
__END_NAMESPACE_C99
#endif


/* Power functions.  */

__BEGIN_NAMESPACE_STD
/* Return X to the Y power.  */
double pow(double __x, double __y);

/* Return the square root of X.  */
double sqrt(double __x);
__END_NAMESPACE_STD

#if defined __USE_MISC || defined __USE_XOPEN || defined __USE_ISOC99 || defined __USE_PUREC
__BEGIN_NAMESPACE_C99
/* Return `sqrt(X*X + Y*Y)'.  */
double hypot(double __x, double __y);
__END_NAMESPACE_C99
#endif

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99 || defined __USE_PUREC
__BEGIN_NAMESPACE_C99
/* Return the cube root of X.  */
double cbrt(double __x);
__END_NAMESPACE_C99
#endif


/* Nearest integer, absolute value, and remainder functions.  */

__BEGIN_NAMESPACE_STD
/* Smallest integral value not less than X.  */
double ceil(double __x) __attribute__((__const__));

/* Absolute value of X.  */
double fabs(double __x) __attribute__((__const__));

/* Largest integer not greater than X.  */
double floor(double __x) __attribute__((__const__));

/* Floating-point modulo remainder of X/Y.  */
double fmod(double __x, double __y);


/* Return 0 if VALUE is finite or NaN, +1 if it
   is +Infinity, -1 if it is -Infinity.  */
int __isinf(double __value) __attribute__ ((__const__));

/* Return nonzero if VALUE is finite and not NaN.  */
int __finite(double __value) __attribute__ ((__const__));
__END_NAMESPACE_STD

#if defined __USE_MISC || defined __USE_PUREC
/* Return 0 if VALUE is finite or NaN, +1 if it
   is +Infinity, -1 if it is -Infinity.  */
int isinf(double __value) __attribute__ ((__const__));

/* Return nonzero if VALUE is finite and not NaN.  */
int finite(double __value) __attribute__ ((__const__));

/* Return the remainder of X/Y.  */
double drem(double __x, double __y);


/* Return the fractional part of X after dividing out `ilogb (X)'.  */
double significand(double __x);
#endif /* Use misc.  */

#if defined __USE_MISC || defined __USE_ISOC99 || defined __USE_PUREC
__BEGIN_NAMESPACE_C99
/* Return X with its signed changed to Y's.  */
double copysign(double __x, double __y) __attribute__((__const__));
__END_NAMESPACE_C99
#endif

#if defined __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Return representation of qNaN for double type.  */
double nan(const char *__tagb) __attribute__((__const__));
__END_NAMESPACE_C99
#endif


/* Return nonzero if VALUE is not a number.  */
int __isnan(double __value) __attribute__ ((__const__));

#if defined __USE_MISC || defined __USE_XOPEN || defined __USE_PUREC
/* Return nonzero if VALUE is not a number.  */
int isnan(double __value) __attribute__ ((__const__));

/* Bessel functions.  */
double j0(double);
double j1(double);
double jn(int, double);
double y0(double);
double y1(double);
double yn(int, double);
#endif


#if defined __USE_MISC || defined __USE_XOPEN || defined __USE_ISOC99 || defined __USE_PUREC
__BEGIN_NAMESPACE_C99
/* Error and gamma functions.  */
double erf(double);
double erfc(double);
double lgamma(double);
__END_NAMESPACE_C99
#endif

#if defined __USE_ISOC99 || defined __USE_PUREC
__BEGIN_NAMESPACE_C99
/* True gamma function.  */
double tgamma(double);
__END_NAMESPACE_C99
#endif

#if defined __USE_MISC || defined __USE_XOPEN || defined __USE_PUREC
/* Obsolete alias for `lgamma'.  */
double gamma(double);
double gamma_r(double, int *__signgamp);
#endif

#if defined __USE_MISC || defined __USE_PUREC
/* Reentrant version of lgamma.  This function uses the global variable
   `signgam'.  The reentrant version instead takes a pointer and stores
   the value through it.  */
double lgamma_r(double, int *__signgamp);
#endif


#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99 || defined __USE_PUREC
__BEGIN_NAMESPACE_C99
/* Return the integer nearest X in the direction of the
   prevailing rounding mode.  */
double rint(double __x);

/* Return X + epsilon if X < Y, X - epsilon if X > Y.  */
double nextafter(double __x, double __y) __attribute__((__const__));
# if defined __USE_ISOC99 && !defined __LDBL_COMPAT
double nexttoward(double __x, long double __y) __attribute__((__const__));
# endif

/* Return the remainder of integer divison X / Y with infinite precision.  */
double remainder(double __x, double __y);

# if defined __USE_MISC || defined __USE_ISOC99
/* Return X times (2 to the Nth power).  */
double scalbn(double __x, int __n);
# endif

/* Return the binary exponent of X, which must be nonzero.  */
int ilogb(double __x);
#endif

#if defined __USE_ISOC99 || defined __USE_PUREC
/* Return X times (2 to the Nth power).  */
double scalbln(double __x, long int __n);

/* Round X to integral value in floating-point format using current
   rounding direction, but do not raise inexact exception.  */
double nearbyint(double __x);

/* Round X to nearest integral value, rounding halfway cases away from
   zero.  */
double round(double __x) __attribute__((__const__));

/* Round X to the integral value in floating-point format nearest but
   not larger in magnitude.  */
double trunc(double __x) __attribute__((__const__));

/* Compute remainder of X and Y and put in *QUO a value with sign of x/y
   and magnitude congruent `mod 2^n' to the magnitude of the integral
   quotient x/y, with n >= 3.  */
double remquo(double __x, double __y, int *__quo);


/* Conversion functions.  */

/* Round X to nearest integral value according to current rounding
   direction.  */
long int lrint(double __x);
#ifndef __NO_LONGLONG
__extension__
long long int llrint(double __x);
#endif

/* Round X to nearest integral value, rounding halfway cases away from
   zero.  */
long int lround(double __x);
#ifndef __NO_LONGLONG
__extension__
long long int llround(double __x);
#endif


/* Return positive difference between X and Y.  */
double fdim(double __x, double __y);

/* Return maximum numeric value from X and Y.  */
double fmax(double __x, double __y) __attribute__((__const__));

/* Return minimum numeric value from X and Y.  */
double fmin(double __x, double __y) __attribute__((__const__));


/* Classify given number.  */
int __fpclassify(double __value) __attribute__ ((__const__));

/* Test for negative number.  */
int __signbit(double __value) __attribute__ ((__const__));
int signbit(double __value) __attribute__ ((__const__));


/* Multiply-add function computed as a ternary operation.  */
double fma(double __x, double __y, double __z);
#endif /* Use ISO C99.  */

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99 || defined __USE_PUREC
__END_NAMESPACE_C99
#endif

#if defined __USE_GNU
/* Test for signaling NaN.  */
int __issignaling(double __value) __attribute__ ((__const__));
int issignaling(double __value) __attribute__ ((__const__));
#endif

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED
/* Return X times (2 to the Nth power).  */
double scalb(double __x, double __n);
#endif


#ifndef __STRICT_ANSI__
double poly(int order, const double *coeffs, double x);
#endif
