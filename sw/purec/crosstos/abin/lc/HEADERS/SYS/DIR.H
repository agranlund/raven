/**
 * $Id: dir.h,v 1.1 1993/11/14 14:58:56 AGK Rel $
 *
 * BSD directory functions
 *
 * (c) Copyright 1991, 1993 HiSoft
**/
 
#ifndef _DIR_H
#define _DIR_H

#define dirent direct

#include <dirent.h>

#undef dirent

#define MAXNAMLEN	13
#define DIRSIZ	128
#endif
