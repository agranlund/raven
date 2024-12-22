/* LINTLIBRARY */
#include	<stdio.h>

/*VARARGS*/
int scanf(args)
char *args;
{
	return _scanf(1, (char *) 0, stdin, &args);
}
