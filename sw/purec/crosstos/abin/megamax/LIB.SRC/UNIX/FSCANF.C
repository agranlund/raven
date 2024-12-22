/* LINTLIBRARY */
#include	<stdio.h>

/*VARARGS1*/
int fscanf(stream, args)
FILE *stream;
char *args;
{
	return _scanf(1, (char *) 0, stream, &args);
}
