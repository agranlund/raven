/*
 * UNIX like execve().
 */

#include <stdio.h>
#include <osbind.h>

int execve(path, argv, envp)
char *path;
char *argv[], *envp[];
/*
 * Returns exit status of program.
 */
{
	int i;
	char tail[130];
	register char *tp, *sp, **cpp;
	char *env;					/* created env. string */
	char *ARGV = NULL;			/* ARGV variable for long command lines */
	int status;					/* Exit() status of program */

	/* Copy params to tail */

	if (argv) 					/* Skip first argument (program name) */
		argv++;

	tp = tail;
	if (argv && *argv) {
		for (i=0; argv[i]; i++) {
			if (tp-tail + strlen(argv[i])+1 > 128)
				break;
			for (sp=argv[i]; *sp;)
				*++tp = *sp++;
			*++tp = ' ';
		}
		tp--;					/* backup over last ' ' */

		/* Place rest of command line in ARGV environment variable */
		if (argv[i]) {
			int j, bytes = 0;
			char *p, *q, **cpp;

			for (j=i; argv[j]; j++)
				bytes += strlen(argv[j])+1;
			ARGV = malloc(bytes + 7);

			strcpy(ARGV, "MMARGV=");
			for (j=i, p=ARGV+7; argv[j]; j++) {
				for (q=argv[j]; *q;)
					*p++ = *q++;
				*p++ = ' ';
			}
			*(p-1) = 0;
		}
	}
	tail[0] = tp-tail;	/* command tail has length byte at front */

	/* Create environment string now */
	for (i = 0, cpp = envp; cpp && *cpp; i += strlen(*cpp)+1, cpp++);
	if (ARGV)
		i += strlen(ARGV)+1;
	env = malloc(i+1);
	for (i = 0, cpp = envp; cpp && *cpp; i += strlen(*cpp)+1, cpp++)
		strcpy(env+i, *cpp);
	if (ARGV) {
		strcpy(env+i, ARGV);
		i += strlen(ARGV)+1;
	}
	env[i] = 0;					/* terminates env. string */

	status = Pexec(0, path, tail, env);

	free(env);
	if (ARGV)
		free(ARGV);

	return status;
}
