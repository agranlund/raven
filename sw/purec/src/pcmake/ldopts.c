#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "getopt.h"
#include "pcmake.h"

#define OPT_BINARY 256

static struct option const long_options[] = {
	{ "global-symbols", no_argument, NULL, 'G' },
	{ "new-object", no_argument, NULL, 'J' },
	{ "local-symbols", no_argument, NULL, 'L' },
	{ "malloc-stram", no_argument, NULL, 'M' },
	{ "no-fastload", no_argument, NULL, 'F' },
	{ "output", required_argument, NULL, 'o' },
	{ "symbol-list", no_argument, NULL, 'n' },
	{ "map", no_argument, NULL, 'p' },
	{ "program-stram", no_argument, NULL, 'R' },
	{ "stacksize", required_argument, NULL, 'S' },
	{ "heapsize", required_argument, NULL, 'H' },
	{ "bss-start", required_argument, NULL, 'B' },
	{ "data-start", required_argument, NULL, 'D' },
	{ "text-start", required_argument, NULL, 'T' },
	{ "imagesize", required_argument, NULL, 'I' },
	{ "verbose", no_argument, NULL, 'V' },
	{ "debug", no_argument, NULL, 'Y' },
	{ "binary", no_argument, NULL, OPT_BINARY },
	{ NULL, no_argument, NULL, 0 }
};


static void print_usage(bool to_stderr)
{
	FILE *fp;
	
	fp = to_stderr ? stderr : stdout;
	fprintf(fp, ("usage: PLINK [<options>] [<file>...]\n"));
	fprintf(fp, ("  -V, --verbose            Verbose\n"));
	fprintf(fp, ("  -J, --new-object         Generate object output (make new linkable object)\n"));
	fprintf(fp, ("  -o, --output=<file>      Output file name\n"));
	fprintf(fp, ("  -N, --symbol-list        Write 'nm' type symbol list\n"));
	fprintf(fp, ("  -P, --map                Write load map to [-o].map\n"));
	fprintf(fp, ("\n"));
	fprintf(fp, ("Options for executable output:\n"));
	fprintf(fp, ("  -S, --stacksize=N        Set output stacksize\n"));
	fprintf(fp, ("  -G, --global-symbols     Add global symbols\n"));
	fprintf(fp, ("  -L, --local-symbols      Add local symbols\n"));
	fprintf(fp, ("  -Y, --debug              Generate debug information\n"));
	fprintf(fp, ("Set load flags:\n"));
	fprintf(fp, ("  -M, --malloc-stram       Mallocs for ST-Ram\n"));
	fprintf(fp, ("  -F, --no-fastload        Suppress fastload bit in output\n"));
	fprintf(fp, ("  -R, --program-stram      Load Program to ST-Ram\n"));
	fprintf(fp, ("  -T, --text-start=N       Make ROM image @ N\n"));
	fprintf(fp, ("  -D, --data-start=N       Set absolute data base address\n"));
	fprintf(fp, ("  -B, --bss-start=N        Set absolute bss base address\n"));
	fprintf(fp, ("  -I, --imagesize=N        Image fixed size\n"));
}


static const char *xgetopt_arg_r(struct _getopt_data *opts)
{
	const char *opt = getopt_arg_r(opts);
	if (opt != NULL && *opt == '=')
		opt++;
	return opt;
}


static void set_ldflag(LD_FLAGS *flg, char *flag, bool on)
{
	switch (*flag)
	{
	case 'f':						/* set fast load flag */
	case 'F':
		flg->no_fastload = on;
		break;
	case 'g':						/* add global symbols */
	case 'G':
		flg->global_symbols = on;
		break;
	case 'j':						/* make new lib */
	case 'J':
		flg->create_new_object = on;
		break;
	case 'l':						/* add local symbols */
	case 'L':
		flg->local_symbols = on;
		break;
	case 'm':						/* set malloc for STram flag */
	case 'M':
		flg->malloc_for_stram = on;
		break;
	case 'n':
	case 'N':
		flg->nm_list = on;			/* print 'nm' symbol list */
		break;
	case 'p':						/* print load map */
	case 'P':
		flg->load_map = on;
		break;
	case 'r':						/* load program in ST ram */
	case 'R':
		flg->program_to_stram = on;
		break;
	case 'y':						/* add debug info */
	case 'Y':
		flg->debug_infos = on;
		break;
	case 'v':						/* verbose message output */
	case 'V':
		flg->verbose = on;
		break;
	}
	*flag = 0;
}


