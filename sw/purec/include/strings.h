/* Copyright (C) 1991, 1992, 1996, 1997 Free Software Foundation, Inc.
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

#ifndef	_STRINGS_H
#define	_STRINGS_H	1

#ifndef	_FEATURES_H
# include <features.h>
#endif

#define __need_size_t
#include <stddef.h>

__BEGIN_DECLS

/* Compare N bytes of S1 and S2 (same as memcmp).  */
__EXTERN int bcmp __P ((const void* __s1, const void* __s2, size_t __n));

/* Copy N bytes of SRC to DEST (like memmove, but args reversed).  */
__EXTERN void bcopy __P ((const void* __src, void* __dest, size_t __n));

/* Set N bytes of S to 0.  */
__EXTERN void bzero __P ((void* __s, size_t __n));

/* Return the position of the first bit set in I, or 0 if none are set.
   The least-significant bit is position 1, the most-significant 32.  */
__EXTERN int ffs __P ((int __i));

#if 0 /* already in string.h */
/* Find the first occurrence of C in S (same as strchr).  */
__EXTERN char *index __P ((const char *__s, int __c));

/* Find the last occurrence of C in S (same as strrchr).  */
__EXTERN char *rindex __P ((const char *__s, int __c));
#endif

/* Compare S1 and S2, ignoring case.  */
__EXTERN int strcasecmp __P ((const char *__s1, const char *__s2));

/* Compare no more than N chars of S1 and S2, ignoring case.  */
__EXTERN int strncasecmp __P ((const char *__s1, const char *__s2,
			       size_t __n));

__END_DECLS

#endif	/* strings.h  */
