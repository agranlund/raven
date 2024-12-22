/**
 * $Id: stat.h,v 1.1 1993/11/14 14:58:56 AGK Rel $
 *
 * Low level file system manipulation
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _SYS_STAT_H
#define	_SYS_STAT_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef __MODES_H
#include <__modes.h>
#endif

struct stat {
	__mode_t st_mode;
	__ino_t st_ino;
	__dev_t st_dev;
	__dev_t st_rdev;
	__nlink_t st_nlink;
	__uid_t st_uid;
	__gid_t st_gid;
	__off_t st_size;
	__off_t st_blksize;
	__off_t st_blocks;
	__time_t st_mtime;
	__time_t st_atime;
	__time_t st_ctime;
	short st_attr;
	short __st_res1;
	long __st_res2[2];
};

#if !defined(_POSIX_SOURCE) \
	|| defined(_MWC)
#define S_IJRON	0x01		/* read only */
#define S_IJHID	0x02		/* hidden */
#define S_IJSYS	0x04		/* system */
#define S_IJVOL	0x08		/* label */
#define S_IJDIR	0x10		/* directory */
#define S_IJWAC	0x20		/* archive */

typedef struct {
	char d_resvd[21];
	char d_fattr;		/* File attributes */
	long d_tandd;		/* Time and date words */
	long d_fsize;		/* File size */
	char d_fname[14];	/* File name */
} DMABUFFER;
#endif

int (chmod)(const char *, __mode_t);
int (fstat)(int, struct stat *);
int (mkdir)(const char *, __mode_t);
int (mkfifo)(const char *, __mode_t);
int (stat)(const char *, struct stat *);
__mode_t (umask)(__mode_t);
#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
int (lstat)(const char *, struct stat *);
int	(fchmod)(int, __mode_t);
#endif
#endif
