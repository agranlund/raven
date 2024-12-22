
#include <osbind.h>
#include <stdio.h>

int close(fildes)
int fildes;
{
	int i;

    _binrem(fildes);
    if (fildes <= STDIN)
		return 0;

    if (i = Fclose(fildes))  {
		errno = i;
		return -1;
	}
	else 
		return 0;
}
