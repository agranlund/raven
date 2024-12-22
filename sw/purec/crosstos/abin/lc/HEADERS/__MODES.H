/**
 * $Id: __modes.h,v 1.1 1993/11/14 14:42:42 AGK Rel $
 *
 * File mode bits
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#define __MODE_H

#define _S_IFMT	0170000		/* mask to select file type */
#define _S_IFDIR	0040000		/* directory file */
#define _S_IFCHR	0020000		/* BIOS special file */
#define _S_IFREG	0100000		/* regular file */
#define _S_IFBLK	0060000		/* block special file */
#define _S_IFIFO	0120000		/* FIFO */

#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
#define _S_IMEM	0140000		/* memory region or process */
#define _S_IFLNK	0160000		/* symbolic link */
#define	_S_IFSOCK	0140000		/* socket */
#define S_IFMT	_S_IFMT
#define S_IFDIR	_S_IFDIR
#define S_IFCHR	_S_IFCHR
#define S_IFREG	_S_IFREG
#define S_IFIFO	_S_IFIFO
#define S_IFBLK	_S_IFBLK
#define S_IMEM	_S_IMEM
#define S_IFLNK	_S_IFLNK
#define S_IFSOCK	_S_IFSOCK
#endif

#define S_ISDIR(f)	(((f) & _S_IFMT) == S_IFDIR)
#define S_ISCHR(f)	(((f) & _S_IFMT) == S_IFCHR)
#define S_ISBLK(f)	(((f) & _S_IFMT) == S_IFBLK)
#define S_ISREG(f)	(((f) & _S_IFMT) == S_IFREG)
#define S_ISFIFO(f)	(((f) & _S_IFMT) == S_IFIFO)
#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
#define S_ISLNK(f)	(((f) & _S_IFMT) == S_IFLNK)
#define S_ISMEM(f)	(((f) & _S_IFMT) == S_IFMEM)
#define S_ISSOCK(f)	(((f) & _S_IFMT) == S_IFSOCK)
#endif

/* special bits: setuid, setgid, sticky bit */
#define S_ISUID	04000
#define S_ISGID 02000

#if !defined(_POSIX_SOURCE) \
	|| defined(_V7)
#define S_ISVTX	01000
#endif

/* file access modes for user, group, and other*/
#define S_IRWXU	0700
#define S_IRUSR	0400
#define S_IWUSR 0200
#define S_IXUSR 0100
#define S_IRWXG 0070
#define S_IRGRP 0040
#define S_IWGRP	0020
#define S_IXGRP	0010
#define S_IRWXO	0007
#define S_IROTH	0004
#define S_IWOTH	0002
#define S_IXOTH	0001

#if !defined(_POSIX_SOURCE) \
	|| defined(_LATTICE)
#define S_IREAD	S_IRUSR
#define S_IWRITE	S_IWUSR
#define S_IEXEC	S_IXUSR
#endif
