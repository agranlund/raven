/*
 * findfirst.c
 *
 *  Created on: 25.07.2019
 *      Author: og
 */

#include <ext.h>
#include <errno.h>
#include <osbind.h>


int
findfirst(const char* filename, struct ffblk* ffblk, int attrib)
{
	_DTA* dta = Fgetdta();
	int   ret;

    // struct ffblk is identical to _DTA
	Fsetdta(ffblk);
	ret = Fsfirst(filename, attrib);

	Fsetdta(dta);

	if (ret < 0) {
		errno = ret;
		ret   = -1;
	} else {
		errno = 0;
	}

	return ret;
}
