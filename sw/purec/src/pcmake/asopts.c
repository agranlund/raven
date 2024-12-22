#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "getopt.h"
#include "pcmake.h"

#define OPT_SUPER            506
#define OPT_NO_OUTPUT        511

static struct option const long_options_as[] = {
	{ "list-all-macro-lines", no_argument, NULL, 'A' },
	{ "dri", no_argument, NULL, 'B' },
	{ "no-include-lines", no_argument, NULL, 'C' },
	{ "define", required_argument, NULL, 'D' },
	{ "no-false-condition-listing", no_argument, NULL, 'F' },
	{ "include-dir", required_argument, NULL, 'I' },
	{ "no-macro-line-listing", no_argument, NULL, 'M' },
	{ "output-directory", required_argument, NULL, 'N' },
	{ "output", required_argument, NULL, 'o' },
	{ "print-listing", no_argument, NULL, 'P' },
	{ "msuper", no_argument, NULL, 'S' },
	{ "fundefined-symbols-external", no_argument, NULL, 'U' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "fleading-underscore", no_argument, NULL, 'X' },
	{ "debug", no_argument, NULL, 'Y' },
	
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
	
	{ "msuper", no_argument, NULL, OPT_SUPER },
	{ "fno-output", no_argument, NULL, OPT_NO_OUTPUT },
	
	{ NULL, no_argument, NULL, 0 }
};


void init_aflags(A_FLAGS *flg)
{
	memset(flg, 0, sizeof(*flg));
	flg->output_DRI = false;
	flg->output_directory = NULL;
	flg->output_name = NULL;
	flg->verbose = 0;
	flg->i2_68010 = false;
	flg->i2_68020 = false;
	flg->i2_68030 = false;
	flg->i2_68040 = false;
	flg->i2_68060 = false;
	flg->i2_68851 = false;
	flg->use_FPU = false;
	flg->Coldfire = false;
	flg->defines = NULL;
	flg->as_includes = NULL;
	
	flg->no_output = false;

	flg->supervisor = false;
	flg->undefined_external = false;
	flg->list_all_macro_lines = false;
	flg->no_include_line_listing = false;
	flg->no_macro_line_listing = false;
	flg->no_false_condition_listing = false;
	flg->print_listing = false;
}


static const char *xgetopt_arg_r(struct _getopt_data *opts)
{
	const char *opt = getopt_arg_r(opts);
	if (opt != NULL && *opt == '=')
		opt++;
	return opt;
}


