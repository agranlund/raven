/*  errno.h -- MiNTLib.
    Copyright (C) 1999 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

/*
 * ISO C Standard: 4.1.3 Errors <errno.h>
 */

#if !defined _ERRNO_H
# define _ERRNO_H 1         /* Allow multiple inclusion.  */

#ifndef	_FEATURES_H
# include <features.h>
#endif

/* Share error codes with the kernel.  */
#ifndef _MINT_ERRNO_H
# include <mint/errno.h>
#endif

#ifndef __KERNEL__

# ifndef __ASSEMBLER__

__BEGIN_DECLS

#ifdef errno
# undef errno
#endif

/* Within the library you should never assign errno directly.  Use
   this macro instead.  Future thread-safe implementations of the
   MiNTLib may require this.  */
#define __set_errno(e) (errno = (e))

extern int errno;

# if defined(__USE_GNU)

/* The full and simple forms of the name with which the program was
   invoked.  These variables are set up automatically at startup based on
   the value of ARGV[0] (this doesn't work when invoked from the desktop).  */
extern char *program_invocation_name;
extern char *program_invocation_short_name;
# endif /* not __USE_GNU */

__END_DECLS

#endif /* not __ASSEMBLER__ */

#endif /* not __KERNEL__ */

#endif /* _ERRNO_H */

#ifndef __ASSEMBLER__
/* An enumeration would be much cooler than an int because then the
   debugger could produce names instead of values when printing error
   types.  */
# if defined __USE_GNU || defined __need_error_t
#  ifndef __error_t_defined
typedef int error_t;
#   define __error_t_defined	1
#  endif
#  undef __need_error_t
# endif
#endif
