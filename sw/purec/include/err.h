/* 4.4BSD utility functions for error messages.
   Copyright (C) 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
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

#ifndef	_ERR_H
#define	_ERR_H	1

#ifndef	_FEATURES_H
# include <features.h>
#endif

#define	__need___va_list
#include <stdarg.h>
#ifndef __va_list
#ifdef __GNUC__
typedef __gnuc_va_list __va_list;
#define __va_list __va_list
#endif
#endif

__BEGIN_DECLS

/* Print "program: ", FORMAT, ": ", the standard error string for errno,
   and a newline, on stderr.  */
__EXTERN void warn __P ((__const char *__format, ...))
     __attribute__ ((__format__ (__printf__, 1, 2)));
__EXTERN void vwarn __P ((__const char *__format, __va_list))
     __attribute__ ((__format__ (__printf__, 1, 0)));

/* Likewise, but without ": " and the standard error string.  */
__EXTERN void warnx __P ((__const char *__format, ...))
     __attribute__ ((__format__ (__printf__, 1, 2)));
__EXTERN void vwarnx __P ((__const char *__format, __va_list))
     __attribute__ ((__format__ (__printf__, 1, 0)));

/* Likewise, and then exit with STATUS.  */
__EXTERN void err __P ((int __status, __const char *__format, ...))
     __attribute__ ((__noreturn__, __format__ (__printf__, 2, 3)));
__EXTERN void verr __P ((int __status, __const char *__format, __va_list))
     __attribute__ ((__noreturn__, __format__ (__printf__, 2, 0)));
__EXTERN void errx __P ((int __status, __const char *__format, ...))
     __attribute__ ((__noreturn__, __format__ (__printf__, 2, 3)));
__EXTERN void verrx __P ((int __status, __const char *, __va_list))
     __attribute__ ((__noreturn__, __format__ (__printf__, 2, 0)));

__END_DECLS

#endif	/* err.h */
