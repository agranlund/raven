#ifndef __SYS_SIGINFO_H__
#define __SYS_SIGINFO_H__

#if !defined(_POSIX_SOURCE)
/*
 * negative signal codes are reserved for future use for user generated
 * signals 
 */

#define SI_FROMUSER(sip)	((sip)->si_code <= 0)
#define SI_FROMKERNEL(sip)	((sip)->si_code > 0)

#define SI_USER		0	/* user generated signal */

/* 
 * SIGILL signal codes 
 */

#define	ILL_ILLOPC	1	/* illegal opcode */
#define	ILL_ILLOPN	2	/* illegal operand */
#define	ILL_ILLADR	3	/* illegal addressing mode */
#define	ILL_ILLTRP	4	/* illegal trap */
#define	ILL_PRVOPC	5	/* privileged opcode */
#define	ILL_PRVREG	6	/* privileged register */
#define	ILL_COPROC	7	/* co-processor */
#define	ILL_BADSTK	8	/* bad stack */
#define NSIGILL		8

/* 
 * SIGFPE signal codes 
 */

#define	FPE_INTDIV	1	/* integer divide by zero */
#define	FPE_INTOVF	2	/* integer overflow */
#define	FPE_FLTDIV	3	/* floating point divide by zero */
#define	FPE_FLTOVF	4	/* floating point overflow */
#define	FPE_FLTUND	5	/* floating point underflow */
#define	FPE_FLTRES	6	/* floating point inexact result */
#define	FPE_FLTINV	7	/* invalid floating point operation */
#define FPE_FLTSUB	8	/* subscript out of range */
#define NSIGFPE		8

/* 
 * SIGSEGV signal codes 
 */

#define	SEGV_MAPERR	1	/* address not mapped to object */
#define	SEGV_ACCERR	2	/* invalid permissions */
#define NSIGSEGV	2

/* 
 * SIGBUS signal codes 
 */

#define	BUS_ADRALN	1	/* invalid address alignment */
#define	BUS_ADRERR	2	/* non-existent physical address */
#define	BUS_OBJERR	3	/* object specific hardware error */
#define NSIGBUS		3

/* 
 * SIGTRAP signal codes 
 */

#define TRAP_BRKPT	1	/* process breakpoint */
#define TRAP_TRACE	2	/* process trace */
#define NSIGTRAP	2

/* 
 * SIGCLD signal codes 
 */

#define	CLD_EXITED	1	/* child has exited */
#define	CLD_KILLED	2	/* child was killed */
#define	CLD_DUMPED	3	/* child has coredumped */
#define	CLD_TRAPPED	4	/* traced child has stopped */
#define	CLD_STOPPED	5	/* child has stopped on signal */
#define	CLD_CONTINUED	6	/* stopped child has continued */
#define NSIGCLD		6

/*
 * SIGPOLL signal codes
 */

#define POLL_IN		1	/* input available */
#define	POLL_OUT	2	/* output buffers available */
#define	POLL_MSG	3	/* output buffers available */
#define	POLL_ERR	4	/* I/O error */
#define	POLL_PRI	5	/* high priority input available */
#define	POLL_HUP	6	/* device disconnected */
#define NSIGPOLL	6

#define SI_MAXSZ	128
#define SI_PAD		((SI_MAXSZ / sizeof(int)) - 3)

typedef struct siginfo {

	int	si_signo;			/* signal from signal.h	*/
	int 	si_code;			/* code from above	*/
	int	si_errno;			/* error from errno.h	*/

	union {

		int	_pad[SI_PAD];		/* for future growth	*/

		struct {			/* kill(), SIGCLD	*/
			pid_t	_pid;		/* process ID		*/
			union {
				struct {
					uid_t	_uid;
				} _kill;
				struct {
					clock_t _utime;
					int	_status;
					clock_t _stime;
				} _cld;
			} _pdata;
		} _proc;			

		struct {	/* SIGSEGV, SIGBUS, SIGILL and SIGFPE	*/
			caddr_t	_addr;		/* faulting address	*/
		} _fault;

		struct {			/* SIGPOLL, SIGXFSZ	*/
		/* fd not currently available for SIGPOLL */
			int	_fd;		/* file descriptor	*/
			long	_band;
		} _file;

	} _data;

} siginfo_t;

/*
 * XXX -- internal version is identical to siginfo_t but without the padding.
 * This must be maintained in sync with it.
 */

typedef struct k_siginfo {

	int	si_signo;			/* signal from signal.h	*/
	int 	si_code;			/* code from above	*/
	int	si_errno;			/* error from errno.h	*/

	union {
		struct {			/* kill(), SIGCLD	*/
			pid_t	_pid;		/* process ID		*/
			union {
				struct {
					uid_t	_uid;
				} _kill;
				struct {
					clock_t _utime;
					int	_status;
					clock_t _stime;
				} _cld;
			} _pdata;
		} _proc;			

		struct {	/* SIGSEGV, SIGBUS, SIGILL and SIGFPE	*/
			caddr_t	_addr;		/* faulting address	*/
		} _fault;

		struct {			/* SIGPOLL, SIGXFSZ	*/
		/* fd not currently available for SIGPOLL */
			int	_fd;		/* file descriptor	*/
			long	_band;
		} _file;

	} _data;

} k_siginfo_t;

#define si_pid		_data._proc._pid
#define si_status	_data._proc._pdata._cld._status
#define si_stime	_data._proc._pdata._cld._stime
#define si_utime	_data._proc._pdata._cld._utime
#define si_uid		_data._proc._pdata._kill._uid
#define si_addr		_data._fault._addr
#define si_fd		_data._file._fd
#define si_band		_data._file._band

typedef struct sigqueue {
	struct sigqueue		*sq_next;
	struct k_siginfo	sq_info;
} sigqueue_t;

#endif /* !defined(_POSIX_SOURCE) */

#ifdef _KERNEL


extern void sigdeq(proc_t *, int, sigqueue_t **);
extern void sigdelq(proc_t *, int);
extern void sigaddq(proc_t *, k_siginfo_t *, int);
extern void sigdupq(proc_t *, proc_t *);
extern sigqueue_t *sigappend(k_sigset_t *, sigqueue_t *,
	k_sigset_t *, sigqueue_t *);
extern sigqueue_t *sigprepend(k_sigset_t *, sigqueue_t *,
	k_sigset_t *, sigqueue_t *);
extern void winfo(proc_t *, k_siginfo_t *, int);

#endif	/* _KERNEL */

#endif	/* __SYS_SIGINFO_H__ */
