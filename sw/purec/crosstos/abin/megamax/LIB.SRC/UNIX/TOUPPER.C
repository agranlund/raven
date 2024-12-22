
#include <ctype.h>

int toupper(c)
int c;
{
    if (c >= 'a' && c <= 'z')
	return _toupper(c);
    else
	return c;
}
