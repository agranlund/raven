/*  bits/mint_stdio.h -- MiNTLib.
    Copyright (C) 2000 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef _BITS_MINT_STDIO_H

#ifndef _STDIO_H
# error "Never include <bits/mint_stdio.h> directly; use <stdio.h> instead."
#endif

/* One of the most annoying inheritances of GEMDOS from MS-DOS was the
   convention that so-called text streams should automatically map
   newline characters to a sequence of carriage return plus newline
   on output and the same sequence back to a single newline on input.
   For compatibility's sake the MiNTLib still supports this stupid
   convention as it's default behavior for streams.
   
   Normally you don't have to worry about all that.  If the user has
   set the environment variable "UNIXMODE" to a value that contains
   the character `b' (not preceded by an `r'!) the MiNTLib will behave
   the standard way, i. e. no translation is performed, all streams
   are "binary" by default.  NOTE: For security reasons this has
   no effect with setuid programs.  These programs will open all
   streams in "binary" mode by default.
   
   You can override the setting in the environment by an explicit 
   call to the function __binmode (see below for a description).
   Calling __binmode is equivalent to setting member "__binary"
   of the internal variable "__default_mode__" but the function
   call is more portable.  All subsequents fopen calls will obey the
   default mode, streams that are already opened (notably the
   standard streams stdin, stdout, and stderr) are not affected.
   
   If you want to control the behavior of one distinct stream you
   should add the qualifier "b" to the mode argument, for example:
   
   	FILE* stream = fopen ("foobar", "rb");
   
   This will ensure that no translation takes place on STREAM.  There
   is no flag with an opposite effect.
   
   In former versions of the MiNTLib you could also change the behavior
   of streams by or'ing the member _flag of the FILE structure with
   the constant _IOBIN (0x4).  Object files that contain such code
   will make the library crash!  One important exception: This will
   still work for the standard streams stdin, stdout, and stderr.
   
   If you can't help to change the behavior of an already opened stream
   you can use the new and completely unportable function __set_binmode
   (see below).  Please consider the behavior of the stream undefined
   until the next call to fflush.  And please also consider the member
   __binary of the __io_mode member of FILE read-only!  Changing the
   flag has no effect!
   
   Of course all that binary/text stuff only applies to streams associated
   to a file and not to memstreams, obstreams or the like.  */
   
/* If you want a certain default mode for your FILEs you can define
 * this variable in your sources (only the member __binary gets
 * evaluated).
 */
extern __io_mode __default_mode__;

#ifdef	__USE_SVID
/* This will be sent to the OS but P_tmpdir will get used if __mint
   is set.  */
#define __ugly_tos_P_tmpdir "\\usr\\tmp"
#endif

__BEGIN_DECLS

/* Default functions for streams opened in so-called text mode.  */
extern __const __io_functions __default_text_io_functions;

/* Calling __binmode with a non-zero argument will cause the library
   to open all streams in binary mode.  Calling it with a zero argument
   has the opposite effect, i. e. all files are opened in text mode.  */
__EXTERN void __binmode __P ((int));
/* This is the same as __binmode, left here for compatibility reasons.  */
__EXTERN void _binmode __P ((int));

/* Set or unset the binary flag on an open stream.  This has only an effect
   if the stream is associated with a file.  If you use this function on
   a stream open for reading you should first fflush it because the
   stream's buffer contents will be left unchanged by this function.  */
__EXTERN void __set_binmode __P ((FILE* __stream, int __binary));

# ifdef __USE_MINTLIB
__EXTERN FILE*	fopenp	__P ((const char* , const char*));
__EXTERN int 	fungetc	__P ((int, FILE*));
__EXTERN long 	getl	__P ((FILE*));
__EXTERN long 	putl	__P ((long, FILE*));
__EXTERN void	_getbuf	__P ((FILE*));
# endif /* __USE_MINTLIB */

__END_DECLS

#endif /* bits/mint_stdio.h  */

