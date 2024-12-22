/**
 * $Id: limits.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * ANSI & POSIX system limits
 *
 * (c) Copyright 1992 HiSoft
**/

#ifndef _LIMITS_H
#define _LIMITS_H

#define CHAR_BIT (8)

#define SCHAR_MAX (127)
#define SCHAR_MIN (-128)
#define UCHAR_MAX (255)

#ifdef _UNSIGNEDCHAR
#define CHAR_MAX UCHAR_MAX
#define CHAR_MIN (0)
#else
#define CHAR_MAX SCHAR_MAX
#define CHAR_MIN SCHAR_MIN
#endif

#define SHRT_MAX (32767)
#define SHRT_MIN (-32767)
#define USHRT_MAX (65535)

#define LONG_MAX (2147483647)
#define LONG_MIN (-2147483647)
#define ULONG_MAX (4294967295)

#ifdef _SHORTINT
#define INT_MAX SHRT_MAX
#define INT_MIN SHRT_MIN
#define UINT_MAX USHRT_MAX
#else
#define INT_MAX LONG_MAX
#define INT_MIN LONG_MIN
#define UINT_MAX ULONG_MAX
#endif

#define MB_LEN_MAX	(1)

#if !defined(_ANSI) \
	|| defined(_POSIX_SOURCE)
#ifndef __LIMITS_H
#include <__limits.h>
#endif

#define _POSIX_ARG_MAX		4096
#define _POSIX_CHILD_MAX	6
#define _POSIX_LINK_MAX		8
#define _POSIX_MAX_CANON	255
#define _POSIX_MAX_INPUT	255
#define _POSIX_NAME_MAX		14
#define _POSIX_NGROUPS_MAX	0
#define _POSIX_OPEN_MAX		16
#define _POSIX_PATH_MAX		255
#define _POSIX_PIPE_BUF		512
#define _POSIX_SSIZE_MAX	32767
#define _POSIX_STREAM_MAX	8
#define _POSIX_TZNAME_MAX	3

#define	NGROUPS_MAX	512

#define OPEN_MAX	32
#define STREAM_MAX	32
#define TZNAME_MAX	8

#define	SSIZE_MAX	LONG_MAX
#endif
#endif
