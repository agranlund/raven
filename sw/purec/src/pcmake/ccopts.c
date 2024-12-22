/*
 *	ccopts.c - commandline/project options for the compiler module
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include "getopt.h"
#include "pcmake.h"


warning const warnings[] = {
/*    category    short name  level   long name    text */
	/* for internal messages */
	{ WARN_ANY,   "*",        999,    NULL,        "" },
/* warnings used by Pure-C: */
	{ WARN_STR,   "str",      1,      NULL,        N_("'XXXXXX' not part of structure.") }, /* gives error */
	{ WARN_AMP,   "amp",      2,      NULL,        N_("Superfluous & with function or array.") },
	{ WARN_RPT,   "rpt",      1,      NULL,        N_("Non portable pointer assignment.") },
	{ WARN_APT,   "apt",      1,      NULL,        N_("Non portable pointer conversion.") },
	{ WARN_CPT,   "cpt",      1,      NULL,        N_("Non portable pointer comparision.") },
	{ WARN_RNG,   "rng",      1,      NULL,        N_("Constant out of range in comparison.") },
	{ WARN_ZST,   "zst",      1,      NULL,        N_("Zero length structure.") },
	{ WARN_CLN,   "cln",      2,      NULL,        N_("Constant is long.") },
	{ WARN_DEF,   "def",      1,      NULL,        N_("Possible use of 'XXXXXX' before definition.") },
	{ WARN_AUS,   "aus",      1,      NULL,        N_("'XXXXXX' is assigned a value which is never used.") },
	{ WARN_USE,   "use",      2,      NULL,        N_("'XXXXXX' declared but never used.") },
	{ WARN_VOI,   "voi",      1,      NULL,        N_("Void functions may not return a value.") },
	{ WARN_RET,   "ret",      1,      NULL,        N_("Both return and return of a value used.") }, /* NYI */
	{ WARN_RVL,   "rvl",      1,      NULL,        N_("Function should return a value.") },
	{ WARN_SUS,   "sus",      1,      NULL,        N_("Suspicious pointer conversion.") },
	{ WARN_SIG,   "sig",      2,      NULL,        N_("Conversion may lose significant digits.") },
	{ WARN_RCH,   "rch",      1,      NULL,        N_("Unreachable code.") },
	{ WARN_EFF,   "eff",      1,      NULL,        N_("Code has no effect.") },
	{ WARN_PAR,   "par",      1,      NULL,        N_("Parameter 'XXXXXX' is never used.") },
	{ WARN_AMB,   "amb",      2,      NULL,        N_("Ambiguous operators need parentheses.") }, /* NYI */
	{ WARN_PIA,   "pia",      1,      NULL,        N_("Possible incorrect assignment.") },
	{ WARN_STV,   "stv",      2,      NULL,        N_("Structure passed by value.") },
	{ WARN_STU,   "stu",      1,      NULL,        N_("Undefined structure 'XXXXXX'.") },
	{ WARN_ASM,   "asm",      2,      NULL,        N_("Unknown assembler instruction") },
	{ WARN_DUP,   "dup",      1,      NULL,        N_("Redefinition of 'XXXXXX' is not identical.") },
	{ WARN_ASC,   "asc",      1,      NULL,        N_("Restarting compile using assembly") },
	{ WARN_PRO,   "pro",      1,      NULL,        N_("Call to function 'XXXXXX' with no prototype.") },
	{ WARN_NOD,   "nod",      2,      NULL,        N_("Implicit declaration of function 'XXXXXX'.") },
	{ WARN_UCP,   "ucp",      2,      NULL,        N_("Mixing pointers to signed and unsigned char.") },
	{ WARN_MLT,   "mlt",      2,      NULL,        N_("Hexadecimal constant too large.") },
	{ WARN_STK,   "stk",      1,      NULL,        N_("") },

