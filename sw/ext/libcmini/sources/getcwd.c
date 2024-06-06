/*
 * getcwd.c
 *
 *  Created on: 25.07.2019
 *      Author: og
 */

#include <ext.h>
#include <errno.h>
#include <string.h>
#include <osbind.h>


char*
getcwd(char* buffer, int bufflen)
{
	char* buf = buffer;

	if (buf == NULL) {
		buf = malloc(bufflen);
	}

	if (buf == NULL) {
		errno = ENOMEM;
	} else {
		char path[MAXPATH];

		path[0] = 'A' + Dgetdrv();
		path[1] = ':';

		if (Dgetpath(&path[2], 0) < 0) {
			errno = ENODEV;
		} else {
			strncpy(buf, path, bufflen);
			buf[bufflen] = '\0';

			if (bufflen <= strlen(path)) {
				errno = ERANGE;
			} else {
				errno = 0;
			}
		}
	}

	return buf;
}
