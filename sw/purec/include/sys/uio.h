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
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

/* Adapted to MiNTLib by Guido Flohr <guido@freemint.de>,
   12 Sep 1999.  */
 
#ifndef _SYS_UIO_H
#define _SYS_UIO_H	1

#ifndef _FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_TYPES_H
# include <sys/types.h>
#endif

/* This file defines `struct iovec'.  */
#include <iovec.h>

__BEGIN_DECLS

/* Read data from file descriptor FD, and put the result in the
   buffers described by VECTOR, which is a vector of COUNT `struct iovec's.
   The buffers are filled in the order specified.
   Operates just like `read' (see <unistd.h>) except that data are
   put in VECTOR instead of a contiguous buffer.  */
extern ssize_t readv (int __fd, struct iovec *__vector, ssize_t __count) __THROW;

/* Write data pointed by the buffers described by VECTOR, which
   is a vector of COUNT `struct iovec's, to file descriptor FD.
   The data is written in the order specified.
   Operates just like `write' (see <unistd.h>) except that the data
   are taken from VECTOR instead of a contiguous buffer.  */
extern ssize_t writev (int __fd, const struct iovec *__vector, ssize_t __count) __THROW;


__END_DECLS

#endif /* sys/uio.h */
