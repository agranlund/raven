/*
 * kludge city: fake out BSD'ish compatible dir routines with
 * our Posix comaptible portable dir stuff
 */

#ifndef _SYS_DIR_H
#define _SYS_DIR_H 1

#ifdef _SYS_DIRENT_H
#  error "<sys/dir.h>: You cannot use both <sys/dirent.h> and <sys/dir.h>.\n\
        <sys/dir.h> is provided for BSD compatibility\n"
#endif

#include <sys/dirent.h>

#define direct		dirent
#define d_namlen	d_reclen

#define DIRSIZ(dp) 	__DIRENTSIZ((dp)->d_namlen)
#define MAXNAMLEN	_LIB_NAME_MAX


#endif
