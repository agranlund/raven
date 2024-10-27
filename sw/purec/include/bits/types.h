/*
 * Never include this file directly; use <sys/types.h> instead.
 */

#ifndef	_BITS_TYPES_H
#define	_BITS_TYPES_H	1

#ifndef _FEATURES_H
# include <features.h>
#endif

#define __need_size_t
#include <stddef.h>

/*
 * used in place where argument or return value needs to be at
 * least 32 bit. We use int when possible for compatibility to
 * existing headers, but may need a larger type for -mshort
 */
#ifdef __MSHORT__
typedef long __mint_int_or_long;
typedef unsigned long __mint_uint_or_long;
#else
typedef int __mint_int_or_long;
typedef unsigned int __mint_uint_or_long;
#endif

/* Convenience types.  */
typedef unsigned char __u_char;
typedef unsigned short __u_short;
typedef unsigned int __u_int;
typedef unsigned long __u_long;

typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
#ifndef __MSHORT__
typedef signed int __int32_t;
typedef unsigned int __uint32_t;
#else
typedef signed long int __int32_t;
typedef unsigned long int __uint32_t;
#endif /* __MSHORT__ */

#ifdef __GNUC__
typedef signed long long int __int64_t;
typedef unsigned long long int __uint64_t;
#endif  /* GNUC */

#ifndef __NO_LONGLONG
typedef unsigned long long int __u_quad_t;
typedef long long int __quad_t;
#define __quad_low(q) ((long)(q))
#define __uquad_low(q) ((long)(q))
#define __quad_high(q) ((long)((q) >> 32))
#define __uquad_high(q) ((unsigned long)((q) >> 32))
#define __quad_make(q, hi, lo) (q) = ((__quad_t)(hi) << 32) | (unsigned long)(lo)
#else
typedef struct
{
  long __val[2];
} __quad_t;
typedef struct
{
  __u_long __val[2];
} __u_quad_t;
/* We need to know the word order here.  This assumes that the word order
   is consistent with the byte order.  */
# include <endian.h>
# if __BYTE_ORDER == __ORDER_BIG_ENDIAN__
#define __quad_low(q) ((q).__val[1])
#define __uquad_low(q) ((q).__val[1])
#define __quad_high(q) ((q).__val[0])
#define __uquad_high(q) ((q).__val[0])
#else
#define __quad_low(q) ((q).__val[0])
#define __uquad_low(q) ((q).__val[0])
#define __quad_high(q) ((q).__val[1])
#define __uquad_high(q) ((q).__val[1])
#endif
#define __quad_make(q, hi, lo) (__quad_high(q) = (hi), __quad_low(q) = (lo))
#endif  /* __NO_LONGLONG */

typedef __quad_t *__qaddr_t;

/* who the hell made the dev_t type 64bit when there are only 16bit used??? */
typedef __quad_t __dev_t;	/* Type of device numbers.  */
typedef __uint32_t __uid_t;	/* Type of user identifications.  */
typedef __uint32_t __gid_t;	/* Type of group identifications.  */
typedef __uint32_t __ino_t;	/* Type of file serial numbers.  */
typedef __quad_t __ino64_t;	/* Type of file serial numbers (LFS).  */
typedef __uint32_t __mode_t;	/* Type of file attribute bitmasks.  */
typedef __uint32_t __nlink_t;   /* Type of file link counts.  */
typedef __int32_t __off_t;	/* Type of file sizes and offsets.  */
typedef __quad_t __loff_t;	/* Type of file sizes and offsets.  */
typedef __loff_t __off64_t;	/* Type of file sizes and offsets (LFS).  */
typedef int __pid_t;		/* Type of process identifications.  */
typedef signed long int __ssize_t;	/* Type of a byte count, or error.  */
typedef __u_quad_t __fsid_t;	/* Type of file system IDs.  */
typedef __int32_t __clock_t;	/* Type of CPU usage counts.  */
typedef __int32_t __rlim_t;	/* Type for resource measurement.  */
typedef __quad_t __rlim64_t;	/* Type for resource measurement (LFS).  */
typedef __uint32_t __id_t;	/* General type for IDs.  */

/* Everythin' else.  */
typedef __int32_t __daddr_t;	/* The type of a disk address.  */
typedef char* __caddr_t;
typedef __int32_t __time_t;
typedef __uint32_t __useconds_t;
typedef long int __suseconds_t;
typedef __int32_t __swblk_t;	/* Type of a swap block maybe?  */
typedef __int32_t __key_t;	/* Type of an IPC key */

/* Clock ID used in clock and timer functions.  */
typedef int __clockid_t;

/* Timer ID returned by `timer_create'.  */
typedef int __timer_t;

/* One element in the file descriptor mask array.  */
typedef unsigned long int __fd_mask;

/* Number of descriptors that can fit in an `fd_set'.  Note that for
   MiNT this is not equivalent to the number of file descriptors you
   can select simultaneously.  If the kernel implements an Fpoll
   system call this is probably correct.  If it doesn't you are
   still stuck with 32 file descriptors.  Any attempt to exceed this
   limit will result in the error condition EINVAL.  */
#define __FD_SETSIZE	1024

/* It's easier to assume 8-bit bytes than to get CHAR_BIT.  We can 
   also assume that sizeof (__fd_mask) is 4 and thus eliminate the
   divisions and modulo operations.  */
#if 0
#define __NFDBITS	(8 * sizeof (__fd_mask))
#define	__FDELT(d)	((d) / __NFDBITS)
#define	__FDMASK(d)	((__fd_mask) 1 << ((d) % __NFDBITS))
#else
#define __NFDBITS	(8 * 4)
#define	__FDELT(d)	((d) >> 5)
#define	__FDMASK(d)	((__fd_mask) 1 << ((d) & 31))
#endif

/* fd_set for select and pselect.  */
typedef struct
  {
    /* XPG4.2 requires this member name.  Otherwise avoid the name
       from the global namespace.  */
#ifdef __USE_XOPEN
    __fd_mask fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->fds_bits)
#else
    __fd_mask __fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->__fds_bits)
#endif
  } __fd_set;

/* XXX Used in `struct shmid_ds'.  */
#ifndef __ipc_pid_t_defined
typedef __uint16_t __ipc_pid_t;
#define __ipc_pid_t_defined
#endif


/* Type to represent block size.  */
typedef unsigned int __blksize_t;

/* Types from the Large File Support interface.  */

/* Type to count number os disk blocks.  */
typedef long int __blkcnt_t;
typedef __quad_t __blkcnt64_t;

/* Type to count file system blocks.  */
typedef unsigned long __fsblkcnt_t;
typedef __u_quad_t __fsblkcnt64_t;

/* Type to count file system inodes.  */
typedef unsigned long int __fsfilcnt_t;
typedef __u_quad_t __fsfilcnt64_t;

/* Type of miscellaneous file system fields.  */
typedef long int __fsword_t;

/* Used in XTI.  */
typedef int __t_scalar_t;
typedef unsigned int __t_uscalar_t;

/* Duplicates info from stdint.h but this is used in unistd.h.  */
typedef long int __intptr_t;

/* Real type of socklen_t, used in places where unistd.h was not included.  */
typedef unsigned long __socklen_t;

#endif /* bits/types.h */
