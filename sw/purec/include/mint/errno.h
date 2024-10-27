/*
 * mint/errno.h -- MiNTLib.
 * Copyright (C) 1999 Guido Flohr <guido@freemint.de>
 * 
 * This file is part of the MiNTLib project, and may only be used
 * modified and distributed under the terms of the MiNTLib project
 * license, COPYMINT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 */

#ifndef	_MINT_ERRNO_H
# define _MINT_ERRNO_H 1         /* Allow multiple inclusion.  */

#ifndef __KERNEL__
# ifndef _FEATURES_H
#  include <features.h>
# endif

/* See below.  */
#define __KERNEL_NEG(c) c

#else /* __KERNEL__ */
  /* The kernel should actually have code like
  
  	if (whew_we_dont_know_that)
  	  return -ENOSYS;
  
     instead of the current
     
        if (whew_we_dont_know_that)
          return ENOSYS;
     
     That seems to be a matter of taste.
     
     As a convenience we offer both possibilities here:
  */

# ifdef POSITIVE_ERROR_CODES
#  define __KERNEL_NEG(c) (c)
# else  /* not POSITIVE_ERROR_CODES */
#  define __KERNEL_NEG(c) (-(c))
# endif /* not POSITIVE_ERROR_CODES */

/* If you prefer to type E_OK instead of 0 ...  */
# define E_OK 0

#endif /* not __KERNEL__ */

/* The original atarierr.h defined quite a few error codes which were
   either useless or non-standard.  Important codes such as EISDIR,
   ENOTEMPTY were missing, others were simply misunderstood (for example
   the usual EINVAL would really be an ENOSYS, EINVAL is not invalid
   function number but invalid argument) and especially in MiNT
   some error codes got really overloaded, for example ENOENT serves
   as ENOENT, ESRCH and ECHILD, or EACCES and EPERM were the same.
   
   The codes in this file try to find the closest equivalent for
   all error codes currently in use, marks some error codes as
   deprecated and tries to be fit for the future by defining all
   error codes that may be needed for future enhancement.
*/

/*    
   The explanations are mostly taken from the GNU libc manual.
   Unfortunately they don't always document the current usage in the
   MiNT system but rather how it should be.  I've marked all current
   misusage with either `KERNEL' or `FIXME'.
   
   We provide way too many synonyms for error codes here.  Both in
   the kernel and the libc only the `official' name (the one in the
   top row that bears the actual define) should be used.
*/
 
/* Where is ENOERR and E_OK?  This file is <errno.h> and not <noerror.h>,
   no error is 0, basta.  */

/* Is this really needed?  Deprecated.  */
#define	ETOS_ERROR			(1)			/* Generic error.  */

/* A system resource that can't be shared is already in use.  For example
   if you try to delete a file that is the root of a currently mounted
   filesystem, you get this error.
   This used to be EDRNRDY resp. EDRNRDY.  */
#define ETOS_BUSY			(2)			/* Resource busy.  */

/* Deprecated.  */
#define	ETOS_UKCMD			(3)			/* Unknown command.  */

/* Checksum error detected while reading from a block device such as
   a floppy drive.  Obviously non-standard but often used.  */
#define	ETOS_CRC			(4)			/* CRC error.  */

/* Bad request.  */
#define ETOS_BADR			(5)			/* Bad request.  */

/* Invalid seek operation (such as on a pipe).  */
/* KERNEL: The pipe file system currently returns 0 for seek on a pipe!  */
#define ETOS_SPIPE			(6)			/* Illegal seek.  */

/* Wrong medium type.  */
#define ETOS_MEDIUMTYPE		(7)			/* Wrong medium type.  */

/* A sector that does not exist was addressed in the call to a bock
   device function.  */
#define	ETOS_SECTOR			(8)			/* Sector not found.  */

/* No paper.  No, not at the conveniences, in the printer.  */
#define	ETOS_PAPER			(9)			/* No paper.  */

/* Read and write faults.  Deprecated, be more specific instead.  */
#define	ETOS_WRITE			(10)		/* Write fault.  */
#define ETOS_READ			(11)		/* Read fault. */

/* Another generic error.  Who can tell the difference to EERROR?  */
#define	ETOS_GENERIC		(12)		/* General mishap.  */

/* An attempt was made to modify something on a read-only file
   file system.  */
#define	ETOS_ROFS			(13)		/* Write protect.  */

/* A removable medium was exchanged before the operation could finish.  */
#define	ETOS_CHMEDIA		(14)		/* Media change.  */

/* The wrong type of device was given to a function that expects a
   particular sort of device.  */
#define	ETOS_NODEV			(15)		/* No such device.  */

/* Bad sectors on block device medium found.  Uhm, if the driver found
   them, mark them as bad and don't bother the user with that.
   Deprecated.  */
#define	ETOS_BADSEC			(16)		/* Bad sectors found.  */

/* A device with a removable media (such as a floppy) is empty.  */
#define ETOS_NOMEDIUM		(17)		/* No medium found.  */

/* MetaDOS error (deprecated).  */
#define ETOS_INSERT			(18)		/* Insert media.  */

/* MetaDOS error (deprecated).  */
#define ETOS_DRVNRSP		(19)		/* Drive not responding.  */

/* No process matches the specified process ID.

   KERNEL: This is a new error code.  Currently ENOENT is returned instead.  */
#define ETOS_SRCH			(20)		/* No such process.  */

/* There are no child processes.  This error happens on operations
   that are supposed to manipulate child processes, when there aren't
   any processes to manipulate.  
   
   KERNEL: This is a new error code.  Currently ENOENT is returned instead.  */
#define ETOS_CHILD			(21)		/* No child processes.  */

