/* bits/ipctypes.h -- Define some types used by SysV IPC/MSG/SHM.  Generic.
   Copyright (C) 2002-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/*
 * Never include <bits/ipctypes.h> directly.
 */

#ifndef _BITS_IPCTYPES_H
#define _BITS_IPCTYPES_H	1

#ifndef _BITS_TYPES_H
# include <bits/types.h>
#endif

/* Used in `struct shmid_ds'.  */
#ifndef __ipc_pid_t_defined
typedef __uint16_t __ipc_pid_t;
#define __ipc_pid_t_defined
#endif


#endif /* bits/ipctypes.h */
