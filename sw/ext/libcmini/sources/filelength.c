/*
 * filelength.c
 *
 *  Created on: 25.07.2019
 *      Author: og
 */

#include <ext.h>
#include <errno.h>
#include <osbind.h>


long
filelength(int handle)
{
	long ret;
	long pos = Fseek(0, handle, SEEK_CUR);

	if (pos < 0) {
		ret   = -1;
		errno = EBADF;
	} else {
		ret = Fseek(0, handle, SEEK_END);

		if (ret < 0) {
			ret   = -1;
			errno = EBADF;
		} else {
			errno = 0;
		}

		Fseek(pos, handle, SEEK_SET);
	}

	return ret;
}
