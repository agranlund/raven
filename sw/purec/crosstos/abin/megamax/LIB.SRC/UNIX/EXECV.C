extern char **envp;
extern char *environ;

int execv(name, argv)
char *name, **argv;
{
	return execve(name, argv, environ);
}
