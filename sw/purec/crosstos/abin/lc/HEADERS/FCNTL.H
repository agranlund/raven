/**
 * $Id: fcntl.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Level 1 I/O interface file
 *
 * (c) Copyright 1991, 1993 HiSoft
**/
 
#ifndef _FCNTL_H
#define _FCNTL_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef __MODES_H
#include <__modes.h>
#endif

#ifndef	_SIZE_T
#define	_SIZE_T
typedef unsigned long int size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef signed long ssize_t;
#endif

/*
 *
 * The following symbols are used for the "open" and "creat" functions.
 * They are generally UNIX-compatible, except for O_APPEND under MSDOS,
 * which has been moved in order to accomodate the file sharing flags
 * defined in MSDOS Version 3.
 *
 */
#define O_RDONLY	0	/* Read-only value (right byte of mode word) */
#define O_WRONLY	1	/* Write-only value */
#define O_RDWR		2	/* Read-write value */
#define O_NDELAY	0	/* Non-blocking I/O flag (N/A) */
#define O_APPEND	8	/* Append mode flag */
#define O_SYNC		0	/* Sync after writes (N/A) */

#define O_SDRW	0x10	/* Share mode: deny read/write */
#define O_SDW	0x20	/* Share mode: deny write */
#define O_SDR	0x30	/* Share mode: deny read */
#define O_SDN	0x40	/* Share mode: deny none */
#define O_PVT	0x80	/* Private file (not passed to child) */

#define O_CREAT	0x0100	/* File creation flag */
#define O_TRUNC	0x0200	/* File truncation flag */
#define O_EXCL	0x0400	/* Exclusive access flag */

#define O_RAW	0x8000	/* Raw I/O flag (Lattice feature) */

#define O_BINARY O_RAW	/* popular synonym */

#ifndef SEEK_SET
#define SEEK_SET	__SEEK_SET
#endif
#ifndef SEEK_CUR
#define SEEK_CUR	__SEEK_CUR
#endif
#ifndef SEEK_END
#define SEEK_END	__SEEK_END
#endif

/*
 * flags for fcntl()
 */
#define	F_DUPFD		0	/* Duplicate fildes */
#define	F_GETFD		1	/* Get fildes flags */
#define	F_SETFD		2	/* Set fildes flags */
#define	F_GETFL		3	/* Get file flags */
#define	F_SETFL		4	/* Set file flags */
#define F_GETLK		5	/* Get file lock */
#define F_SETLK		6	/* Set file lock */
#define F_SETLKW	7	/* Get/Set file lock */

struct flock {
#define F_RDLCK	O_RDONLY
#define F_WRLCK	O_WRONLY
#define F_UNLCK	3
	short l_type;
	short l_whence;
	long l_start;
	long l_len;
	short l_pid;
};

/*
 * External definitions
 */

int open(const char *, int, ...);
int opene(const char *, int, int, char *);
int creat(const char *, int);
int fcntl(int,int,void *);

#define creat(name, prot)	open(name,O_WRONLY|O_TRUNC|O_CREAT|(prot & O_RAW),(prot & ~O_RAW))
#endif
