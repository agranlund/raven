#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#if defined(__TOS__) || defined(__atarist__)
#ifdef __GNUC__
#include <osbind.h>
#include <mintbind.h>
#define DOSTIME _DOSTIME
#else
#include <tos.h>
#endif
#ifndef DISABLE_NATFEATS
#include <mint/arch/nf_ops.h>
#endif
#define STRBSLASH(s) s
#else
#include <unistd.h>
#include <sys/stat.h>
#include <utime.h>
#define STRBSLASH(s) strbslash(s)
#endif
#include "pcmake.h"
#  ifndef CLK_TCK
#   define CLK_TCK	CLOCKS_PER_SEC
#  endif


typedef struct
{
	unsigned long bytes;
	unsigned long lines;
	unsigned long cbytes;
	unsigned long clines;
	unsigned long start;
	unsigned long end;
	unsigned long time;
} STATS;

static STATS stats;


/**************************************************************************/
/* ---------------------------------------------------------------------- */
/**************************************************************************/

/* remove trailing whitespace */
static void chomp(char *ln)
{
	if (*ln)
	{
		char *t = ln + strlen(ln);

		t--;
		while (t >= ln && (*t == ' ' || *t == '\t' || *t == '\r' || *t == '\n'))
		{
			*t = 0;
			t--;
		}
	}
}

/* ---------------------------------------------------------------------- */

static char nextchar(char **ln)					/* skip only analysed character */
{
	if (**ln)
		return *(++(*ln));
	return 0;
}

/* ---------------------------------------------------------------------- */

/* skip white space */
static char skipwhite(char **ln)
{
	char c;

	while ((c = **ln) == ' ' || c == '\t' || c == '\r' || c == '\n')
		++(*ln);
	return c;							/* can be zero: end input */
}

/* ---------------------------------------------------------------------- */

/* skip analysed character and following white space */
static char skipchar(char **ln)
{
	if (**ln)
		++(*ln);
	return skipwhite(ln);
}

/* ---------------------------------------------------------------------- */

static bool peekstr(const char *ln, const char *s, int l)
{
	while (l)
	{
		if (*s != *ln)
			return false;
		if (*s == 0 || *ln == 0)
			return false;
		--l, ++s, ++ln;
	}
	return true;
}

/* ---------------------------------------------------------------------- */

/* terminated by space or ( or ) or , */
static char *parse_filename(char **ln)
{
	char c;
	char *s, *p, *str;
	size_t len;
	
	s = *ln;
	p = s;
	for (;;)
	{
		c = *p;
		if (c == 0 || c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '(' || c == ')' || c == ',')
			break;
		p++;
	}
	*ln = p;
	len = p - s;
	str = g_new(char, len + 1);
	if (str != NULL)
	{
		memcpy(str, s, len);
		str[len] = '\0';
	}
	return str;
}

/* ---------------------------------------------------------------------- */

char *get_cwd(void)
{
	char path[1024];
#if defined(__TOS__) || defined(__atarist__)
	int drv;
	int r;
	
	drv = Dgetdrv();
	path[0] = drv + 'A';
	path[1] = ':';
	path[2] = '\0';
	r = (int)Dgetcwd(path + 2, 0, (int)sizeof(path) - 2);
	if (r == -32)
	{
		r = (int)Dgetpath(path + 2, 0);
	}
	if (r < 0)
		return NULL;
#else
	if (getcwd(path, sizeof(path)) == NULL)
		return NULL;
#endif
	return g_strdup(path);
}

/* ---------------------------------------------------------------------- */

static char *dup_filename(const char *name)
{
	char *str = g_strdup(name);
	if (str)
#if defined(__TOS__) || defined(__atarist__)
		strbslash(str);
#else
		strfslash(str);
#endif
	return str;
}

/**************************************************************************/
/* ---------------------------------------------------------------------- */
/**************************************************************************/

static bool inlist(filearg *list, const char *f)
{
	while (list)
	{
		if (stricmp(list->name, f) == 0)
			return true;
		list = list->next;
	}
	return false;
}


static filearg *putflist(filearg **list, const char *f, FILETYPE type)
{
	filearg *entry = NULL;
	
	if (!inlist(*list, f))
	{
		entry = (filearg *)g_malloc(sizeof(*entry) + strlen(f));
		if (entry)
		{
			strcpy(entry->name, f);
			entry->next = NULL;
			entry->filetype = type;
			entry->u.cflags = NULL;
			entry->dependencies = NULL;
			entry->src_time = 0;
			entry->obj_time = 0;
			while (*list)
				list = &(*list)->next;
			*list = entry;
		}
	}
	return entry;
}


/*
 *  keepfile(prj, f, l) - remember the filename 'f' in the appropriate place
 */
static filearg *keepfile(PRJ *prj, const char *f)
{
	FILETYPE ftype;
	filearg *entry = NULL;
	
	if (f == NULL)
		return NULL;

	ftype = filetype(f);
	switch (ftype)
	{
	case FT_NONE:
		/* no suffix */
		/* default is .C */
		ftype = FT_CSOURCE;
		{
			char *ps;
	
			ps = g_new(char, strlen(f) + 3);
			if (ps)
			{
				strcpy(ps, f);
				strcat(ps, ".c");
				entry = putflist(&prj->inputs, ps, ftype);
				g_free(ps);
			}
		}
		break;
	case FT_PROGRAM:
	case FT_SHAREDLIB:
		entry = putflist(&prj->inputs, f, ftype);
		break;
	case FT_LIBRARY:
		entry = putflist(&prj->inputs, f, ftype);
		break;
	case FT_CSOURCE:
		entry = putflist(&prj->inputs, f, ftype);
		if (entry)
		{
			entry->u.cflags = copy_cflags(&prj->c_flags);
		}
		break;
	case FT_ASSOURCE:
		entry = putflist(&prj->inputs, f, ftype);
		if (entry)
		{
			entry->u.aflags = copy_aflags(&prj->a_flags);
		}
		break;
	case FT_OBJECT:
		entry = putflist(&prj->inputs, f, ftype);
		break;
	case FT_HEADER:
		entry = putflist(&prj->inputs, f, ftype);
		break;
	case FT_PROJECT:
		entry = putflist(&prj->inputs, f, ftype);
		break;
	default:
		break;
	}
	return entry;
}


