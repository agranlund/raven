#include <stdio.h>

extern char **environ;
extern char *index();

char *getenv(name)
char *name;
{
	register char **p, *q;

	for (p = environ; p && *p; p++)
		if (q = index(*p, '='))
			if (!strncmp(name, *p, q - *p))
				return q+1;
	return NULL;
}
