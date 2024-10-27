/* O_*, F_*, FD_* bit values for stub configuration.
   Copyright (C) 1991, 1992, 1997, 2000 Free Software Foundation, Inc.
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

/* These values should be changed as appropriate for your system.  */

#ifndef	_FCNTL_H
# error "Never use <bits/fcntl.h> directly; include <fcntl.h> instead."
#endif


/* File access modes for `open' and `fcntl'.  */
#ifndef O_RDONLY
#define	O_RDONLY	0x00	/* Open read-only.  */
#define	O_WRONLY	0x01	/* Open write-only.  */
#define	O_RDWR		0x02	/* Open read/write.  */

/* Mask for file access modes.  This is system-dependent in case
   some system ever wants to define some other flavor of access.  */
#define	O_ACCMODE	(O_RDONLY|O_WRONLY|O_RDWR)
#endif


/*
 * For historic reasons, and to support older versions
 * of the library, the O_* flags are not always the same
 * as what the MiNT-Kernel uses.
 * Here is a brief overview:
 *
 *                    MiNTLib      Pure-C       Kernel
 * O_RDONLY           0x000000     0x000000     0x000000
 * O_WRONLY           0x000001     0x000001     0x000001
 * O_RDWR             0x000002     0x000002     0x000002
 * O_NOATIME          0x000004     -            0x000004
 * O_APPEND           0x001000     0x000008     0x000008
 * O_DENYRW           0x000010     0x000100     0x000010
 * O_DENYW            0x000020     0x000200     0x000020
 * O_DENYR            0x000030     0x000300     0x000030
 * O_DENYNONE         0x000040     0x000400     0x000040
 * O_NOINHERIT        0x000080     -            0x000080  not inherited by child upon fork()
 * O_NONBLOCK         0x000100     0x001000     0x000100
 * O_CREAT            0x000200     0x000020     0x000200
 * O_TRUNC            0x000400     0x000040     0x000400
 * O_EXCL             0x000800     0x000080     0x000800
 * O_GLOBAL           -            -            0x001000  used internally by kernel
 * O_PIPE             0x002000     -            -         dunno what that was meant for
 * O_NOCTTY           0x004000     -            -         not passed to kernel
 * O_CLOEXEC          0x008000     -            -         available via Fcntl(), but not as flag in Fopen; since 0.70 can be used with fcntl(F_SETFL)/open()
 * O_DIRECTORY        0x010000     -            0x010000
 * O_NOFOLLOW         0x020000     -            -         not passed to kernel
 * O_DIRECT           0x040000     -            -         NYI
 * O_LARGEFILE        0x080000     -            -         NYI
 * O_SYNC             -            -            -         NYI
 */

/* Bits OR'd into the second argument to Fopen.  */
#define _REALO_NOATIME	 0x000004		/* Do not set atime.  */
#define _REALO_APPEND	 0x000008		/* this is what MiNT uses */
#define _REALO_NOINHERIT 0x000080		/* this is what MiNT uses */
#define _REALO_NONBLOCK	 0x000100		/* this is what MiNT uses */
#define _REALO_NDELAY _REALO_NONBLOCK
#define _REALO_CREAT 	 0x000200		/* create new file if needed */
#define _REALO_TRUNC	 0x000400		/* make file 0 length */
#define _REALO_EXCL  	 0x000800		/* error if file exists */
#define _REALO_DIRECTORY 0x010000UL
#define _REALO_SYNC      0x000000       /* sync after writes (NYI) */

#define O_PIPE		0x2000		/* serial pipe     */
#define O_NOCTTY	0x4000		/* do not open new controlling tty */

#define __O_NOATIME     _REALO_NOATIME
#define __O_NOINHERIT   _REALO_NOINHERIT
#define __O_DIRECTORY   0x010000UL	/* Must be a directory.	 */
#define __O_NOFOLLOW    0x020000UL	/* Do not follow links.	 */
#define __O_DIRECT		0x040000UL	/* Direct disk access. (NYI) */
#define __O_LARGEFILE   0x080000UL	/* (LFS) Allow files whose sizes cannot be represented in an off_t. (NYI) */
#define __O_CLOEXEC     0x100000UL	/* Set close_on_exec. */

/*
 * all the relevant functions take an int as argument/return code;
 * so some of the flags are not available with __MSHORT__
 */
#if defined (__USE_XOPEN2K8)
# define O_DIRECTORY	__O_DIRECTORY
# define O_NOFOLLOW		__O_NOFOLLOW
# define O_CLOEXEC		__O_CLOEXEC
#endif
#if defined(__USE_GNU)
# define O_DIRECT		__O_DIRECT
#endif
#  define O_LARGEFILE	__O_LARGEFILE




