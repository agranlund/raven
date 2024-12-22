/*
 * syserr.c - define errno error text
 *
 * $Log: syserr.c,v $
 * Revision 1.2  1991/12/10  16:23:18  AGK
 * Added MetaDOS(tm) and Network GEMDOS error codes
 *
 * Revision 1.1  1991/06/17  12:48:08  AGK
 * Initial revision
 *
 * Copyright (c) 1991 HiSoft
 */

#include <errno.h>

const char *sys_errlist[] = { 
						"Unknown error code",
/* EPERM 1 */			"User is not owner",
/* ENOENT 2 */			"No such file or directory",
/* ESRCH 3 */			"No such process",
/* EINTR 4 */			"Interrupted system call",
/* EIO 5 */ 			"I/O error",
/* ENXIO 6 */			"No such device or address",
/* E2BIG 7 */			"Arg list is too long",
/* ENOEXEC 8 */ 		"Exec format error",
/* EBADF 9 */			"Bad file number",
/* ECHILD 10 */ 		"No child process",
/* EAGAIN 11 */ 		"No more processes allowed",
/* ENOMEM 12 */ 		"No memory available",
/* EACCES 13 */ 		"Access denied",
/* EFAULT 14 */ 		"Bad address",
/* ENOTBLK 15 */		"Bulk device required",
/* EBUSY 16 */			"Resource is busy",
/* EEXIST 17 */ 		"File already exists",
/* EXDEV 18 */			"Cross-device link",
/* ENODEV 19 */ 		"No such device",
/* ENOTDIR 20 */		"Not a directory",
/* EISDIR 21 */ 		"Is a directory",
/* EINVAL 22 */ 		"Invalid argument",
/* ENFILE 23 */ 		"No more files (units) allowed",
/* EMFILE 24 */ 		"No more files (units) allowed for this process",
/* ENOTTY 25 */ 		"Not a terminal",
/* ETXTBSY 26 */		"Text file is busy",
/* EFBIG 27 */			"File is too large",
/* ENOSPC 28 */ 		"No space left",
/* ESPIPE 29 */ 		"Seek issued to pipe",
/* EROFS 30 */			"Read-only file system",
/* EMLINK 31 */ 		"Too many links",
/* EPIPE 32 */			"Broken pipe",
/* EDOM 33 */			"Math function argument error",
/* ERANGE 34 */ 		"Math function result is out of range",
/* EDEADLK 35 */		"Record locking deadlock",
};

/* Highest valid error number */
int sys_nerr = sizeof(sys_errlist)/sizeof(sys_errlist[0]);