/* Deadlock avoided; allocating a system resource would have resulted
   in a deadlock situation.  The system does not guarantee that it
   will notice all such situations.  This error means you got lucky
   and the system noticed; it might just hang.  
   
   KERNEL: This is a new error code.  The kernel currently never returns
   it.  But that doesn't mean that there are no deadlock situations in
   the kernel.  */
#define ETOS_DEADLK			(22)		/* Resource deadlock would occur.  */

/* A file that isn't a block special file was given in a situation
   that requires one.  For example, trying to mount an ordinary file
   as a file system in Unix gives this error.  New code.  */
#define ETOS_NOTBLK			(23)		/* Block device required.  */

/* File is a directory; you cannot open a directory for writing, or
   create or remove hard links to it.  
   
   KERNEL: This is a new error code.  Flink() should be fixed.  */
#define ETOS_ISDIR			(24)		/* Is a directory.  */

/* Invalid argument.  This is used to indicate various kinds of 
   problems with passing the wrong argument to a library function. 
   
   FIXME: This used to be the same as `ENOSYS'.  In fact that should
   be two distinct error codes.  For example, the `fchmod' function
   should return `EINVAL' if the file descriptor passed as an
   argument corresponds to a pipe or socket, not an ordinary file.
   But since `fchmod' is a function that is currently only supported
   under MiNT the same error would be reported if the operating
   system does not support the call at all.  It is common practice
   to cache such failures because in the case of `ENOSYS' there
   is no chance that succeeding calls will proceed.  So how do
   you want to distinguish between these cases?  */
#define ETOS_INVAL			(25)		/* Invalid argument.  */

/* Inappropriate file type or format.  The file was the wrong type
   for the operation, or a data file had the wrong format.
   
   On some systems `chmod' returns this error if you try to set the
   sticky bit on a non-directory file.  */
#define ETOS_FTYPE			(26)		/* Inappropriate file type or format.  */

/* While decoding a multibyte character the function came along an
   invalid or an incomplete sequence of bytes or the given wide
   character is invalid.  */
#define ETOS_ILSEQ			(27)		/* Illegal byte sequence.  */

/* Function not implemented.  Some functions have commands or options
   defined that might not be supported in all implementations, and
   this is the kind of error you get if your request them and they are
   not supported.
   
   KERNEL: This was usually called EINVFN (invalid function number) and
   that's why it was probably mixed up with EINVAL (invalid argument).
   
   FIXME: Grep thru the MiNTLib sources for EINVAL and replace it with
   the correct ENOSYS.  Same for the kernel.  */
#define	ETOS_NOSYS			(32)		/* Function not implemented.  */

/* This is a "file doesn't exist" error for ordinary files that are
   referenced in contexts where they are expected to already exist.
   
   KERNEL/FIXME: This used to be the same as ESRCH and and ECHILD,
   in the kernel it was usually called EFILNF.  Fix the wrong usage
   of ESRCH and ECHILD and replace EFILNF with ENOENT.  */
#define	ETOS_NOENT			(33)		/* No such file or directory.  */

/* A file that isn't a directory was specified when a directory is
   required.  The usage of the synonyme EPATH is deprecated!  */
#define ETOS_NOTDIR			(34)		/* Not a directory.  */

/* The current process has too many files open and can't open any
   more.  Duplicate descriptors do count toward this limit.
   
   In BSD and GNU, the number of open files is controlled by a
   resource limit that can usually be increased.  If you get this
   error, you might want to increase the `RLIMIT_NOFILE' limit
   or make it unlimited.  
   
   Under MiNT we currently have to live without such luxury.  */
#define	ETOS_MFILE			(35)		/* Too many open files.  */

/* Permission denied; the file permissions do not allow the
   attempted operation.  
   
   KERNEL: This used to be mixed up with EPERM (operation not permitted).  */
#define ETOS_ACCES			(36)		/* Permission denied.  */

/* Bad file descriptor; for example, I/O on a descriptor that has been
   closed or reading from a descriptor open only for writing (or vice
   versa).  */
#define	ETOS_BADF			(37)		/* Bad file descriptor.  */

/* Operation not permitted; only the owner of the file (or other
   resource) or processes with special privileges can perform the
   operation.
   
   This is a new error code (used to be a synonyme for EACCES).  As
   a general rule you can say, whenever you want to disallow something
   take EACCES.  Only if you positively know that EPERM should be
   used, then take this one.
   
   FIXME: Some kernel functions should return EPERM, for example
   Tsettimeofday, Tsetdate, Tsettime, Setdate ... */
#define ETOS_PERM			(38)		/* Operation not permitted.  */

/* No memory available.  The system cannot allocate more virtual
   memory because its capacity is full.  */
#define	ETOS_NOMEM			(39)		/* Cannot allocate memory.  */

/* Bad address; an invalid pointer was detected.  In the GNU system,
   this error never happens; you get a signal instead.  In the MiNT
   system this error often happens, and you get angry mails and flames
   on the usenet.  */
#define	ETOS_FAULT			(40)		/* Bad address.  */

/* No such device or address.  The system tried to use the device
   represented by a file you specified, and it couldn't find the
   device.  This can mean that the device file was installed 
   incorrectly, or that the physical device is missing or not
   correctly attached to the computer.  
   
   Under MiNT this can also happen if a pathname starts with a
   drive letter followed by a colon (e. g. `x:/dont/exist').  This
   was called `invalid drive specification' or `invalid drive id'.  */
#define	ETOS_NXIO			(46)		/* No such device or address.  */

/* An attempt to make an improper link across file systems was 
   detected.  This happens not only when you use `link' (for
   hard links) but also when you rename a file with `rename'.  */
#define	ETOS_XDEV			(48)		/* Cross-device link.  */

/* No more matching file names, only used by Fsnext(2).  Don't mix
   that up with ENFILE.  */
