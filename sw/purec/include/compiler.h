/* compiler specific defines */
/* this file is guaranteed to be included exactly once if you include
   anything at all. all site-dependent or compiler-dependent stuff
   should go here!!!
 */

#ifndef _COMPILER_H
# define _COMPILER_H 1

/* symbol to identify the library itself */
#ifndef __MINT__
# define __MINT__
#endif

/* Convenience macros to test the versions of glibc and gcc.
   Use them like this:
   #if __GNUC_PREREQ (2,8)
   ... code requiring gcc 2.8 or later ...
   #endif
   Note - they won't work for gcc1 or glibc1, since the _MINOR macros
   were not defined then.  */
#if defined __GNUC__ && defined __GNUC_MINOR__
# define __GNUC_PREREQ(maj, min) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
# define __GNUC_PREREQ(maj, min) 0
#endif

/* symbols to identify the type of compiler */

/* general library stuff */
/* __SIZE_TYPEDEF__: 	the type returned by sizeof() */
/* __SSIZE_TYPEDEF__:   signed long values.  */
/* __PTRDIFF_TYPEDEF__: the type of the difference of two pointers */
/* __WCHAR_TYPEDEF__: 	wide character type (i.e. type of L'x') */
/* __WINT_TYPEDEF__:    the type wint_t (whatever this is).  */
/* __EXITING:           the type of a function that exits */
/* __NORETURN:          attribute of a function that exits (gcc >= 2.5) */
/* __CDECL:             function must get parameters on stack */
		/* if !__CDECL, passing in registers is OK */

/* symbols to report about compiler features */
/* #define __NEED_VOID__	compiler doesn't have a void type */
/* #define __MSHORT__		compiler uses 16 bit integers */
/* (note that gcc define this automatically when appropriate) */

#if defined(__TOS__) && !defined(__atarist__)
#  define __atarist__ 1
#endif

#if defined(__TOS__) && !defined(__mc68000__)
#  define __mc68000__ 1
#endif

#if defined(__TOS__) && !defined(__m68k__)
#  define __m68k__ 1
#endif

#ifndef __MSHORT__
#  if (defined(__PUREC__) && (__PUREC__ < 0x400)) || defined(__TURBOC__) || defined(__AHCC__)
#     define __MSHORT__
#  endif
#endif

#if defined(__COLDFIRE__) && !defined(__mcoldfire__)
#  define __mcoldfire__ 1
#endif

#if (defined(__68881__) || defined(_M68881) || defined(__M68881__)) && !defined(__HAVE_68881__)
#  define __HAVE_68881__ 1
#endif

#if (defined(__HAVE_68881__) || defined(__FPU__) || (defined(__mcoldfire__) && defined(__mcffpu__))) && !defined(__HAVE_FPU__)
#  define __HAVE_FPU__ 1
#  define __HAVE_M68KFPU__ 1
#endif

/* Note: PureC until version 2.50 does not define any symbol when using -2 or better,
   you will have to do that in the Project/Makefile */
#if (defined(mc68020) || defined(__68020__) || defined(__M68020__)) && !defined(__mc68020__)
#  define __mc68020__ 1
#endif

#ifdef __GNUC__

#define __SIZE_TYPEDEF__ __SIZE_TYPE__
#define __PTRDIFF_TYPEDEF__ __PTRDIFF_TYPE__

#ifdef __GNUG__
/* In C++, wchar_t is a distinct basic type,
   and we can expect __wchar_t to be defined by cc1plus.  */
#define __WCHAR_TYPEDEF__ __wchar_t
#else
/* In C, cpp tells us which type to make an alias for.  */
#define __WCHAR_TYPEDEF__ __WCHAR_TYPE__
#endif

#if __GNUC_PREREQ(2, 5)
#ifndef __NORETURN
#define __NORETURN __attribute__ ((noreturn))
#endif
#define __EXITING void
#else
#define __EXITING volatile void
#endif

