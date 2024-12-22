#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#if defined(__TOS__) || defined(__atarist__)
#ifdef __GNUC__
#include <osbind.h>
#define DTA _DTA
#else
#include <tos.h>
#endif
#ifndef DISABLE_NATFEATS
#include <mint/arch/nf_ops.h>
#endif
#else
#include <sys/stat.h>
#include <unistd.h>
#endif
#include "pcmake.h"

char const Warning[] = N_("warning");
char const Error[] = N_("error");
char const suff_o[] = ".o";
char const suff_prg[] = ".prg";
bool errout_nfdebug;

#define bslash '\\'
#define fslash '/'


/* ---------------------------------------------------------------------- */

void errout_va(const char *format, va_list args)
{
	vfprintf(stderr, format, args);
	fputc('\n', stderr);
#if defined(__TOS__) || defined(__atarist__)
#ifndef DISABLE_NATFEATS
	if (errout_nfdebug)
	{
		nf_debugvprintf(format, args);
		nf_debug("\n");
	}
#endif
#endif
}

/* ---------------------------------------------------------------------- */

void errout(const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	errout_va(format, args);
	va_end(args);
}

/* ---------------------------------------------------------------------- */

void oom(size_t size)
{
	errout(_("Not enough memory for %lu bytes"), (unsigned long)size);
	exit(1);
}

/* ---------------------------------------------------------------------- */

#if DEBUG_ALLOC < 2
char *g_strndup(const char *str, size_t len)
{
	char *p;

	if (str == NULL)
		return NULL;
	if ((p = g_new(char, len + 1)) != NULL)
	{
		memcpy(p, str, len);
		p[len] = '\0';
		return p;
	}
	oom(len + 1);
	return NULL;
}

/* ---------------------------------------------------------------------- */

char *g_strdup(const char *str)
{
	char *p;
	size_t len;
	
	if (str == NULL)
		return NULL;
	len = strlen(str) + 1;
	if ((p = g_new(char, len)) != NULL)
	{
		strcpy(p, str);
		return p;
	}
	oom(len);
	return NULL;
}
#endif

/* ---------------------------------------------------------------------- */

void strfreev(char **argv)
{
	size_t i;
	
	if (argv != NULL)
	{
		for (i = 0; argv[i] != NULL; i++)
			g_free(argv[i]);
		g_free(argv);
	}
}

/* ---------------------------------------------------------------------- */

char **split_args(const char *argv0, const char *argstring, int *pargc, char delim)
{
	char **argv;
	int argc;
	const char *s;
	const char *current;
	
#define isdelim(c) ((c) == ' ' || (c) == '\t' || (c) == delim)

	argc = 0;
	if (argv0 != NULL)
		argc++;
	while (isdelim(*argstring))
		argstring++;
	s = current = argstring;
	while (*s != '\0')
	{
		if (isdelim(*s))
		{
			++argc;
			while (isdelim(*s))
				s++;
			current = s;
		} else
		{
			++s;
		}
	}
	if (s != current)
		argc++;
	argv = g_new(char *, argc + 1);

	argc = 0;
	if (argv0 != NULL)
		argv[argc++] = g_strdup(argv0);
	s = current = argstring;
	while (*s != '\0')
	{
		if (isdelim(*s))
		{
			argv[argc++] = g_strndup(current, s - current);
			while (isdelim(*s))
				s++;
			current = s;
		} else
		{
			++s;
		}
	}
	if (s != current)
		argv[argc++] = g_strndup(current, s - current);
	argv[argc] = NULL;
#undef isdelim

	if (pargc != NULL)
		*pargc = argc;
	return argv;
}

/* ---------------------------------------------------------------------- */

char *strrslash(const char *f)
{
	char *p1, *p2;
	
	p1 = strrchr(f, bslash);
	p2 = strrchr(f, fslash);
	if (p1 == NULL || p2 > p1)
		p1 = p2;
	return p1;
}

/* ---------------------------------------------------------------------- */

static int drive_from_letter(char c)
{
	if (c >= 'a' && c <= 'z')
		return c - 'a';
	if (c >= 'A' && c <= 'Z')
		return c - 'A';
	if (c >= '1' && c <= '6')
		return c - '1' + 26;
	return -1;
}

/* ---------------------------------------------------------------------- */

char *basename(const char *f)
{
	char *p;
	
	p = strrslash(f);
	if (p)
		return p + 1;
	p = (char *)f;
	if (drive_from_letter(p[0]) >= 0 && p[1] == ':')
		return p + 2;
	return p;
}

/* ---------------------------------------------------------------------- */

bool is_absolute_path(const char *s)
{
	if (s == NULL || s[0] == '\0')
		return false;
	if (drive_from_letter(s[0]) >= 0 && s[1] == ':')
		return true;
	if (s[0] == bslash || s[0] == fslash)
		return true;
	return false;
}

/* ---------------------------------------------------------------------- */