#define	ETOS_NMFILES		(49)		/* No more matching file names.  */

/* There are too many distinct file openings in the entire system.
   Note that any number of linked channels count as just one file
   opening.  This error never occurs in the GNU system, and it probably
   never occurs in the MiNT system, too.  */
#define ETOS_NFILE			(50)		/* File table overflow.  */
   
/* Locking conflict.  Deprecated.

   KERNEL: If the LOCK_NB flag was selected EWOULDBLOCK should be used
   instead.  Otherwise the process should be blocked until the lock
   is available?  */
#define ETOS_LOCKED			(58)		/* Locking conflict.  */

/* Deprecated, should only happen with Dlock().  */
#define ETOS_NSLOCK			(59)		/* No such lock.  */

/* Bad argument, used to be `range error/context unknown'.  Deprecated.  */
#define	ETOS_BADARG			(64)		/* Bad argument.  */

/* Another spurious error, deprecated.  */
#define	ETOS_INTERNAL		(65)		/* Internal error.  */

/* Invalid executable file format.  This condition is detected by the
   `exec' functions.  */
#define	ETOS_NOEXEC			(66)		/* Invalid executable file format.  */

/* Can't grow block, deprecated.  At least within the library this should
   be handled with ENOMEM.  */
#define	ETOS_SBLOCK			(67)		/* Memory block growth failure.  */

/* Terminated with CTRL-C (Kaos, MagiC, BigDOS).  Nonsense.  */
#define ETOS_BREAK			(68)		/* Aborted by user.  */

/* This looks like a joke but it isn't.  Who has introduced that
   in the kernel?  - fna: comes from MagiC */
#define ETOS_XCPT			(69)		/* Terminated with bombs.  */

/* An attempt to execute a file that is currently open for writing, or
   write to a file that is currently being executed.  Often using a
   debugger to run a program is considered having it open for writing
   and will cause this error.  (The name stands for "text file
   busys".)  This is not an error in the GNU system; the text is
   copied as necessary.  In the MiNT system this error currently cannot
   occur.  */
#define ETOS_TXTBSY			(70)		/* Text file busy.  */

/* File too big; the size of a file would be larger than allowed by
   the system.
   
   KERNEL: This is a new code.  If this error condition can occur,
   then the correct code should be returned.  */
#define ETOS_FBIG			(71)		/* File too big.  */

/* Too many levels of symbolic links were encountered in looking up a
   file name.  This often indicates a cycle of symbolic links.  
   
   KERNEL: There was no difference between EMLINK (too many links and
   ELOOP (too many symbolic links).  The more common meaning of this
   error code is preserved, but whenenver EMLINK should be used, this
   has to be fixed in the kernel.  */
#define ETOS_LOOP			(80)		/* Too many symbolic links.  */

/* Broken pipe; there is no process reading from the other end of a
   pipe.  Every function that returns this error code also generates
   a `SIGPIPE' signal; this signal terminates the program if not
   handled or blocked.  Thus, your program will never actually see
   `EPIPE' unless it has handled or blocked `SIGPIPE'.  */
#define ETOS_PIPE			(81)		/* Broken pipe.  */

/* Too many links; the link count of a single file would become too
   large.  `rename' can cause this error if the file being renamed
   already has as many links as it can take.  
   
   KERNEL: This is a new error code.  See the note for ELOOP.  */
#define ETOS_MLINK			(82)		/* Too many links.  */

/* Directory not empty, where an empty directory was expected.  Typically
   this error occurs when you are trying to delete a directory.
   
   KERNEL: New error code, for the case of non-empty directories
   EEXIST was returned.  The new behavior could cause compatibility
   problems since inproper implementations of recursive rm commands
   could rely on the old error code (85) being returned.
   
   Which error is more probable?  */
#define ETOS_NOTEMPTY		(83)		/* Directory not empty.  */

/* File exists; an existing file was specified in a context where it
   only makes sense to specify a new file.  */
#define ETOS_EXIST			(85)		/* File exists.  */

/* Filename too long (longer than `PATH_MAX') or host name too long
   (in `gethostname' or `sethostname').  */
#define ETOS_NAMETOOLONG 		(86)		/* Name too long.  */

/* Inappropriate I/O control operation, such as trying to set terminal
   modes on ordinary files.  */
#define ETOS_NOTTY			(87)		/* Not a tty.  */

/* Range error; used by mathematical functions when the result value
   is not representable because of overflow or underflow.
   
   The string to number conversion functions (`strtol', `strtoul',
   `strtod', etc. can also return this error.  */
#define ETOS_RANGE			(88)		/* Range error.  */

/* Domain error; used by mathematical functions when an argument
   value does not fall into the domain over which the function is
   defined.  */
#define ETOS_DOM			(89)		/* Domain error.  */

/* Input/output error; usually used for physical read or write errors.  */
#define ETOS_IO				(90)		/* I/O error */

/* No space left on device; write operation on a file failed because
   the disk is full.  */
#define ETOS_NOSPC			(91)		/* No space left on device.  */

/* Error number 92-99 reserved for TraPatch.  */

/* This means that the per-user limit on new process would be
   exceeded by an attempted `fork'.  */
#define ETOS_PROCLIM		(100)		/* Too many processes for user.  */

/* This file quota system is confused because there are too many users.  */
#define ETOS_USERS			(101)		/* Too many users.  */

/* The user's disk quota was exceeded.  */
#define ETOS_DQUOT			(102)		/* Quota exceeded.  */

/* Stale NFS handle.  This indicates an internal confusion in
   the NFS system which is due to file system rearrangements on the
   server host.  Reparing this condition usually requires unmounting
   and remounting the NFS file system on the local host.  */
