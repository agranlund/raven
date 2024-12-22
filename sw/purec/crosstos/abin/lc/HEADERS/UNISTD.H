/**
 * $Id: unistd.h,v 1.2 1993/11/14 14:43:52 AGK Rel $
 *
 * POSIX.1 miscellaneous macros and functions
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _UNISTD_H
#define _UNISTD_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef __size_t size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef __ssize_t ssize_t;
#endif

#ifndef NULL
#define NULL	__NULL
#endif

#ifndef SEEK_SET
#define SEEK_SET	0	/* Seek from beginning of file */
#endif
#ifndef SEEK_CUR
#define SEEK_CUR	1	/* Seek from current file position */
#endif
#ifndef SEEK_END
#define SEEK_END	2	/* Seek from end of file */
#endif

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

#define F_OK	0
#define X_OK	1
#define W_OK	2
#define R_OK	4

#define _POSIX_VDISABLE	0
#define _POSIX_JOB_CONTROL
#define _POSIX_CHOWN_RESTRICTED	0

__pid_t (getpid)(void);
__pid_t (getppid)(void);

__uid_t (getuid)(void);
__uid_t (geteuid)(void);
__gid_t (getgid)(void);
__gid_t (getegid)(void);
int (setuid)(__uid_t);
int (setgid)(__gid_t);
int (getgroups)(int, __gid_t []);
char *(getlogin)(void);
__pid_t (getpgrp)(void);
__pid_t (setsid)(void);
int (setpgid)(__pid_t, __pid_t);

#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
__pid_t setpgrp(__pid_t, __pid_t);
__gid_t setregid(__gid_t, __gid_t);
__uid_t setreuid(__uid_t, __uid_t);
#endif

int pause(void);
void _exit(int);
int access(const char *, int);
int chown(const char *, __uid_t, __gid_t);
int close(int);
int dup(int);
int dup2(int, int);
int isatty(int);
__off_t lseek(int, __off_t, int);
int pipe(int *);
ssize_t read(int, void *, size_t);
ssize_t write(int, const void *, size_t);

int chdir(const char *);
char *getcwd(char *, int);
int rmdir(const char *);

#if !defined(_POSIX_SOURCE) \
	|| defined(_LATTICE)
__off_t filelength(int);
int iomode(int, int);
__off_t tell(int);
#endif

__pid_t fork(void);
int execl(const char *, ...);
int execle(const char *, ...);
int execlp(const char *, ...);
int execlpe(const char *, ...);
int execv(const char *, const char **);
int execve(const char *, const char **, const char **);
int execvp(const char *, const char **);
int execvpe(const char *, const char **, const char **);

#if !defined(_POSIX_SOURCE) \
	|| defined(_BSD)
__pid_t vfork(void);
#endif

#if !defined(_POSIX_SOURCE)
void *sbrk(size_t);
extern char **environ;
#endif
#endif