/* AHCC specific warnings: */
	{ WARN_CON,   "con",      2,      NULL,        N_("uninitialized const object") },
	{ WARN_GOT,   "got",      1,      NULL,        N_("'goto' used") },
	{ WARN_ASC,   "asc",      2,      NULL,        N_("ascii constant too wide") },
	{ WARN_MUL,   "mul",      2,      NULL,        N_("multi-character character constant") },
	{ WARN_PRC,   "prc",      1,      NULL,        N_("current C does not support local procedures") },
	{ WARN_NST,   "nst",      2,      NULL,        N_("\"/*\" within comment") },
	{ WARN_CPP,   "cpp",      2,      NULL,        N_("C++ style comments are not allowed in ISO C90") },
	{ WARN_DEC,   "dec",      2,      NULL,        N_("type defaults to 'int' in declaration of '%s'") },
	{ WARN_CST,   "cst",      2,      NULL,        N_("Cast pointer constant to real") },
	{ WARN_ASS,   "ass",      1,      NULL,        N_("'%s' is a assembler directive") },
	{ WARN_PMA,   "pma",      3,      NULL,        N_("unknown pragma") },
	{ WARN_PCC,   "pcc",      3,      NULL,        "" },
/* Assembler warnings: */
	{ WARN_LBL,   "lbl",      2,      NULL,        N_("'%%N' needs constant expression or register name") },
	{ WARN_DIF,   "dif",      2,      NULL,        N_("can not diff %%N with advance ref") },
	{ WARN_2ND,   "2nd",      2,      NULL,        N_("Assembler needs second phase") },
	{ WARN_XNL,   "xnl",      2,      NULL,        N_("Xn defaults to .l for Coldfire") },
};

int get_warning_level(warning_category category)
{
	int i;
	
	for (i = 0; i < (int)(sizeof(warnings) / sizeof(warnings[0])); i++)
		if (warnings[i].category == category)
			return warnings[i].level;
	return MAX_WARNINGLEVEL;
}


int get_warning_idx(warning_category category)
{
	int i;
	
	for (i = 0; i < (int)(sizeof(warnings) / sizeof(warnings[0])); i++)
		if (warnings[i].category == category)
			return i;
	return -1;
}


#define OPT_MSHORT           504
#define OPT_MLONG            505
#define OPT_SUPER            506
#define OPT_NO_OUTPUT        511

static struct option const long_options[] = {
/*		PURE_C compatible options		*/
	{ "strict-ansi", no_argument, NULL, 'A' },
	{ "dri", no_argument, NULL, 'B' },
	{ "nested-comments", no_argument, NULL, 'C' },
	{ "define", required_argument, NULL, 'D' },
	{ "max-errors", required_argument, NULL, 'E' },
	{ "max-warnings", required_argument, NULL, 'F' },
	{ "optimize-size", no_argument, NULL, 'G' },
	{ "fcdecl-calling", no_argument, NULL, 'H' },
	{ "include-dir", required_argument, NULL, 'I' },
	{ "fno-jump-optimization", no_argument, NULL, 'J' },
	{ "funsigned-char", no_argument, NULL, 'K' },
	{ "fsigned-char", no_argument, NULL, 'K' + 256 },
	{ "fno-unsigned-char", no_argument, NULL, 'K' + 256 },
	{ "max-id-length", required_argument, NULL, 'L' },
	{ "fmerge-strings", no_argument, NULL, 'M' },
	{ "output-directory", required_argument, NULL, 'N' },
	{ "output", required_argument, NULL, 'o' },
	{ "fabsolute-calls", no_argument, NULL, 'P' },
	{ "fpascal-calling", no_argument, NULL, 'Q' },
	{ "fno-register-vars", no_argument, NULL, 'R' },
	{ "fframe-pointer", no_argument, NULL, 'S' },
	{ "fno-frame-pointer", no_argument, NULL, 'S' + 256 },
	{ "fomit-frame-pointer", no_argument, NULL, 'S' + 256 },
	{ "fno-omit-frame-pointer", no_argument, NULL, 'S' },
	{ "fstack-check", no_argument, NULL, 'T' },
	{ "undefine", required_argument, NULL, 'U' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "fleading-underscore", no_argument, NULL, 'X' },
	{ "debug", no_argument, NULL, 'Y' },
	{ "fno-register-reload", no_argument, NULL, 'Z' },
	
