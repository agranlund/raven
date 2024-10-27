#ifndef __PUREC_STDIO_H__
#define __PUREC_STDIO_H__ 1

struct __stdio_file
{
#if defined(__AHCC__) || defined(__AHCCLIB__)
        unsigned char	*BufPtr;	/* current buffer pointer */
        long		     _cnt;		/* # of bytes in buffer */
        unsigned char	*BufStart;	/* base of file buffer */
        size_t		    _bsiz;		/* buffer size */
#else
        void *BufPtr;           /* next byte write              */
        void *BufLvl;           /* next byte read               */
        void *BufStart;         /* first byte of buffer         */
        void *BufEnd;           /* first byte after buffer      */
#endif
        int  Handle;            /* gemdos handle                */
        char Flags;             /* some Flags                   */
        char Mode;
        char ChrBuf;            /* little buffer                */
        char ungetFlag;
};

#define stdout      (&_StdOutF)
#define stdin       (&_StdInF)
#define stderr      (&_StdErrF)
#define stdaux      (&_StdAuxF)
#define stdprn      (&_StdPrnF)

#define _IOFBF      0
#define _IOLBF      1
#define _IONBF      2

#undef FOPEN_MAX
#define FOPEN_MAX       32

/* for OPEN_MAX etc. */
#include <bits/posix1lm.h>
/* for O_* constants */
#include <fcntl.h>

/****** External data **************************************************/

extern FILE         _StdOutF;
extern FILE         _StdInF;
extern FILE         _StdErrF;
extern FILE         _StdAuxF;
extern FILE         _StdPrnF;

/****** FileIo macros ***************************************************/

#define getc( c )     fgetc(c)
#define getchar()     fgetc(stdin)
#define putc( c, s )  fputc(c, s)
#define putchar( c )  fputc(c, stdout)
#define fileno( s )   ((s)->Handle)

/*
 * functions also declared in unistd.h
 */
#ifndef __ssize_t_defined
typedef __ssize_t ssize_t;
# define __ssize_t_defined
#endif

extern __off_t lseek (int __fd, __off_t __offset, int __whence) __THROW;
extern int close (int __fd) __THROW;
extern ssize_t read (int __fd, void *__buf, size_t __nbytes) __THROW;
extern ssize_t write (int __fd, __const void *__buf, size_t __n) __THROW;

extern int unlink (__const char *__name) __THROW;


/*
 * should not really be here,
 * but were in the original,
 * and Pure-C programs might expect this.
 */
extern int errno;

#endif
