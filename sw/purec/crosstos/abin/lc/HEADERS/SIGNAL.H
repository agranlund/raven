/**
 * $Id: signal.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * ANSI/POSIX.1 macros and functions for signal handling
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _SIGNAL_H
#define _SIGNAL_H

#define SIGINT	2	/* sent by ^C */
#define SIGILL	4	/* illegal instruction */
#define SIGABRT	6	/* abort signal */
#define SIGFPE	8	/* divide by zero */
#define SIGSEGV	11	/* illegal memory reference */
#define SIGTERM	15	/* software termination signal */

typedef void (*__sigfunc_t)(int);

#define SIG_DFL	((__sigfunc_t)0)	/* default action */
#define SIG_IGN	((__sigfunc_t)1)	/* ignore the signal */
#define SIG_ERR	((__sigfunc_t)-1)	/* error return */

__sigfunc_t signal(int, __sigfunc_t);
int raise(int);

typedef long sig_atomic_t;

#if !defined(_ANSI) \
	|| defined(_POSIX_SOURCE) \
	|| defined(_BSD)
#define SIGNULL	0		/* not really a signal */
#define SIGHUP	1		/* hangup signal */
#define SIGQUIT	3		/* quit signal */
#define SIGTRAP	5		/* trace trap */
#define SIGPRIV	7		/* privilege violation */
#define SIGKILL	9		/* cannot be ignored */
#define SIGBUS	10		/* bus error */
#define SIGSYS	12		/* bad argument to a system call */
#define SIGPIPE	13		/* broken pipe */
#define SIGALRM	14		/* alarm clock */
#define SIGURG	16		/* urgent condition on I/O channel */
#define SIGSTOP	17		/* stop signal not from terminal */
#define SIGTSTP	18		/* stop signal from terminal */
#define SIGCONT	19		/* continue stopped process */
#define SIGCHLD	20		/* child stopped or exited */
#define SIGTTIN	21		/* read by background process */
#define SIGTTOU	22		/* write by background process */
#define SIGIO	23		/* I/O possible on a descriptor */
#define SIGXCPU	24		/* CPU time exhausted */
#define SIGXFSZ	25		/* file size limited exceeded */
#define SIGVTALRM	26	/* virtual timer alarm */
#define SIGPROF	27		/* profiling timer expired */
#define SIGWINCH	28	/* window size changed */
#define SIGUSR1	29		/* user signal 1 */
#define SIGUSR2	30		/* user signal 2 */

#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
#define NSIG	31	/* number of signals recognised */
#endif

#ifndef __TYPES_H
#include <__types.h>
#endif

int kill(__pid_t, int);

typedef __sigset_t sigset_t;

struct sigaction {
	__sigfunc_t sa_handler;
	sigset_t sa_mask;
#define SA_NOCLDSTOP	1
	int sa_flags;
};

int sigaddset(sigset_t *, int);
#define sigaddset(s, n)	((*(s) |= (sigset_t)1 << (n)), 0)
int sigdelset(sigset_t *, int);
#define sigdelset(s, n)	((*(s) &= ~((sigset_t)1 << (n))), 0)
int sigemptyset(sigset_t *);
#define sigemptyset(s, n)	((*(s) = 0), 0)
int sigfillset(sigset_t *);
#define sigfillset(s, n)	((*(s) = ~(sigset_t)0), 0)
int sigismember(const sigset_t *,int);
#define sigismember(s, n)	(!!(*(s) & ((sigset_t)1 << (n))))

int sigaction(int, const struct sigaction *, struct sigaction *);
int sigpending(sigset_t *);
#define SIG_BLOCK	0
#define SIG_UNBLOCK	1
#define SIG_SETMASK	2
int sigprocmask(int, const sigset_t *, sigset_t *);
int sigsuspend(const sigset_t *);

#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
int sigpause(long);
long sigsetmask(long);
long sigblock(long);
int killpg(int,int);
#endif
#endif
#endif
