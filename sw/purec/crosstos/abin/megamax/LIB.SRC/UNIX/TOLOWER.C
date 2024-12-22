
#include <ctype.h>

int tolower(c)
int c;
{
    if (c >= 'A' && c <= 'Z')
	return _tolower(c);
    else
	return c;
}