#define ETOS_STALE			(103)		/* Stale NFS file handle.  */

/* An attempt was made to NFS-mount a remote file system with a file
   name that already specifies an NFS-mounted file.  (This is an
   error on some operating systems, but we expect it to work properly
   on the GNU system, making this error code impossible).  */
#define ETOS_REMOTE			(104)		/* Object is remote.  */

/* ??? */
#define ETOS_BADRPC			(105)	/* RPC struct is bad.  */
#define ETOS_RPCMISMATCH	(106)	/* RPC version wrong.  */
#define ETOS_PROGUNAVAIL	(107)	/* RPC program not available.  */
#define ETOS_PROGMISMATCH	(108)	/* RPC program version wrong.  */
#define ETOS_PROCUNAVAIL	(109)	/* RPC bad procedure for program.  */

/* No locks available.  This is used by the file locking facilities;
   This error is never generated by the GNU system, but it can result
   from an operation to an NFS server running another operating system.  */
#define ETOS_NOLCK			(110)	/* No locks available.  */

/* ??? */
#define ETOS_AUTH			(111)	/* Authentication error.  */
#define ETOS_NEEDAUTH		(112)	/* Need authenticator.  */

/* In the GNU system, servers supporting the `term' protocol return
   this error for certain operations when the caller is not in the
   foreground process group of the terminal.  Users do not usually
   see this error because functions such as `read' and `write'
   translate it into a `SIGTTIN' or `SIGTTOU' signal.  */
#define ETOS_BACKGROUND		(113)		/* Inappropriate operation for background process.  */

/* ??? */ 
#define ETOS_BADMSG			(114)		/* Not a data message.  */
#define ETOS_IDRM			(115)		/* Identifier removed.  */
#define ETOS_MULTIHOP		(116)		/* Multihop attempted.  */
#define ETOS_NODATA			(117)		/* No data available.  */
#define ETOS_NOLINK			(118)		/* Link has been severed.  */
#define ETOS_NOMSG			(119)		/* No message of desired type.  */
#define ETOS_NOSR			(120)		/* Out of streams resources.  */
#define ETOS_NOSTR			(121)		/* Device not a stream.  */
#define ETOS_OVERFLOW		(122)		/* Value too large for defined data type.  */
#define ETOS_PROTO			(123)		/* Protocol error.  */
#define ETOS_TIME			(124)		/* Timer expired.  */

/* Argument list too long; used when the arguments passed to a new
   program being executed with one of the `exec' functions occupy
   too much memory space.  This condition never arises in the
   MiNT system.  */
#define ETOS_2BIG			(125)		/* Argument list too long.  */

/* The following error codes are defined by the Linux/i386 kernel.
   Some of them are probably useful for MiNT, too.  */
#define ETOS_RESTART		(126)		/* Interrupted system call should be
					   restarted.  */
#define ETOS_CHRNG			(127)		/* Channel number out of range.  */

/* Interrupted function call; an asynchronous signal occured and
   prevented completion of the call.  When this happens, you should
   try the call again.
   
   On other systems you can choose to have functions resume after a
   signal handled, rather than failing with `EINTR'.  */
#define ETOS_INTR	        (128)		/* Interrupted function call.  */

/* Falcon XBIOS errors.  */
#define ETOS_SNDLOCKED		(129)		/* Sound system is already locked.  */
#define ETOS_SNDNOTLOCK 	(130)		/* Sound system is not locked.  */

#define ETOS_L2NSYNC		(131)		/* Level 2 not synchronized.  */
#define ETOS_L3HLT			(132)		/* Level 3 halted.  */
#define ETOS_L3RST			(133)		/* Level 3 reset.  */
#define ETOS_LNRNG			(134)		/* Link number out of range.  */
#define ETOS_UNATCH			(135)		/* Protocol driver not attached.  */
#define ETOS_NOCSI			(136)		/* No CSI structure available.  */
#define ETOS_L2HLT			(137)		/* Level 2 halted.  */
#define ETOS_BADE			(138)		/* Invalid exchange.  */
#define ETOS_XFULL			(139)		/* Exchange full.  */
#define ETOS_NOANO			(140)		/* No anode.  */
#define ETOS_BADRQC			(141)		/* Invalid request code.  */
#define ETOS_BADSLT			(142)		/* Invalid slot.  */
#define ETOS_BFONT			(143)		/* Bad font file format.  */
#define ETOS_NONET			(144)		/* Machine is not on the network.  */
#define ETOS_NOPKG			(145)		/* Package is not installed.  */
#define ETOS_ADV			(146)		/* Advertise error.  */
#define ETOS_SRMNT			(147)		/* Srmount error.  */
#define ETOS_COMM			(148)		/* Communication error on send.  */
#define ETOS_DOTDOT			(149)		/* RFS specific error.  */
#define ETOS_LIBACC			(150)		/* Cannot access a needed shared library. */
#define ETOS_LIBBAD			(151)		/* Accessing a corrupted shared library.  */
#define ETOS_LIBSCN			(152)		/* .lib section in a.out corrupted.  */
#define ETOS_LIBMAX			(153)		/* Attempting to link too many shared
						           libraries.  */
#define ETOS_LIBEXEC		(154)		/* Cannot exec a shared library directly. */
#define ETOS_STRPIPE		(155)		/* Streams pipe error.  */
#define ETOS_UCLEAN			(156)		/* Structure needs cleaning.  */
#define ETOS_NOTNAM			(157)		/* Not a XENIX named type file.  */
#define ETOS_NAVAIL			(158)		/* NO XENIX semaphores available.  */
#define ETOS_REMOTEIO		(159)		/* Remote I/O error.  */

/* This is not really an error but a dummy error code used within the kernel
   to indicate that a mount point may have been crossed.  */
