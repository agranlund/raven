/*****************************************************************************/
/*                                                                           */
/* PORTAB.H                                                                  */
/*                                                                           */
/* Use of this file may make your code compatible with all C compilers       */
/* listed.                                                                   */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* ENVIRONMENT                                                               */
/*****************************************************************************/

#ifndef __PORTAB_H__
#define __PORTAB_H__

#ifndef __STDIO_H__
#  include <stdio.h>
#endif
#ifndef __STRING_H__
#  include <string.h>
#endif
#ifndef __STDLIB_H__
#  include <stdlib.h>
#endif

#define GEMDOS     1                          /* Digital Research GEMDOS     */

#define M68000     1                          /* Motorola Processing Unit    */
#define I8086      0                          /* Intel Processing Unit       */

#define TURBO_C    0                          /* Turbo C Compiler            */
#define PCC        1                          /* Portable C-Compiler         */

#define GEM1       0x0001                     /* ATARI GEM version           */
#define GEM2       0x0002                     /* MSDOS GEM 2.x versions      */
#define GEM3       0x0004                     /* MSDOS GEM/3 version         */
#define XGEM       0x0100                     /* OS/2,FlexOS X/GEM version   */

#ifndef GEM
#if (defined(GEMDOS) && GEMDOS)
#define GEM        GEM1                       /* GEMDOS default is GEM1      */
#endif /* GEMDOS */

#if defined(MSDOS) && MSDOS
#define GEM        GEM3                       /* MSDOS default is GEM3       */
#endif /* MSDOS */

#if defined(OS2) && OS2
#define GEM        XGEM                       /* OS/2 default is X/GEM       */
#endif /* MSDOS */

#if defined(FLEXOS) || defined(__unix__)
#define GEM        XGEM                       /* FlexOS default is X/GEM     */
#endif /* FLEXOS */
#endif /* GEM */

/*****************************************************************************/
/* STANDARD TYPE DEFINITIONS                                                 */
/*****************************************************************************/

#define CHAR    signed char                   /* Signed byte                 */
#define UCHAR   unsigned char                 /* Unsigned byte               */

#define BYTE	signed char
#define UBYTE	unsigned char

#if (!(defined(__PUREC__) || defined(__AHCC__) || defined(__TURBOC__)) || (defined(_COMPILER_H) && !defined(__MSHORT__)) || defined(__GEMLIB__)) && !defined(__USE_GEMLIB)
#define __USE_GEMLIB 1
#endif
#ifdef __USE_GEMLIB
#define WORD    short                         /* Signed word (16 bits)       */
#define UWORD   unsigned short                /* Unsigned word               */
#else
#define WORD    int                           /* Signed word (16 bits)       */
#define UWORD   unsigned int                  /* Unsigned word               */
#endif

#define LONG    long                          /* Signed long (32 bits)       */
#define ULONG   unsigned long                 /* Unsigned long               */

#define BOOLEAN int                           /* 2 valued (true/false)       */

#define FLOAT   float                         /* Single precision float      */
#define DOUBLE  double                        /* Double precision float      */

#define INT     int                           /* A machine dependent int     */
#define UINT    unsigned int                  /* A machine dependent uint    */

#define REG     register                      /* Register variable           */
#define AUTO    auto                          /* Local to function           */
#define LOCAL   static                        /* Local to module             */
#define MLOCAL  LOCAL                         /* Local to module             */
#define GLOBAL                                /* Global variable             */
#define LIB_GLOBAL GLOBAL
#define OS_GLOBAL  GLOBAL

/*****************************************************************************/
/* COMPILER DEPENDENT DEFINITIONS                                            */
/*****************************************************************************/

#if GEMDOS                                    /* GEMDOS compilers            */

#if TURBO_C
#define graf_mbox graf_movebox                /* Wrong GEM binding           */
#define graf_rubbox graf_rubberbox            /* Wrong GEM binding           */
#endif /* TURBO_C */

#endif /* GEMDOS */


#define CONST    const
#define VOLATILE volatile
#define CDECL    cdecl
#ifndef __CDECL
#define __CDECL  cdecl
#endif
#ifdef __NO_CDECL
#define _CDECL
#else
#define _CDECL	 __CDECL
#endif
#define _PASCAL  pascal

#define SIZE_T   size_t

#ifndef VOID
#define VOID     void
#endif

/*****************************************************************************/
/* MISCELLANEOUS DEFINITIONS                                                 */
/*****************************************************************************/

#ifndef FALSE
#define FALSE   (BOOLEAN)0                    /* Function FALSE value        */
#define TRUE    (BOOLEAN)1                    /* Function TRUE  value        */
#endif

#define FAILURE (-1)                          /* Function failure return val */
#define SUCCESS 0                             /* Function success return val */
#define FOREVER for (;;)                      /* Infinite loop declaration   */
#define EOS     '\0'                          /* End of string value         */


#ifndef EOF
#define EOF     (-1)                          /* EOF value                   */
#endif

#define BOOL BOOLEAN


#define _BYTE BYTE
#define _UBYTE UBYTE
#define _WORD WORD
#define _UWORD UWORD
#define _LONG LONG
#define _LONG_PTR _LONG
#define _ULONG ULONG
#define _VOID void
#define _BOOL BOOLEAN
#define _DOUBLE double

#ifndef _LPVOID
#define _LPVOID void *
#endif

#ifndef _LPBYTE
#  define _LPBYTE char *
#endif

#define LOCAL static
#define RLOCAL LOCAL
#define GLOBAL /**/
#define _HUGE 
#define EXP_PTR
#define EXP_PROC

#define FUNK_NULL 0l

#ifndef FALSE
#define FALSE 0
#define TRUE  (!FALSE)
#endif

#ifndef UNUSED
# define UNUSED(x) (void)(x)
#endif

#define BigEndian 1
#ifndef BigEndian
#  define BigEndian (is_big_endian())
#  define IfBigEndian if (BigEndian)
#  define IfNotBigEndian if (!BigEndian)
_BOOL is_big_endian ( void );
#else
#  if BigEndian
#    define IfBigEndian /**/
#    define IfNotBigEndian if (stdout){} else
#  else
#    define IfBigEndian if (stdout){} else
#    define IfNotBigEndian /**/
#  endif
#endif

#define ATARI 1
#define PU_MOTOROLA  1			/* Motorola Processing Unit    */
#define NO_GEM 0

#ifndef __attribute__
#  ifndef __GNUC__
#    define __attribute__(x)
#  endif
#endif

#if defined(__PUREC__) || defined(__TURBOC__)
#  define ANONYMOUS_STRUCT_DUMMY(x) struct x { int dummy; };
#endif

#ifndef ANONYMOUS_STRUCT_DUMMY
#  define ANONYMOUS_STRUCT_DUMMY(x)
#endif

#define STDC_HEADERS 1
#define HAVE_STRING_H 1
#define HAVE_STRSTR

#define ALL_FILE_MASK "*.*"

#define PACKED
#define INLINE

#ifndef NO_CONST
#  ifdef __GNUC__
#    define NO_CONST(p) __extension__({ union { CONST void *cs; void *s; } x; x.cs = p; x.s; })
#  else
#    define NO_CONST(p) ((void *)(p))
#  endif
#endif

#ifdef __cplusplus
#  define EXTERN_C_BEG extern "C" {
#  define EXTERN_C_END }
#  define EXTERN_C     extern "C"
#else
#  define EXTERN_C_BEG
#  define EXTERN_C_END
#  define EXTERN_C
#endif

#define HOST_BYTE_ORDER BYTE_ORDER_BIG_ENDIAN

#endif /* __PORTAB_H__ */
