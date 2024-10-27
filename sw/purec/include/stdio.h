/* Copyright (C) 1991,92,93,94,95,96,97,98 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Slightly modified for MiNTLib by Guido Flohr <guido@freemint.de>.  */

/*
 *	ISO C Standard: 4.9 INPUT/OUTPUT	<stdio.h>
 */

#ifndef	_STDIO_H
#if !defined __need_FILE && !defined __need___FILE
#define	_STDIO_H	1

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

#define	__need_size_t
#define	__need_NULL
#include <stddef.h>

#define	__need___va_list
#include <stdarg.h>
#ifndef __va_list
#ifdef __GNUC__
typedef __gnuc_va_list __va_list;
#define __va_list __va_list
#endif
#endif

#include <bits/types.h>
# define __need___FILE
#endif /* Don't need FILE.  */
#undef	__need_FILE


#ifndef	__FILE_defined

/* The opaque type of streams.  */
typedef struct __stdio_file FILE;

#define	__FILE_defined	1
#endif /* FILE not defined.  */

#if !defined ____FILE_defined && defined __need___FILE

/* The opaque type of streams.  This is the definition used elsewhere.  */
typedef struct __stdio_file __FILE;

# define ____FILE_defined	1
#endif /* __FILE not defined.  */
#undef	__need___FILE


#ifdef	_STDIO_H

/* The type of the second argument to `fgetpos' and `fsetpos'.  */
typedef __off_t fpos_t;

/* The mode of I/O, as given in the MODE argument to fopen, etc.  */
typedef struct
{
  unsigned int __read:1;	/* Open for reading.  */
  unsigned int __write:1;	/* Open for writing.  */
  unsigned int __append:1;	/* Open for appending.  */
  unsigned int __binary:1;	/* Opened binary.  */
  unsigned int __create:1;	/* Create the file.  */
  unsigned int __exclusive:1;	/* Error if it already exists.  */
  unsigned int __truncate:1;	/* Truncate the file on opening.  */
} __io_mode;


/* Functions to do I/O and file management for a stream.  */

/* Read NBYTES bytes from COOKIE into a buffer pointed to by BUF.
   Return number of bytes read.  */
typedef __ssize_t __io_read_fn (__ptr_t __cookie, char *__buf,
				       size_t __nbytes);

/* Write N bytes pointed to by BUF to COOKIE.  Write all N bytes
   unless there is an error.  Return number of bytes written, or -1 if
   there is an error without writing anything.  If the file has been
   opened for append (__mode.__append set), then set the file pointer
   to the end of the file and then do the write; if not, just write at
   the current file pointer.  */
typedef __ssize_t __io_write_fn (__ptr_t __cookie, __const char *__buf,
				      size_t __n);

/* Move COOKIE's file position to *POS bytes from the
   beginning of the file (if W is SEEK_SET),
   the current position (if W is SEEK_CUR),
   or the end of the file (if W is SEEK_END).
   Set *POS to the new file position.
   Returns zero if successful, nonzero if not.  */
typedef int __io_seek_fn (__ptr_t __cookie, fpos_t *__pos, int __w);

/* Close COOKIE.  */
typedef int __io_close_fn (__ptr_t __cookie);

/* Return the file descriptor associated with COOKIE,
   or -1 on error.  There need not be any associated file descriptor.  */
typedef int __io_fileno_fn (__ptr_t __cookie);

#ifdef __USE_GNU
/* User-visible names for the above.  */
typedef __io_read_fn cookie_read_function_t;
typedef __io_write_fn cookie_write_function_t;
typedef __io_seek_fn cookie_seek_function_t;
typedef __io_close_fn cookie_close_function_t;
typedef __io_fileno_fn cookie_fileno_function_t;
#endif

/* Low level interface, independent of FILE representation.  */
#if defined __USE_GNU && !defined _LIBC
/* Define the user-visible type, with user-friendly member names.  */
typedef struct
{
  __io_read_fn *read;		/* Read bytes.  */
  __io_write_fn *write;		/* Write bytes.  */
  __io_seek_fn *seek;		/* Seek/tell file position.  */
  __io_close_fn *close;		/* Close file.  */
  __io_fileno_fn *fileno;	/* Return file descriptor.  */
} cookie_io_functions_t;
/* This name is still used in the prototypes in this file.  */
typedef cookie_io_functions_t __io_functions;
#else
/* Stick to ANSI-safe names.  */
typedef struct
{
  __io_read_fn *__read;		/* Read bytes.  */
  __io_write_fn *__write;	/* Write bytes.  */
  __io_seek_fn *__seek;		/* Seek/tell file position.  */
  __io_close_fn *__close;	/* Close file.  */
  __io_fileno_fn *__fileno;	/* Return file descriptor.  */
} __io_functions;
#endif

