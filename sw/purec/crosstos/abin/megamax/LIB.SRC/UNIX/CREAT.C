/* LINTLIBRARY */

#include <fcntl.h>

int creat(path, pmode)
char *path;
int pmode;
{
	return (open(path, O_CREAT | O_RDWR | O_TRUNC, pmode));
}
