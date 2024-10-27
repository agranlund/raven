#if !defined _STDIO_H
# error "Never use <bits/stdio.h> directly; include <stdio.h> instead."
#endif

#ifndef __extern_inline
# define __STDIO_INLINE inline
#else
# define __STDIO_INLINE __extern_inline
#endif

__BEGIN_DECLS

/* For thread safe I/O functions we need a lock in each stream.  We
   keep the type opaque here.  */
#if (defined(__PUREC__) || defined(__TURBOC__)) && !defined(_LIBC)
struct __stdio_lock { int dummy; };
#else
struct __stdio_lock;
#endif

/* The FILE structure.  */
struct __stdio_file
{
  /* Magic number for validation.  Must be negative in open streams
     for the glue to Unix stdio getc/putc to work.
     NOTE: stdio/glue.c has special knowledge of these first four members.  */
  unsigned long __magic;
#define	_IOMAGIC (0xfedabeebUL)	/* Magic number to fill `__magic'.  */
#define	_GLUEMAGIC (0xfeedbabeUL)	/* Magic for glued Unix streams.  */

  char *__bufp;			/* Pointer into the buffer.  */
  char *__get_limit;		/* Reading limit.  */
  char *__put_limit;		/* Writing limit.  */

  char *__buffer;		/* Base of buffer.  */
  size_t __bufsize;		/* Size of the buffer.  */
  __ptr_t __cookie;		/* Magic cookie.  */
  __io_mode __mode;		/* File access mode.  */
  __io_functions __io_funcs;	/* I/O functions.  */
  __room_functions __room_funcs;/* I/O buffer room functions.  */
  fpos_t __offset;		/* Current file position.  */
  fpos_t __target;		/* Target file position.  */
  FILE *__next;			/* Next FILE in the linked list.  */
  char *__pushback_bufp;	/* Old bufp if char pushed back.  */
  unsigned char __pushback;	/* Pushed-back character.  */
  unsigned int __pushed_back:1;	/* A char has been pushed back.  */
  unsigned int __eof:1;		/* End of file encountered.  */
  unsigned int __error:1;	/* Error encountered.  */
  unsigned int __userbuf:1;	/* Buffer from user (should not be freed).  */
  unsigned int __linebuf:1;	/* Flush on newline.  */
  unsigned int __linebuf_active:1; /* put_limit is not really in use.  */
  unsigned int __seen:1;	/* This stream has been seen.  */
  unsigned int __ispipe:1;	/* Nonzero if opened by popen.  */
  struct __stdio_lock *__lock;	/* Pointer to associated lock.  */

#define _IO_CURRENTLY_NONE      0x000000000
#define _IO_CURRENTLY_PUTTING   0x000000001
#define _IO_CURRENTLY_GETTING   0x000000002
#define _IO_CURRENTLY_MASK      0x000000003
#define _IO_USER_LOCK           0x000008000
  unsigned int __flags;		/* miscellaneous flags. */
};


/* All macros used internally by other macros here and by stdio functions begin
   with `__'.  All of these may evaluate their arguments more than once.  */


/* Nonzero if STREAM is a valid stream.
   STREAM must be a modifiable lvalue (wow, I got to use that term).
   See stdio/glue.c for what the confusing bit is about.  */
#define	__validfp(stream)						      \
  (stream != NULL &&							      \
   ({ if (stream->__magic == _GLUEMAGIC)				      \
	stream = *((struct { int __magic; FILE **__p; } *) stream)->__p;      \
      stream->__magic == _IOMAGIC; }))

/* Clear the error and EOF indicators of STREAM.  */
#define	__clearerr(stream)	((stream)->__error = (stream)->__eof = 0)

/* Nuke STREAM, making it unusable but available for reuse.  */
extern void __invalidate __P ((FILE *__stream));

/* Make sure STREAM->__offset and STREAM->__target are initialized.
   Returns 0 if successful, or EOF on
   error (but doesn't set STREAM->__error).  */
extern int __stdio_check_offset __P ((FILE *__stream));


/* All the known streams are in a linked list
   linked by the `next' field of the FILE structure.  */
extern FILE *__stdio_head;	/* Head of the list.  */