#define ETOS_MOUNT			(200)

/* There used to be a distinction between MiNTNet errors and ordinary
   MiNT errors.  This macro is provided for backwards compatibility.  */
#define _NE_BASE 300

/* A file that isn't a socket was specified when a socket is required.  */
#define	ETOS_NOTSOCK		(300)		/* Socket operation on non-socket.  */

/* No default destination address was set for the socket.  You get
   this error when you try to transmit data over a connectionless
   socket, without first specifying a destination for the data with
   `connect'.  */
#define	ETOS_DESTADDRREQ	(301)		/* Destination address required.  */

/* The size of a message sent on a socket was larger than the
   supported maximum size.  */
#define	ETOS_MSGSIZE		(302)		/* Message too long.  */

/* The socket type does not support the requested communications
   protocol.  */
#define	ETOS_PROTOTYPE		(303)		/* Protocol wrong type for socket.  */

/* You specified a socket option that doesn't make sense for the
   particular protocol being used by the socket.  */
#define	ETOS_NOPROTOOPT		(304)		/* Protocol not available.  */

/* The socket domain does not support the requested communications
   protocol (perhaps because the requested protocol is completely
   invalid).  */
#define	ETOS_PROTONOSUPPORT	(305)		/* Protocol not supported.  */

/* The socket type is not supported.  */
#define	ETOS_SOCKTNOSUPPORT	(306)		/* Socket type not supported.  */

/* The operation you requested is not supported.  Some socket
   functions don't make sense for all type of sockets, and others
   may not be implemented for all communications prototcols.  In the
   GNU system, this error can happen for many calls when the object
   does not support the particular operation; it is a generic
   indication that the server knows nothing to do for that
   cal.  */
#define	ETOS_OPNOTSUPP		(307)		/* Operation not supported.  */

/* The socket communications protocol family you requested is not
   supported.  */
#define	ETOS_PFNOSUPPORT	(308)	 	/* Protocol family not supported.  */

/* The address family specified for a socket is not supported; it is
   inconsistent with the protocol being used on the socket.  */
#define	ETOS_AFNOSUPPORT	(309)		/* Address family not supported 
					   by protocol. */
					   
/* The requested socket address is already in use.  */
#define	ETOS_ADDRINUSE		(310)		/* Address already in use */

/* The requested socket address is not available; for example, you
   tried to give a socket a name that doesn't match the local host
   name.  */
#define	ETOS_ADDRNOTAVAIL	(311)		/* Cannot assign requested address.  */

/* A socket operation failed because the network was down.  */
#define	ETOS_NETDOWN		(312)		/* Network is down.  */

/* A socket operation failed because the subnet containing the remote
   host was unreachable.  */
#define	ETOS_NETUNREACH		(313)		/* Network is unreachable.  */

/* A network connection was reset because the remote host crashed.  */
#define	ETOS_NETRESET		(314)		/* Network dropped conn. because of
					   reset.  */

/* A network connnection was aborted locally.  */
#define	ETOS_CONNABORTED	(315)		/* Software caused connection abort.  */

/* A network connection was closed for reasons outside the control of
   the local host, such as by the remote machine rebooting or an
   unrecoverable protocol violation.  */
#define	ETOS_CONNRESET		(316)		/* Connection reset by peer.  */

/* You tried to connect a socket that is already connected.  */
#define	ETOS_ISCONN			(317)		/* Socket is already connected.  */

/* The socket is not connected to anything.  You get this error when
   you try to transmit data over the socket, without first specifying
   a destination for the data.  For a connectionless socket (for
   datagram protocols, such as UDP), you get `EDESTADDRREQ' instead.  */
#define	ETOS_NOTCONN		(318)		/* Socket is not connected.  */

/* The socket has already been shut down.  */
#define	ETOS_SHUTDOWN		(319)		/* Cannot send after shutdown.  */

/* A socket operation with a specified timeout received nor response
   during the timeout period.  */
#define	ETOS_TIMEDOUT		(320)		/* Connection timed out.  */

/* A remote host refused to allow the network connection (typically
   because it is not running the requested service).  */
#define	ETOS_CONNREFUSED	(321)		/* Connection refused.  */

/* The remote host for a requested network connection is down.  */
#define	ETOS_HOSTDOWN		(322)		/* Host is down.  */

/* The remote host for a requested network connection is not
   reachable.  */
#define	ETOS_HOSTUNREACH	(323)		/* No route to host.  */

/* An operation is already in progress on an object that has
   non-blocking mode selected.  */
#define	ETOS_ALREADY		(324)		/* Operation already in progress.  */

/* An operation that cannot complete immediately was initiated on an
   object that has non-blocking mode selected.  Some functions that
   must always block (such as `connect') never return `EAGAIN'.  Instead
   they return `EINPROGRESS' to indicate that the operation has begun and
   will take some time.  Attempts to manipulate the object before the
   call completes return `EALREADY'.  You can use the `select' function
   to find out when the pending operation has completed.  */
#define	ETOS_INPROGRESS		(325)		/* Operation now in progress.  */

