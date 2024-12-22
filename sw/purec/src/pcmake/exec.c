#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#if defined(__TOS__) || defined(__atarist__)
#ifdef __GNUC__
#include <osbind.h>
#define DOSTIME _DOSTIME
#else
#include <tos.h>
#endif
#define CROSSTOS_PREFIX 
#define EXE_EXT ".ttp"
#define PATH_SEP "\\"
#else
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#define CROSSTOS_PREFIX "m68k-atari-tos-pc-"
#define EXE_EXT ""
#define PATH_SEP "/"
#endif
#include "pcmake.h"

static char *pc_dir;
static char *pc_bindir;
static char *pc_libdir;
static char *pc_includedir;
static char *compiler_executable;
static char *linker_executable;
static char *assembler_executable;
static char *ahcc_executable;
static char *ahcl_executable;

extern char **environ;

/**************************************************************************/
/* ---------------------------------------------------------------------- */
/**************************************************************************/

/*
 * return next element of PATH.
 * Similar to strtok(s, ";,:"), but handle ':'
 * specially when it delimits a single drive letter only
 */
static char *pathtok(char *s, char **scanpoint)
{
	char *scan;
	char *tok;

	if (s == NULL && *scanpoint == NULL)
		return NULL;
	if (s != NULL)
		scan = s;
	else
		scan = *scanpoint;

	/*
	 * Skip leading delimiters.
	 */
	while (*scan == ';' || *scan == ',' || *scan == ':')
		scan++;

	if (*scan == '\0') 
	{
		*scanpoint = NULL;
		return NULL;
	}

	tok = scan;

	/*
	 * Scan token.
	 */
	for (; *scan != '\0'; scan++) 
	{
		if (*scan == ';' || *scan == ',' || (*scan == ':' && scan > tok + 1))
		{
			*scanpoint = scan + 1;
			*scan = '\0';
			return tok;
		}
	}

	/*
	 * Reached end of string.
	 */
	*scanpoint = NULL;
	return tok;
}

/* ---------------------------------------------------------------------- */

void set_pcdir(const char *argv0)
{
	if (argv0 == NULL || argv0[0] == '\0')
	{
		pc_dir = g_strdup("C:\\pc");
		pc_bindir = pc_dir;
	} else
	{
		char *t;
		
		pc_dir = dirname(argv0);
		if (*pc_dir == '\0')
		{
			/*
			 * toswin2/bash might give us only the name, not the path.
			 */
			char *scanpoint = NULL;
			char *pathbuf;
			char *p;
			char *file;

			pathbuf = g_strdup(getenv("PATH"));
			if (pathbuf != NULL)
			{
				for (p = pathtok(pathbuf, &scanpoint); p != NULL; p = pathtok(NULL, &scanpoint))
				{
					file = build_path(p, argv0);
					if (file != NULL)
					{
						if (file_exists(file))
						{
							g_free(pc_dir);
							pc_dir = build_path(p, NULL);
							break;
						}
						g_free(file);
					}
				}
				g_free(pathbuf);
			}
		}
		t = strrslash(pc_dir);
		if (t && t[1] == '\0')
		{
			*t = '\0';
			t = strrslash(pc_dir);
		}
		pc_bindir = g_strdup(pc_dir);
		strbslash(pc_bindir);
		if (t && stricmp(t + 1, "bin") == 0)
		{
			*t = '\0';
		}
	}
	pc_libdir = build_path(pc_dir, "lib");
	pc_includedir = build_path(pc_dir, "include");
}

/* ---------------------------------------------------------------------- */

void exec_exit(void)
{
	if (pc_bindir != pc_dir)
		g_free(pc_bindir);
	g_free(pc_dir);
	pc_dir = NULL;
	pc_bindir = NULL;
	g_free(pc_libdir);
	pc_libdir = NULL;
	g_free(pc_includedir);
	pc_includedir = NULL;
	g_free(compiler_executable);
	compiler_executable = NULL;
	g_free(ahcc_executable);
	ahcc_executable = NULL;
	g_free(assembler_executable);
	assembler_executable = NULL;
	g_free(linker_executable);
	linker_executable = NULL;
	g_free(ahcl_executable);
	ahcl_executable = NULL;
}

/* ---------------------------------------------------------------------- */

const char *get_pcdir(void)
{
	return pc_dir;
}

/* ---------------------------------------------------------------------- */

const char *get_libdir(void)
{
	return pc_libdir;
}

