/*  bits/stdiolim.h -- MiNTLib.
    Copyright (C) 2000 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifdef	_STDIO_H

#define L_tmpnam 128

#ifdef __MSHORT__
/* Maximum number temporary file names you can create with tmpnam.  
   Actually the values are much higher (you will run out of inodes 
   before you reach the limit) but TMP_MAX should allow a comparison
   against an integer.  */
# define TMP_MAX 32767
#else
# define TMP_MAX 2147483647L
#endif

#ifdef __USE_POSIX
# define L_ctermid 128
# define L_cuserid 80
#endif
#define FILENAME_MAX (128)
#undef __need_FOPEN_MAX
#define __need_FOPEN_MAX 1
#endif

#if defined __need_FOPEN_MAX && !defined FOPEN_MAX
#define FOPEN_MAX 256
#endif
#undef __need_FOPEN_MAX

#if defined __need_IOV_MAX && !defined IOV_MAX
# define IOV_MAX 1024 /* sysctl(KERN_IOV_MAX) */
#endif
#undef __need_IOV_MAX