/* Resource temporarily unavailable; the call might work if you try
   again later.  The macro `EWOULDBLOCK' is another name for `EAGAIN';
   they are always the same in the MiNT system.
   
   This error can happen in a few different situations:
   
   	* An operation that would block was attempted on an object that
   	  has non-blocking mode selected.  Trying the same operation
   	  again will block until some external condition makes it
   	  possible to read, write or connect (whatever the operation).
   	  You can use `select' to find out when the operation will be
   	  possible.
   	  
   	  Portability Note: In many older Unix systems, this condition
   	  was indicated by `EWOULDBLOCK', which was a distinct error
   	  code different from `EAGAIN'.  To make your program portable,
   	  you should check for both codes and treat them the same.  
   	  In C programs you should not use a case statement for that
   	  because that will trigger a compile-time error for systems
   	  where `EWOULDBLOCK' and `EAGAIN' expand to the same numerical
   	  value.  You should either handle treat it with preprocessor
   	  macros (test if they are equal or not) or use an if conditional.
   	  
   	* A temporary resource shortage made an operation impossible.
   	  `fork' can return this error.  It indicates that the shortage
   	  is expected to pass, so your program can try the call again
   	  later and it may succeed.  It is probably a good idea to
   	  delay for a few seconds before trying it again, to allow time
   	  for other processes to release scarce resources.  Such
   	  shortages are usually fairly serious and affect the whole
   	  system, so usually an interactive program should report the
   	  error to the user and return to its command loop.  */
#define ETOS_AGAIN			(326)		/* Operation would block.  */

/* The kernel's buffer for I/O operations are all in use.  In GNU,
   this error is always synonymous with `ENOMEM'; you may get one or
   the other from network operations.  */
#define ETOS_NOBUFS			(327)		/* No buffer space available.  */

/* Too many references: cannot splice.  */
#define ETOS_TOOMANYREFS	(328)		/* Too many references.  */

/* Function not implemented (POSIX.4 / HPUX) */
#define ETOS_NOTSUP			(350)		/* Function not implemented.  */

/* aio request was canceled before complete (POSIX.4 / HPUX) */
#define ETOS_CANCELLED      (351)       /* aio request was canceled before complete */

/* Owner died */
#define ETOS_OWNERDEAD      (352)       /* Owner died */

/* State not recoverable */
#define ETOS_NOTRECOVERABLE (353)       /* State not recoverable */

#if defined(__KERNEL__) || defined(_LIBC) || defined(__USE_GNU) || defined(__GNUC__)