/* ---------------------------------------------------------------------- */

const char *get_includedir(void)
{
	return pc_includedir;
}

/* ---------------------------------------------------------------------- */

const char *get_compiler_executable(void)
{
	if (compiler_executable == NULL)
	{
		compiler_executable = build_path(pc_bindir, CROSSTOS_PREFIX "pcc" EXE_EXT);
		if (!file_exists(compiler_executable))
		{
			g_free(compiler_executable);
			compiler_executable = g_strdup(CROSSTOS_PREFIX "pcc" EXE_EXT);
		}
	}
	return compiler_executable;
}

/* ---------------------------------------------------------------------- */

const char *get_assembler_executable(void)
{
	if (assembler_executable == NULL)
	{
		assembler_executable = build_path(pc_bindir, CROSSTOS_PREFIX "pasm" EXE_EXT);
		if (!file_exists(assembler_executable))
		{
			g_free(assembler_executable);
			assembler_executable = g_strdup(CROSSTOS_PREFIX "pasm" EXE_EXT);
		}
	}
	return assembler_executable;
}

/* ---------------------------------------------------------------------- */

const char *get_linker_executable(void)
{
	if (linker_executable == NULL)
	{
		linker_executable = build_path(pc_bindir, CROSSTOS_PREFIX "plink" EXE_EXT);
		if (!file_exists(linker_executable))
		{
			g_free(linker_executable);
			linker_executable = g_strdup(CROSSTOS_PREFIX "plink" EXE_EXT);
		}
	}
	return linker_executable;
}

/* ---------------------------------------------------------------------- */

const char *get_ahcc_executable(void)
{
	if (ahcc_executable == NULL)
	{
		ahcc_executable = build_path(pc_bindir, CROSSTOS_PREFIX "ahcccf" EXE_EXT);
		if (!file_exists(ahcc_executable))
		{
			g_free(ahcc_executable);
			ahcc_executable = g_strdup(CROSSTOS_PREFIX "ahcccf" EXE_EXT);
		}
	}
	return ahcc_executable;
}

/* ---------------------------------------------------------------------- */

const char *get_ahcl_executable(void)
{
	if (ahcl_executable == NULL)
	{
		ahcl_executable = build_path(pc_bindir, CROSSTOS_PREFIX "ahclcf" EXE_EXT);
		if (!file_exists(ahcl_executable))
		{
			g_free(ahcl_executable);
			ahcl_executable = g_strdup(CROSSTOS_PREFIX "ahclcf" EXE_EXT);
		}
	}
	return ahcl_executable;
}

/**************************************************************************/
/* ---------------------------------------------------------------------- */
/**************************************************************************/

/*
 * Find a command given a partially qualified command name
 */
static char *findcmd(const char *cmd)
{
	char *pathbuf;
	char *path, *p;
	char *file;
	char *baseptr;
	int hassuf;
	int i;
	char *scanpoint = NULL;
	static const char *const suf[] =
	{
		"",
		".ttp",
		".tos",
		".prg",
		".app",
		(char *) 0
	};
	
	baseptr = basename(cmd);

	hassuf = strrchr(baseptr, '.') != NULL;

	if (baseptr != cmd && hassuf)	   /* absolute path with suffix */
		return g_strdup(cmd);

	path = getenv("PATH");

	if (baseptr != cmd || path == NULL)
	{								   /* absolute, or no path */
		for (i = 0; suf[i] != NULL; i++)
		{							   /* abs path, no suf */
			file = g_new(char, strlen(cmd) + strlen(suf[i]) + 1);
			if (file == NULL)
				return NULL;
			strcat(strcpy(file, cmd), suf[i]);
			if (file_exists(file))
				return file;
			g_free(file);
		}
		return g_strdup(cmd);					   /* will have to do */
	}
	pathbuf = g_strdup(path);
	if (pathbuf == NULL)
		return g_strdup(cmd);
	
	for (p = pathtok(pathbuf, &scanpoint); p != NULL; p = pathtok(NULL, &scanpoint))
	{
		if (hassuf)
		{
			file = build_path(p, cmd);
			if (file == NULL)
			{
				g_free(pathbuf);
				return NULL;
			}
			if (file_exists(file))
			{
				g_free(pathbuf);
				return file;
			}
			g_free(file);
		} else
		{
			for (i = 0; suf[i] != NULL; i++)
			{
				file = g_new(char, strlen(p) + 1 + strlen(cmd) + strlen(suf[i]) + 1);
				if (file == NULL)
				{
					g_free(pathbuf);
					return NULL;
				}
				strcat(strcat(strcat(strcpy(file, p), PATH_SEP), cmd), suf[i]);
				if (file_exists(file))
				{
					g_free(pathbuf);
					return file;
				}
				g_free(file);
			}
		}
	}
	g_free(pathbuf);
	return g_strdup(cmd);						   /* will have to do */
}

