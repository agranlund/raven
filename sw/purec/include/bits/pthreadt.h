/* Copyright (C) 2002-2018 Free Software Foundation, Inc.
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

/* Modified for MiNTLib by Thorsten Otto <admin@tho-otto.de>. */

#ifndef _BITS_PTHREADTYPES_H
#define _BITS_PTHREADTYPES_H	1

#include <endian.h>
#include <sched.h>

/* Thread identifiers. The structure of the attribute type is
   deliberately not exposed. */
typedef struct __pthread_t *pthread_t;


#define __LOCK_ALIGNMENT __attribute__ ((__aligned__ (4)))
#define __ONCE_ALIGNMENT __attribute__ ((__aligned__ (4)))


/* Thread attribute data structure. */
#ifndef __have_pthread_attr_t
# define __have_pthread_attr_t	1
typedef struct __pthread_attr pthread_attr_t;
#endif
#if !defined(__PUREC__) && !defined(__TURBOC__)
struct __pthread_attr
{
	/* Scheduler parameters and priority. */
	struct sched_param schedparam;
	int schedpolicy;
	/* Various flags like detachstate, scope, etc. */
	__int32_t flags;
	/* Size of guard area. */
	size_t guardsize;
	/* Stack handling. */
	void *stackaddr;
	size_t stacksize;
	/* Affinity map. */
	cpu_set_t *cpuset;
	size_t cpusetsize;
};


typedef struct __pthread_internal_slist
{
	struct __pthread_internal_slist *__next;
} __pthread_slist_t;


/* Data structures for mutex handling. */
typedef struct __pthread_mutex
{
	__int32_t __lock __LOCK_ALIGNMENT;
	__uint32_t __count;
	__int32_t __owner;
	__int32_t __kind;
	__uint32_t __nusers;
	union
	{
		__int32_t __spins;
		__pthread_slist_t __list;
	} u;
} pthread_mutex_t;

/* Mutex __spins initializer used by PTHREAD_MUTEX_INITIALIZER. */
#define __PTHREAD_SPINS 0

/* Mutex attribute data structure. */
typedef struct __pthread_mutexattr pthread_mutexattr_t;
struct __pthread_mutexattr
{
	__int32_t mutexkind;
};


/* Data structure for conditional variable handling. */
typedef struct __pthread_cond
{
    __uint64_t __wseq;
    __uint64_t __g1_start;
	__uint32_t __g_refs[2] __LOCK_ALIGNMENT;
	__uint32_t __g_size[2];
	__uint32_t __g1_orig_size;
	__uint32_t __wrefs;
	__uint32_t __g_signals[2];
} pthread_cond_t;

/* Conditional variable attribute data structure. */
typedef struct __pthread_condattr pthread_condattr_t;
struct __pthread_condattr
{
	__int32_t value;
};


/* Keys for thread-specific data */
typedef __uint32_t pthread_key_t;


/* Once-only execution */
typedef __int32_t __ONCE_ALIGNMENT pthread_once_t;


#if defined __USE_UNIX98 || defined __USE_XOPEN2K
/* Data structure for read-write lock variable handling. */
typedef struct __pthread_rwlock
{
	__uint32_t __readers  __LOCK_ALIGNMENT;
	__uint32_t __writers;
	__uint32_t __wrphase_futex;
	__uint32_t __writers_futex;
	__uint32_t __pad3;
	__uint32_t __pad4;
	__uint8_t __pad1;
	__uint8_t __pad2;
	__uint8_t __shared;
	__uint8_t __flags;
	__int32_t __cur_writer;
} pthread_rwlock_t;

#define __PTHREAD_RWLOCK_ELISION_EXTRA 0

/* Read-write lock variable attribute data structure. */
typedef struct __pthread_rwlockattr pthread_rwlockattr_t;
struct __pthread_rwlockattr
{
	int lockkind;
	int pshared;
};
#endif


#ifdef __USE_XOPEN2K
/* POSIX spinlock data type. */
typedef volatile __int32_t pthread_spinlock_t;


/* Barrier data structure. See pthread_barrier_wait for a description
   of how these fields are used. */
typedef struct __pthread_barrier pthread_barrier_t;
struct __pthread_barrier
{
	__uint32_t in;
	__uint32_t current_round;
	__uint32_t count;
	__int32_t shared;
	__uint32_t out;
};

/* Barrier variable attribute data structure. */
typedef struct __pthread_barrierattr pthread_barrierattr_t;
struct __pthread_barrierattr
{
	__int32_t pshared;
};

#endif

#endif /* __PUREC__ */

#endif	/* bits/pthreadtypes.h */