static void default_output_file(PRJ *prj)
{
	filearg *ft;
	char *output;
	
	/*
	 * FIXME maybe: PureC uses project filename as default
	 */
	if (prj->output == NULL)
	{
		for (ft = prj->inputs; ft != NULL; ft = ft->next)
		{
			if (ft->filetype == FT_CSOURCE || ft->filetype == FT_ASSOURCE)
				break;
		}
		if (ft && (ft->filetype == FT_CSOURCE || ft->filetype == FT_ASSOURCE))
		{
			output = change_suffix(ft->name, suff_prg);
		} else
		{
			output = g_strdup("a.prg");
		}
		putflist(&prj->output, output, filetype(output));
	}
}


static void clear_project(PRJ *prj, int level)
{
	filearg *ft, *next;

	if (prj)
	{
		for (ft = prj->inputs; ft != NULL; ft = next)
		{
			next = ft->next;
			
			switch (ft->filetype)
			{
			case FT_CSOURCE:
				free_cflags(ft->u.cflags);
				g_free(ft->u.cflags);
				break;
			case FT_ASSOURCE:
				free_aflags(ft->u.aflags);
				g_free(ft->u.aflags);
				break;
			case FT_PROJECT:
				clear_project(ft->u.prj, level + 1);
				break;
			default:
				break;
			}
			list_free(&ft->dependencies);
			g_free(ft);
		}
		g_free(prj->output);
		g_free(prj->filename);
		g_free(prj->directory);
		
		free_cflags(&prj->c_flags);
		free_aflags(&prj->a_flags);
		free_ldflags(&prj->ld_flags);
		
		g_free(prj);
	}
}


void free_project(PRJ *prj)
{
	clear_project(prj, 0);
}


static char *objname_for_src(PRJ *prj, filearg *ft)
{
	char *objname;
	char *tmp;
	
	if (ft->filetype == FT_CSOURCE && ft->u.cflags && ft->u.cflags->output_name)
	{
		objname = dup_filename(ft->u.cflags->output_name);
	} else if (ft->filetype == FT_CSOURCE && prj->c_flags.output_directory)
	{
		tmp = build_path(prj->c_flags.output_directory, basename(ft->name));
		objname = change_suffix(tmp, suff_o);
		g_free(tmp);
	} else if (ft->filetype == FT_ASSOURCE && ft->u.aflags && ft->u.aflags->output_name)
	{
		objname = dup_filename(ft->u.aflags->output_name);
	} else if (ft->filetype == FT_ASSOURCE && prj->a_flags.output_directory)
	{
		tmp = build_path(prj->a_flags.output_directory, basename(ft->name));
		objname = change_suffix(tmp, suff_o);
		g_free(tmp);
	} else
	{
		objname = change_suffix(ft->name, suff_o);
#if defined(__TOS__) || defined(__atarist__)
		strbslash(objname);
#else
		strbslash(objname);
#endif
	}
	return objname;
}


#if defined(__TOS__) || defined(__atarist__)
static time_t mktimet(const DOSTIME *t)
{
	return (((unsigned long)t->date) << 16) | ((unsigned long)t->time);
}
#endif


static time_t currdate(void)
{
#if defined(__TOS__) || defined(__atarist__)
	DOSTIME t;
	
	t.time = Tgettime();
	t.date = Tgetdate();
	return mktimet(&t);
#else
	return time(0);
#endif
}


static int stattime(const char *filename, time_t *t)
{
#if defined(__TOS__) || defined(__atarist__)
	DOSTIME timestamp;
	int fh;

	fh = (int)Fopen(filename, FO_READ);
	if (fh <= 0)
		return -1;
	Fdatime(&timestamp, fh, 0);
	*t = mktimet(&timestamp);
	Fclose(fh);
	return 0;
#else
	struct stat st;
	
	if (stat(filename, &st) != 0)
		return -1;
	*t = st.st_mtime;
	return 0;
#endif
}

static void touch(PRJ *prj, filearg *ft)
{
#if defined(__TOS__) || defined(__atarist__)
	char *name;
	int h;
	DOSTIME t;
	
	name = dup_filename(ft->name);
	if ((h = (int)Fopen(name, FO_READ)) >= 0)
	{
		char *o;

		Fdatime(&t, h, 0);
		ft->src_time = mktimet(&t);
		Fclose(h);
		o = objname_for_src(prj, ft);
		if ((h = (int)Fopen(o, FO_RW)) >= 0)
		{
			t.time = 0;
			t.date = 0x21;
			Fdatime(&t, h, 1);			/* touch .o (datime --> 80-1-1) */
			ft->obj_time = mktimet(&t);
			Fclose(h);
		}
		g_free(o);
	}
	g_free(name);
#else
	char *name;
	struct utimbuf t;
	
	name = dup_filename(ft->name);
	if (stattime(name, &ft->src_time) == 0)
	{
		char *o;

		o = objname_for_src(prj, ft);
		t.actime = 0;
		t.modtime = 0;
		if (utime(o, &t) == 0)
		{
			ft->obj_time = t.modtime;
		}
		g_free(o);
	}
	g_free(name);
#endif
}