static bool findupsuf(const char *suf, const char *s)
{
	if (stricmp(suf, s) == 0)
		return true;
	return false;
}


FILETYPE filetype(const char *filename)
{
	const char *suf;
	
	if (filename == NULL)
		return FT_NONE;
	if ((suf = strrchr(basename(filename), '.')) == NULL)	/* no suffix */
		return FT_NONE;
	if (findupsuf(suf, ".app") ||
		findupsuf(suf, suff_prg) ||
		findupsuf(suf, ".ttp") ||
		findupsuf(suf, ".tos") ||
		findupsuf(suf, ".gtp") ||
		findupsuf(suf, ".cpx") ||
		findupsuf(suf, ".cp") ||
		findupsuf(suf, ".acc"))
		return FT_PROGRAM;
	if (findupsuf(suf, ".c"))
		return FT_CSOURCE;
	if (findupsuf(suf, ".h"))
		return FT_HEADER;
	if (findupsuf(suf, ".s"))
		return FT_ASSOURCE;
	if (findupsuf(suf, suff_o) ||
		findupsuf(suf, ".oo"))
		return FT_OBJECT;
	if (findupsuf(suf, ".a") ||
		findupsuf(suf, ".lib") ||
		findupsuf(suf, ".l"))
		return FT_LIBRARY;
	if (findupsuf(suf, ".slb"))
		return FT_SHAREDLIB;
	if (findupsuf(suf, ".prj"))
		return FT_PROJECT;
	return FT_UNKNOWN;
}

/* ---------------------------------------------------------------------- */

void strbslash(char *str)
{
	if (str == NULL)
		return;
	while (*str)
	{
		if (*str == fslash)
			*str = bslash;
		str++;
	}
}

/* ---------------------------------------------------------------------- */

void strfslash(char *str)
{
	if (str == NULL)
		return;
	while (*str)
	{
		if (*str == bslash)
			*str = fslash;
		str++;
	}
}

/* ---------------------------------------------------------------------- */

char *build_path(const char *dir, const char *fname)
{
	size_t dirlen;
	size_t fnamelen;
	char *name;
	
	if (fname && is_absolute_path(fname))
	{
		name = g_strdup(fname);
		if (name)
#if defined(__TOS__) || defined(__atarist__)
			strbslash(name);
#else
			strfslash(name);
#endif
		return name;
	}
	dirlen = strlen(dir);
	fnamelen = fname ? strlen(fname) : 0;
	name = g_new(char, dirlen + 1 + fnamelen + 1);
	if (name != NULL)
	{
		strcpy(name, dir);
		if (dirlen > 0 && dir[dirlen - 1] != bslash && dir[dirlen - 1] != fslash)
			strcat(name, "/");
		if (fname)
			strcat(name, fname);
#if defined(__TOS__) || defined(__atarist__)
		strbslash(name);
#else
		strfslash(name);
#endif
	}
	return name;
}

/* ---------------------------------------------------------------------- */

char *change_suffix(const char *filename, const char *ext)
{
	char *dot;
	char *name;
	
	name = g_new(char, strlen(filename) + strlen(ext) + 1);
	if (name)
	{
		strcpy(name, filename);
		dot = strrchr(basename(name), '.');
		if (dot)
			strcpy(dot, ext);
	}
	return name;
}

/* ---------------------------------------------------------------------- */

bool file_exists(const char *f)
{
#if defined(__TOS__) || defined(__atarist__)
	DTA dta;
	DTA *olddta;
	int ret;
	
	olddta = Fgetdta();
	Fsetdta(&dta);
	ret = Fsfirst(f, FA_RDONLY);
	Fsetdta(olddta);
	return ret == 0;
#else
	struct stat st;
	return stat(f, &st) == 0;
#endif
}

/* ---------------------------------------------------------------------- */

char *dirname(const char *f)
{
	char *t, *m;

	m = g_strdup(f);
	t = strrslash(m);
	if (t)
	{
		*(t + 1) = 0;
	} else
	{
		*m = '\0';
	}
	return m;
}

/* ---------------------------------------------------------------------- */

int ch_dir(const char *path)
{
#if defined(__TOS__) || defined(__atarist__)
	int r = 0;
	int drv;
	char *tmp;
	
	if (path == NULL)
		return -ENOENT;
	if (path[0] >= 'A' && path[0] <= 'Z')
		drv = path[0] - 'A';
	else if (path[0] >= 'a' && path[0] <= 'z')
		drv = path[0] - 'a';
	else
		drv = -1;
	if (drv >= 0 && path[1] == ':' &&
		(path[2] == fslash || path[2] == bslash || path[2] == '\0'))
	{
		Dsetdrv(drv);
		path += 2;
	}
	if (path[0] != '\0')
	{
		tmp = g_strdup(path);
		if (tmp == NULL)
		{
			r = -39;
		} else
		{
			strbslash(tmp);
			r = (int)Dsetpath(tmp);
			g_free(tmp);
		}
	}
	return r;
#else
	return chdir(path);
#endif
}