static void set_asflag(A_FLAGS *flg, char *flag, bool on)
{
	switch (*flag)
	{
	case 'a':
	case 'A':
		flg->list_all_macro_lines = on;
		break;
	case 'b':
	case 'B':
		flg->output_DRI = on;
		break;
	case 'c':
	case 'C':
		flg->no_include_line_listing = on;
		break;
	case 'f':
	case 'F':
		flg->no_false_condition_listing = on;
		break;
	case 'm':
	case 'M':
		flg->no_macro_line_listing = on;
		break;
	case 'p':
	case 'P':
		flg->print_listing = on;
		break;
	case 's':
	case 'S':
		flg->supervisor = on;
		break;
	case 'u':
	case 'U':
		flg->undefined_external = on;
		break;
	case 'y':
	case 'Y':
		flg->debug_infos = on;
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
		flg->i2_68020 = on;
		flg->i2_68030 = on;
		break;
	case '4':
		flg->i2_68020 = on;
		flg->i2_68030 = on;
		flg->i2_68040 = on;
		flg->use_FPU = on;
		break;
	case '5':
		flg->i2_68851 = on;
		break;
	case '6':
		flg->i2_68020 = on;
		flg->i2_68030 = on;
		flg->i2_68040 = on;
		flg->i2_68060 = on;
		flg->use_FPU = on;
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


static bool parse_asflags(int argc, const char **argv, A_FLAGS *flg, int *poptind)
{
	struct _getopt_data *opts;
	int c;
	bool result = true;
	char lastflag = 0;
	
	getopt_init_r(program_name, &opts);
	getopt_seterrprint_r(opts, errout);
	while ((c = getopt_long_only_r(argc, argv, "aAbBcCd:D:fFi:I:mMo:O:pPsSuUvVw:W:xXyY012345678-+", long_options_as, NULL, opts)) != EOF)
	{
		if (c == '-')
			set_asflag(flg, &lastflag, false);
		else
			set_asflag(flg, &lastflag, true);
		switch (c)
		{
		case 'a':						/* list all macro lines */
		case 'A':
		case 'b':						/* DRI output */
		case 'B':
		case 'c':						/* no include line lsiting */
		case 'C':
		case 'f':						/* no false condition listing */
		case 'F':
		case 'm':
		case 'M':
		case 'p':
		case 'P':
		case 's':
		case 'S':
		case 'u':
		case 'U':
		case 'x':
		case 'X':
		case 'y':
		case 'Y':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			lastflag = c;
			break;
		case 'd':
		case 'D':
			adddef(&flg->defines, xgetopt_arg_r(opts));
			break;
		case 'i':
		case 'I':
			doincl(&flg->as_includes, xgetopt_arg_r(opts));
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
		case 'v':
		case 'V':
			flg->verbose += 1;		/* number of v's = level of verbosity */
			break;
		case 'w':
		case 'W':
			/* TODO: enable warnings */
			break;

		case OPT_SUPER:
			flg->supervisor = true;
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

	set_asflag(flg, &lastflag, true);

	c = getopt_finish_r(&opts);
	if (poptind)
		*poptind = c;
	
	return result;
}



static const char *ON(bool opt)
{
	return opt ? _("ON") : _("OFF");
}


static void print_usage(A_FLAGS *flg, bool to_stderr)
{
	FILE *fp;
	
	fp = to_stderr ? stderr : stdout;
	fprintf(fp, _("usage: PASM [options] file [file...]\n"));
	fprintf(fp, _("-A, --list-all-macro-lines  List all macro lines                (%s)\n"), ON(flg->list_all_macro_lines));
	fprintf(fp, _("-B, --dri                   DRI object output                   (%s)\n"), ON(flg->output_DRI));
	fprintf(fp, _("-C, --no-include-lines      No include line listing             (%s)\n"), ON(flg->no_include_line_listing));
	fprintf(fp, _("-D, --define X..X           define macro; optional =value\n"));
	fprintf(fp, _("-F, --no-false-condition    No false condition listing          (%s)\n"), ON(flg->no_false_condition_listing));
	fprintf(fp, _("-I, --include-dir=<dir>     include directory\n"));
	fprintf(fp, _("-M, --no-macro-line         No macro line listing               (%s)\n"), ON(flg->no_macro_line_listing));
	fprintf(fp, _("-P, --print-listing         Print a listing                     (%s)\n"), ON(flg->print_listing));
	fprintf(fp, _("-S, --msuper                Privileged instructions             (%s)\n"), ON(flg->supervisor));
	fprintf(fp, _("-U, --fundefined-external   Undefined symbols external          (%s)\n"), ON(flg->undefined_external));
	fprintf(fp, _("-V, --verbose               Verbose message output              (%d)\n"), flg->verbose);
	fprintf(fp, _("-Y, --debug                 generate debug information          (%s)\n"), ON(flg->debug_infos));
	fprintf(fp, _("-2, --m68020                generate for MC68020+               (%s)\n"), ON(flg->i2_68020));
	fprintf(fp, _("-3, --m68030                generate for MC68030+               (%s)\n"), ON(flg->i2_68030));
	fprintf(fp, _("-4, --m68040                generate for MC68040+               (%s)\n"), ON(flg->i2_68040));
	fprintf(fp, _("-6, --m68060                generate for MC68060+               (%s)\n"), ON(flg->i2_68060));
	fprintf(fp, _("-8, --m68881                generate directly for MC68881/2     (%s)\n"), ON(flg->use_FPU));
	fprintf(fp, _("AHCC specific options:\n"));
	fprintf(fp, _("-7, --mcoldfire             Coldfire                            (%s)\n"), ON(flg->Coldfire));
	fprintf(fp, _("-27                         Coldfire, also runnable on 68020    (%s,%s)\n"), ON(flg->Coldfire), ON(flg->i2_68020));
}


void free_aflags(A_FLAGS *a_flags)
{
	list_free(&a_flags->defines);
	list_free(&a_flags->as_includes);
}


A_FLAGS *copy_aflags(const A_FLAGS *src)
{
	A_FLAGS *dst;
	
	dst = g_new(A_FLAGS, 1);
	if (dst != NULL)
	{
		*dst = *src;
		dst->output_directory = g_strdup(src->output_directory);
		dst->output_name = NULL;
		dst->defines = list_copy(src->defines);
		dst->as_includes = list_copy(src->as_includes);
	}
	return dst;
}


bool parse_as_options(const char *arg, A_FLAGS *flg)
{
	char **argv;
	int argc;
	bool result;
	
	argv = split_args("as", arg, &argc, ',');
	result = parse_asflags(argc, (const char **)argv, flg, NULL);
	strfreev(argv);
	if (!result)
		print_usage(flg, true);
	return result;
}
