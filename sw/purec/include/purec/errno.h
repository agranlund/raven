/*      ERRNO.H

        Library error code definitions

        Copyright (c) Borland International 1990
        All Rights Reserved.
*/


#if !defined(__PUREC_ERRNO_H__)
#define __PUREC_ERRNO_H__


#define EPERM   1       /* file permission denied     */
#define ENOENT  2       /* file not found             */
#define ESRCH	3	    /* No such process            */
#define EINTR	4		/* Interrupted system call    */
#define EIO     5       /* general i/o error          */
#define ENXIO	6		/* No such device or address  */
#define E2BIG   7       /* Argument list too long     */
/* #define ENOEXEC	8	/ * Exec format error         */
#define EBADF   9       /* invalid file handle        */
#define EILLSPE 10      /* illegal file specification */ /* should be ECHILD */
#define EINVMEM 11      /* invalid heap block         */ /* should be EAGAIN */
#define ENOMEM  12      /* heap overflow              */
#define EACCES  13      /* file access mode error     */
#define EFAULT	14		/* Bad address                */
#define ENOTBLK	15		/* Bulk device required       */
#define EBUSY	16		/* Resource is busy           */
#define EEXIST  17      /* file already exists        */
#define EPLFMT  18      /* program load format error  */
#define ENODEV  19      /* device error               */
#define ENOTDIR 20      /* path not found             */
#define EISDIR	21		/* Is a directory 	          */
#define EINVAL  22      /* invalid parameter          */
#define ENFILE  23      /* file table overflow        */
#define EMFILE  24      /* too many open files        */
#define ENOTTY	25		/* Not a terminal             */
#define ETXTBSY	26		/* Text file is busy          */
#define EFBIG	27		/* File is too large          */
#define ENOSPC  28      /* disk full                  */
#define ESPIPE  29      /* seek error                 */
#define EROFS   30      /* read only device           */
#define EMLINK	31		/* Too many links             */
#define EPIPE	32		/* Broken pipe                */
#define EDOM    33      /* domain error               */
#define ERANGE  34      /* range error                */
#define ENMFILE 35      /* no more matching file      */ /* should be EDEADLK */
#define EILSEQ  84      /* Illegal byte sequence      */

#define ENOEXEC EPLFMT

extern int errno;

#endif /* __PUREC_ERNNO_H__ */

/************************************************************************/
