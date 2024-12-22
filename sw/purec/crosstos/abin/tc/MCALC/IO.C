/* IO.C
   Source File for porting applications using low level I/O-
   functions from TC on MSDOS to TC on Atari ST

   Copyright (c)  Heimsoeth & Borland  1988

*/


#include <stdio.h>
#include <ext.h>
#include <errno.h>
#include "io.h"

#pragma warn -par /* Warnung bei unbenutztem Parameter ausschalten */
int access(const char *name, int amode)
{       struct ffblk dta;
        amode = ((amode & 0x02) ? 0x16 : 0x17);
        return((int)(findfirst((char *)name, &dta, amode) != 0 /* E_OK */));
}

int _chmod(const char *filename, int func, ...)
{       return(EIO);
}

int chmod(const char *filename, int func)
{       return(EIO);
}

int _creat(const char *filename, int attrib)
{       return(creat(filename, attrib));
}

int creatnew(const char *filename, int attrib)
{       return(creat(filename, attrib));
}

int eof(int handle)
{       return(EIO);
}

int ioctl(int handle, int cmd, ...)
{       return(EIO);
}

int _open(const char *pathname, int access)
{       return(open(pathname, access));
}

int setmode(int handle, int mode)
{       return(EIO);
}

#pragma warn .par