/* Higher level interface, dependent on FILE representation.  */
typedef struct
{
  /* Make room in the input buffer.  */
  int (*__input) (FILE *__stream);
  /* Make room in the output buffer.  */
  void (*__output) (FILE *__stream, int __c);
} __room_functions;

extern __const __io_functions __default_io_functions;
extern __const __room_functions __default_room_functions;


/* Default close function.  */
extern __io_close_fn __stdio_close;
/* Open FILE with mode M, store cookie in *COOKIEPTR.  */
extern int __stdio_open __P ((__const char *__file, __io_mode __m,
			      __ptr_t *__cookieptr));
/* Put out an error message for when stdio needs to die.  */
extern void __stdio_errmsg __P ((__const char *__msg, size_t __len));


/* Default buffer size.  */
#define	BUFSIZ	1024


/* End of file character.
   Some things throughout the library rely on this being -1.  */
#define	EOF	(-1)


/* The possibilities for the third argument to `fseek'.
   These values should not be changed.  */
#define	SEEK_SET	0	/* Seek from beginning of file.  */
#define	SEEK_CUR	1	/* Seek from current position.  */
#define	SEEK_END	2	/* Seek from end of file.  */


#ifdef	__USE_SVID
/* Default path prefix for `tempnam' and `tmpnam'.  */
#define	P_tmpdir	"/usr/tmp"
#endif


/* Get the values:
   L_tmpnam	How long an array of chars must be to be passed to `tmpnam'.
   TMP_MAX	The minimum number of unique filenames generated by tmpnam
   		(and tempnam when it uses tmpnam's name space),
		or tempnam (the two are separate).
   L_ctermid	How long an array to pass to `ctermid'.
   L_cuserid	How long an array to pass to `cuserid'.
   FOPEN_MAX	Minimum number of files that can be open at once.
   FILENAME_MAX	Maximum length of a filename.  */
#include <bits/stdiolim.h>


/* Remove file FILENAME.  */
extern int remove __P ((__const char *__filename));
/* Rename file OLD to NEW.  */
extern int rename __P ((__const char *__old, __const char *__new));


/* Create a temporary file and open it read/write.  */
extern FILE *tmpfile __P ((void));
#ifdef __USE_LARGEFILE64
extern FILE *tmpfile64 __P ((void));
#endif
/* Generate a temporary filename.  */
extern char *tmpnam __P ((char *__s));

#ifdef __USE_MISC
/* This is the reentrant variant of `tmpnam'.  The only difference is
   that it does not allow S to be NULL.  */
extern char *tmpnam_r __P ((char *__s));
#endif


#if defined __USE_SVID || defined __USE_XOPEN
/* Generate a unique temporary filename using up to five characters of PFX
   if it is not NULL.  The directory to put this file in is searched for
   as follows: First the environment variable "TMPDIR" is checked.
   If it contains the name of a writable directory, that directory is used.
   If not and if DIR is not NULL, that value is checked.  If that fails,
   P_tmpdir is tried and finally "/tmp".  The storage for the filename
   is allocated by `malloc'.  */
extern char *tempnam __P ((__const char *__dir, __const char *__pfx));
#endif


/* Close STREAM.  */
extern int fclose __P ((FILE *__stream));
/* Flush STREAM, or all streams if STREAM is NULL.  */
extern int fflush __P ((FILE *__stream));

#ifdef __USE_MISC
/* Faster versions when locking is not required.  */
extern int fflush_unlocked __P ((FILE *__stream));
#endif

#ifdef __USE_GNU
/* Close all streams.  */
extern int __fcloseall __P ((void));
extern int fcloseall __P ((void));
#endif


/* Open a file and create a new stream for it.  */
extern FILE *fopen __P ((__const char *__filename, __const char *__modes));
/* Open a file, replacing an existing stream with it. */
extern FILE *freopen __P ((__const char *__restrict __filename,
			   __const char *__restrict __modes,
			   FILE *__restrict __stream));

/* Return a new, zeroed, stream.
   You must set its cookie and io_mode.
   The first operation will give it a buffer unless you do.
   It will also give it the default functions unless you set the `seen' flag.
   The offset is set to -1, meaning it will be determined by doing a
   stationary seek.  You can set it to avoid the initial tell call.
   The target is set to -1, meaning it will be set to the offset
   before the target is needed.
   Returns NULL if a stream can't be created.  */
