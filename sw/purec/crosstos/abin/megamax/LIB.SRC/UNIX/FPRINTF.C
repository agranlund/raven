/* LINTLIBRARY */
#include <stdio.h>

/*VARARGS1*/
int fprintf(stream, args)
FILE *stream;
char *args;
{
	return _fprintf(stream, args, &args+1);
}
