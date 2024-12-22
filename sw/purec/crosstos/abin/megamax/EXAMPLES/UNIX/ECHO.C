#include <stdio.h>

main(argc, argv, envp)
int argc;
char **argv, **envp;
{
	int i;

	puts("*** Command line args:");
	for (i=0; i<argc; i++)
		printf("%d:%s\n", i, argv[i]);

	puts("\n*** Environment vars:");
	for (i=0; envp[i]; i++)
		puts(envp[i]);
	getchar();
}
