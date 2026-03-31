#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void)
{
	struct stat st;
	int ret;
	char path[128];

	strcpy(path, "C:\\");
	errno = 0;
	ret = stat(path, &st);
	printf("stat %s: %d (%s)\n", path, ret, strerror(errno));

	strcpy(path, "C:");
	errno = 0;
	ret = stat(path, &st);
	printf("stat %s: %d (%s)\n", path, ret, strerror(errno));

	strcpy(path, "C:\\etc");
	errno = 0;
	ret = stat(path, &st);
	printf("stat %s: %d (%s)\n", path, ret, strerror(errno));

	return 0;
}