static int makeok(PRJ *prj, MAKEOPTS *opts, filearg *ft, const char *objname, int lvl)
{
	time_t src_time;
	strlist *dep;
	char *f;
	char *srcname;
	
	(void)(prj);
	if (ft->obj_time == 0)
	{
		if (stattime(objname, &ft->obj_time) != 0)
		{
			if (opts->verbose > 1)
				fprintf(stdout, _("compiling %s because %s is missing\n"), ft->name, objname);
			ft->obj_time = 0;
			return 1;						/* .o absent, must compile */
		}
	}

	srcname = dup_filename(ft->name);
	if (stattime(srcname, &ft->src_time) != 0)
	{
		errout(_("%d>%s: Can't open %s"), lvl, program_name, srcname);
		g_free(srcname);
		return -1;
	}

	if (ft->src_time < ft->obj_time)
	{
		g_free(srcname);
		/* src older than object; check dependencies */
		for (dep = ft->dependencies; dep; dep = dep->next)
		{
			f = dup_filename(dep->str);
			if (stattime(f, &src_time) != 0)
			{
				errout(_("%d>%s: Can't open %s"), lvl, program_name, f);
				g_free(f);
				return -1;
			}
			if (ft->obj_time < src_time)
			{
				if (opts->verbose > 1)
					fprintf(stdout, _("compiling %s because %s is newer than %s\n"), ft->name, f, objname);
				g_free(f);
				return 1;
			}
			g_free(f);
		}
		return 0;
	}
	if (opts->verbose > 1)
		fprintf(stdout, _("compiling %s because %s is newer than %s\n"), ft->name, srcname, objname);
	g_free(srcname);
	return 1;		/* we must compile */
}



static void add_arg(int *argc, char ***argv, const char *arg)
{
	*argv = g_realloc(*argv, (*argc + 2) * sizeof(char *));
	if (*argv == NULL)
		return;
	(*argv)[*argc] = g_strdup(arg);
	++(*argc);
	(*argv)[*argc] = NULL;
}


static char *add_optarg(int *argc, char ***argv, const char *sw, const char *arg)
{
	char *str = g_new(char, strlen(sw) + strlen(arg) + 1);
	if (str)
	{
		strcat(strcpy(str, sw), arg);
		*argv = g_realloc(*argv, (*argc + 2) * sizeof(char *));
		if (*argv == NULL)
		{
			g_free(str);
			return NULL;
		}
		(*argv)[*argc] = str;
		++(*argc);
		(*argv)[*argc] = NULL;
	}
	return str;
}


/* called by docomp() */
static void prj_cparams(const C_FLAGS *cflags, int *argc, char ***argv)
{
	const strlist *str;
	
	for (str = cflags->defines; str != NULL; str = str->next)
	{
		add_optarg(argc, argv, "-D", str->str);
	}

	for (str = cflags->undefines; str != NULL; str = str->next)
	{
		add_optarg(argc, argv, "-U", str->str);
	}

	for (str = cflags->c_includes; str != NULL; str = str->next)
	{
		STRBSLASH(add_optarg(argc, argv, "-I", str->str));
	}
}


/* called by docomp() */
static void prj_aparams(const A_FLAGS *aflags, int *argc, char ***argv)
{
	const strlist *str;
	
	for (str = aflags->defines; str != NULL; str = str->next)
	{
		add_optarg(argc, argv, "-D", str->str);
	}

	for (str = aflags->as_includes; str != NULL; str = str->next)
	{
		STRBSLASH(add_optarg(argc, argv, "-I", str->str));
	}
}


static void remove_output(const char *filename)
{
	fprintf(stdout, "rm %s\n", filename);
#if defined(__TOS__) || defined(__atarist__)
	Fdelete(filename);
#else
	unlink(filename);
#endif
}



/*
 * docomp(prj, f) - run the compiler on the given .c file
 */
