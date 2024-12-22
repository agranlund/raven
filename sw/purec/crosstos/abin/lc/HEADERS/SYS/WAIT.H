/**
 * $Id: wait.h,v 1.1 1993/11/14 14:58:56 AGK Rel $
 *
 * POSIX process termination functions and macros
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#define WNOHANG	 1
#define WUNTRACED	2

#define WIFEXITED(x)	((int)((x) & 0xFF00) == 0)
#define WEXITSTATUS(x)	((int)((x) & 0xFF))
#define WIFSIGNALED(x)	(((int)((x) & 0xFF00) > 0) && ((int)(((x) & 0xFF) == 0)))
#define WTERMSIG(x)	((int)(((x) & 0xFF00) >> 8))
#define WIFSTOPPED(x)	(((int)((x) & 0xFF) == 0x7f) && ((int)(((x) >> 8) & 0xFF) != 0))
#define WSTOPSIG(x)	((int)(((x) >> 8) & 0xFF))

__pid_t (wait)(int *);
__pid_t (waitpid)(__pid_t, int *, int);
#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
struct rusage;

__pid_t (wait3)(int *, int, struct rusage *);
__pid_t (wait4)(__pid_t, int *, int, struct rusage *);
#endif
#endif