/* ---------------------------------------------------------------------- */

#if defined(__TOS__) || defined(__atarist__)
static char *copyenv(const char *const *argv)
{
	char **parent_env;
	const char *p;
	char *env, *envp;
	size_t size, len;
	int i;
	
	size = 0;
	parent_env = environ;
	while (*parent_env != NULL)
	{
		p = *parent_env++;
		len = strlen(p) + 1;
		size += len;
	}
	size += sizeof("ARGV=");
	for (i = 0; (p = argv[i]) != NULL; i++)
		size += strlen(p) + 1;
	size += 1;
	env = g_new(char, size);
	if (env != NULL)
	{
		envp = env;
		/* copy environment */
		parent_env = environ;
		while (*parent_env != NULL)
		{
			p = *parent_env++;
			if (strncmp(p, "ARGV=", 5) != 0)
			{
				while ((*envp++ = *p++) != '\0')
					;
			}
		}
		p = "ARGV=";
		while ((*envp++ = *p++) != '\0')
			;
		/* copy arguments */
		for (i = 0; (p = argv[i]) != NULL; i++)
		{
			while ((*envp++ = *p++) != '\0')
				;
		}
		*envp++ = '\0';
	}
	return env;
}
#endif


static int do_exec(int argc, const char **argv)
{
	char *path;
	int result = -1;
	
	path = findcmd(argv[0]);
	if (path != NULL)
	{
#if defined(__TOS__) || defined(__atarist__)
		char *env;
		char tail[128];
		size_t n;
		int i;
		
		env = copyenv(argv);
		if (env == NULL)
		{
			result = -1;
		} else
		{
			tail[0] = 127;
			tail[1] = 0;
			n = 0;
			for (i = 1; i < argc; i++)
			{
				if (i != 1)
					n += 1;
				n += strlen(argv[i]);
			}
			if (n <= 126)
			{
				/* tail[0] = (char) n; */
				for (i = 1; i < argc; i++)
				{
					if (i != 1)
						strcat(&tail[1], " ");
					strcat(&tail[1], argv[i]);
				}
			}
			
			result = (int)Pexec(0, path, tail, env);
			if (result == -33)
			{
				errno = ENOENT;
				result = -1;
			} else if (result < 0)
			{
				result = 1;
			}
			g_free(env);
		}
#else
		int pipefds[2];
		int count, err;
		pid_t child;

		(void)argc;
		if (pipe(pipefds) == 0 &&
			fcntl(pipefds[1], F_SETFD, fcntl(pipefds[1], F_GETFD) | FD_CLOEXEC) == 0)
		{
			switch (child = fork())
			{
			case -1:
				result = -1;
				break;
			case 0:
				close(pipefds[0]);
				execv(path, (char **)argv);
				write(pipefds[1], &errno, sizeof(int));
				_exit(0);
			default:
				close(pipefds[1]);
				while ((count = read(pipefds[0], &err, sizeof(errno))) == -1)
					if (errno != EAGAIN && errno != EINTR)
						break;
				if (count)
				{
					if (count == sizeof(errno))
						errno = err;
					else
						errno = EINTR;
					result = -1;
				} else
				{
					/*
					 * we get here if we could not read the errno from the pipe,
					 * because the pipe was closed by a successful execvp() in the child
					 */
					close(pipefds[0]);
					while (waitpid(child, &err, 0) == -1)
					{
						if (errno != EINTR)
						{
							result = -1;
							break;
						}
					}
					if (WIFEXITED(err))
						result = WEXITSTATUS(err);
					else if (WIFSIGNALED(err))
						result = WTERMSIG(err) + 128;
				}
			}
		}
#endif
		g_free(path);
	}
	if (result < 0)
	{
		errout("%s: %s: %s", program_name, argv[0], strerror(errno));
	}
	
	return result;
}


int linker(int argc, const char **argv)
{
	return do_exec(argc, argv);
}


int compiler(int argc, const char **argv)
{
	return do_exec(argc, argv);
}