static int docomp(PRJ *prj, MAKEOPTS *opts, filearg *ft)
{
	int warn;
	int argc;
	char **argv;
	char buf[32];
	bool is_asm = ft->filetype == FT_ASSOURCE;
	char *srcname;
	char *output_name;
	const char *compiler_name;
	
	argc = 0;
	argv = NULL;
	if (is_asm)
	{
		const A_FLAGS *aflags = ft->u.aflags;

		if (aflags->Coldfire)
		{
			/* for ahcc, compiler can be used as assembler */
			compiler_name = get_compiler_executable();
			prj->needs_ahcc = true;
		} else
		{
			compiler_name = get_assembler_executable();
		}
		add_arg(&argc, &argv, compiler_name);
		/* many levels of verbosity */
		if (aflags->verbose > 0)
			add_arg(&argc, &argv, "-V");
		if (aflags->verbose > 1)
			add_arg(&argc, &argv, "-V");
		if (aflags->verbose > 2)
			add_arg(&argc, &argv, "-V");
		if (aflags->verbose > 3)
			add_arg(&argc, &argv, "-V");
		if (aflags->output_DRI)
			add_arg(&argc, &argv, "-B");
		if (aflags->supervisor)
			add_arg(&argc, &argv, "-S");						/* default .super in assembly */
		if (aflags->undefined_external)
			add_arg(&argc, &argv, "-U");

		if (aflags->Coldfire)
			add_arg(&argc, &argv, "-7");
		if (aflags->i2_68020 && !aflags->Coldfire)
			add_arg(&argc, &argv, "-2");	/* >= 68020 */
		if (aflags->i2_68030)
			add_arg(&argc, &argv, "-3");	/* 68030 */
		if (aflags->i2_68040)
			add_arg(&argc, &argv, "-4");	/* 68040 */
		if (aflags->i2_68851)
			add_arg(&argc, &argv, "-5");	/* 68851 */
		if (aflags->i2_68060)
			add_arg(&argc, &argv, "-6");	/* 68060 */
		if (aflags->use_FPU)
			add_arg(&argc, &argv, "-8");	/* FPU */
		if (aflags->debug_infos)
			add_arg(&argc, &argv, "-Y");
		if (aflags->no_output)
			add_arg(&argc, &argv, "--fno-output");

		prj_aparams(aflags, &argc, &argv);

		output_name = objname_for_src(prj, ft);
	} else
	{
		const C_FLAGS *cflags = ft->u.cflags;

		if (cflags->Coldfire || cflags->default_int32)
		{
			compiler_name = get_ahcc_executable();;
			prj->needs_ahcc = true;
		} else
		{
			compiler_name = get_compiler_executable();
		}
		add_arg(&argc, &argv, compiler_name);
		/* many levels of verbosity */
		if (cflags->verbose > 0)
			add_arg(&argc, &argv, "-V");
		if (cflags->verbose > 1)
			add_arg(&argc, &argv, "-V");
		if (cflags->verbose > 2)
			add_arg(&argc, &argv, "-V");
		if (cflags->verbose > 3)
			add_arg(&argc, &argv, "-V");
		if (cflags->output_DRI)
			add_arg(&argc, &argv, "-B");

		if (cflags->nested_comments)
			add_arg(&argc, &argv, "-C");	/* nested comments */
		if (cflags->max_errors >= 0 && cflags->max_errors != DEFAULT_MAXERRS)
		{
			sprintf(buf, "-E%d", cflags->max_errors);
			add_arg(&argc, &argv, buf);
		}
		if (cflags->max_warnings >= 0 && cflags->max_warnings != DEFAULT_MAXWARNS)
		{
			sprintf(buf, "-F%d", cflags->max_warnings);
			add_arg(&argc, &argv, buf);
		}
		if (cflags->optimize_size)
			add_arg(&argc, &argv, "-G");
		if (cflags->cdecl_calling)
			add_arg(&argc, &argv, "-H");					/* cdecl calling */
		if (cflags->no_jump_optimization)
			add_arg(&argc, &argv, "-J");
		if (cflags->char_is_unsigned)
			add_arg(&argc, &argv, "-K");					/* default char is unsigned */
		if (cflags->identifier_max_length >= 0 && cflags->identifier_max_length != DEFAULT_IDLENGTH)
		{
			sprintf(buf, "-L%d", cflags->identifier_max_length);
			add_arg(&argc, &argv, buf);
		}
		if (cflags->string_merging)
			add_arg(&argc, &argv, "-M");
		if (cflags->absolute_calls)
			add_arg(&argc, &argv, "-P");
		if (cflags->pascal_calling)
			add_arg(&argc, &argv, "-Q");
		if (cflags->no_register_vars)
			add_arg(&argc, &argv, "-R");	/* suppress registerization */
		if (cflags->frame_pointer)
			add_arg(&argc, &argv, "-S");
		if (cflags->stack_checking)
			add_arg(&argc, &argv, "-T");
		if (cflags->add_underline)
			add_arg(&argc, &argv, "-X");
		if (cflags->no_register_reload)
			add_arg(&argc, &argv, "-Z");	/* suppress registerization */
			
		if (cflags->default_int32)
			add_arg(&argc, &argv, "--mno-short");					/* default int is 32 bits */
		
		if (cflags->warning_level < DEFAULT_WARNINGLEVEL)
			add_optarg(&argc, &argv, "-W-", "");
		else if (cflags->warning_level > DEFAULT_WARNINGLEVEL)
			add_optarg(&argc, &argv, "-W+", "");
		for (warn = 0; warn < WARN_MAX; warn++)
		{
			if (cflags->warning_enabled[warn] >= 0)
				add_optarg(&argc, &argv, cflags->warning_enabled[warn] ? "-W" : "-W-", warnings[get_warning_idx(warn)].short_switch);
		}

		if (!is_asm && cflags->Coldfire && cflags->i2_68020)
			add_arg(&argc, &argv, "-27");
		else if (cflags->Coldfire)
			add_arg(&argc, &argv, "-7");
		if (cflags->i2_68020 && !cflags->Coldfire)
			add_arg(&argc, &argv, "-2");	/* >= 68020 */
		if (cflags->i2_68030)
			add_arg(&argc, &argv, "-3");	/* 68030 */
		if (cflags->i2_68040)
			add_arg(&argc, &argv, "-4");	/* 68040 */
		if (cflags->i2_68851)
			add_arg(&argc, &argv, "-5");	/* 68851 */
		if (cflags->i2_68060)
			add_arg(&argc, &argv, "-6");	/* 68060 */
		if (cflags->use_FPU)
			add_arg(&argc, &argv, "-8");	/* FPU */
		if (cflags->debug_infos)
			add_arg(&argc, &argv, "-Y");
		if (cflags->no_output)
			add_arg(&argc, &argv, "--fno-output");
	
		prj_cparams(cflags, &argc, &argv);

		STRBSLASH(add_optarg(&argc, &argv, "-I", get_includedir()));

		output_name = objname_for_src(prj, ft);
	}

	STRBSLASH(srcname = dup_filename(ft->name));

	/* ahcc does not understand -O option */
	if (!prj->needs_ahcc)
		STRBSLASH(add_optarg(&argc, &argv, "-O", output_name));

	add_arg(&argc, &argv, srcname);
	
	if (!opts->silent)
	{
		int i;

		if (opts->verbose > 0)
			fprintf(stdout, _("\n****  Compiling %s\n"), srcname);
		fprintf(stdout, "%s", argv[0]);
		for (i = 1; i < argc; i++)
			fprintf(stdout, " %s", argv[i]);
		fprintf(stdout, "\n");
#if defined(__TOS__) || defined(__atarist__)
#ifndef DISABLE_NATFEATS
		if (opts->nfdebug)
		{
			nf_debug(argv[0]);
			for (i = 1; i < argc; i++)
			{
				nf_debug(" ");
				nf_debug(argv[i]);
			}
			nf_debug("\n");
		}
#endif
#endif
	}
	g_free(output_name);

	if (opts->dryrun)
	{
		warn = 0;
		ft->obj_time = currdate();
	} else if ((warn = compiler(argc, (const char **)argv)) != 0)
	{
#if 0
		remove_output(output_name);
#endif
	}
	
	g_free(srcname);
	strfreev(argv);
	
	if (warn == 0 && !opts->dryrun)
	{
		if (opts->verbose > 0)
			fprintf(stdout, _("compilation OK\n"));
	}

	return warn;
}


