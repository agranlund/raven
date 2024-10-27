/* siginfo_t, sigevent and constants.  Stub version.
   Copyright (C) 1997-2013 Free Software Foundation, Inc.
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

#if !defined _SIGNAL_H && !defined __need_siginfo_t \
    && !defined __need_sigevent_t
# error "Never include this file directly.  Use <signal.h> instead"
#endif

#if (!defined __have_sigval_t \
     && (defined _SIGNAL_H || defined __need_siginfo_t \
	 || defined __need_sigevent_t))
# define __have_sigval_t 1

/* Type for data associated with a signal.  */
typedef union sigval
  {
    int sival_int;
    void *sival_ptr;
  } sigval_t;
#endif

#if (!defined __have_siginfo_t \
     && (defined _SIGNAL_H || defined __need_siginfo_t))
# define __have_siginfo_t	1

typedef struct siginfo
  {
    int si_signo;		/* Signal number.  */
    int si_errno;		/* If non-zero, an errno value associated with
				   this signal, as defined in <errno.h>.  */
    int si_code;		/* Signal code.  */
    __pid_t si_pid;		/* Sending process ID.  */
    __uid_t si_uid;		/* Real user ID of sending process.  */
    void *si_addr;		/* Address of faulting instruction.  */
    int si_status;		/* Exit value or signal.  */
    long int si_band;		/* Band event for SIGPOLL.  */
    union sigval si_value;	/* Signal value.  */
  } siginfo_t;


#ifndef SI_FROMUSER
# define SI_FROMUSER(sip)	((sip)->si_code <= 0)
#endif

/* Values for `si_code'.  Positive values are reserved for kernel-generated
   signals.  */
enum
{
  SI_KERNEL = 0x80,	/* sent by kernel */
#define SI_KERNEL SI_KERNEL
  SI_ASYNCNL = -60,		/* Sent by asynch name lookup completion.  */
#define SI_ASYNCNL SI_ASYNCNL
  SI_TKILL = -6,		/* sent by tkill. */
# define SI_TKILL SI_TKILL
  SI_SIGIO = -5,		/* sent by SIGIO */
# define SI_SIGIO SI_SIGIO
  SI_ASYNCIO = -4,		/* Sent by AIO completion.  */
# define SI_ASYNCIO	SI_ASYNCIO
  SI_MESGQ,			/* Sent by real time mesq state change.  */
# define SI_MESGQ	SI_MESGQ
  SI_TIMER,			/* Sent by timer expiration.  */
# define SI_TIMER	SI_TIMER
  SI_QUEUE,			/* Sent by sigqueue.  */
# define SI_QUEUE	SI_QUEUE
  SI_USER			/* Sent by kill, sigsend, raise.  */
# define SI_USER	SI_USER
};


# if defined __USE_XOPEN_EXTENDED || defined __USE_XOPEN2K8
/* `si_code' values for SIGILL signal.  */
enum
{
  ILL_ILLOPC = 1,		/* Illegal opcode.  */
#  define ILL_ILLOPC	ILL_ILLOPC
  ILL_ILLOPN,			/* Illegal operand.  */
#  define ILL_ILLOPN	ILL_ILLOPN
  ILL_ILLADR,			/* Illegal addressing mode.  */
#  define ILL_ILLADR	ILL_ILLADR
  ILL_ILLTRP,			/* Illegal trap. */
#  define ILL_ILLTRP	ILL_ILLTRP
  ILL_PRVOPC,			/* Privileged opcode.  */
#  define ILL_PRVOPC	ILL_PRVOPC
  ILL_PRVREG,			/* Privileged register.  */
#  define ILL_PRVREG	ILL_PRVREG
  ILL_COPROC,			/* Coprocessor error.  */
#  define ILL_COPROC	ILL_COPROC
  ILL_BADSTK			/* Internal stack error.  */
#  define ILL_BADSTK	ILL_BADSTK
};

/* `si_code' values for SIGFPE signal.  */
enum
{
  FPE_INTDIV = 1,		/* Integer divide by zero.  */
#  define FPE_INTDIV	FPE_INTDIV
  FPE_INTOVF,			/* Integer overflow.  */
#  define FPE_INTOVF	FPE_INTOVF
  FPE_FLTDIV,			/* Floating point divide by zero.  */
#  define FPE_FLTDIV	FPE_FLTDIV
  FPE_FLTOVF,			/* Floating point overflow.  */
#  define FPE_FLTOVF	FPE_FLTOVF
  FPE_FLTUND,			/* Floating point underflow.  */
#  define FPE_FLTUND	FPE_FLTUND
  FPE_FLTRES,			/* Floating point inexact result.  */
#  define FPE_FLTRES	FPE_FLTRES
  FPE_FLTINV,			/* Floating point invalid operation.  */
#  define FPE_FLTINV	FPE_FLTINV
  FPE_FLTSUB			/* Subscript out of range.  */
#  define FPE_FLTSUB	FPE_FLTSUB
};

