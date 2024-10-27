/*
 *	ISO C Standard: 4.10 GENERAL UTILITIES	<stdlib.h>
 */

#ifndef _STDLIB_H
#define _STDLIB_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

#define __need_size_t
#define __need_wchar_t
#define __need_ptrdiff_t
#define __need_NULL
#include <stddef.h>

#ifdef __MSHORT__
#define	RAND_MAX	(0x7FFF)	/* maximum value from rand() */
#else
#define	RAND_MAX	(0x7FFFFFFFL)	/* maximum value from rand() */
#endif

#define MB_CUR_MAX	1		/* max. length of multibyte character
					   in current locale */

#ifndef EXIT_FAILURE
#define EXIT_FAILURE	(1)
#define EXIT_SUCCESS	(0)
#endif

/* Returned by `div'.  */
typedef struct {
	int quot;			/* Quotient.  */
	int rem;			/* Remainder.  */
} div_t;

/* Returned by `ldiv'.  */
#ifndef __ldiv_t_defined
typedef struct {
	long int quot;		/* Quotient.  */
	long int rem;		/* Remainder.  */
} ldiv_t;
# define __ldiv_t_defined	1
#endif

#if defined __USE_ISOC99 && !defined __lldiv_t_defined && !defined(__NO_LONGLONG)
/* Returned by `lldiv'.  */
__extension__ typedef struct {
	long long int quot;		/* Quotient.  */
	long long int rem;		/* Remainder.  */
} lldiv_t;
# define __lldiv_t_defined	1
#endif

/* Shorthand for type of comparison functions.  */
#ifndef __COMPAR_FN_T
# define __COMPAR_FN_T
typedef int (*__compar_fn_t) (__const void*, __const void*);

# ifdef	__USE_GNU
typedef __compar_fn_t comparison_fn_t;
# endif
#endif

/* Convert a string to a floating-point number.  */
extern double atof (__const char *__nptr) __THROW;
/* Convert a string to an integer.  */
extern int atoi (__const char *__nptr) __THROW;
/* Convert a string to a long integer.  */
extern long int atol (__const char *__nptr) __THROW;

#if (defined __USE_ISOC99 || (defined __GNUC__ && defined __USE_MISC)) && !defined(__NO_LONGLONG)
/* This function will be part of the standard C library in ISO C 9X.  */
extern long long int atoll (__const char *__nptr) __THROW;
#endif

/* Convert a string to a floating-point number.  */
double strtod (__const char *__restrict __nptr, char **__restrict __endptr) __THROW;

#ifdef	__USE_ISOC99
/* Likewise for `float' and `long double' sizes of floating-point numbers.  */
extern float strtof (__const char *__restrict __nptr,
		     char **__restrict __endptr) __THROW;

extern long double strtold (__const char *__restrict __nptr,
			    char **__restrict __endptr) __THROW;
#endif

#ifdef __USE_PUREC
char *itoa(int value, char *string, int radix);
char *ltoa(long value, char *string, int radix);
char *ultoa(unsigned long value, char *string, int radix);
#endif


/* Convert a string to a long integer.  */
long int strtol (__const char *__restrict __nptr, char **__restrict __endptr, int __base) __THROW;
/* Convert a string to an unsigned long integer.  */
unsigned long int strtoul (__const char *__restrict __nptr, char **__restrict __endptr, int __base) __THROW;

#if defined __GNUC__ && defined __USE_BSD && !defined(__NO_LONGLONG)
/* Convert a string to a quadword integer.  */
extern long long int strtoq (__const char* __nptr, char** _endptr,
                                        int __base) __THROW;
/* Convert a string to an unsigned quadword integer.  */
extern long long int strtouq (__const char* __nptr, char** _endptr,
                                         int __base) __THROW;
#endif  /* GCC and use BSD.  */

#if (defined __USE_ISOC99 || (defined __GNUC__ && defined __USE_MISC)) && !defined(__NO_LONGLONG)
/* These functions will be part of the standard C library in ISO C 9X.  */
/* Convert a string to a quadword integer.  */
extern long long int strtoll (__const char* __nptr, char** _endptr,
                                         int __base) __THROW;
/* Convert a string to an unsigned quadword integer.  */
extern unsigned long long int strtoull (__const char* __nptr, char** _endptr,
                                                   int __base) __THROW;
