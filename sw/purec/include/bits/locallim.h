/*  include/bits/local_lim.h -- MiNTLib.
    Copyright (C) 2000 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

/* These are the MiNT specific limits.  */

#define NGROUPS_MAX 8		/* 8 supplementary groups.  */
#define ARG_MAX 32767
#define CHILD_MAX 999
#define OPEN_MAX 32
#define LINK_MAX 32767
#define MAX_CANON 256
#define PIPE_BUF 1024

/* For SVR3, this is 14.  For SVR4, it is 255, at least on ufs
   file systems, even though the System V limits.h incorrectly
   defines it as 14.  Giving it a value which is too large
   is harmless (it is a maximum).  */
#define NAME_MAX 255

#define PATH_MAX 1024

/* The number of data keys per process.  */
#define _POSIX_THREAD_KEYS_MAX	128
/* This is the value this implementation supports.  */
#define PTHREAD_KEYS_MAX	1024

/* Controlling the iterations of destructors for thread-specific data.  */
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS	4

/* Number of iterations this implementation does.  */
#define PTHREAD_DESTRUCTOR_ITERATIONS	_POSIX_THREAD_DESTRUCTOR_ITERATIONS

/* The number of threads per process.  */
#define _POSIX_THREAD_THREADS_MAX	64

/* We have no predefined limit on the number of threads.  */
#undef PTHREAD_THREADS_MAX

/* Maximum value the semaphore can have.  */
#define SEM_VALUE_MAX   (2147483647L)

/* Minimum size for a thread.  We are free to choose a reasonable value.  */
#define PTHREAD_STACK_MIN	16384

/* The gcc header depends on the macro __GNU_LIBRARY__ which is 
   not set for the MiNTLib.  */
#if defined (__USE_GNU)
/* Minimum and maximum values a `signed long long int' can hold.  */
#ifndef __LONG_LONG_MAX__
#define __LONG_LONG_MAX__ 9223372036854775807LL
#endif
#undef LONG_LONG_MIN
#define LONG_LONG_MIN (-LONG_LONG_MAX-1)
#undef LONG_LONG_MAX
#define LONG_LONG_MAX __LONG_LONG_MAX__

/* Maximum value an `unsigned long long int' can hold.  (Minimum is 0).  */
#undef ULONG_LONG_MAX
#define ULONG_LONG_MAX (LONG_LONG_MAX * 2ULL + 1)
#endif

/* Sigh, mshort stuff.  Maybe this could be removed if gcc installs
   separate header files for each multilib target.  */
#if defined(__MSHORT__) && !defined(__INT_MAX__)
#define __INT_MAX__ 0x7fff
#endif