static char *lookup_object(PRJ *prj, filearg *ft, const char *msg)
{
	char *name;
	bool found;
	
	name = dup_filename(ft->name);
	if (file_exists(name) || is_absolute_path(ft->name))
	{
		found = true;
	} else if ((ft->filetype == FT_OBJECT && ft == prj->inputs) || ft->filetype == FT_LIBRARY)
	{
		g_free(name);
		name = build_path(get_libdir(), ft->name);
		found = file_exists(name);
	} else
	{
		found = false;
	}
	if (!found)
	{
		errout(_("can't find %s '%s'"), msg, ft->name);
		g_free(name);
		name = NULL;
	}

	return name;
}


/*
 * dold() - run the loader
 */

static bool dold(PRJ *prj, MAKEOPTS *opts)
{
	char **argv;
	int argc;
	int rep = 0;
	char buf[50];
	char *name;
	
	if (prj->inputs == NULL)
	{
		errout(_("nothing to link"));
		return false;
	}
	
	default_output_file(prj);

	argc = 0;
	argv = g_new(char *, 1);
	add_arg(&argc, &argv, prj->needs_ahcc ? get_ahcl_executable() : get_linker_executable());
	if (prj->ld_flags.verbose > 0)
		add_arg(&argc, &argv, "-V");
	if (prj->ld_flags.verbose > 1)
		add_arg(&argc, &argv, "-V");
	if (prj->ld_flags.verbose > 2)
		add_arg(&argc, &argv, "-V");
	if (prj->ld_flags.verbose > 3)
		add_arg(&argc, &argv, "-V");
	if (prj->ld_flags.global_symbols)
		add_arg(&argc, &argv, "-G");
	if (prj->ld_flags.local_symbols)
		add_arg(&argc, &argv, "-L");
	if (prj->ld_flags.nm_list)
		add_arg(&argc, &argv, "-N");
	if (prj->ld_flags.load_map)
		add_arg(&argc, &argv, "-P");
	if (prj->ld_flags.debug_infos)
		add_arg(&argc, &argv, "-Y");
	if (prj->ld_flags.create_new_object || prj->output->filetype == FT_LIBRARY)
		add_arg(&argc, &argv, "-J");
	if (prj->ld_flags.malloc_for_stram)
		add_arg(&argc, &argv, "-M");
	if (prj->ld_flags.no_fastload)
		add_arg(&argc, &argv, "-F");
	if (prj->ld_flags.program_to_stram)
		add_arg(&argc, &argv, "-R");
	if (prj->ld_flags.binary)
		add_arg(&argc, &argv, "-binary");
	if (prj->ld_flags.create_new_object || prj->output->filetype == FT_LIBRARY)
	{
		if (prj->ld_flags.heap_size > 0 ||
			prj->ld_flags.text_start > 0 ||
			prj->ld_flags.data_start > 0 ||
			prj->ld_flags.bss_start > 0)
			errout(_("%s: %s: ignoring base addresses when linking library"), Warning, prj->filename);
	} else
	{
		/*
		 * no hex output here; the linker does not accept this
		 * (it accepts '$' as prefix, but that might be substituted by the shell)
		 */
		if (prj->ld_flags.heap_size > 0)
		{
			sprintf(buf, "-H=%lu", prj->ld_flags.heap_size);
			add_arg(&argc, &argv, buf);
		}
		if (prj->ld_flags.stacksize >= 0)
		{
			sprintf(buf, "-S=%lu", prj->ld_flags.stacksize);
			add_arg(&argc, &argv, buf);
		}
		if (prj->ld_flags.text_start > 0)
		{
			sprintf(buf, "-T=%lu", prj->ld_flags.text_start);
			add_arg(&argc, &argv, buf);
		}
		if (prj->ld_flags.data_start > 0)
		{
			sprintf(buf, "-D=%lu", prj->ld_flags.data_start);
			add_arg(&argc, &argv, buf);
		}
		if (prj->ld_flags.bss_start > 0)
		{
			sprintf(buf, "-B=%lu", prj->ld_flags.bss_start);
			add_arg(&argc, &argv, buf);
		}
	}
	if (prj->ld_flags.nm_list)
		add_arg(&argc, &argv, "--symbol-list");
	if (prj->ld_flags.load_map)
		add_arg(&argc, &argv, "--map");
	STRBSLASH(add_optarg(&argc, &argv, "-O=", prj->output->name));
		
	if (prj->inputs)
	{
		filearg *ft = prj->inputs;
		
		while (rep == 0 && ft)
		{
			switch (ft->filetype)
			{
			case FT_CSOURCE:
				STRBSLASH(name = objname_for_src(prj, ft));
				add_arg(&argc, &argv, name);
				g_free(name);
				break;
			case FT_ASSOURCE:
				STRBSLASH(name = objname_for_src(prj, ft));
				add_arg(&argc, &argv, name);
				g_free(name);
				break;
			case FT_OBJECT:
				if ((name = lookup_object(prj, ft, _("start up"))) == NULL)
				{
					rep = 1;
				} else
				{
					(void) STRBSLASH(name);
					add_arg(&argc, &argv, name);
				}
				g_free(name);
				break;
			case FT_LIBRARY:
				if ((name = lookup_object(prj, ft, _("library"))) == NULL)
				{
					rep = 1;
				} else
				{
					(void) STRBSLASH(name);
					add_arg(&argc, &argv, name);
				}
				g_free(name);
				break;
			case FT_PROJECT:
				if (ft->u.prj)
				{
					if (ft->u.prj->ld_flags.create_new_object || ft->u.prj->output->filetype == FT_LIBRARY)
					{
						STRBSLASH(name = build_path(ft->u.prj->directory, ft->u.prj->output->name));
						add_arg(&argc, &argv, name);
						g_free(name);
					}
				}
				break;
			default:
				break;
			}
			ft = ft->next;
		}
	}
	argv[argc] = NULL;
	
	if (rep == 0)
	{
		if (!opts->silent)
		{
			int i;
			
			if (opts->verbose > 0)
				fprintf(stdout, _("\n****  Linking %s\n"), prj->filename);
			fprintf(stdout, "%s", argv[0]);
			for (i = 1; i < argc; i++)
				fprintf(stdout, " %s", argv[i]);
			fprintf(stdout, "\n");
#if defined(__TOS__) || defined(__atarist__)
#ifndef DISABLE_NATFEATS
			if (opts->nfdebug)
			{
				nf_debug(argv[0]);
				for (i = 1; i < argc; i++)
				{
					nf_debug(" ");
					nf_debug(argv[i]);
				}
				nf_debug("\n");
			}
#endif
#endif
		}
		
		if (opts->dryrun)
		{
			prj->output->obj_time = currdate();
		} else
		{
			rep = linker(argc, (const char **)argv);
			if (rep)
			{
				errout(_("%s: linking failed: %d"), prj->filename, rep);
				if (prj->output->filetype != FT_PROJECT)
					remove_output(prj->output->name);
			}
			if (rep == 0)
			{
				if (opts->verbose > 0)
					fprintf(stdout, _("link OK\n"));
			}
		}
	}
	
	strfreev(argv);
	
	return rep == 0;
}


