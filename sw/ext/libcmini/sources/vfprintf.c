#include <stdio.h>
#include <stdarg.h>
#include "lib.h"

static int last_char;

static int fpc(int c, void *fp)
{
	int   ret    = '\0';
	FILE* stream = fp;

	if (!stream->__mode.__binary) {
		if (c == '\n' && last_char != '\r') {
			if (putc('\r', stream) == EOF) {
				ret = EOF;
			}
		}

		last_char = c;
	}

	if (ret != EOF) {
		ret = putc(c, stream);
	}

	return ret;
}

int vfprintf(FILE *stream, const char *format, va_list ap)
{
	return doprnt(fpc, stream, format, ap);
}


