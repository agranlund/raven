/* LINTLIBRARY */
#include	<stdio.h>

/*VARARGS1*/
int sscanf(s, args)
char *s;
char *args;
{
	return _scanf(0, s, (FILE *) 0, &args);
}
