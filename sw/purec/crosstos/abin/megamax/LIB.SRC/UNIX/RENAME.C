
#include <stdio.h>
#include <fcntl.h>
#include <osbind.h>

int rename(from, to)
char *from, *to;
{
	int i;

	Fdelete( to );
    if (i = Frename(0, from, to))  {
		errno = i;
 		return -1; 
	}
	else 
		return 0;
}
