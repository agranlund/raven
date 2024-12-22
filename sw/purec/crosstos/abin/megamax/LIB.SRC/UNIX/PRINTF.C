
/* LINTLIBRARY */
#include <stdio.h>

/*VARARGS*/
int printf(args)
	char *args;
{
	return _fprintf(stdout, args, &args+1);
}