#endif

extern void srand (unsigned int __seed) __THROW;
extern int rand (void) __THROW;

#if defined __USE_SVID || defined __USE_XOPEN_EXTENDED || defined __USE_BSD
#ifndef __USE_PUREC
extern long random (void) __THROW;
extern void srandom (unsigned int seed) __THROW;
#endif
#endif

#if defined(__USE_SVID) || defined(__USE_XOPEN)
/* System V style 48-bit random number generator functions.  */

/* Return non-negative, double-precision floating-point value in [0.0,1.0).  */
extern double drand48 (void) __THROW;
extern double erand48 (unsigned short int __xsubi[3]) __THROW;

/* Return non-negative, long integer in [0,2^31).  */
extern long lrand48 (void) __THROW;
extern long nrand48 (unsigned short int __xsubi[3]) __THROW;

/* Return signed, long integers in [-2^31,2^31).  */
extern long mrand48 (void) __THROW;
extern long jrand48 (unsigned short int __xsubi[3]) __THROW;

/* Seed random number generator.  */
extern void srand48 (long __seedval) __THROW;
extern unsigned short int *seed48 (unsigned short int __seed16v[3]) __THROW;
extern void lcong48 (unsigned short int __param[7]) __THROW;

/* Data structure for communication with thread safe versions.  */
struct drand48_data
  {
    unsigned short int x[3];	/* Current state.  */
    unsigned short int a[3];	/* Factor in congruential formula.  */
    unsigned short int c;	/* Additive const. in congruential formula.  */
    unsigned short int old_x[3]; /* Old state.  */
    int init;			/* Flag for initializing.  */
  };

#ifdef __USE_MISC
/* Return non-negative, double-precision floating-point value in [0.0,1.0).  */
extern int drand48_r (struct drand48_data *__buffer, 
                                 double *__result) __THROW;
extern int erand48_r (unsigned short int __xsubi[3],
			         struct drand48_data *__buffer, 
			         double *__result) __THROW;

/* Return non-negative, long integer in [0,2^31).  */
extern int lrand48_r (struct drand48_data *__buffer, 
                                 long *__result) __THROW;
extern int nrand48_r (unsigned short int __xsubi[3],
			         struct drand48_data *__buffer, 
			         long *__result) __THROW;

/* Return signed, long integers in [-2^31,2^31).  */
extern int mrand48_r (struct drand48_data *__buffer, 
                                 long *__result) __THROW;
extern int jrand48_r (unsigned short int __xsubi[3],
			         struct drand48_data *__buffer, 
			         long *__result) __THROW;

/* Seed random number generator.  */
extern int srand48_r (long __seedval, 
                                 struct drand48_data *__buffer) __THROW;
extern int seed48_r (unsigned short int __seed16v[3],
			        struct drand48_data *__buffer) __THROW;
extern int lcong48_r (unsigned short int __param[7],
			         struct drand48_data *__buffer) __THROW;
#endif	/* Use misc.  */

/* Internal function to compute next state of the generator.  */
extern int __drand48_iterate (unsigned short int __xsubi[3],
				         struct drand48_data *__buffer) __THROW;
#endif	/* Use SVID or X/Open.  */

extern void *malloc (size_t __n) __THROW;
extern void free (void* __param) __THROW;
extern void *realloc (void* __r, size_t __n) __THROW;
extern void *calloc (size_t __n, size_t __s) __THROW;

#if defined __USE_GNU || defined __USE_BSD || defined __USE_MISC
# include <alloca.h>
#endif /* Use GNU, BSD, or misc.  */

#if (defined __USE_XOPEN_EXTENDED && !defined __USE_XOPEN2K) \
    || defined __USE_MISC
/* Allocate SIZE bytes on a page boundary.  The storage cannot be freed.  */
extern void *valloc (size_t __size) __THROW __attribute_malloc__ __wur;
#endif

#ifdef __USE_XOPEN2K
/* Allocate memory of SIZE bytes with an alignment of ALIGNMENT.  */
extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __THROW __nonnull ((1)) __wur;
#endif

#ifdef __USE_ISOC11
/* ISO C variant of aligned allocation.  */
extern void *aligned_alloc (size_t __alignment, size_t __size)
     __THROW __attribute_malloc__ __attribute_alloc_size__ ((2)) __wur;