/* `si_code' values for SIGSEGV signal.  */
enum
{
  SEGV_MAPERR = 1,		/* Address not mapped to object.  */
#  define SEGV_MAPERR	SEGV_MAPERR
  SEGV_ACCERR,			/* Invalid permissions for mapped object.  */
#  define SEGV_ACCERR	SEGV_ACCERR
  SEGV_BNDERR,			/* Bounds checking failure.  */
#  define SEGV_BNDERR	SEGV_BNDERR
  SEGV_PKUERR			/* Protection key checking failure.  */
#  define SEGV_PKUERR	SEGV_PKUERR
};

/* `si_code' values for SIGBUS signal.  */
enum
{
  BUS_ADRALN = 1,		/* Invalid address alignment.  */
#  define BUS_ADRALN	BUS_ADRALN
  BUS_ADRERR,			/* Non-existant physical address.  */
#  define BUS_ADRERR	BUS_ADRERR
  BUS_OBJERR,			/* Object specific hardware error.  */
#  define BUS_OBJERR	BUS_OBJERR
  BUS_MCEERR_AR,		/* Hardware memory error: action required.  */
#  define BUS_MCEERR_AR	BUS_MCEERR_AR
  BUS_MCEERR_AO			/* Hardware memory error: action optional.  */
#  define BUS_MCEERR_AO	BUS_MCEERR_AO
};
# endif

# ifdef __USE_XOPEN_EXTENDED
/* `si_code' values for SIGTRAP signal.  */
enum
{
  TRAP_BRKPT = 1,		/* Process breakpoint.  */
#  define TRAP_BRKPT	TRAP_BRKPT
  TRAP_TRACE			/* Process trace trap.  */
#  define TRAP_TRACE	TRAP_TRACE
};
# endif

# if defined __USE_XOPEN_EXTENDED || defined __USE_XOPEN2K8
/* `si_code' values for SIGCHLD signal.  */
enum
{
  CLD_EXITED = 1,		/* Child has exited.  */
#  define CLD_EXITED	CLD_EXITED
  CLD_KILLED,			/* Child was killed.  */
#  define CLD_KILLED	CLD_KILLED
  CLD_DUMPED,			/* Child terminated abnormally.  */
#  define CLD_DUMPED	CLD_DUMPED
  CLD_TRAPPED,			/* Traced child has trapped.  */
#  define CLD_TRAPPED	CLD_TRAPPED
  CLD_STOPPED,			/* Child has stopped.  */
#  define CLD_STOPPED	CLD_STOPPED
  CLD_CONTINUED			/* Stopped child has continued.  */
#  define CLD_CONTINUED	CLD_CONTINUED
};

/* `si_code' values for SIGPOLL signal.  */
enum
{
  POLL_IN = 1,			/* Data input available.  */
#  define POLL_IN	POLL_IN
  POLL_OUT,			/* Output buffers available.  */
#  define POLL_OUT	POLL_OUT
  POLL_MSG,			/* Input message available.   */
#  define POLL_MSG	POLL_MSG
  POLL_ERR,			/* I/O error.  */
#  define POLL_ERR	POLL_ERR
  POLL_PRI,			/* High priority input available.  */
#  define POLL_PRI	POLL_PRI
  POLL_HUP			/* Device disconnected.  */
#  define POLL_HUP	POLL_HUP
};
# endif

# undef __need_siginfo_t
#endif	/* !have siginfo_t && (have _SIGNAL_H || need siginfo_t).  */


#if (defined _SIGNAL_H || defined __need_sigevent_t) \
    && !defined __have_sigevent_t
# define __have_sigevent_t	1

/* Structure to transport application-defined values with signals.  */
# define SIGEV_MAX_SIZE	64
# define SIGEV_PAD_SIZE	((SIGEV_MAX_SIZE / sizeof (int)) - 3)

/* Forward declaration.  */
#ifndef __have_pthread_attr_t
# define __have_pthread_attr_t	1
typedef struct __pthread_attr pthread_attr_t;
#if (defined(__PUREC__) || defined(__TURBOC__)) && !defined(_BITS_PTHREADTYPES_H)
struct __pthread_attr { int dummy; };
#endif
#endif

typedef struct sigevent
  {
    sigval_t sigev_value;
    int sigev_signo;
    int sigev_notify;
    void (*sigev_notify_function) (sigval_t);	    /* Function to start.  */
    pthread_attr_t *sigev_notify_attributes;		/* Thread attributes.  */
  } sigevent_t;

/* `sigev_notify' values.  */
enum
{
  SIGEV_SIGNAL = 0,		/* Notify via signal.  */
# define SIGEV_SIGNAL	SIGEV_SIGNAL
  SIGEV_NONE,			/* Other notification: meaningless.  */
# define SIGEV_NONE	SIGEV_NONE
  SIGEV_THREAD,			/* Deliver via thread creation.  */
# define SIGEV_THREAD	SIGEV_THREAD

#ifndef __MINT__
  SIGEV_THREAD_ID = 4		/* Send signal to specific thread.
				   This is a Linux extension.  */
#define SIGEV_THREAD_ID	SIGEV_THREAD_ID
#endif
};

#endif	/* have _SIGNAL_H.  */