extern FILE *__newstream __P ((void));

#ifdef	__USE_POSIX
/* Create a new stream that refers to an existing system file descriptor.  */
extern FILE *__fdopen __P ((int __fd, __const char *__modes));
extern FILE *fdopen __P ((int __fd, __const char *__modes));
#endif

#ifdef	__USE_GNU
/* Create a new stream that refers to the given magic cookie,
   and uses the given functions for input and output.  */
extern FILE *fopencookie __P ((__ptr_t __magic_cookie, __const char *__modes,
			       __io_functions __io_funcs));

/* Create a new stream that refers to a memory buffer.  */
extern FILE *fmemopen __P ((__ptr_t __s, size_t __len, __const char *__modes));

/* Open a stream that writes into a malloc'd buffer that is expanded as
   necessary.  *BUFLOC and *SIZELOC are updated with the buffer's location
   and the number of characters written on fflush or fclose.  */
extern FILE *open_memstream __P ((char **__bufloc, size_t *__sizeloc));
#endif


/* If BUF is NULL, make STREAM unbuffered.
   Else make it use buffer BUF, of size BUFSIZ.  */
extern void setbuf __P ((FILE *__restrict __stream, char *__restrict __buf));
/* Make STREAM use buffering mode MODE.
   If BUF is not NULL, use N bytes of it for buffering;
   else allocate an internal buffer N bytes long.  */
extern int setvbuf __P ((FILE *__restrict __stream, char *__restrict __buf,
			 int __modes, size_t __n));

#ifdef	__USE_BSD
/* If BUF is NULL, make STREAM unbuffered.
   Else make it use SIZE bytes of BUF for buffering.  */
extern void setbuffer __P ((FILE *__stream, char *__buf, size_t __size));

/* Make STREAM line-buffered.  */
extern void setlinebuf __P ((FILE *__stream));
#endif


/* Write formatted output to STREAM.  */
extern int fprintf __P ((FILE *__restrict __stream,
			 __const char *__restrict __format, ...))
     __attribute__ ((__format__ (__printf__, 2, 3)));
/* Write formatted output to stdout.  */
extern int printf __P ((__const char *__restrict __format, ...));
/* Write formatted output to S.  */
extern int sprintf __P ((char *__restrict __s,
			 __const char *__restrict __format, ...))
     __attribute__ ((__format__ (__printf__, 2, 3)));

/* Write formatted output to S from argument list ARG.  */
extern int vfprintf __P ((FILE *__restrict __s,
			  __const char *__restrict __format,
			  __va_list __arg))
     __attribute__ ((__format__ (__printf__, 2, 0)));
/* Write formatted output to stdout from argument list ARG.  */
extern int vprintf __P ((__const char *__restrict __format,
			 __va_list __arg))
     __attribute__ ((__format__ (__printf__, 1, 0)));
/* Write formatted output to S from argument list ARG.  */
extern int vsprintf __P ((char *__restrict __s,
			  __const char *__restrict __format,
			  __va_list __arg))
     __attribute__ ((__format__ (__printf__, 2, 0)));

#if defined __USE_BSD || defined __USE_ISOC99
/* Maximum chars of output to write in MAXLEN.  */
extern int snprintf __P ((char *__s, size_t __maxlen,
			  __const char *__format, ...))
     __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf __P ((char *__s, size_t __maxlen,
			   __const char *__format, __va_list __arg))
     __attribute__ ((__format__ (__printf__, 3, 0)));
#endif

#ifdef __USE_GNU
/* Write formatted output to a string dynamically allocated with `malloc'.
   Store the address of the string in *PTR.  */
extern int vasprintf __P ((char **__restrict __ptr,
			   __const char *__restrict __f, __va_list __arg))
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int asprintf __P ((char **__restrict __ptr,
			  __const char *__restrict __fmt, ...))
     __attribute__ ((__format__ (__printf__, 2, 3)));

/* Write formatted output to a file descriptor.  */
extern int vdprintf __P ((int __fd, __const char *__restrict __fmt,
			  __va_list __arg))
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf __P ((int __fd, __const char *__restrict __fmt, ...))
     __attribute__ ((__format__ (__printf__, 2, 3)));
#endif


/* Read formatted input from STREAM.  */
extern int fscanf __P ((FILE *__restrict __stream,
			__const char *__restrict __format, ...))
     __attribute__ ((__format__ (__scanf__, 2, 3)));
/* Read formatted input from stdin.  */
extern int scanf __P ((__const char *__restrict __format, ...));
/* Read formatted input from S.  */
extern int sscanf __P ((__const char *__restrict __s,
			__const char *__restrict __format, ...))
     __attribute__ ((__format__ (__scanf__, 2, 3)));

