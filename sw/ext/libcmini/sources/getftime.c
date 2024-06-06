/*
 * getftime.c
 *
 *  Created on: 25.07.2019
 *      Author: og
 */

#include <ext.h>
#include <errno.h>
#include <osbind.h>


int
getftime(int handle, struct ftime* ftimep)
{
	int ret = 0;

	if (handle < 0 || Fseek(0, handle, SEEK_CUR) < 0) {
		ret   = -1;
		errno = EBADF;
	} else {
		Fdatime(ftimep, handle, 0);

		ret   = 0;
		errno = 0;
	}

	return ret;
}
