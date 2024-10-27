/*  sys/resource.h -- MiNTLib.
    Copyright (C) 1999 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef	_SYS_RESOURCE_H
# define _SYS_RESOURCE_H 1

/* Get the definition of struct timeval.  */
#ifndef	_SYS_TIME_H
# include <sys/time.h>
#endif

__BEGIN_DECLS

/* Used by getrusage.  */
struct rusage 
  {
    struct timeval ru_utime;  /* User time used.  */
    struct timeval ru_stime;  /* System time used.  */
    
    /* The following rusage elements are fake.  They will all contain 
       0 or some other fake value until such time as they are supported 
       under MiNT.  */
    long ru_maxrss;	    /* Maximum resident set size.  */
    long ru_ixrss;	    /* Integral shared memory size.  */
    long ru_idrss;	    /* Integral unshared data size.  */
    long ru_isrss;	    /* Integral unshared stack size.  */
    long ru_minflt;	    /* Page reclaims.  */
    long ru_majflt;	    /* Page faults (requiring I/O).  */
    long ru_nswap;	    /* Memory swaps.  */
    long ru_inblock;	    /* Block input operations.  */
    long ru_oublock;	    /* Block output operations.  */
    long ru_msgsnd;	    /* Messages sent.  */
    long ru_msgrcv;	    /* Messages received.  */
    long ru_nsignals;	    /* Signals received.  */
    long ru_nvcsw;	    /* Voluntary context switches.  */
    long ru_nivcsw;	    /* Involuntary context switches.  */
  };


/* Values for the first argument to `getrlimit' and `setrlimit'.  */
enum __rlimit_resource
  {
	/* Per-process CPU limit, in seconds.  */
    RLIMIT_CPU = 0,
#define RLIMIT_CPU RLIMIT_CPU

    /* Largest file that can be created, in bytes.  */
    RLIMIT_FSIZE = 1,
#define RLIMIT_FSIZE RLIMIT_FSIZE

    /* Maximum size of data segment, in bytes.  */
    RLIMIT_DATA = 2,
#define RLIMIT_DATA RLIMIT_DATA

    /* Maximum size of stack segment, in bytes.  */
    RLIMIT_STACK = 3,
#define RLIMIT_STACK RLIMIT_STACK

    /* Largest core file that can be created, in bytes.  */
    RLIMIT_CORE = 4,
#define RLIMIT_CORE RLIMIT_CORE

  /* Largest resident set size, in bytes.
     This affects swapping; processes that are exceeding their
     resident set size will be more likely to have physical memory
     taken from them.  */
    RLIMIT_RSS = 5,
#define RLIMIT_RSS RLIMIT_RSS

	/* Number of processes.  */
	__RLIMIT_NPROC = 6,
#define RLIMIT_NPROC __RLIMIT_NPROC

    /* Number of open files.  */
    RLIMIT_NOFILE = 7,
    __RLIMIT_OFILE = RLIMIT_NOFILE, /* BSD name for same.  */
#define RLIMIT_NOFILE RLIMIT_NOFILE
#define RLIMIT_OFILE __RLIMIT_OFILE

	/* Locked-in-memory address space.  */
	__RLIMIT_MEMLOCK = 8,
#define RLIMIT_MEMLOCK __RLIMIT_MEMLOCK

	/* Address space limit.  */
	RLIMIT_AS = 9,
#define RLIMIT_AS RLIMIT_AS

	/* Maximum number of file locks.  */
	__RLIMIT_LOCKS = 10,
#define RLIMIT_LOCKS __RLIMIT_LOCKS

	/* Maximum number of pending signals.  */
	__RLIMIT_SIGPENDING = 11,
#define RLIMIT_SIGPENDING __RLIMIT_SIGPENDING

	/* Maximum bytes in POSIX message queues.  */
	__RLIMIT_MSGQUEUE = 12,
#define RLIMIT_MSGQUEUE __RLIMIT_MSGQUEUE

	/* Maximum nice priority allowed to raise to.
	   Nice levels 19 .. -20 correspond to 0 .. 39
	   values of this resource limit.  */
	__RLIMIT_NICE = 13,
#define RLIMIT_NICE __RLIMIT_NICE

	/* Maximum realtime priority allowed for non-priviledged
	   processes.  */
	__RLIMIT_RTPRIO = 14,
#define RLIMIT_RTPRIO __RLIMIT_RTPRIO

	/* Maximum CPU time in µs that a process scheduled under a real-time
	   scheduling policy may consume without making a blocking system
	   call before being forcibly descheduled.  */
	__RLIMIT_RTTIME = 15,
#define RLIMIT_RTTIME __RLIMIT_RTTIME

    /* Number of limit flavors.  */
    __RLIMIT_NLIMITS = 16,
    __RLIM_NLIMITS = __RLIMIT_NLIMITS
    /* Traditional name for same.  */
#define RLIMIT_NLIMITS RLIMIT_NLIMITS  
#define RLIM_NLIMITS __RLIM_NLIMITS  
  };

