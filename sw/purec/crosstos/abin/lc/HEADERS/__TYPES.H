/**
 * $Id: __types.h,v 1.1 1993/11/14 14:42:42 AGK Rel $
 *
 * Library type definitions
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#define __TYPES_H

/* ANSI X3.159-1989 types */
typedef long __clock_t;
typedef long int __ptrdiff_t;
typedef unsigned long __size_t;
typedef long __time_t;
typedef unsigned char __wchar_t;
typedef char *__va_list;

#define __NULL	((void *) 0)
#define __SEEK_SET	(0)
#define __SEEK_CUR	(1)
#define __SEEK_END	(2)
#define __EOF	(-1)

/* ISO/IEC 9945-1:1990 types */
typedef long __off_t;
typedef short __nlink_t;
typedef short __pid_t;
typedef signed long __ssize_t;
typedef unsigned short __dev_t;
typedef unsigned short __gid_t;
typedef unsigned short __mode_t;
typedef unsigned short __uid_t;
typedef unsigned long __ino_t;
typedef unsigned long __sigset_t;