#define	EERROR			__KERNEL_NEG(ETOS_ERROR)
#define EBUSY			__KERNEL_NEG(ETOS_BUSY)
#define EUKCMD			__KERNEL_NEG(ETOS_UKCMD)
#define ECRC			__KERNEL_NEG(ETOS_CRC)
#define EBADR			__KERNEL_NEG(ETOS_BADR)
#define ESPIPE			__KERNEL_NEG(ETOS_SPIPE)
#define EMEDIUMTYPE		__KERNEL_NEG(ETOS_MEDIUMTYPE)
#define ESECTOR			__KERNEL_NEG(ETOS_SECTOR)
#define EPAPER			__KERNEL_NEG(ETOS_PAPER)
#define EWRITE			__KERNEL_NEG(ETOS_WRITE)
#define EREAD			__KERNEL_NEG(ETOS_READ)
#define EGENERIC		__KERNEL_NEG(ETOS_GENERIC)
#define EROFS			__KERNEL_NEG(ETOS_ROFS)
#define ECHMEDIA		__KERNEL_NEG(ETOS_CHMEDIA)
#define ENODEV			__KERNEL_NEG(ETOS_NODEV)
#define EBADSEC			__KERNEL_NEG(ETOS_BADSEC)
#define ENOMEDIUM		__KERNEL_NEG(ETOS_NOMEDIUM)
#define EINSERT			__KERNEL_NEG(ETOS_INSERT)
#define EDRVNRSP		__KERNEL_NEG(ETOS_DRVNRSP)
#define ESRCH			__KERNEL_NEG(ETOS_SRCH)
#define ECHILD			__KERNEL_NEG(ETOS_CHILD)
#define EDEADLK			__KERNEL_NEG(ETOS_DEADLK)
#define ENOTBLK			__KERNEL_NEG(ETOS_NOTBLK)
#define EISDIR			__KERNEL_NEG(ETOS_ISDIR)
#define EINVAL			__KERNEL_NEG(ETOS_INVAL)
#define EFTYPE			__KERNEL_NEG(ETOS_FTYPE)
#define EILSEQ			__KERNEL_NEG(ETOS_ILSEQ)
#define ENOSYS			__KERNEL_NEG(ETOS_NOSYS)
#define ENOENT			__KERNEL_NEG(ETOS_NOENT)
#define ENOTDIR			__KERNEL_NEG(ETOS_NOTDIR)
#define EMFILE			__KERNEL_NEG(ETOS_MFILE)
#define EACCES			__KERNEL_NEG(ETOS_ACCES)
#define EBADF			__KERNEL_NEG(ETOS_BADF)
#define EPERM			__KERNEL_NEG(ETOS_PERM)
#define ENOMEM			__KERNEL_NEG(ETOS_NOMEM)
#define EFAULT			__KERNEL_NEG(ETOS_FAULT)
#define ENXIO			__KERNEL_NEG(ETOS_NXIO)
#define EXDEV			__KERNEL_NEG(ETOS_XDEV)
#define ENMFILES		__KERNEL_NEG(ETOS_NMFILES)
#define ENFILE			__KERNEL_NEG(ETOS_NFILE)
#define ELOCKED			__KERNEL_NEG(ETOS_LOCKED)
#define ENSLOCK			__KERNEL_NEG(ETOS_NSLOCK)
#define EBADARG			__KERNEL_NEG(ETOS_BADARG)
#define EINTERNAL		__KERNEL_NEG(ETOS_INTERNAL)
#define ENOEXEC			__KERNEL_NEG(ETOS_NOEXEC)
#define ESBLOCK			__KERNEL_NEG(ETOS_SBLOCK)
#define EBREAK			__KERNEL_NEG(ETOS_BREAK)
#define EXCPT			__KERNEL_NEG(ETOS_XCPT)
#define ETXTBSY			__KERNEL_NEG(ETOS_TXTBSY)
#define EFBIG			__KERNEL_NEG(ETOS_FBIG)
#define ELOOP			__KERNEL_NEG(ETOS_LOOP)
#define EPIPE			__KERNEL_NEG(ETOS_PIPE)
#define EMLINK			__KERNEL_NEG(ETOS_MLINK)
#define ENOTEMPTY		__KERNEL_NEG(ETOS_NOTEMPTY)
#define EEXIST			__KERNEL_NEG(ETOS_EXIST)
#define ENAMETOOLONG	__KERNEL_NEG(ETOS_NAMETOOLONG)
#define ENOTTY			__KERNEL_NEG(ETOS_NOTTY)
#define ERANGE			__KERNEL_NEG(ETOS_RANGE)
#define EDOM			__KERNEL_NEG(ETOS_DOM)
#define EIO				__KERNEL_NEG(ETOS_IO)
#define ENOSPC			__KERNEL_NEG(ETOS_NOSPC)
#define EPROCLIM		__KERNEL_NEG(ETOS_PROCLIM)
#define EUSERS			__KERNEL_NEG(ETOS_USERS)
#define EDQUOT			__KERNEL_NEG(ETOS_DQUOT)
#define ESTALE			__KERNEL_NEG(ETOS_STALE)
#define EREMOTE			__KERNEL_NEG(ETOS_REMOTE)
#define EBADRPC			__KERNEL_NEG(ETOS_BADRPC)
#define ERPCMISMATCH	__KERNEL_NEG(ETOS_RPCMISMATCH)
#define EPROGUNAVAIL	__KERNEL_NEG(ETOS_PROGUNAVAIL)
#define EPROGMISMATCH	__KERNEL_NEG(ETOS_PROGMISMATCH)
#define EPROCUNAVAIL	__KERNEL_NEG(ETOS_PROCUNAVAIL)
#define ENOLCK			__KERNEL_NEG(ETOS_NOLCK)
#define EAUTH			__KERNEL_NEG(ETOS_AUTH)
#define ENEEDAUTH		__KERNEL_NEG(ETOS_NEEDAUTH)
#define EBACKGROUND		__KERNEL_NEG(ETOS_BACKGROUND)
#define EBADMSG			__KERNEL_NEG(ETOS_BADMSG)
#define EIDRM			__KERNEL_NEG(ETOS_IDRM)
#define EMULTIHOP		__KERNEL_NEG(ETOS_MULTIHOP)
#define ENODATA			__KERNEL_NEG(ETOS_NODATA)
#define ENOLINK			__KERNEL_NEG(ETOS_NOLINK)
#define ENOMSG			__KERNEL_NEG(ETOS_NOMSG)
#define ENOSR			__KERNEL_NEG(ETOS_NOSR)
#define ENOSTR			__KERNEL_NEG(ETOS_NOSTR)
#define EOVERFLOW		__KERNEL_NEG(ETOS_OVERFLOW)
#define EPROTO			__KERNEL_NEG(ETOS_PROTO)
#define ETIME			__KERNEL_NEG(ETOS_TIME)
#define E2BIG			__KERNEL_NEG(ETOS_2BIG)
#define ERESTART		__KERNEL_NEG(ETOS_RESTART)
#define ECHRNG			__KERNEL_NEG(ETOS_CHRNG)
#define EINTR			__KERNEL_NEG(ETOS_INTR)
#define ESNDLOCKED		__KERNEL_NEG(ETOS_SNDLOCKED)
#define ESNDNOTLOCK		__KERNEL_NEG(ETOS_SNDNOTLOCK)
#define EL2NSYNC		__KERNEL_NEG(ETOS_L2NSYNC)
#define EL3HLT			__KERNEL_NEG(ETOS_L3HLT)
#define EL3RST			__KERNEL_NEG(ETOS_L3RST)
#define ELNRNG			__KERNEL_NEG(ETOS_LNRNG)
#define EUNATCH			__KERNEL_NEG(ETOS_UNATCH)
#define ENOCSI			__KERNEL_NEG(ETOS_NOCSI)
#define EL2HLT			__KERNEL_NEG(ETOS_L2HLT)
#define EBADE			__KERNEL_NEG(ETOS_BADE)
#define EXFULL			__KERNEL_NEG(ETOS_XFULL)
#define ENOANO			__KERNEL_NEG(ETOS_NOANO)
#define EBADRQC			__KERNEL_NEG(ETOS_BADRQC)
#define EBADSLT			__KERNEL_NEG(ETOS_BADSLT)
#define EBFONT			__KERNEL_NEG(ETOS_BFONT)
#define ENONET			__KERNEL_NEG(ETOS_NONET)
#define ENOPKG			__KERNEL_NEG(ETOS_NOPKG)
#define EADV			__KERNEL_NEG(ETOS_ADV)
#define ESRMNT			__KERNEL_NEG(ETOS_SRMNT)
#define ECOMM			__KERNEL_NEG(ETOS_COMM)
#define EDOTDOT			__KERNEL_NEG(ETOS_DOTDOT)
#define ELIBACC			__KERNEL_NEG(ETOS_LIBACC)
#define ELIBBAD			__KERNEL_NEG(ETOS_LIBBAD)
#define ELIBSCN			__KERNEL_NEG(ETOS_LIBSCN)
#define ELIBMAX			__KERNEL_NEG(ETOS_LIBMAX)
#define ELIBEXEC		__KERNEL_NEG(ETOS_LIBEXEC)
#define ESTRPIPE		__KERNEL_NEG(ETOS_STRPIPE)
#define EUCLEAN			__KERNEL_NEG(ETOS_UCLEAN)
#define ENOTNAM			__KERNEL_NEG(ETOS_NOTNAM)
#define ENAVAIL			__KERNEL_NEG(ETOS_NAVAIL)
#define EREMOTEIO		__KERNEL_NEG(ETOS_REMOTEIO)
#ifdef __KERNEL__
#define EMOUNT			__KERNEL_NEG(ETOS_MOUNT)
#endif
#define ENOTSOCK		__KERNEL_NEG(ETOS_NOTSOCK)
#define EDESTADDRREQ	__KERNEL_NEG(ETOS_DESTADDRREQ)
#define EMSGSIZE		__KERNEL_NEG(ETOS_MSGSIZE)
#define EPROTOTYPE		__KERNEL_NEG(ETOS_PROTOTYPE)
#define ENOPROTOOPT		__KERNEL_NEG(ETOS_NOPROTOOPT)
#define EPROTONOSUPPORT	__KERNEL_NEG(ETOS_PROTONOSUPPORT)
#define ESOCKTNOSUPPORT	__KERNEL_NEG(ETOS_SOCKTNOSUPPORT)
#define EOPNOTSUPP		__KERNEL_NEG(ETOS_OPNOTSUPP)
#define EPFNOSUPPORT	__KERNEL_NEG(ETOS_PFNOSUPPORT)
#define EAFNOSUPPORT	__KERNEL_NEG(ETOS_AFNOSUPPORT)
#define EADDRINUSE		__KERNEL_NEG(ETOS_ADDRINUSE)
#define EADDRNOTAVAIL	__KERNEL_NEG(ETOS_ADDRNOTAVAIL)
#define ENETDOWN		__KERNEL_NEG(ETOS_NETDOWN)
#define ENETUNREACH		__KERNEL_NEG(ETOS_NETUNREACH)
#define ENETRESET		__KERNEL_NEG(ETOS_NETRESET)
#define ECONNABORTED	__KERNEL_NEG(ETOS_CONNABORTED)
#define ECONNRESET		__KERNEL_NEG(ETOS_CONNRESET)
#define EISCONN			__KERNEL_NEG(ETOS_ISCONN)
#define ENOTCONN		__KERNEL_NEG(ETOS_NOTCONN)
#define ESHUTDOWN		__KERNEL_NEG(ETOS_SHUTDOWN)
#define ETIMEDOUT		__KERNEL_NEG(ETOS_TIMEDOUT)
#define ECONNREFUSED	__KERNEL_NEG(ETOS_CONNREFUSED)
#define EHOSTDOWN		__KERNEL_NEG(ETOS_HOSTDOWN)
#define EHOSTUNREACH	__KERNEL_NEG(ETOS_HOSTUNREACH)
#define EALREADY		__KERNEL_NEG(ETOS_ALREADY)
#define EINPROGRESS		__KERNEL_NEG(ETOS_INPROGRESS)
#define EAGAIN			__KERNEL_NEG(ETOS_AGAIN)
#define ENOBUFS			__KERNEL_NEG(ETOS_NOBUFS)
#define ETOOMANYREFS	__KERNEL_NEG(ETOS_TOOMANYREFS)
#define ENOTSUP			__KERNEL_NEG(ETOS_NOTSUP)
#define ECANCELLED		__KERNEL_NEG(ETOS_CANCELLED)
#define ECANCELED       ECANCELLED      /* SuSv3 and Solaris wants one 'L' */
#define EOWNERDEAD		__KERNEL_NEG(ETOS_OWNERDEAD)
#define ENOTRECOVERABLE	__KERNEL_NEG(ETOS_NOTRECOVERABLE)