#if defined(__USE_ISOC99) || defined(__USE_PUREC)
/* Read formatted input from S into argument list ARG.  */
extern int vfscanf __P ((FILE *__s, __const char *__format,
			 __va_list __arg))
     __attribute__ ((__format__ (__scanf__, 2, 0)));

/* Read formatted input from stdin into argument list ARG.  */
extern int vscanf __P ((__const char *__format, __va_list __arg))
     __attribute__ ((__format__ (__scanf__, 1, 0)));

/* Read formatted input from S into argument list ARG.  */
extern int vsscanf __P ((__const char *__s, __const char *__format,
			 __va_list __arg))
     __attribute__ ((__format__ (__scanf__, 2, 0)));

#endif /* Use ISO C9x.  */


/* Read a character from STREAM.  */
extern int fgetc __P ((FILE *__stream));
extern int getc __P ((FILE *__stream));

/* Read a character from stdin.  */
extern int getchar __P ((void));

/* Write a character to STREAM.  */
extern int fputc __P ((int __c, FILE *__stream));
extern int putc __P ((int __c, FILE *__stream));

/* Write a character to stdout.  */
extern int putchar __P ((int __c));


#if defined __USE_SVID || defined __USE_MISC
/* Get a word (int) from STREAM.  */
extern int getw __P ((FILE *__stream));

/* Write a word (int) to STREAM.  */
extern int putw __P ((int __w, FILE *__stream));
#endif


/* Get a newline-terminated string of finite length from STREAM.  */
extern char *fgets __P ((char *__restrict __s, int __n,
			 FILE *__restrict __stream));

#ifdef __USE_GNU
/* This function does the same as `fgets' but does not lock the stream.  */
extern char *fgets_unlocked __P ((char *__restrict __s, int __n,
				  FILE *__restrict __stream));
#endif

/* Get a newline-terminated string from stdin, removing the newline.
   DO NOT USE THIS FUNCTION!!  There is no limit on how much it will read.  */
extern char *gets __P ((char *__s));


#ifdef	__USE_GNU
#include <sys/types.h>

/* Read up to (and including) a DELIMITER from STREAM into *LINEPTR
   (and null-terminate it). *LINEPTR is a pointer returned from malloc (or
   NULL), pointing to *N characters of space.  It is realloc'd as
   necessary.  Returns the number of characters read (not including the
   null terminator), or -1 on error or EOF.  */
ssize_t __getdelim __P ((char **__lineptr, size_t *__n,
			 int __delimiter, FILE *__stream));
ssize_t getdelim __P ((char **__lineptr, size_t *__n,
		       int __delimiter, FILE *__stream));

/* Like `getdelim', but reads up to a newline.  */
ssize_t __getline __P ((char **__lineptr, size_t *__n, FILE *__stream));
ssize_t getline __P ((char **__lineptr, size_t *__n, FILE *__stream));

#ifdef __USE_EXTERN_INLINES
__extern_inline ssize_t
getline (char **__lineptr, size_t *__n, FILE *__stream)
{
  return __getdelim (__lineptr, __n, '\n', __stream);
}
#endif /* Optimizing.  */
#endif


/* Write a string to STREAM.  */
extern int fputs __P ((__const char *__restrict __s,
		       FILE *__restrict __stream));

#ifdef __USE_GNU
/* This function does the same as `fputs' but does not lock the stream.  */
extern int fputs_unlocked __P ((__const char *__restrict __s,
				FILE *__restrict __stream));
#endif

/* Write a string, followed by a newline, to stdout.  */
extern int puts __P ((__const char *__s));


/* Push a character back onto the input buffer of STREAM.  */
extern int ungetc __P ((int __c, FILE *__stream));


/* Read chunks of generic data from STREAM.  */
extern size_t fread __P ((__ptr_t __restrict __ptr, size_t __size,
			  size_t __n, FILE *__restrict __stream));
/* Write chunks of generic data to STREAM.  */
extern size_t fwrite __P ((__const __ptr_t __restrict __ptr, size_t __size,
			   size_t __n, FILE *__restrict __s));

#ifdef __USE_MISC
/* Faster versions when locking is not necessary.  */
extern size_t fread_unlocked __P ((void *__restrict __ptr, size_t __size,
				   size_t __n, FILE *__restrict __stream));
extern size_t fwrite_unlocked __P ((__const void *__restrict __ptr,
				    size_t __size, size_t __n,
				    FILE *__restrict __stream));
#endif

