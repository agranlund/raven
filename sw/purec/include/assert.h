/* Copyright (C) 1991, 92, 94, 95, 96, 97, 98 Free Software Foundation, Inc.
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

/* Modified for MiNTLib by Guido Flohr <guido@freemint.de>.  */

/*
 *	ISO C Standard: 4.2 DIAGNOSTICS	<assert.h>
 */

#ifdef	_ASSERT_H

# undef	_ASSERT_H
# undef	assert

# ifdef	__USE_GNU
#  undef assert_perror
# endif

#endif /* assert.h	*/

#define	_ASSERT_H	1

#ifndef	_FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_CDEFS_H
# include <sys/cdefs.h>
#endif

/* void assert (int expression);

   If NDEBUG is defined, do nothing.
   If not, and EXPRESSION is zero, print an error message and abort.  */

#ifdef	NDEBUG

# define assert(expr)		((void) 0)

/* void assert_perror (int errnum);

   If NDEBUG is defined, do nothing.  If not, and ERRNUM is not zero, print an
   error message with the error text for ERRNUM and abort.
   (This is a GNU extension.) */

# ifdef	__USE_GNU
#  define assert_perror(errnum)	((void) 0)
# endif

#else /* Not NDEBUG.  */

__BEGIN_DECLS

/* This prints an "Assertion failed" message and aborts.  */
extern void __assert_fail (const char *__assertion,
			   const char *__file,
			   unsigned int __line,
			   const char *__function)
     __attribute__ ((__noreturn__));

/* Likewise, but prints the error text for ERRNUM.  */
extern void __assert_perror_fail (int __errnum,
				  const char *__file,
				  unsigned int __line,
				  const char *__function)
     __attribute__ ((__noreturn__));

__END_DECLS

#if defined(_PUREC_SOURCE) && 0

#define __need_assert
#include <purec/assert.h>

#else

# define assert(expr)							      \
  ((void) ((expr) ? 0 :							      \
	   (__assert_fail (__STRING(expr),				      \
			   __FILE__, __LINE__, __ASSERT_FUNCTION), 0)))

#endif

# ifdef	__USE_GNU
#  define assert_perror(errnum)						      \
  ((void) (!(errnum) ? 0 : (__assert_perror_fail ((errnum),		      \
						  __FILE__, __LINE__,	      \
						  __ASSERT_FUNCTION), 0)))
# endif

/* Version 2.4 and later of GCC define a magical variable `__PRETTY_FUNCTION__'
   which contains the name of the function currently being defined.
   This is broken in G++ before version 2.6.  */
# if !(defined __cplusplus ? __GNUC_PREREQ(2, 6) : __GNUC_PREREQ(2, 4))
#  define __ASSERT_FUNCTION	((const char *) 0)
# else
#  define __ASSERT_FUNCTION	__PRETTY_FUNCTION__
# endif


#endif /* NDEBUG.  */
