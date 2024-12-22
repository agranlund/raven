
#include <stdio.h>
#include <fcntl.h>
#include <osbind.h>

int unlink(path)
char *path;
{
	int i;

    if (i = Fdelete(path)) { 
		errno = i;
 		return -1; 
	}
	else 
		return 0;
}