/* Seek to a certain position on STREAM.  */
extern int fseek __P ((FILE *__stream, long int __off, int __whence));
/* Return the current position of STREAM.  */
extern long int ftell __P ((FILE *__stream));
/* Rewind to the beginning of STREAM.  */
extern void rewind __P ((FILE *__stream));

/* The Single Unix Specification, Version 2, specifies an alternative,
   more adequate interface for the two functions above which deal with
   file offset.  `long int' is not the right type.  These definitions
   are originally defined in the Large File Support API.  */

#if defined __USE_LARGEFILE || defined __USE_XOPEN2K
# ifndef __USE_FILE_OFFSET64
/* Seek to a certain position on STREAM.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int fseeko __P ((FILE *__stream, __off_t __off, int __whence));
/* Return the current position of STREAM.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern __off_t ftello __P ((FILE *__stream));
# endif
#endif

/* Get STREAM's position.  */
extern int fgetpos __P ((FILE *__restrict __stream, fpos_t *__restrict __pos));
/* Set STREAM's position.  */
extern int fsetpos __P ((FILE *__stream, __const fpos_t *__pos));


/* Clear the error and EOF indicators for STREAM.  */
extern void clearerr __P ((FILE *__stream));
/* Return the EOF indicator for STREAM.  */
extern int feof __P ((FILE *__stream));
/* Return the error indicator for STREAM.  */
extern int ferror __P ((FILE *__stream));

#ifdef __USE_MISC
/* Faster versions when locking is not required.  */
extern void clearerr_unlocked __P ((FILE *__stream));
extern int feof_unlocked __P ((FILE *__stream));
extern int ferror_unlocked __P ((FILE *__stream));
#endif

/* Print a message describing the meaning of the value of errno.  */
extern void perror __P ((__const char *__s));


#if defined(__USE_POSIX) || defined(__USE_PUREC)
/* Return the system file descriptor for STREAM.  */
extern int fileno __P ((FILE *__stream));
#endif /* Use POSIX.  */

#ifdef __USE_MISC
/* Faster version when locking is not required.  */
extern int fileno_unlocked __P ((FILE *__stream));
#endif


#if (defined __USE_POSIX2 || defined __USE_SVID || defined __USE_BSD || defined __USE_MISC) || defined(__USE_PUREC)
/* Create a new stream connected to a pipe running the given command.  */
extern FILE *popen __P ((__const char *__command, __const char *__modes));

/* Close a stream opened by popen and return the status of its child.  */
extern int pclose __P ((FILE *__stream));
#endif


#ifdef	__USE_POSIX
/* Return the name of the controlling terminal.  */
extern char *ctermid __P ((char *__s));
#endif


#ifdef __USE_XOPEN
/* Return the name of the current user.  */
extern char *cuserid __P ((char *__s));
#endif


#ifdef	__USE_GNU
#include <obstack.h>

/* Open a stream that writes to OBSTACK.  */
extern FILE *open_obstack_stream __P ((struct obstack *__obstack));

/* Write formatted output to an obstack.  */
extern int obstack_printf __P ((struct obstack *__obstack,
				__const char *__format, ...))
     __attribute__ ((__format__ (__printf__, 2, 3)));
extern int obstack_vprintf __P ((struct obstack *__obstack,
				 __const char *__format,
				 __va_list __args))
     __attribute__ ((__format__ (__printf__, 2, 0)));
#endif


#if defined __USE_POSIX || defined __USE_MISC
/* These are defined in POSIX.1:1996.  */

/* Acquire ownership of STREAM.  */
extern void flockfile __P ((FILE *__stream));

/* Try to acquire ownership of STREAM but do not block if it is not
   possible.  */
extern int ftrylockfile __P ((FILE *__stream));

/* Relinquish the ownership granted for STREAM.  */
extern void funlockfile __P ((FILE *__stream));
#endif /* POSIX || misc */

#if defined __USE_XOPEN && !defined __USE_GNU
/* The X/Open standard requires some functions and variables to be
   declared here which do not belong into this header.  But we have to
   follow.  In GNU mode we don't do this nonsense.  */
# define __need_getopt
# include <getopt.h>
#endif

/* Print out MESSAGE on stderr and abort.  */
extern void __libc_fatal __P ((const char* __message));

__END_DECLS

#ifndef _PUREC_SOURCE
#include <bits/stdio.h>
#endif

#if defined(__MINT__)
# include <bits/mint_stdio.h>
#endif

#endif /* <stdio.h> included.  */

#ifdef _PUREC_SOURCE
#include <purec/stdio.h>
#endif

#endif /* stdio.h  */
