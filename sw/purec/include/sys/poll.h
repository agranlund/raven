/* Compatibility definitions for System V `poll' interface.
   Copyright (C) 1994, 1996, 1997, 1998 Free Software Foundation, Inc.
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

#ifndef	_SYS_POLL_H
# define _SYS_POLL_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

/* Get the platform dependent bits of `poll'.  */
#include <bits/poll.h>

/* Get sizes.  */
#ifndef _BITS_TYPES_H
# include <bits/types.h>
#endif

/* Type used for the number of file descriptors.  */
typedef unsigned long int nfds_t;

/* Data structure describing a polling request.  */
struct pollfd
  {
    __int32_t fd;		/* File descriptor to poll.  */
    short int events;		/* Types of events poller cares about.  */
    short int revents;		/* Types of events that actually occurred.  */
  };


/* Poll the file descriptors described by the NFDS structures starting at
   FDS.  If TIMEOUT is nonzero and not -1, allow TIMEOUT milliseconds for
   an event to occur; if TIMEOUT is -1, block until an event occurs.
   Returns the number of file descriptors with events, zero if timed out,
   or -1 for errors.  */
extern int poll (struct pollfd *__fds, nfds_t __nfds,
		 __int32_t __timeout) __THROW;
extern int __poll (struct pollfd *__fds, nfds_t __nfds,
		   __int32_t __timeout) __THROW;

__END_DECLS

#endif	/* sys/poll.h */