#endif

#ifndef __STRICT_ANSI__

# ifdef __atarist__
extern void _malloczero (int __yes) __THROW;
extern void _mallocChunkSize (size_t __siz) __THROW;
# endif

#endif /* __STRICT_ANSI__ */

extern __EXITING abort (void) __THROW __NORETURN;
#if !(defined (__GNUG__) && __GNUG__ == 1)
  /* bug in g++ 1.39.0 -- cant digest proto */
extern int atexit (void (*)(void)) __THROW;
#endif
extern __EXITING exit (int) __THROW __NORETURN;
#ifdef __USE_PUREC
/* also declared in unistd.h; PureC programs might expect it to be declared here */
extern __EXITING _exit (int) __THROW __NORETURN;
#endif

#if defined __USE_ISOC11 || defined __USE_ISOCXX11
/* Call all functions registered with `at_quick_exit' in the reverse
   of the order in which they were registered and terminate program
   execution with STATUS.  */
extern void quick_exit (int __status) __THROW __attribute__ ((__noreturn__));
#endif

#ifdef __USE_ISOC99
/* Terminate the program with STATUS without calling any of the
   functions registered with `atexit' or `on_exit'.  */
extern void _Exit (int __status) __THROW __attribute__ ((__noreturn__));
#endif


extern char *getenv (__const char* __tag) __THROW;
extern int system (__const char* __s) __THROW;

extern void *bsearch (__const void* __key, __const void* __base, 
                                size_t __num, size_t __size, 
                                __compar_fn_t __compar) __THROW;
extern void qsort (void* __base, size_t __total_elems, 
                             size_t __size, 
                             __compar_fn_t __compar) __THROW;

#if defined(__AHCC__) || defined(__AHCCLIB__)
void hsort(void *__base, size_t __nmemb, size_t __size, __compar_fn_t __compar);
#endif

extern int abs (int __x) __THROW;
extern long labs (long __x) __THROW;

#if defined(__USE_ISOC99) && !defined(__NO_LONGLONG)
__extension__ extern long long int llabs (long long int __x)
     __THROW;
#endif


/* Return the `div_t', `ldiv_t' or `lldiv_t' representation
   of the value of NUMER over DENOM. */
/* GCC may have built-ins for these someday.  */
extern div_t div (int __numer, int __denom) __THROW __attribute__ ((__const__));
extern ldiv_t ldiv (long int __numer, long int __denom) __THROW
     __attribute__ ((__const__));
#if defined(__USE_ISOC99) && !defined(__NO_LONGLONG)
__extension__ extern lldiv_t lldiv (long long int __numer,
				   long long int __denom) __THROW
     __attribute__ ((__const__));
#endif

#if defined __USE_SVID || defined __USE_XOPEN_EXTENDED || defined __USE_PUREC
/* Convert floating point numbers to strings.  The returned values are
   valid only until another call to the same function.  */

/* Convert VALUE to a string with NDIGIT digits and return a pointer to
   this.  Set *DECPT with the position of the decimal character and *SIGN
   with the sign of the number.  */
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
		   int *__restrict __sign) __THROW;

/* Convert VALUE to a string rounded to NDIGIT decimal digits.  Set *DECPT
   with the position of the decimal character and *SIGN with the sign of
   the number.  */
extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
		   int *__restrict __sign) __THROW;

/* If possible convert VALUE to a string with NDIGIT significant digits.
   Otherwise use exponential representation.  The resulting string will
   be written to BUF.  */
extern char *gcvt (double __value, int __ndigit, char *__buf) __THROW;


# ifdef __USE_MISC
/* Long double versions of above functions.  */
extern char *qecvt (long double __value, int __ndigit,
		    int *__restrict __decpt, int *__restrict __sign) __THROW;
extern char *qfcvt (long double __value, int __ndigit,
		    int *__restrict __decpt, int *__restrict __sign) __THROW;
extern char *qgcvt (long double __value, int __ndigit, char *__buf) __THROW;


/* Reentrant version of the functions above which provide their own
   buffers.  */
extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
		   int *__restrict __sign, char *__restrict __buf,
		   size_t __len) __THROW;
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
		   int *__restrict __sign, char *__restrict __buf,
		   size_t __len) __THROW;

