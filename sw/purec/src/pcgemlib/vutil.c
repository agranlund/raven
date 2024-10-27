#include "gem_vdiP.h"

#undef min
#undef max
#undef abs
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define abs(x) ((x)<0?(-(x)):(x))

void vdi_array2str(const _WORD *src, char *des, _WORD len)
{
	while (len > 0)
	{
		*(des++) = (char) *(src++);
		len--;
	}
	*des = '\0';
}


_WORD vdi_str2array(const char *src, _WORD *des)
{
	_WORD len = 0;
	const unsigned char *c = (const unsigned char *) src;

	while (*c)
	{
		*(des++) = *(c++);
		len++;
	}
	return len;
}


_WORD vdi_str2arrayn(const char *src, _WORD *des, _WORD nmax)
{
	_WORD len = 0;
	const unsigned char *c = (const unsigned char *) src;

	while (len < nmax && *c)
	{
		*(des++) = *(c++);
		len++;
	}
	return len;
}


_WORD vdi_wstrlen(const _WORD *wstr)
{
	register _WORD len = 0;
	
	while (*wstr++)
		len++;
	
	return len;
}