	{ "m68000", no_argument, NULL, '0' },
	{ "m68010", no_argument, NULL, '1' },
	{ "m68020", no_argument, NULL, '2' },
	{ "m68030", no_argument, NULL, '3' },
	{ "m68040", no_argument, NULL, '4' },
	{ "m68851", no_argument, NULL, '5' },
	{ "m68060", no_argument, NULL, '6' },
	{ "mcoldfire", no_argument, NULL, '7' },
	{ "mcfv4e", no_argument, NULL, '7' },
	{ "m68881", no_argument, NULL, '8' },
	{ "m68882", no_argument, NULL, '8' },
	
	{ "mshort", no_argument, NULL, OPT_MSHORT },
	{ "mno-short", no_argument, NULL, OPT_MLONG },
	{ "msuper", no_argument, NULL, OPT_SUPER },

	{ "fno-output", no_argument, NULL, OPT_NO_OUTPUT },
	
	{ NULL, no_argument, NULL, 0 }
};


void init_cflags(C_FLAGS *flg)
{
	int i;
	
	memset(flg, 0, sizeof(*flg));
	flg->strict_ANSI = false;
	flg->output_DRI = false;
	flg->nested_comments = false;
	flg->max_errors = DEFAULT_MAXERRS;
	flg->max_warnings = DEFAULT_MAXWARNS;
	flg->optimize_size = false;
	flg->cdecl_calling = false;
	flg->no_jump_optimization = false;
	flg->char_is_unsigned = false;
	flg->identifier_max_length = DEFAULT_IDLENGTH;
	flg->string_merging = false;
	flg->output_directory = NULL;
	flg->output_name = NULL;
	flg->absolute_calls = false;
	flg->pascal_calling = false;
	flg->no_register_vars = false;
	flg->frame_pointer = false;
	flg->stack_checking = false;
	flg->verbose = 0;
	flg->add_underline = false;
	flg->no_register_reload = false;
	flg->i2_68010 = false;
	flg->i2_68020 = false;
	flg->i2_68030 = false;
	flg->i2_68040 = false;
	flg->i2_68060 = false;
	flg->i2_68851 = false;
	flg->use_FPU = false;
	flg->Coldfire = false;
	flg->defines = NULL;
	flg->undefines = NULL;
	flg->c_includes = NULL;
	
	flg->warning_level = DEFAULT_WARNINGLEVEL;
	for (i = 0; i < WARN_MAX; i++)
		flg->warning_enabled[i] = -1;
	
	flg->no_output = false;

	flg->default_int32 = false;
}


void free_cflags(C_FLAGS *c_flags)
{
	list_free(&c_flags->defines);
	list_free(&c_flags->undefines);
	list_free(&c_flags->c_includes);
}


void adddef(strlist **defines, const char *ns)
{
	strlist *entry;
	const char *equal;
	size_t len1, len2;
	
	equal = strchr(ns, '=');
	if (equal)
		len1 = equal - ns;
	else
		len1 = 0;
	for (entry = *defines; entry != NULL; entry = entry->next)
	{
		/*
		 * simply ignore duplicate definitions;
		 * happens if we pass some definition on the
		 * command line that is also part of the project file
		 */
		if (strcmp(entry->str, ns) == 0)
			return;
		/*
		 * now check if overriding an already existing
		 * definition with a different value
		 */
		if (len1 != 0)
		{
			equal = strchr(entry->str, '=');
			if (equal)
			{
				len2 = equal - entry->str;
				if (len1 == len2 && strncmp(entry->str, ns, len1) == 0)
				{
					/*
					 * if new string fits in old entry,
					 * just overwrite it
					 */
					len1 = strlen(ns);
					len2 = strlen(entry->str);
					if (len1 <= len2)
					{
						strcpy(entry->str, ns);
						return;
					}
					/*
					 * no such luck. Just append the new definition, and let
					 * the compiler handle it
					 */
					break;
				}
			}
		}
	}
	list_append(defines, ns);
}


