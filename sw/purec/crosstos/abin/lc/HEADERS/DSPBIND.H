/**
 * $Id: dspbind.h,v 1.4 1993/11/14 14:43:52 AGK Rel $
 *
 * Interface to Motorola DSP5600x
 *
 * Copyright (c) 1992, 1993 HiSoft
**/

#ifndef _DSPBIND_H
#define _DSPBIND_H

#include <osbind.h>

struct dspblock {
	short blocktype;
	long blocksize;
	void *blockaddr;
};
#endif
