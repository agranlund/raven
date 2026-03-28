/*
 * strcpy.c
 *
 *  Created on: 29.05.2013
 *      Author: mfro
 */

#include <string.h>
#undef strcpy

char *strcpy(char *dst, const char *src)
{
#ifdef __OPTIMIZE__
	return __inline_strcpy(dst, src);
#else
	char *dscan = dst;
	const char *sscan = src;

	while ((*dscan++ = *sscan++) != '\0')
		continue;
	return dst;
#endif
}