static void subdef(C_FLAGS *cflags, const char *s)
{
	list_append(&cflags->undefines, s);
}


void doincl(strlist **includes, const char *s)				/* optincl */
{
	char *buf;
	char *pt;
	char *dir;

	buf = g_new(char, strlen(s) + 2);
	if (buf == NULL)
		return;
	strcpy(buf, s);
	
	/*
	 * Convert ',' and ';' to nulls
	 */
	for (pt = buf; *pt != 0; pt++)
		if (*pt == ',' || *pt == ';')
			*pt = 0;
	pt[1] = 0;							/* double null terminated */

	/*
	 * Grab each directory, make sure it ends with a backslash,
	 * and add it to the directory list.
	 */
	for (pt = buf; *pt != 0; pt++)
	{
		const strlist *old;
		
		dir = build_path(pt, NULL);

		for (old = *includes; old != NULL; old = old->next)
			if (stricmp(old->str, dir) == 0)
			{
				dir[0] = '\0';
				break;
			}

		if (dir[0] != '\0')
		{
			list_append(includes, dir);
		}
		g_free(dir);
		
		while (*pt != 0)
			pt++;
	}
	g_free(buf);
}


static void set_cflag(C_FLAGS *flg, char *flag, bool on)
{
	switch (*flag)
	{
	case 'a':						/* set fast load flag */
	case 'A':
		/* MAYBE TODO: conflicts with assembler option: list all macro lines */
		flg->strict_ANSI = on;
		break;
	case 'b':
	case 'B':
		flg->output_DRI = on;
		break;
	case 'c':						/* nested comments */
	case 'C':
		/* MAYBE TODO: conflicts with assembler option: no include file listing */
		flg->nested_comments = on;
		break;
	case 'g':						/* size optimization */
	case 'G':
		flg->optimize_size = on;
		break;
	case 'h':						/* use cdecl calling */
	case 'H':
		flg->cdecl_calling = on;
		break;
	case 'j':						/* Dont optimize jumps */
	case 'J':
		flg->no_jump_optimization = on;
		break;
	case 'k':						/* default char is unsigned */
	case 'K':
		flg->char_is_unsigned = on;
		break;
	case 'm':
	case 'M':
		/* MAYBE TODO: conflicts with assembler option: no macro line listing */
		flg->string_merging = on;
		break;
	case 'p':
	case 'P':
		/* MAYBE TODO: conflicts with assembler option: print a listing */
		flg->absolute_calls = on;
		break;
	case 'q':
	case 'Q':
		flg->pascal_calling = on;
		break;
	case 'r':
	case 'R':
		flg->no_register_vars = on;
		break;
	case 's':
	case 'S':
		/* MAYBE TODO: conflicts with assembler option: supervisor-instructions */
		flg->frame_pointer = on;
		break;
	case 't':
	case 'T':
		flg->stack_checking = on;
		break;
	case 'x':
	case 'X':
		flg->add_underline = on;
		break;
	case 'y':
	case 'Y':
		flg->debug_infos = on;
		break;
	case 'z':
	case 'Z':
		flg->no_register_reload = on;
		break;
	case '1':
		flg->i2_68010 = on;
		/* fall through */
	case '0':
		flg->i2_68020 = !on;
		flg->i2_68030 = !on;
		flg->i2_68040 = !on;
		flg->i2_68060 = !on;
		flg->use_FPU = !on;
		flg->Coldfire = !on;
		break;
	case '2':						/* generate for >= 68020 or CF compatible */
		flg->i2_68020 = on;
		break;
	case '3':
		flg->i2_68020 = !on;
		flg->i2_68030 = on;
		break;
	case '4':
		flg->i2_68020 = !on;
		flg->i2_68030 = !on;
		flg->i2_68040 = on;
		flg->use_FPU = !on;
		break;
	case '5':
		flg->i2_68851 = on;
		break;
	case '6':
		flg->i2_68020 = !on;
		flg->i2_68030 = !on;
		flg->i2_68040 = on;
		flg->i2_68060 = on;
		flg->use_FPU = !on;
		break;
	case '7':						/* Coldfire (double is 64 bits)  */
		flg->Coldfire = on;
		break;
	case '8':						/* generate for fpu MC68882 (reals) */
		flg->use_FPU = on;
		break;
	}
	*flag = 0;
}