/* Values for the second argument to `fcntl'.  */
#define	F_DUPFD	  	0	/* Duplicate file descriptor.  */
#define	F_GETFD		1	/* Get file descriptor flags.  */
#define	F_SETFD		2	/* Set file descriptor flags.  */
#define	F_GETFL		3	/* Get file status flags.  */
#define	F_SETFL		4	/* Set file status flags.  */
#define	F_GETLK		5	/* Get record locking info.  */
#define	F_SETLK		6	/* Set record locking info.  */
#define	F_SETLKW	7	/* Set record locking info, wait.  */
#if defined __USE_BSD || defined __USE_XOPEN2K
# define F_GETOWN	8	/* Get owner (receiver of SIGIO).  */
# define F_SETOWN	9	/* Set owner (receiver of SIGIO).  */
#endif

/* File descriptor flags used with F_GETFD and F_SETFD.  */
#define	FD_CLOEXEC	1	/* Close on exec.  */

#ifdef __USE_GNU
# define F_DUPFD_CLOEXEC 1030	/* Duplicate file descriptor with
				   close-on-exit set.  */
#endif

#include <bits/types.h>

/* The structure describing an advisory lock.  This is the type of the third
   argument to `fcntl' for the F_GETLK, F_SETLK, and F_SETLKW requests.  */
struct flock
  {
    short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.  */
    short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
    long int  l_start;	/* Offset where the lock begins.  */
    long int  l_len;	/* Size of the locked area; zero means until EOF.  */
    short int l_pid;	/* Process holding the lock.  */
  };

/* Values for the `l_type' field of a `struct flock'.  */
#define	F_RDLCK	O_RDONLY	/* Read lock.  */
#define	F_WRLCK	O_WRONLY	/* Write lock.  */
#define	F_UNLCK	3		/* Remove lock.  */

/* Advise to `posix_fadvise'.  */
#ifdef __USE_XOPEN2K
# define POSIX_FADV_NORMAL	0 /* No further special treatment.  */
# define POSIX_FADV_RANDOM	1 /* Expect random page references.  */
# define POSIX_FADV_SEQUENTIAL	2 /* Expect sequential page references.  */
# define POSIX_FADV_WILLNEED	3 /* Will need these pages.  */
# define POSIX_FADV_DONTNEED	4 /* Don't need these pages.  */
# define POSIX_FADV_NOREUSE	5 /* Data will be accessed once.  */
#endif


/* Bits OR'd into the second argument to open.  */

#ifdef _PUREC_SOURCE
#undef O_APPEND
#define O_APPEND    0x0008		/* position at EOF */
#undef O_CREAT
#define O_CREAT     0x0020		/* create new file if needed */
#undef O_TRUNC
#define O_TRUNC     0x0040		/* make file 0 length */
#undef O_EXCL
#define O_EXCL      0x0080		/* error if file exists */
#define	O_NONBLOCK	0x1000		/* Non-blocking I/O */

/* file sharing modes (not POSIX) */
#define O_COMPAT	0x000	/* old TOS compatibility mode */
#define O_DENYRW	0x100	/* deny both reads and writes */
#define O_DENYW		0x200
#define O_DENYR		0x300
#define O_DENYNONE	0x400	/* don't deny anything */
#define O_SHMODE	0x700	/* mask for file sharing mode */
#define _REALO_SHMODE(mode) (((mode) & (O_SHMODE)) >> 4)

#else

#define O_APPEND    0x1000				/* position at EOF */
#define O_CREAT     _REALO_CREAT		/* create new file if needed */
#define O_TRUNC     _REALO_TRUNC		/* make file 0 length */
#define O_EXCL      _REALO_EXCL			/* error if file exists */
#define	O_NONBLOCK	_REALO_NONBLOCK		/* Non-blocking I/O */
#define O_NOINHERIT __O_NOINHERIT
#define _REALO_SHMODE(mode) ((mode) & (O_SHMODE))

/* file sharing modes (not POSIX) */
#define O_COMPAT	0x00	/* old TOS compatibility mode */
#define O_DENYRW	0x10	/* deny both reads and writes */
#define O_DENYW		0x20
#define O_DENYR		0x30
#define O_DENYNONE	0x40	/* don't deny anything */
#define O_SHMODE	0x70	/* mask for file sharing mode */

#endif

#define O_SYNC		0x00	/* sync after writes (not implemented) */

#ifdef __USE_BSD
#define O_NOATIME __O_NOATIME
#endif
#define O_NDELAY	O_NONBLOCK


/* smallest valid gemdos handle
 * note handle is only word (16 bit) negative, not long negative,
 * and since Fopen etc are declared as returning long in osbind.h
 * the sign-extension will not happen -- thanks ers
 */
#ifdef __MSHORT__
#define __SMALLEST_VALID_HANDLE (-3)
#else
#define __SMALLEST_VALID_HANDLE (0)
#endif
