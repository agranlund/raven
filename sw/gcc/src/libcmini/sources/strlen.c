/*
 * strlen.c
 *
 *  Created on: 29.05.2013
 *      Author: mfro
 */

#define strlen __inline_strlen
#include <string.h>
#undef strlen

size_t strlen(const char *scan);

size_t strlen(const char *s)
{
#ifdef __OPTIMIZE__
	return __inline_strlen(s);
#else
	const char *start = s;

	while (*s++ != '\0')
		continue;
	return s - start - 1;
#endif
}