#define EDEADLOCK	EDEADLK
#define EWOULDBLOCK	EAGAIN

#ifdef __LOOSE_ERRROR_CODES
# define EDRNRDY		__KERNEL_NEG(ETOS_BUSY)
# define EDRVNR			__KERNEL_NEG(ETOS_BUSY)
# define EUNCMD			__KERNEL_NEG(ETOS_UKCMD)
# define E_CRC			__KERNEL_NEG(ETOS_CRC)
# define EBADREQ		__KERNEL_NEG(ETOS_BADR)
# define EBADRQ			__KERNEL_NEG(ETOS_BADR)
# define ESEEK			__KERNEL_NEG(ETOS_SPIPE)
# define E_SEEK			__KERNEL_NEG(ETOS_SPIPE)
# define EUKMEDIA		__KERNEL_NEG(ETOS_MEDIUMTYPE)
# define EMEDIA			__KERNEL_NEG(ETOS_MEDIUMTYPE)
# define ESECNF			__KERNEL_NEG(ETOS_SECTOR)
# define EWRITEF		__KERNEL_NEG(ETOS_WRITE)
# define EREADF			__KERNEL_NEG(ETOS_READ)
# define E_CHNG			__KERNEL_NEG(ETOS_CHMEDIA)
# define EUKDEV			__KERNEL_NEG(ETOS_NODEV)
# define EUNDEV			__KERNEL_NEG(ETOS_NODEV)
# define EBADSF			__KERNEL_NEG(ETOS_BADSEC)
# define EIDISK			__KERNEL_NEG(ETOS_NOMEDIUM)
# define EOTHER			__KERNEL_NEG(ETOS_NOMEDIUM)
# define EPATH			__KERNEL_NEG(ETOS_NOTDIR)
# define EACCESS		__KERNEL_NEG(ETOS_ACCES)
# define ENMFIL			__KERNEL_NEG(ETOS_NMFILES)
# define EINTRN			__KERNEL_NEG(ETOS_INTERNAL)
# define EPLFMT			__KERNEL_NEG(ETOS_NOEXEC)
# define EGSBF			__KERNEL_NEG(ETOS_SBLOCK)
#endif

#elif defined(__PUREC__) || defined(__TURBOC__) || defined(__AHCC__)

#include <purec/errno.h>

#endif

#endif /* _MINT_ERRNO_H */