static int clear_dates(PRJ *prj, int level)
{
	filearg *ft;
	int r = 0;
	
	for (ft = prj->inputs; ft && r >= 0; ft = ft->next)
	{
		switch (ft->filetype)
		{
		case FT_CSOURCE:
		case FT_ASSOURCE:
			touch(prj, ft);
			break;
		case FT_PROJECT:
			r = clear_dates(ft->u.prj, level + 1);
			break;
		default:
			break;
		}
	}

	return r;
}


static int make_prj(PRJ *prj, MAKEOPTS *opts, int level)
{
	int r;
	int anycomp;
	filearg *ft;
	char *name;
	char *curdir;
	
	r = 0;
	anycomp = 0;
	curdir = get_cwd();
	if (ch_dir(prj->directory) < 0)
	{
		errout(_("%s: cannot chdir to %s"), program_name, prj->directory);
		r = -1;
	} else
	{
		if (!opts->silent && opts->print_directory)
		{
			/* errout(_("%s: entering directory %s"), program_name, prj->directory); */
			errout(_("%s: processing %s"), program_name, prj->filename);
		}
	}

	if (opts->make_all && !opts->dryrun)
	{
		/*
		 * we will ignore timestamps on this run,
		 * but it might still be useful to clear the dates
		 * in case the shell is restarted
		 */
		r = clear_dates(prj, level);
	}
	
	for (ft = prj->inputs; ft && r >= 0; ft = ft->next)
	{
		if (ft->filetype == FT_CSOURCE || ft->filetype == FT_ASSOURCE)
		{
			char *of = objname_for_src(prj, ft);

			if (opts->ignore_date)
				r = 1;
			else
				r = makeok(prj, opts, ft, of, level);	/* recursive check timestamp */
			g_free(of);
			if (r >= 0)
			{
				anycomp += r;
	
				if (r == 1)
				{
					if (docomp(prj, opts, ft) != 0)
						r = -1;		/* errors */
				}
			}
		} else if (ft->filetype == FT_PROJECT)
		{
			r = make_prj(ft->u.prj, opts, level + 1);
	
			if (r >= 0)
				anycomp += r;
			prj->needs_ahcc |= ft->u.prj->needs_ahcc;
		}
	}

	if (anycomp == 0 && r >= 0)
	{
		if (opts->ignore_date)
		{
			anycomp = 1;
		} else
		{
			time_t prg_time, src_time;
			
			if (prj->output == NULL || stattime(prj->output->name, &prj->output->obj_time) != 0)
			{
				anycomp = 1;
			} else
			{
				prg_time = prj->output->obj_time;
				for (ft = prj->inputs; ft && r >= 0; ft = ft->next)
				{
					switch (ft->filetype)
					{
					case FT_CSOURCE:
						name = objname_for_src(prj, ft);
						break;
					case FT_ASSOURCE:
						name = objname_for_src(prj, ft);
						break;
					case FT_OBJECT:
						if ((name = lookup_object(prj, ft, _("start up"))) == NULL)
						{
							r = -1;
						}
						break;
					case FT_LIBRARY:
						if ((name = lookup_object(prj, ft, _("library"))) == NULL)
						{
							r = -1;
						}
						break;
					default:
						name = dup_filename(ft->name);
						break;
					}
					if (r >= 0)
					{
						if (stattime(name, &src_time) != 0)
						{
							if (opts->dryrun && (ft->filetype == FT_CSOURCE || ft->filetype == FT_ASSOURCE) && ft->obj_time != 0)
							{
							} else
							{
								errout(_("%d>%s: Can't open %s"), level, program_name, name);
								g_free(name);
								r = -1;
							}
						} else
						{
							ft->src_time = src_time;
							
							if (prg_time < src_time)
							{
								if (prj->output->filetype != FT_PROJECT)
									anycomp = 1;
								break;
							}
						}
					}
				}
			}
		}
	}
	
	if (r < 0)
	{
		anycomp = r;
	} else if (anycomp > 0 && (prj->output == NULL || prj->output->filetype != FT_PROJECT))
	{
		if (dold(prj, opts) == false)						/* run the loader */
			anycomp = -1;
	}
	
	ch_dir(curdir);
	g_free(curdir);
	
	return anycomp;
}


