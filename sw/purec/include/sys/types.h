/*
 *	POSIX Standard: 2.6 Primitive System Data Types	<sys/types.h>
 */

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

#include <bits/types.h>

#ifdef	__USE_BSD
typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;
#endif

typedef __loff_t loff_t;

#ifndef __ino_t_defined
# ifndef __USE_FILE_OFFSET64
typedef __ino_t ino_t;
# else
typedef __ino64_t ino_t;
# endif
# define __ino_t_defined
#endif
#if defined __USE_LARGEFILE64 && !defined __ino64_t_defined
typedef __ino64_t ino64_t;
# define __ino64_t_defined
#endif

#ifndef dev_t
typedef __dev_t dev_t;
# define dev_t dev_t
#endif

#ifndef __gid_t_defined
typedef __gid_t gid_t;
# define __gid_t_defined
#endif

#ifndef __mode_t_defined
typedef __mode_t mode_t;
# define __mode_t_defined
#endif

#ifndef nlink_t
typedef __nlink_t nlink_t;
# define nlink_t nlink_t
#endif

#ifndef __uid_t_defined
typedef __uid_t uid_t;
# define __uid_t_defined
#endif

#ifndef __off_t_defined
# ifndef __USE_FILE_OFFSET64
typedef __off_t off_t;
# else
typedef __off64_t off_t;
# endif
# define __off_t_defined
#endif
#if defined __USE_LARGEFILE64 && !defined __off64_t_defined
typedef __off64_t off64_t;
# define __off64_t_defined
#endif

#ifndef __pid_t_defined
typedef __pid_t pid_t;
# define __pid_t_defined
#endif

#if defined __USE_SVID || defined __USE_XOPEN
typedef __id_t id_t;
#endif

#ifndef __ssize_t_defined
typedef __ssize_t ssize_t;
# define __ssize_t_defined
#endif

#ifdef	__USE_BSD
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;
#endif

#if defined __USE_SVID || defined __USE_XOPEN
#ifndef __key_t_defined
typedef __key_t key_t;
# define __key_t_defined
#endif
#endif

#ifdef __USE_XOPEN
# define __need_clock_t
#endif
#define	__need_time_t
#include <time.h>

#ifdef __USE_XOPEN
# ifndef __useconds_t_defined
typedef __useconds_t useconds_t;
#  define __useconds_t_defined
# endif
# ifndef __suseconds_t_defined
typedef __suseconds_t suseconds_t;
#  define __suseconds_t_defined
# endif
#endif

#define	__need_size_t
#include <stddef.h>

#ifdef __USE_MISC
/* Old compatibility names for C types.  */
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
#endif

/* These size-specific names are used by some of the inet code.  */

#if !__GNUC_PREREQ(2, 7)

/* These types are defined by the ISO C 9x header <inttypes.h>. */
# ifndef __int8_t_defined
#  define __int8_t_defined
typedef	signed char int8_t;
typedef	short int int16_t;
#ifdef __MSHORT__
typedef	long int32_t;
#else
typedef	int int32_t;
#endif
#  ifdef __GNUC__
__extension__ typedef long long int int64_t;
#  endif
# endif

/* But these were defined by ISO C without the first `_'.  */
typedef	unsigned char u_int8_t;
typedef	unsigned short int u_int16_t;
#ifdef __MSHORT__
typedef	unsigned long u_int32_t;
#else
typedef	unsigned int u_int32_t;
#endif
# ifdef __GNUC__
__extension__ typedef unsigned long long int u_int64_t;
# endif

typedef int32_t register_t;

#else

/* For GCC 2.7 and later, we can use specific type-size attributes.  */
# define __intN_t(N, MODE) \
  typedef int int##N##_t __attribute__ ((__mode__ (MODE)))
# define __u_intN_t(N, MODE) \
  typedef unsigned int u_int##N##_t __attribute__ ((__mode__ (MODE)))

# ifndef __int8_t_defined
#  define __int8_t_defined
__intN_t (8, __QI__);
__intN_t (16, __HI__);
__intN_t (32, __SI__);
__intN_t (64, __DI__);
# endif

__u_intN_t (8, __QI__);
__u_intN_t (16, __HI__);
__u_intN_t (32, __SI__);
__u_intN_t (64, __DI__);

typedef int register_t __attribute__ ((__mode__ (__word__)));


/* Some code from BIND tests this macro to see if the types above are
   defined.  */
#endif
#define __BIT_TYPES_DEFINED__	1


#ifdef	__USE_BSD
/* In BSD <sys/types.h> is expected to define BYTE_ORDER.  */
# include <endian.h>

/* It also defines `fd_set' and the FD_* macros for `select'.  */
# include <sys/select.h>

/* BSD defines these symbols, so we follow.  */
# include <sys/sysmacro.h>
#endif /* Use BSD.  */


#if (defined __USE_UNIX98 || defined __USE_XOPEN2K8) && !defined __blksize_t_defined
typedef __blksize_t blksize_t;
# define __blksize_t_defined
#endif

/* Types from the Large File Support interface.  */
#ifndef __USE_FILE_OFFSET64
typedef __blkcnt_t blkcnt_t;	 /* Type to count number of disk blocks.  */
typedef __fsblkcnt_t fsblkcnt_t; /* Type to count file system blocks.  */
typedef __fsfilcnt_t fsfilcnt_t; /* Type to count file system inodes.  */
#else
typedef __blkcnt64_t blkcnt_t;	   /* Type to count number of disk blocks.  */
typedef __fsblkcnt64_t fsblkcnt_t; /* Type to count file system blocks.  */
typedef __fsfilcnt64_t fsfilcnt_t; /* Type to count file system inodes.  */
#endif

#ifdef __USE_LARGEFILE64
typedef __blkcnt64_t blkcnt64_t;     /* Type to count number of disk blocks. */
typedef __fsblkcnt64_t fsblkcnt64_t; /* Type to count file system blocks.  */
typedef __fsfilcnt64_t fsfilcnt64_t; /* Type to count file system inodes.  */
#endif

__END_DECLS

#endif /* _TYPES_H */
