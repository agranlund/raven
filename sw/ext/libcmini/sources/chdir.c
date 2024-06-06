/*
 * chdir.c
 *
 *  Created on: 25.07.2019
 *      Author: og
 */

#include <ext.h>
#include <errno.h>
#include <osbind.h>


int
chdir(char* filename)
{
	int ret = Dsetpath(filename);

	if (ret < 0) {
		errno = ENOENT;
		ret   = -1;
	} else {
		errno = 0;
	}

	return ret;
}