bool domake(PRJ *prj, MAKEOPTS *opts)
{
	int anycomp;

	memset(&stats, 0, sizeof(stats));
	stats.start = clock();
	
	if (opts->make_all && !opts->dryrun)
	{
		/*
		 * we will ignore timestamps on this run,
		 * but it might still be useful to clear the dates
		 * in case the shell is restarted
		 */
		clear_dates(prj, 0);
	}
	
	/* run make */
	if (prj->inputs == NULL)
	{
		errout(_("nothing to make"));
	} else
	{
		anycomp = make_prj(prj, opts, 0);	/* recursive make */

		if (anycomp < 0)
			return false;
		
		if (anycomp == 0)
			fprintf(stdout, _("%s: %s is up to date\n"), program_name, prj->output ? prj->output->name : prj->filename);
	}

	stats.end = clock();
#if 0
	if (opts->make_all)
	{
		unsigned long cmins, csecs, secs;

		stats.time = stats.end - stats.start;
		csecs = stats.time / CLK_TCK;
		cmins = csecs / 60;
		secs = csecs % 60;
		fprintf(stdout, _("\nMake_all statistics\n"));
#if 0
		fprintf(stdout, _("Project:\n"));
		fprintf(stdout, _("bytes  : %7ld\n"), stats.bytes);
		fprintf(stdout, _("lines  : %7ld\n"), stats.lines);
		if (stats.lines)
			fprintf(stdout, ("       = %7ld bytes/line\n"), stats.bytes / stats.lines);
		fprintf(stdout, _("Compiled:\n"));
		fprintf(stdout, _("bytes  : %7ld\n"), stats.cbytes);
		fprintf(stdout, _("lines  : %7ld\n"), stats.clines);
		if (stats.clines)
			fprintf(stdout, _("       = %7ld bytes/line\n"), stats.cbytes / stats.clines);
#endif
		fprintf(stdout, _("time   : %4ld'%02ld\" (%ld)\n"), cmins, secs, csecs);
#if 0
		if (csecs)
		{
			fprintf(stdout, _("       = %7ld bytes/second\n"), stats.cbytes / csecs);
			fprintf(stdout, _("         %7ld lines/second\n"), stats.clines / csecs);
		}
#endif
	}
#endif
	return true;
}


static char *add_options(char **fro)
{
	char *p, *s;
	char *to;
	
	if (skipwhite(fro) == '[')
		skipchar(fro);

	s = *fro;
	p = s;
	while (*p && *p != '\r' && *p != '\n' && *p != ']')
		p++;

	to = g_strndup(s, p - s);
	if (*p == ']')
		*p++ = 0;
	*fro = p;
	return to;
}