/* Standard streams.  */
extern FILE *stdin, *stdout, *stderr;
#ifdef __STRICT_ANSI__
/* ANSI says these are macros; satisfy pedants.  */
#define	stdin	stdin
#define	stdout	stdout
#define	stderr	stderr
#endif


/* The possibilities for the third argument to `setvbuf'.  */
#define _IOFBF	0x1		/* Full buffering.  */
#define _IOLBF	0x2		/* Line buffering.  */
#define _IONBF	0x4		/* No buffering.  */


/* This performs actual output when necessary, flushing
   STREAM's buffer and optionally writing another character.  */
extern int __flshfp __P ((FILE *__stream, int __c));

/* This does actual reading when necessary, filling STREAM's
   buffer and returning the first character in it.  */
extern int __fillbf __P ((FILE *__stream));


/* The C standard explicitly says this can
   re-evaluate its arguments, so it does.  */
#define	__putc(c, stream)						      \
  ((stream)->__bufp < (stream)->__put_limit ?				      \
   (int) (unsigned char) (*(stream)->__bufp++ = (unsigned char) (c)) :	      \
   __flshfp ((stream), (unsigned char) (c)))

#define	__putwc(c, stream) putwc_unlocked(c, stream)

/* The C standard explicitly says this can be a macro,
   so we always do the optimization for it.  */
#define	putc(c, stream)	__putc ((c), (stream))

#ifdef __USE_EXTERN_INLINES
__STDIO_INLINE int
putchar (int __c)
{
  return __putc (__c, stdout);
}
#endif


#ifdef __USE_MISC
/* Faster version when locking is not necessary.  */
extern int fputc_unlocked __P ((int __c, FILE *__stream));

# ifdef __USE_EXTERN_INLINES
__STDIO_INLINE int
fputc_unlocked (int __c, FILE *__stream)
{
  return __putc (__c, __stream);
}
# endif /* Optimizing.  */
#endif /* Use MISC.  */

#if defined __USE_POSIX || defined __USE_MISC
/* These are defined in POSIX.1:1996.  */
extern int putc_unlocked __P ((int __c, FILE *__stream));
extern int putchar_unlocked __P ((int __c));

# ifdef __USE_EXTERN_INLINES
__STDIO_INLINE int
putc_unlocked (int __c, FILE *__stream)
{
  return __putc (__c, __stream);
}

__STDIO_INLINE int
putchar_unlocked (int __c)
{
  return __putc (__c, stdout);
}
# endif /* Optimizing.  */
#endif /* Use POSIX or MISC.  */


/* The C standard explicitly says this can
   re-evaluate its argument, so it does. */
#define	__getc(stream)							      \
  ((stream)->__bufp < (stream)->__get_limit ?				      \
   (int) ((unsigned char) *(stream)->__bufp++) : __fillbf(stream))

/* The C standard explicitly says this is a macro,
   so we always do the optimization for it.  */
#define	getc(stream)	__getc(stream)


#ifdef __USE_EXTERN_INLINES
__STDIO_INLINE int
getchar (void)
{
  return __getc (stdin);
}
#endif /* Optimizing.  */

#if defined __USE_POSIX || defined __USE_MISC
/* These are defined in POSIX.1:1996.  */
extern int getc_unlocked __P ((FILE *__stream));
extern int getchar_unlocked __P ((void));

# ifdef __USE_EXTERN_INLINES
__STDIO_INLINE int
getc_unlocked (FILE *__stream)
{
  return __getc (__stream);
}

__STDIO_INLINE int
getchar_unlocked (void)
{
  return __getc (stdin);
}
# endif /* Optimizing.  */
#endif /* Use POSIX or MISC.  */

#if defined(__OPTIMIZE__)
#define	feof(stream)	((stream)->__eof != 0)
#define	ferror(stream)	((stream)->__error != 0)
#ifdef __USE_MISC
#  define feof_unlocked(stream)		((stream)->__eof != 0)
#  define ferror_unlocked(stream)	((stream)->__error != 0)
#endif
#endif /* Optimizing.  */

#ifdef __USE_EXTERN_INLINES
__STDIO_INLINE int
vprintf (const char *__restrict __fmt, __va_list __arg)
{
  return vfprintf (stdout, __fmt, __arg);
}
#endif /* Optimizing.  */

/* Define helper macro.  */
#undef __STDIO_INLINE

__END_DECLS
