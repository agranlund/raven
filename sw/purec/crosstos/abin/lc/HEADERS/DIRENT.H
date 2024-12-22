/**
 * $Id: dirent.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * POSIX directory entry manipulation
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _DIRENT_H
#define _DIRENT_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef __LIMITS_H
#include <__limits.h>
#endif

struct dirent {
	__ino_t d_ino;
	__off_t d_off;
	short d_reclen;
	char d_name[__POSIX_NAME_MAX + 1];
};

typedef struct __dir DIR;

DIR *opendir(const char *);
struct dirent *readdir(DIR *);
void rewinddir(DIR *);
int closedir(DIR *);

#if !defined(_POSIX_SOURCE)
__off_t telldir(DIR *);
void seekdir(DIR *, __off_t);
#endif
#endif
