/* Copyright (C) 1992, 1996, 1997, 2000 Free Software Foundation, Inc.
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

#ifndef _SYS_STAT_H
# error "Never include <bits/stat.h> directly; use <sys/stat.h> instead."
#endif

/* This structure needs to be defined in accordance with the
   implementation of __stat, __fstat, and __lstat.  */

#include <bits/types.h>

/* Structure describing file characteristics.  */

struct __st_mtim {
	unsigned long tv_nsec;
};
	
/* This is what Fstat wants and what we use in the library.  */
struct stat {
  __dev_t st_dev;		/* Device.  */
  __ino_t st_ino;		/* File serial number.  */
  __mode_t st_mode;		/* File mode.  */
  __nlink_t st_nlink;		/* (Hard) link count.  */
  __uid_t st_uid;		/* User ID of the file's owner.  */
  __gid_t st_gid;		/* Group ID of the file's group.  */
  __dev_t st_rdev;		/* Device number, if device.  */
  long __st_high_atime;
  __time_t st_atime;		/* Time of last access, UTC.  */
  struct __st_mtim st_atim;
  long __st_high_mtime;
  __time_t st_mtime;		/* Time of last access, UTC.  */
  struct __st_mtim st_mtim;
  long __st_high_ctime;
  __time_t st_ctime;		/* Time of last status change, UTC.  */
  struct __st_mtim st_ctim;
  unsigned long __st_hi_size;	/* Upper 4 bytes of st_size.  */
  __off_t st_size;		/* File size, in bytes.  */
  unsigned long __st_hi_blocks; /* Upper 4 bytes of st_blocks.  */
  __off_t st_blocks;		/* Number of 512-bytes blocks allocated.  */
  unsigned long int st_blksize;	/* Optimal blocksize for I/O.  */
  unsigned long	int st_flags;	/* User defined flags for file.  */
  unsigned long	int st_gen;	/* File generation number.  */
  long int __res[7];
};

/* Tell code we have these members.  */
#define	_STATBUF_ST_BLKSIZE
#define _STATBUF_ST_RDEV


/* Encoding of the file mode.  These are the standard Unix values,
   but POSIX.1 does not specify what values should be used.  */

#define	__S_IFMT	0170000	/* These bits determine file type.  */

/* File types.  */
#define __S_IFSOCK	0010000	/* Socket.  */
#define	__S_IFCHR	0020000	/* Character device.  */
#define	__S_IFDIR	0040000	/* Directory.  */
#define __S_IFBLK	0060000	/* Block device.  */
#define	__S_IFREG	0100000	/* Regular file.  */
#define __S_IFIFO	0120000	/* FIFO.  */
#define __S_IFMEM	0140000 /* memory region or process */
#define	__S_IFLNK	0160000	/* Symbolic link.  */


/* POSIX.1b objects.  */
#define __S_TYPEISMQ(buf)  ((buf)->st_mode - (buf)->st_mode)
#define __S_TYPEISSEM(buf) ((buf)->st_mode - (buf)->st_mode)
#define __S_TYPEISSHM(buf) ((buf)->st_mode - (buf)->st_mode)


/* Protection bits.  */

#define	__S_ISUID	04000	/* Set user ID on execution.  */
#define	__S_ISGID	02000	/* Set group ID on execution.  */
#define	__S_ISVTX	01000	/* Save swapped text after use (sticky).  */
#define	__S_IREAD	00400	/* Read by owner.  */
#define	__S_IWRITE	00200	/* Write by owner.  */
#define	__S_IEXEC	00100	/* Execute by owner.  */