static const char *xgetopt_arg_r(struct _getopt_data *opts)
{
	const char *opt = getopt_arg_r(opts);
	if (opt != NULL && *opt == '=')
		opt++;
	return opt;
}


static bool parse_c_warning_or_level(const char *arg, C_FLAGS *flg)
{
	char *end;
	int val;
	size_t i;
	bool on;

	if (*arg >= '0' && *arg <= '9')
	{	
		val = (int)strtol(arg, &end, 10);
		if (*end == '\0')
		{
			if (val < 0 || val > MAX_WARNINGLEVEL)
			{
				errout(_("invalid argument: %s"), arg);
				return false;
			}
			flg->warning_level = val;
			return true;
		}
	}

	on = true;
	if (*arg == '-')
	{
		on = false;
		arg++;
		/* -W- without arg: set warning_level 0 */
		if (*arg == '\0')
		{
			flg->warning_level = 0;
			return true;
		}
	} else if (*arg == '+')
	{
		on = true;
		arg++;
		/* -W+ without arg: set warning_level 2 */
		if (*arg == '\0')
		{
			flg->warning_level = 2;
			return true;
		}
	}
	for (i = 0; i < sizeof(warnings) / sizeof(warnings[0]); i++)
	{
		if (strcmp(arg, warnings[i].short_switch) == 0)
			break;
		if (warnings[i].long_switch != NULL && strcmp(arg, warnings[i].long_switch) == 0)
			break;
	}
	if (i >= sizeof(warnings) / sizeof(warnings[0]))
	{
		errout(_("%s: unknown warning category %s"), Warning, arg);
		/* allow for future upward compatibility and dont treat as error */
		return true;
	}
	flg->warning_enabled[warnings[i].category] = on;
	return true;
}


static const char *ON(bool opt)
{
	return opt ? _("ON") : _("OFF");
}