void init_ldflags(LD_FLAGS *flg)
{
	memset(flg, 0, sizeof(*flg));
	flg->verbose = 0;
	flg->no_fastload = false;
	flg->malloc_for_stram = false;
	flg->program_to_stram = false;
	flg->create_new_object = false;
	flg->debug_infos = false;
	flg->load_map = false;
	flg->global_symbols = false;
	flg->local_symbols = false;
	flg->nm_list = false;
	flg->heap_size = 0;
	flg->text_start = 0;
	flg->data_start = 0;
	flg->bss_start = 0;
	flg->imgsize = 0;
	flg->binary = false;
	flg->stacksize = DEFAULT_STACKSIZE;
	flg->output_filename = NULL;
}


void free_ldflags(LD_FLAGS *flg)
{
	g_free(flg->output_filename);
	flg->output_filename = NULL;
}


static unsigned long hexval(const char *s)
{
	if (*s == '=')
		s++;
	if (*s == '$')
		return strtoul(s + 1, NULL, 16);
	return strtoul(s, NULL, 0);
}


static bool parse_ldflags(int argc, const char **argv, LD_FLAGS *flg, int *poptind)
{
	struct _getopt_data *opts;
	int c;
	bool result = true;
	char lastflag = 0;
	
	getopt_init_r(program_name, &opts);
	getopt_seterrprint_r(opts, errout);
	while ((c = getopt_long_only_r(argc, argv, "b:B:c:C:d:D:gGh:H:i:I:jJlLmMnNo:pPrRs:S:t:T:vVyY-+", long_options, NULL, opts)) != EOF)
	{
		if (c == '-')
			set_ldflag(flg, &lastflag, false);
		else
			set_ldflag(flg, &lastflag, true);
		switch (c)
		{
		case 'b':						/* bss  segment address */
		case 'B':
			flg->bss_start = hexval(xgetopt_arg_r(opts));
			break;
		case 'd':						/* data segment address */
		case 'D':
			flg->data_start = hexval(xgetopt_arg_r(opts));
			break;
		case 'f':						/* set fast load flag */
		case 'F':
		case 'g':						/* add global symbols */
		case 'G':
		case 'j':						/* make new lib */
		case 'J':
		case 'l':						/* add local symbols */
		case 'L':
		case 'm':						/* set malloc for STram flag */
		case 'M':
		case 'n':
		case 'N':
		case 'p':						/* print load map */
		case 'P':
		case 'r':						/* load program in ST ram */
		case 'R':
		case 'y':						/* add debug info */
		case 'Y':
			lastflag = c;
			break;
		case 'h':						/* heap size */
		case 'H':
			flg->heap_size = hexval(xgetopt_arg_r(opts));
			break;
		case 'i':
		case 'I':
			flg->imgsize = hexval(xgetopt_arg_r(opts));
			break;
		case 'o':						/* object file name */
			g_free(flg->output_filename);
			flg->output_filename = g_strdup(xgetopt_arg_r(opts));
			break;
		case 's':
		case 'S':
			flg->stacksize = hexval(xgetopt_arg_r(opts));
			break;
		case 't':						/* text segment address */
		case 'T':
			flg->text_start = hexval(xgetopt_arg_r(opts));
			break;
		case 'v':						/* verbose */
		case 'V':
			flg->verbose++;
			break;

		case OPT_BINARY:
			flg->binary = true;
			break;

		case '-':
		case '+':
			break;
		
		default:
			result = false;
			break;
		}
	}
	set_ldflag(flg, &lastflag, true);
	
	c = getopt_finish_r(&opts);
	if (poptind)
		*poptind = c;
	
	return result;
}


bool parse_ld_options(const char *arg, LD_FLAGS *flg)
{
	char **argv;
	int argc;
	bool result;
	
	argv = split_args("ld", arg, &argc, ',');
	result = parse_ldflags(argc, (const char **)argv, flg, NULL);
	strfreev(argv);
	if (!result)
		print_usage(true);
	return result;
}
