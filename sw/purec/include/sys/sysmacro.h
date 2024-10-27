/* Definitions of macros to access `dev_t' values.
   Copyright (C) 1996, 1997, 1999 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

/* Modified for MiNTLib by Guido Flohr <guido@freemint.de>.  */

#ifndef _SYS_SYSMACROS_H
#define _SYS_SYSMACROS_H	1

/* For compatibility we provide alternative names.

   The problem here is that compilers other than GCC probably don't
   have the `long long' type and so `dev_t' is actually an array.  */
#define major(dev) ((unsigned int)((__quad_low(dev) >> 8) & 0xff))
#define minor(dev) ((unsigned int)(__quad_low(dev) & 0xff))
#ifndef __NO_LONGLONG
# define makedev(major, minor) ((((unsigned long int) (major)) << 8) \
				| ((unsigned long int) (minor)))
#else
/* We need to know the word order here.  This assumes that the word order
   is consistent with the byte order.  */
# include <endian.h>
#  define makedev(major, minor) { __LONG_LONG_PAIR(0, ((((unsigned int) ((major) & 0xff)) << 8) \
				      | ((unsigned int) ((minor) & 0xff)))) }
#endif

#endif /* sys/sysmacros.h */