static void print_usage(C_FLAGS *flg, bool to_stderr)
{
	FILE *fp;
	
	fp = to_stderr ? stderr : stdout;
	fprintf(fp, _("usage: PCC [options] file [file...]\n"));
	fprintf(fp, _("-A, --strict-ansi           strict ANSI                         (%s)\n"), ON(flg->strict_ANSI));
	fprintf(fp, _("-B, --dri                   DRI object output                   (%s)\n"), ON(flg->output_DRI));
	fprintf(fp, _("-C, --nested-comments       allow nested comments               (%s)\n"), ON(flg->nested_comments));
	fprintf(fp, _("-D, --define X..X           define macro; optional =value\n"));
	fprintf(fp, _("-E, --max-errors=N          maximum no of erorrs                (%d)\n"), flg->max_errors);
	fprintf(fp, _("-F, --max-warnings=N        maximum no of warnings              (%d)\n"), flg->max_warnings);
	fprintf(fp, _("-G, --optimize-size         size optimization                   (%s)\n"), ON(flg->optimize_size));
	fprintf(fp, _("-H, --fcdecl-calling        cdecl calling                       (%s)\n"), ON(flg->cdecl_calling));
	fprintf(fp, _("-I, --include-dir=<dir>     include directory\n"));
	fprintf(fp, _("-J, --fno-jump-optimization no jump optimization                (%s)\n"), ON(flg->no_jump_optimization));
	fprintf(fp, _("-K, --funsigned-char        default char is unsigned            (%s)\n"), ON(flg->char_is_unsigned));
	fprintf(fp, _("-L, --max-id-length=N       maximum identifier length           (%d)\n"), flg->identifier_max_length);
	fprintf(fp, _("-M, --fmerge-strings        merge identical strings             (%s)\n"), ON(flg->string_merging));
	fprintf(fp, _("-P, --fabsolute-calls       use absolute calls                  (%s)\n"), ON(flg->absolute_calls));
	fprintf(fp, _("-Q, --fpascal-calling       pascal calling                      (%s)\n"), ON(flg->pascal_calling));
	fprintf(fp, _("-R, --fno-register-vars     suppress registerization            (%s)\n"), ON(flg->no_register_vars));
	fprintf(fp, _("-S, --fframe-pointer        force frame pointers                (%s)\n"), ON(flg->frame_pointer));
	fprintf(fp, _("-T, --fstack-check          stack checking                      (%s)\n"), ON(flg->stack_checking));
	fprintf(fp, _("-U, --undefine X..X         undefine macro\n"));
	fprintf(fp, _("-v, --verbose               verbosity                           (%d)\n"), flg->verbose);
	fprintf(fp, _("-W, --warn=<warning>        enable specific Warning\n"));
	fprintf(fp, _("-X, --fleading-underscore   prepend underline to identifiers    (%s)\n"), ON(flg->add_underline));
	fprintf(fp, _("-Y, --debug                 generate debug information          (%s)\n"), ON(flg->debug_infos));
	fprintf(fp, _("-Z, --fno-register-reload   suppress register optimization      (%s)\n"), ON(flg->no_register_reload));
	fprintf(fp, _("-2, --m68020                generate for MC68020+               (%s)\n"), ON(flg->i2_68020));
	fprintf(fp, _("-3, --m68030                generate for MC68030+               (%s)\n"), ON(flg->i2_68030));
	fprintf(fp, _("-4, --m68040                generate for MC68040+               (%s)\n"), ON(flg->i2_68040));
	fprintf(fp, _("-6, --m68060                generate for MC68060+               (%s)\n"), ON(flg->i2_68060));
	fprintf(fp, _("-8, --m68881                generate directly for MC68881/2     (%s)\n"), ON(flg->use_FPU));
	fprintf(fp, _("AHCC specific options:\n"));
	fprintf(fp, _("-7, --mcoldfire             Coldfire                            (%s)\n"), ON(flg->Coldfire));
	fprintf(fp, _("-27                         Coldfire, also runnable on 68020    (%s,%s)\n"), ON(flg->Coldfire), ON(flg->i2_68020));
	fprintf(fp, _("--mno-short                 default int is 32 bits              (%s)\n"), ON(flg->default_int32));
}


static unsigned long hexval(const char *s)
{
	if (*s == '=')
		s++;
	if (*s == '$')
		return strtoul(s + 1, NULL, 16);
	return strtoul(s, NULL, 0);
}


