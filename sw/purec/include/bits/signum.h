/* Signal number definitions.  FreeMiNT version.
   Copyright (C) 1995, 1996, 1997, 1998, 1999 Free Software Foundation, Inc.
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

#ifdef	_SIGNAL_H

#ifndef __mint_sighandler_t_defined
#define __mint_sighandler_t_defined 1
#ifdef __NO_CDECL
typedef void *__mint_sighandler_t;
#else
typedef void _CDECL (*__mint_sighandler_t) (long signum);
#endif
#endif

#define	__MINT_NSIG		32		/* number of signals recognized */

#define	__MINT_SIGNULL		0		/* not really a signal */
#define __MINT_SIGHUP		1		/* hangup signal */
#define __MINT_SIGINT		2		/* sent by ^C */
#define __MINT_SIGQUIT		3		/* quit signal */
#define __MINT_SIGILL		4		/* illegal instruction */
#define __MINT_SIGTRAP		5		/* trace trap */
#define __MINT_SIGABRT		6		/* abort signal */
#define __MINT_SIGPRIV		7		/* privilege violation */
# define SIGEMT SIGPRIV
#define __MINT_SIGFPE		8		/* divide by zero */
#define __MINT_SIGKILL		9		/* cannot be ignored */
#define __MINT_SIGBUS		10		/* bus error */
#define __MINT_SIGSEGV		11		/* illegal memory reference */
#define __MINT_SIGSYS		12		/* bad system call */
#define __MINT_SIGPIPE		13		/* broken pipe */
#define __MINT_SIGALRM		14		/* alarm clock */
#define __MINT_SIGTERM		15		/* software termination signal */

#define __MINT_SIGURG		16		/* urgent condition on I/O channel */
#define __MINT_SIGSTOP		17		/* stop signal not from terminal */
#define __MINT_SIGTSTP		18		/* stop signal from terminal */
#define __MINT_SIGCONT		19		/* continue stopped process */
#define __MINT_SIGCHLD		20		/* child stopped or exited */
#define __MINT_SIGTTIN		21		/* read by background process */
#define __MINT_SIGTTOU		22		/* write by background process */
#define __MINT_SIGIO		23		/* I/O possible on a descriptor */
# define __MINT_SIGPOLL SIGIO
#define __MINT_SIGXCPU		24		/* CPU time exhausted */
#define __MINT_SIGXFSZ		25		/* file size limited exceeded */
#define __MINT_SIGVTALRM	26		/* virtual timer alarm */
#define __MINT_SIGPROF		27		/* profiling timer expired */
#define __MINT_SIGWINCH	28		/* window size changed */
#define __MINT_SIGUSR1		29		/* user signal 1 */
#define __MINT_SIGUSR2		30		/* user signal 2 */
#define __MINT_SIGPWR		31		/* power failure restart (System V) */

#define       __MINT_SIG_DFL	((__mint_sighandler_t) 0L)
#define       __MINT_SIG_IGN	((__mint_sighandler_t) 1L)
#define       __MINT_SIG_ERR	((__mint_sighandler_t)-1L)
#define       __MINT_SIG_SYS	((__mint_sighandler_t)-2L)
#ifdef __USE_UNIX98
# define      __MINT_SIG_HOLD	((__mint_sighandler_t) 2L)	/* Add signal to hold mask.  */
#endif

#if !defined(_PUREC_SOURCE) || defined(__USE_MINT_SIGNAL)

#define __NSIG __MINT_NSIG

#define	SIGNULL __MINT_SIGNULL
#define SIGHUP __MINT_SIGHUP
#define SIGINT __MINT_SIGINT
#define SIGQUIT __MINT_SIGQUIT
#define SIGILL __MINT_SIGILL
#define SIGTRAP __MINT_SIGTRAP
#define SIGABRT __MINT_SIGABRT
#define SIGPRIV __MINT_SIGPRIV
#define SIGEMT SIGPRIV
#define SIGFPE __MINT_SIGFPE
#define SIGKILL __MINT_SIGKILL
#define SIGBUS __MINT_SIGBUS
#define SIGSEGV __MINT_SIGSEGV
#define SIGSYS __MINT_SIGSYS
#define SIGPIPE __MINT_SIGPIPE
#define SIGALRM __MINT_SIGALRM
#define SIGTERM __MINT_SIGTERM

#define SIGURG __MINT_SIGURG
#define SIGSTOP __MINT_SIGSTOP
#define SIGTSTP __MINT_SIGTSTP
#define SIGCONT __MINT_SIGCONT
#define SIGCHLD __MINT_SIGCHLD
#define SIGTTIN __MINT_SIGTTIN
#define SIGTTOU __MINT_SIGTTOU
#define SIGIO __MINT_SIGIO
# define SIGPOLL SIGIO
#define SIGXCPU __MINT_SIGXCPU
#define SIGXFSZ __MINT_SIGXFSZ
#define SIGVTALRM __MINT_SIGVTALRM
#define SIGPROF __MINT_SIGPROF
#define SIGWINCH __MINT_SIGWINCH
#define SIGUSR1 __MINT_SIGUSR1
#define SIGUSR2 __MINT_SIGUSR2
#define SIGPWR __MINT_SIGPWR

#ifndef BADSIG
#define BADSIG		SIG_ERR
#endif


#define       SIG_DFL	((__sighandler_t) 0L)
#define       SIG_IGN	((__sighandler_t) 1L)
#define       SIG_ERR	((__sighandler_t)-1L)
#define       SIG_SYS	((__sighandler_t)-2L)
#ifdef __USE_UNIX98
# define      SIG_HOLD	((__sighandler_t) 2L)	/* Add signal to hold mask.  */
#endif


#ifdef __USE_MISC
# define SignalBad	SIG_ERR
# define SignalDefault	SIG_DFL
# define SignalIgnore	SIG_IGN
#endif

#endif

/* Archaic names for compatibility. */
#define	SIGIOT		SIGABRT	/* IOT instruction, abort() on a PDP11 */
#define	SIGCLD		SIGCHLD	/* Old System V name */

#endif	/* <signal.h> included.  */