extern int qecvt_r (long double __value, int __ndigit,
		    int *__restrict __decpt, int *__restrict __sign,
		    char *__restrict __buf, size_t __len) __THROW;
extern int qfcvt_r (long double __value, int __ndigit,
		    int *__restrict __decpt, int *__restrict __sign,
		    char *__restrict __buf, size_t __len) __THROW;
# endif	/* misc */
#endif	/* use MISC || use X/Open Unix */


/* Return the length of the multibyte character
   in S, which is no longer than N.  */
extern int mblen (__const char *__s, size_t __n) __THROW;
/* Return the length of the given multibyte character,
   putting its `wchar_t' representation in *PWC.  */
extern int mbtowc (wchar_t *__restrict __pwc,
		   __const char *__restrict __s, size_t __n) __THROW;
/* Put the multibyte character represented
   by WCHAR in S, returning its length.  */
extern int wctomb (char *__s, wchar_t __wchar) __THROW;


/* Convert a multibyte string to a wide char string.  */
extern size_t mbstowcs (wchar_t *__restrict  __pwcs,
			__const char *__restrict __s, size_t __n) __THROW;
/* Convert a wide char string to multibyte string.  */
extern size_t wcstombs (char *__restrict __s,
			__const wchar_t *__restrict __pwcs, size_t __n)
     __THROW;


#ifdef __USE_GNU
/* This function is similar to the above but returns NULL if the
   programs is running with SUID or SGID enabled.  */
extern char *secure_getenv (const char *__name)
     __THROW __nonnull ((1)) __wur;
#endif

#if defined(__USE_SVID) || defined(__USE_XOPEN) || defined(__USE_PUREC)
/* The SVID says this is in <stdio.h>, but this seems a better place.	*/
/* Put STRING, which is of the form "NAME=VALUE", in the environment.
   If there is no `=', remove NAME from the environment.  */
extern int putenv (__const char *__string) __THROW;
#endif

#if defined(__USE_BSD) || defined(__USE_XOPEN2K)
/* Set NAME to VALUE in the environment.
   If REPLACE is nonzero, overwrite an existing value.  */
extern int setenv (__const char *__name, __const char *__value,
			int __replace) __THROW;

/* Remove the variable NAME from the environment.  */
extern int unsetenv (__const char *__name) __THROW;
#endif

#ifdef __USE_MISC
/* The `clearenv` was planned to be added to POSIX.1 but probably
   never made it.  Nevertheless the POSIX.9 standard (POSIX bindings
   for Fortran 77) requires this function.  */
extern int clearenv (void) __THROW;
#endif

#if defined(__USE_MISC) || defined(__USE_XOPEN_EXTENDED)
/* Generate a unique temporary file name from TEMPLATE.
   The last six characters of TEMPLATE must be "XXXXXX";
   they are replaced with a string that makes the file name unique.
   Returns TEMPLATE, or a null pointer if it cannot get a unique file name.  */
extern char *mktemp (char* __template) __THROW;

/* Generate a unique temporary file name from TEMPLATE.
   The last six characters of TEMPLATE must be "XXXXXX";
   they are replaced with a string that makes the filename unique.
   Returns a file descriptor open on the file for reading and writing,
   or -1 if it cannot create a uniquely-named file.  */
extern int mkstemp (char* __template) __THROW;
#endif

#ifdef __USE_GNU
/* Return a malloc'd string containing the canonical absolute name of the
   named file.  The last file name component need not exist, and may be a
   symlink to a nonexistent file.  */
extern char* canonicalize_file_name (__const char* __name) __THROW;
#endif

#if defined __USE_BSD || defined __USE_XOPEN_EXTENDED
/* Return the canonical absolute name of file NAME.  The last file name
   component need not exist, and may be a symlink to a nonexistent file.
   If RESOLVED is null, the result is malloc'd; otherwise, if the canonical
   name is PATH_MAX chars or more, returns null with `errno' set to
   ENAMETOOLONG; if the name fits in fewer then PATH_MAX chars, returns
   the name in RESOLVED.  */
extern char* realpath (__const char* __name, char* __resolved) __THROW;
#endif

#if defined(__AHCC__) && !defined(_XA_MEMORY_H)
#include <ahcm.h>		/* HR: A home cooked memory allocator */
#endif

__END_DECLS

#endif /* _STDLIB_H */
