/* The proper definitions for FreeMiNT's sigaction.
   Copyright (C) 1993-1999, 2000 Free Software Foundation, Inc.
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

/* Modified for MiNTLib by Frank Naumann <fnaumann@freemint.de>.  */

#ifndef _SIGNAL_H
# error "Never include <bits/sigaction.h> directly; use <signal.h> instead."
#endif

/* values for "how" parameter to sigprocmask() */
#define SIG_BLOCK	0
#define SIG_UNBLOCK	1
#define SIG_SETMASK	2

#define SA_NOCLDSTOP   0x0001       /* don't send SIGCHLD when they stop */
#define SA_NOCLDWAIT   0x0002		/* do not generate zombies on unwaited child */
#define SA_NODEFER     0x0010		/* don't mask the signal we're delivering */
#define SA_SIGINFO     0x0040
#define SA_ONSTACK     0x2000		/* take signal on signal stack */
#define SA_RESTART     0x4000		/* restart system on signal return */
#define SA_RESETHAND   0x8000       /* Reset to SIG_DFL on entry to handler. */

/*
 * SA_ONESHOT and SA_NOMASK are the historical Linux names for the Single
 * Unix names RESETHAND and NODEFER respectively.
 */
#define SA_NOMASK	SA_NODEFER
#define SA_ONESHOT	SA_RESETHAND

struct sigaction {
	__sighandler_t 	sa_handler;	/* Pointer to signal handler.  */
	__sigset_t	sa_mask;	/* Additional signals masked during
					   delivery.  */
	int		sa_flags;	/* Signal specific flags.  */
};