static PRJ *load_prj(MAKEOPTS *opts, const char *f, int level)
{
	FILE *fp;
	char st[1024];
	char *s;
	int section = 0;
	long lineno;
	bool retval = true;
	PRJ *prj;
	filearg *keep;
	
	s = dup_filename(f);
	if ((fp = fopen(s, "r")) == NULL)
	{
		errout(_("%d>Can't open project file: %s"), level, f);
		g_free(s);
		return NULL;
	}

	prj = g_new(PRJ, 1);
	if (prj == NULL)
	{
		fclose(fp);
		g_free(s);
		return NULL;
	}
	init_cflags(&prj->c_flags);
	init_aflags(&prj->a_flags);
	init_ldflags(&prj->ld_flags);

	prj->filename = s;
	prj->directory = dirname(f);
	prj->inputs = NULL;
	prj->output = NULL;
	prj->needs_ahcc = false;
	
	lineno = 0;
	while (fgets(st, (int)sizeof(st) - 1, fp) != 0)
	{
		char c;

		lineno++;

		s = st;
		while (*s && *s != ';')
			s++;						/* remove comment */
		if (*s == ';')
			*s = 0;
		
		s = st;
		chomp(s);

		if (*s)							/* anything left ? */
		{
			if (opts->verbose >= 2)
				fprintf(stdout, "%d>'%s'\n", level, s);

			c = skipwhite(&s);
			if (c == '.' && !peekstr(s, "..", 2))
			{
				bool result;
				
				c = nextchar(&s);
				if (c == 'C' || c == 'c')
				{
					char *lopt;
					
					skipchar(&s);
					lopt = add_options(&s);
					if (lopt == NULL)
						result = false;
					else
						result = parse_cc_options(lopt, &prj->c_flags);
					g_free(lopt);
				} else if (c == 'S' || c == 's')
				{
					char *lopt;
					
					skipchar(&s);
					lopt = add_options(&s);
					if (lopt == NULL)
						result = false;
					else
						result = parse_as_options(lopt, &prj->a_flags);
					g_free(lopt);
				} else if (c == 'L' || c == 'l')
				{
					char *lopt;
					
					skipchar(&s);
					lopt = add_options(&s);
					if (lopt == NULL)
						result = false;
					else
						result = parse_ld_options(lopt, &prj->ld_flags);
					g_free(lopt);
				} else
				{
					result = false;
				}
				if (result == false)
				{
					errout(_("%s: %s:%ld: Illegal option specification"), Error, f, lineno);
					retval = false;
				}
			} else if (c == '=')
			{
				if (section != 0)
				{
					errout(_("%s: %s:%ld: Illegal filename"), Error, f, lineno);
					retval = false;
				} else
				{
					const char *opt;
					
					if ((opt = getenv("PCCFLAGS")) != NULL)
					{
						if (parse_cc_options(opt, &prj->c_flags) == false)
						{
							errout(_("%s: Illegal option specification: %s"), Error, opt);
							retval = false;
						}
					}
					if ((opt = getenv("PCASFLAGS")) != NULL)
					{
						if (parse_as_options(opt, &prj->a_flags) == false)
						{
							errout(_("%s: Illegal option specification: %s"), Error, opt);
							retval = false;
						}
					}
					if ((opt = getenv("PCLDFLAGS")) != NULL)
					{
						if (parse_ld_options(opt, &prj->ld_flags) == false)
						{
							errout(_("%s: Illegal option specification: %ss"), Error, opt);
							retval = false;
						}
					}

					/* TODO what if no output file in project?
					   i think PureC uses name of project file in this case */
					section++;
				}
			} else if (c != 0 && c != '=')
			{
				char *ps, *fnm;

				skipwhite(&s);
				fnm = parse_filename(&s);
				ps = dup_filename(fnm);
				
				if (section == 0)
				{
					if (prj->output)
					{
						errout(_("%s: duplicate output file '%s'"), program_name, ps);
						g_free(ps);
						g_free(fnm);
						return FT_UNKNOWN;
					}
					/* TODO: replace "*.PRG" in filename with editor filename, for DEFAULT.PRJ */
					putflist(&prj->output, ps, filetype(ps));
					if (prj->output->filetype == FT_UNKNOWN)
						prj->output->filetype = FT_PROGRAM;
					g_free(ps);
					g_free(fnm);
					continue;
				}
			
				keep = keepfile(prj, fnm);
				g_free(fnm);
				if (keep == NULL || keep->filetype == FT_UNKNOWN)
				{
					errout(_("%d>%s: %s:%ld: unknown file suffix '%s'"), level, Error, f, lineno, ps);
					retval = false;
					g_free(ps);
				} else
				{
					g_free(ps);
					if (keep->filetype != FT_NONE)
					{
						if (skipwhite(&s) == '(')	/* has dependencies */
						{
							/* only for prj_dependencies */
							for (;;)
							{
								c = skipchar(&s);	/* either ( or , */
								fnm = parse_filename(&s);
								if (fnm && fnm[0])
								{
									list_append(&keep->dependencies, fnm);
								}
								g_free(fnm);
								if (skipwhite(&s) != ',')
									break;
							}
							if (skipwhite(&s) != ')')
							{
								errout(_("%s: %s:%ld: ')' expected"), Error, f, lineno);
								retval = false;
							} else
							{
								skipchar(&s);
							}
						}
						
						if (skipwhite(&s) == '[')
						{
							char *opt;
							bool result;
							
							opt = add_options(&s);
							if (opt == NULL)
								result = false;
							else if (keep->filetype == FT_CSOURCE)
								result = parse_cc_options(opt, keep->u.cflags);
							else if (keep->filetype == FT_ASSOURCE)
								result = parse_as_options(opt, keep->u.aflags);
							else
								result = false;
							if (result == false)
							{
								errout(_("%s: %s:%ld: Illegal option specification: %s"), Error, f, lineno, opt);
								retval = false;
							}
							g_free(opt);
						}
						
						/* nested project */
						if (keep->filetype == FT_PROJECT)
						{
							ps = build_path(prj->directory, keep->name);
							keep->u.prj = load_prj(opts, ps, level + 1);
							g_free(ps);
							if (keep->u.prj == NULL)
								retval = false;
						}
					}
				}
			}
		}
	}

	fclose(fp);

	if (retval)
	{
		default_output_file(prj);
		if (prj->output->filetype == FT_LIBRARY)
			prj->ld_flags.create_new_object = true;
	} else
	{
		free_project(prj);
		prj = NULL;
	}
	
	return prj;
}


PRJ *loadmake(MAKEOPTS *opts, const char *f)
{
	if (opts->verbose >= 1)
		fprintf(stdout, _("Loading project file %s\n"), f);
	
	return load_prj(opts, f, 0);
}