#ifndef __NO_INLINE__
# define __GNUC_INLINE__
#endif

#if __GNUC_PREREQ(3, 3)
# define __CLOBBER_RETURN(a) 
#else
# define __CLOBBER_RETURN(a) a,
#endif

#if __GNUC_PREREQ(2, 6)
#define AND_MEMORY , "memory"
#else
#define AND_MEMORY
#endif

#endif /* __GNUC__ */

#if defined(__PUREC__) || defined(__TURBOC__)
#if defined(__STDC__) && !defined(__STRICT_ANSI__)
# define __STRICT_ANSI__
#endif
/*
 * orginal Pure-C defines __STDC__ only when using -A,
 * which otherwise indicates that use of cdecl etc. will cause an
 * error. Define it to indicate that prototypes
 * still can be used.
 */
#ifndef __STDC__
#  define __STDC__ 1
#else
#  define __NO_CDECL
#endif
#define __SIZE_TYPEDEF__ unsigned long
#define __PTRDIFF_TYPEDEF__ long
#define __WCHAR_TYPEDEF__ char
#define __EXITING void
#define __VA_LIST__ char *
#ifndef __CDECL
#define __CDECL cdecl
#endif
#undef _EXTERN_INLINE
#define _EXTERN_INLINE extern
#endif /* __PUREC__ */

/* some default declarations */
/* if your compiler needs something
 * different, define it above
 */
#ifndef __VA_LIST__
#define __VA_LIST__ char *
#endif

#ifndef __CDECL
#if defined(__GNUC__) && defined(__FASTCALL__)
#define __CDECL __attribute__((__cdecl__))
#else
#define __CDECL
#endif
#endif

#ifdef __NO_CDECL
#define _CDECL
#else
#define _CDECL	 __CDECL
#endif

#ifndef __NORETURN
#define __NORETURN
#endif

/* this should go away */
#ifndef __EXTERN
#define __EXTERN extern
#endif

#ifndef __NULL
#  define __NULL ((void *)0)
#endif

#ifdef __MSHORT__
# define __SSIZE_TYPEDEF__ long
# define __WINT_TYPEDEF__ unsigned long
#else
# define __SSIZE_TYPEDEF__ int
# define __WINT_TYPEDEF__ unsigned int
#endif

/* "__OPTIMIZE__" is currently used only to
 * define some inline functions, which is currently
 * supported by GNU-C only. If that ever changes,
 * change that test here.
 */
#ifndef __GNUC__
# ifdef __OPTIMIZE__
#  undef __OPTIMIZE__
# endif
#endif

#if defined (__GNUC__) && !defined (__MSHORT__)
# ifndef __unix__
#  define __unix__ 1
# endif
# ifndef __unix
#  define __unix __unix__
# endif
# ifndef __UNIX__
#  define __UNIX__ __unix__
# endif
# ifndef __UNIX
#  define __UNIX __unix__
# endif
# ifndef _unix
#  define _unix __unix__
# endif
# ifndef __STRICT_ANSI__
#  ifndef unix
#   define unix __unix__
#  endif
# endif
#endif

#ifndef __USER_LABEL_PREFIX__
#  if defined(__PUREC__) || defined(__AHCC__) || defined(__TURBOC__) || defined(__ELF__)
#    define __USER_LABEL_PREFIX__
#  else
#    define __USER_LABEL_PREFIX__ _
#  endif
#endif

#ifndef __SYMBOL_PREFIX
# define __SYMBOL_PREFIX __STRINGIFY(__USER_LABEL_PREFIX__)
#endif
#ifndef __ASM_SYMBOL_PREFIX
# define __ASM_SYMBOL_PREFIX __USER_LABEL_PREFIX__
#endif

#ifndef _FEATURES_H
# include <features.h>
#endif

#ifdef _LIBC
# define _MINTLIB
#endif

#endif /* _COMPILER_H */