#define RLIM_INFINITY	0x7fffffffL

/* We can represent all limits */
#define RLIM_SAVED_MAX	RLIM_INFINITY
#define RLIM_SAVED_CUR	RLIM_INFINITY

typedef __rlim_t rlim_t;

/* Used by [gs]etrlimit.  */
struct rlimit 
  {
    long int rlim_cur;  /* Current value of the limit in question.  */
    long int rlim_max;  /* Maximum permissible value of the of the 
                         * limit in question.
                         */
};

/* Put the soft and hard limits for RESOURCE in *RLIMITS.
   Returns 0 if successful, -1 if not (and sets errno).  */
extern int getrlimit (enum __rlimit_resource __resource,
		      struct rlimit *__rlimits) __THROW;
extern int __getrlimit (enum __rlimit_resource __resource,
		        struct rlimit *__rlimits) __THROW;

/* Set the soft and hard limits for RESOURCE to *RLIMITS.
   Only the super-user can increase hard limits.
   Return 0 if successful, -1 if not (and sets errno).  */
extern int setrlimit (enum __rlimit_resource __resource,
		      const struct rlimit *__rlimits) __THROW;

/* Possible values for first argument to `getrusage'.  */
enum __rusage_who 
  {
    /* Current process.  */
    RUSAGE_SELF = -1,
#define RUSAGE_SELF RUSAGE_SELF
    /* Current processes children.  */
    RUSAGE_CHILDREN = -2
#define RUSAGE_CHILDREN RUSAGE_CHILDREN
  };

/* Return resource usage information on process indicated by WHO
   and put it in *USAGE.  Returns 0 for success, -1 for failure.  */
extern int getrusage (enum __rusage_who __who, 
                      struct rusage *__usage) __THROW;
extern int __getrusage (enum __rusage_who __who, 
                        struct rusage *__usage) __THROW;

#if 0
/* Function depends on CMD:
   1 = Return the limit on the size of a file, in units of 512 bytes.
   2 = Set the limit on the size of a file to NEWLIMIT.  Only the
       super-user can increase the limit.
   3 = Return the maximum possible address of the data segment.
   4 = Return the maximum number of files that the calling process can open.
   Returns -1 on errors.  */
extern long int ulimit (int __cmd, long int __newlimit) __THROW;
extern long int __ulimit (int __cmd, long int __newlimit) __THROW;
#endif

#define PRIO_MIN -20   /* The smallest valid priority value.  */
#define PRIO_MAX  20   /* The highest valid priority value.  */

/* Values for the first argument to `getpriority' and `setpriority'.  */
enum __priority_which
  {
    /* Read or set the priority of one process.  Second argument is
       a process ID.  */
    PRIO_PROCESS = 0,
#define PRIO_PROCESS PRIO_PROCESS
    /* Read or set the priority of one process group.  Second argument is
       a process group ID.  */
    /* Note that this opcode is not implemented yet.  The return value
       will be -1 and errno will be set to ENOSYS.  */
    PRIO_PGRP = 1,
#define PRIO_PGRP PRIO_PGRP
    /* Read or set the priority of one user's processes.  Second argument is
       a user ID.  */
    /* Note that this opcode is not implemented yet.  The return value
       will be -1 and errno will be set to ENOSYS.  */
    PRIO_USER = 2
#define PRIO_USER PRIO_USER
  };
  
/* Return the highest priority of any process specified by WHICH and WHO
   (see above); if WHO is zero, the current process, process group, or user
   (as specified by WHO) is used.  A lower priority number means higher
   priority.  Priorities range from PRIO_MIN to PRIO_MAX (above).  */
extern int getpriority (enum __priority_which __which, int __who) __THROW;
extern int __getpriority (enum __priority_which __which, int __who) __THROW;

/* Set the priority of all processes specified by WHICH and WHO (see above)
   to PRIO.  Returns 0 on success, -1 on errors.  */
extern int setpriority (enum __priority_which __which, int __who,
			int __prio) __THROW;
extern int __setpriority (enum __priority_which __which, int __who,
			  int __prio) __THROW;

/* Increment the priority of the current process by INCREMENT.  Return
   value is not meaningful.  */
extern int nice (int increment) __THROW;
extern int __nice (int increment) __THROW;

__END_DECLS

#endif	/* resource.h  */
