#include <stdio.h>
#include <unistd.h>
#include <mint/osbind.h>
#include <sys/types.h>
#include "lib.h"

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	long rc = Fwrite((int)FILE_GET_HANDLE(stream), size * nmemb, ptr);

	if (rc < 0)
	{
		stream->__error = 1;
		__set_errno(-rc);
		rc = 0;
	} else
	{
		rc /= size;
	}

	return rc;
}