static bool parse_cflags(int argc, const char **argv, C_FLAGS *flg, int *poptind)
{
	struct _getopt_data *opts;
	int c;
	bool result = true;
	char lastflag = 0;
	
	getopt_init_r(program_name, &opts);
	getopt_seterrprint_r(opts, errout);
	while ((c = getopt_long_only_r(argc, argv, "aAbBcCd:D:e:E:f:F:gGhHi:I:jJkKl:L:mMn:N:o:O:pPqQrRsStTu:U:vVw:W:xXyYzZ7012345678-+", long_options, NULL, opts)) != EOF)
	{
		if (c == '-')
			set_cflag(flg, &lastflag, false);
		else
			set_cflag(flg, &lastflag, true);
		switch (c)
		{
		case 'a':						/* strict ANSI */
		case 'A':
		case 'b':						/* DRI output */
		case 'B':
		case 'c':						/* nested comments */
		case 'C':
		case 'g':						/* size optimization */
		case 'G':
		case 'h':						/* use cdecl calling */
		case 'H':
		case 'j':						/* Dont optimize jumps */
		case 'J':
		case 'k':						/* default char is unsigned */
		case 'K':
		case 'm':						/* no string merging */
		case 'M':
		case 'p':						/* use absolute calls */
		case 'P':
		case 'q':						/* use pascal calling */
		case 'Q':
		case 'r':						/* no register variables */
		case 'R':
		case 's':						/* standard stack frames */
		case 'S':
		case 't':						/* stack checking */
		case 'T':
		case 'x':						/* generate underbars */
		case 'X':
		case 'y':						/* add debug information */
		case 'Y':
		case 'z':						/* no register optimization */
		case 'Z':
		case '0':						/* mc68000 */
		case '1':						/* mc68010 */
		case '2':						/* mc68020 */
		case '3':						/* mc68030 */
		case '4':						/* mc68040 */
		case '5':						/* mc68851 */
		case '6':						/* mc68060 */
		case '7':						/* coldfire */
		case '8':						/* mc68881/2 */
			lastflag = c;
			break;
		case 'd':
		case 'D':
			adddef(&flg->defines, xgetopt_arg_r(opts));
			break;
		case 'e':						/* no of errors */
		case 'E':
			flg->max_errors = hexval(xgetopt_arg_r(opts));
			break;
		case 'f':						/* no of warnings */
		case 'F':
			/* MAYBE TODO: conflicts with assembler option: no false condition listing */
			flg->max_warnings = hexval(xgetopt_arg_r(opts));
			break;
		case 'i':
		case 'I':
			doincl(&flg->c_includes, xgetopt_arg_r(opts));
			break;
		case 'K' + 256:
			flg->char_is_unsigned = false;
			break;
		case 'l':
		case 'L':
			flg->identifier_max_length = hexval(xgetopt_arg_r(opts));
			if (flg->identifier_max_length < 0)
				flg->identifier_max_length = DEFAULT_IDLENGTH;
			else if (flg->identifier_max_length > MAX_IDLENGTH)
				flg->identifier_max_length = MAX_IDLENGTH;
			/* zero is ok and means no limit */
			break;
		case 'n':
		case 'N':
			g_free(flg->output_directory);
			flg->output_directory = g_strdup(xgetopt_arg_r(opts));
			break;
		case 'o':
		case 'O':
			g_free(flg->output_name);
			flg->output_name = g_strdup(xgetopt_arg_r(opts));
			break;
		case 'S' + 256:
			flg->frame_pointer = false;
			break;
		case 'u':
		case 'U':
			subdef(flg, xgetopt_arg_r(opts));
			break;
		case 'v':
		case 'V':
			flg->verbose += 1;		/* number of v's = level of verbosity */
			break;
		case 'w':
		case 'W':
			result &= parse_c_warning_or_level(xgetopt_arg_r(opts), flg);
			break;

		case OPT_MLONG:						/* default int is 32 bits */
			flg->default_int32 = true;
			break;
		case OPT_MSHORT:					/* default int is 16 bits */
			flg->default_int32 = false;
			break;
		case OPT_SUPER:
			/* flg->supervisor = true; */
			break;
		case OPT_NO_OUTPUT:					/* No object output */
			flg->no_output = true;
			break;
		
		case '-':
		case '+':
			break;
		
		default:
			result = false;
			break;
		}
	}

	set_cflag(flg, &lastflag, true);

	c = getopt_finish_r(&opts);
	if (poptind)
		*poptind = c;
	
	return result;
}


C_FLAGS *copy_cflags(const C_FLAGS *src)
{
	C_FLAGS *dst;
	
	dst = g_new(C_FLAGS, 1);
	if (dst != NULL)
	{
		*dst = *src;
		dst->output_directory = g_strdup(src->output_directory);
		dst->output_name = NULL;
		dst->defines = list_copy(src->defines);
		dst->undefines = list_copy(src->undefines);
		dst->c_includes = list_copy(src->c_includes);
	}
	return dst;
}


bool parse_cc_options(const char *arg, C_FLAGS *flg)
{
	char **argv;
	int argc;
	bool result;
	
	argv = split_args("cc", arg, &argc, ',');
	result = parse_cflags(argc, (const char **)argv, flg, NULL);
	strfreev(argv);
	if (!result)
		print_usage(flg, true);
	return result;
}
