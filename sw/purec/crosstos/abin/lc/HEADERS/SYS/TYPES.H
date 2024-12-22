/**
 * $Id: types.h,v 1.1 1993/11/14 14:58:56 AGK Rel $
 *
 * POSIX.1 types header file
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
#endif

#if !defined(_POSIX_SOURCE) \
	|| defined(_SYSV)
typedef unsigned char unchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
#endif

#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD) \
	|| defined(_SYSV)
typedef long daddr_t;
typedef char *caddr_t;
#endif

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef _DEV_T
#define _DEV_T
typedef __dev_t dev_t;
#endif

#ifndef _GID_T
#define _GID_T
typedef __gid_t gid_t;
#endif

#ifndef _INO_T
#define _INO_T
typedef __ino_t ino_t;
#endif

#ifndef _MODE_T
#define _MODE_T
typedef __mode_t mode_t;
#endif

#ifndef _NLINK_T
#define _NLINK_T
typedef __nlink_t nlink_t;
#endif

#ifndef _OFF_T
#define _OFF_T
typedef __off_t off_t;
#endif

#ifndef _PID_T
#define _PID_T
typedef __pid_t pid_t;
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef __size_t size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef __ssize_t ssize_t;
#endif

#ifndef	_CLOCK_T
#define _CLOCK_T
typedef	__clock_t clock_t;
#endif

#ifndef	_TIME_T
#define _TIME_T
typedef	__time_t time_t;
#endif

#ifndef _UID_T
#define _UID_T
typedef __uid_t uid_t;
#endif

#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
#define	NBBY	8

#ifndef	FD_SETSIZE
#define	FD_SETSIZE	32
#endif

typedef long fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)

#ifndef howmany
#define	howmany(x, y)	(((x) + ((y) - 1)) / (y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n) / NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n) / NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n) / NFDBITS] & (1 << ((n) % NFDBITS)))
#define	FD_ZERO(p)	memset((p), '\0', sizeof(*(p)))
#endif
#endif
