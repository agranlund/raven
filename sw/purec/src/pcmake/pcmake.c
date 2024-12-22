#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#if defined(__TOS__) || defined(__atarist__)
#ifdef __GNUC__
#include <osbind.h>
#include <mint/mintbind.h>
#define DTA _DTA
#else
#include <tos.h>
#endif
#else
#include <dirent.h>
#endif
#include "getopt.h"
#include "pcmake.h"

enum opt {
	OPT_ALL =             'B',
	OPT_VERBOSE =         'v',
	OPT_SILENT =          's',
	OPT_CHANGEDIR =       'C',
	OPT_QUESTION =        'q',
	OPT_DEBUG =           'd',
	OPT_ENVIRON =         'e',
	OPT_JOBS =            'j',
	OPT_MAKEFILE =        'f',
	OPT_KEEP_GOING =      'k',
	OPT_IGNORE_ERRORS =   'i',
	OPT_DRYRUN =          'n',
	OPT_OLD_FILE =        'o',
	OPT_TOUCH =           't',
	OPT_PRINT_DIRECTORY = 'w',
	OPT_WHATIF =          'W',
	OPT_NFDEBUG =         'F',
	OPT_HELP =            'h',
	OPT_VERSION =         'V',
	
	OPT_NO_PRINT_DIRECTORY = 256,
};

char const program_name[] = "pcmake";
char const program_version[] = "1.1";

static bool show_version;
static bool show_help;

static MAKEOPTS makeopts;

static struct option const long_options[] = {
	{ "always-make", no_argument, NULL, OPT_ALL },
	{ "verbose", no_argument, NULL, OPT_VERBOSE },
	{ "silent", no_argument, NULL, OPT_SILENT },
	{ "quiet", no_argument, NULL, OPT_SILENT },
	{ "debug", optional_argument, NULL, OPT_DEBUG },
	{ "environment-overrides", no_argument, NULL, OPT_ENVIRON },
	{ "question", no_argument, NULL, OPT_QUESTION },
	{ "keep-going", no_argument, NULL, OPT_KEEP_GOING },
	{ "just-print", no_argument, NULL, OPT_DRYRUN },
	{ "dry-run", no_argument, NULL, OPT_DRYRUN },
	{ "recon", no_argument, NULL, OPT_DRYRUN },
	{ "touch", no_argument, NULL, OPT_TOUCH },
	{ "ignore-errors", no_argument, NULL, OPT_IGNORE_ERRORS },
	{ "file", required_argument, NULL, OPT_MAKEFILE },
	{ "makefile", required_argument, NULL, OPT_MAKEFILE },
	{ "what-if", required_argument, NULL, OPT_WHATIF },
	{ "new-file", required_argument, NULL, OPT_WHATIF },
	{ "assume-new", required_argument, NULL, OPT_WHATIF },
	{ "jobs", optional_argument, NULL, OPT_JOBS },
	{ "directory", required_argument, NULL, OPT_CHANGEDIR },
	{ "old-file", required_argument, NULL, OPT_OLD_FILE },
	{ "natfeat-debug", no_argument, NULL, OPT_NFDEBUG },
	{ "nfdebug", no_argument, NULL, OPT_NFDEBUG },
	{ "no-print-directory", no_argument, NULL, OPT_NO_PRINT_DIRECTORY },
	{ "print-directory", no_argument, NULL, OPT_PRINT_DIRECTORY },
	{ "help", no_argument, NULL, OPT_HELP },
	{ "version", no_argument, NULL, OPT_VERSION },
	
	{ NULL, no_argument, NULL, 0 }
};


static void print_usage(bool to_stderr)
{
	FILE *fp;
	
	fp = to_stderr ? stderr : stdout;
	fprintf(fp, _("usage: %s [options] [<project-file>]\n"), program_name);
	fprintf(fp, _("options:\n"));
	fprintf(fp, _("  -B, --always-make        Unconditionally make all targets.\n"));
	fprintf(fp, _("  -C, --directory=DIR      Change to DIRECTORY before doing anything.\n"));
	fprintf(fp, _("  -f, --file=FILE          Read FILE as a project file.\n"));
	fprintf(fp, _("  -s, --silent             Don't echo commands.\n"));
	fprintf(fp, _("  -v, --verbose            Increase verbosity.\n"));
#if defined(__TOS__) || defined(__atarist__)
	fprintf(fp, _("  -F, --nfdebug            Echo commands also to emulator, if present.\n"));
#endif
	fprintf(fp, _("  -n, --dry-run            Don't actually run any command; just print them.\n"));
	fprintf(fp, _("  -w, --print-directory    Print the current directory.\n"));
	fprintf(fp, _("      --no-print-directory Turn off -w, even if it was turned on implicitly.\n"));
	fprintf(fp, _("  -V, --version            Print the version number and exit.\n"));
	fprintf(fp, _("  -h, --help               Display this help and exit.\n"));
	fprintf(fp, "\n");
	fprintf(fp, _("If no project file is given, the first *.prj file in\n"
	              "the current directory will be used\n"));
}


static void print_version(bool to_stderr)
{
	FILE *fp;
	
	fp = to_stderr ? stderr : stdout;
	fprintf(fp, _("%s version %s\n"), program_name, program_version);
}


int main(int argc, const char **argv)
{
	struct _getopt_data *opts;
	int c;
	int opti;
	int err = EXIT_SUCCESS;
	PRJ *prj = NULL;
	const char *prj_name;
	char *prj_name_copy = NULL;
	
#if defined(__TOS__) || defined(__atarist__)
	static DTA dta;
	Fsetdta(&dta);
	Pdomain(1);
	_mallocChunkSize(0);
#endif
	set_pcdir(argv[0]);
#if 0
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
#endif
	
	getopt_init_r(program_name, &opts);
	getopt_seterrprint_r(opts, errout);
	
	show_help = false;
	show_version = false;
	makeopts.make_all = false;
	makeopts.ignore_date = false;
	makeopts.verbose = 0;
	makeopts.silent = false;
	makeopts.debug = false;
	makeopts.nfdebug = false;
	makeopts.print_directory = true;
	makeopts.directory = NULL;
	makeopts.dryrun = false;
	prj_name = NULL;
	
	while ((c = getopt_long_only_r(argc, argv, "BC:FW:d::ef:ij::kno:qstvwhV", long_options, NULL, opts)) != EOF)
	{
		switch (c)
		{
		case OPT_ALL:
			makeopts.make_all = true;
			makeopts.ignore_date = true;
			break;
		case OPT_VERBOSE:
			++makeopts.verbose;
			break;
		case OPT_SILENT:
			makeopts.silent = true;
			break;
		case OPT_CHANGEDIR:
			g_free(makeopts.directory);
			makeopts.directory = g_strdup(getopt_arg_r(opts));
			strbslash(makeopts.directory);
			break;
		case OPT_DEBUG:
			makeopts.debug = true;
			break;
		case OPT_NFDEBUG:
#if defined(__TOS__) || defined(__atarist__)
			makeopts.nfdebug = true;
#else
			errout(_("%s: option %c not supported on this system"), program_name, c);
#endif
			break;
		case OPT_MAKEFILE:
			prj_name = getopt_arg_r(opts);
			break;
		case OPT_PRINT_DIRECTORY:
			makeopts.print_directory = true;
			break;
		case OPT_NO_PRINT_DIRECTORY:
			makeopts.print_directory = false;
			break;
		case OPT_DRYRUN:
			makeopts.dryrun = true;
			break;

		case OPT_QUESTION:
		case OPT_JOBS:
		case OPT_KEEP_GOING:
		case OPT_ENVIRON:
		case OPT_TOUCH:
		case OPT_WHATIF:
		case OPT_OLD_FILE:
			errout(_("NYI: -%c"), c);
			err = EXIT_FAILURE;
			break;
		
		case OPT_HELP:
			show_help = true;
			break;
		case OPT_VERSION:
			show_version = true;
			break;

		default:
			errout(_("try %s --help for a list of valid options"), program_name);
			err = EXIT_FAILURE;
			break;
		}
	}
	opti = getopt_finish_r(&opts);
	
	argc -= opti;
	argv += opti;
	if (err != EXIT_SUCCESS)
	{
		/* nothing */
	} else if (show_version)
	{
		print_version(false);
	} else if (show_help)
	{
		print_usage(false);
	} else
	{
		errout_nfdebug = makeopts.nfdebug;

		if (err == EXIT_SUCCESS)
		{
			if (makeopts.directory)
			{
				char *dir = build_path(makeopts.directory, NULL);
				if (ch_dir(dir) < 0)
				{
					errout(_("%s: cannot chdir to %s"), program_name, dir);
					err = EXIT_FAILURE;
				}
				g_free(dir);
			}
		}

		if (prj_name == NULL)
		{
			if (argc == 1)
			{
				prj_name = argv[0];
			} else if (argc == 0)
			{
#if defined(__TOS__) || defined(__atarist__)
				if (Fsfirst("*.prj", FA_RDONLY) == 0)
				{
					prj_name_copy = g_strdup(dta.dta_name);
					while (Fsnext() == 0)
					{
						errout(_("%s: more than one project file found"), program_name);
						err = EXIT_FAILURE;
					}
				}
#else
				DIR *dir;
				struct dirent *entry;
				
				dir = opendir(".");
				if (dir != NULL)
				{
					while ((entry = readdir(dir)) != NULL)
					{
						size_t len = strlen(entry->d_name);
						if (len > 4 && stricmp(entry->d_name + len - 4, ".prj") == 0)
						{
							if (prj_name_copy != NULL)
							{
								errout(_("%s: more than one project file found"), program_name);
								err = EXIT_FAILURE;
							} else
							{
								prj_name_copy = g_strdup(entry->d_name);
							}
						}
					}
					closedir(dir);
				}
#endif
				if (prj_name_copy == NULL)
				{
					errout(_("%s: no project file found"), program_name);
					err = EXIT_FAILURE;
				}
				prj_name = prj_name_copy;
			} else
			{
				print_usage(true);
				err = EXIT_FAILURE;
			}
		}

		if (err == EXIT_SUCCESS)
		{
			char *dir = get_cwd();
			char *name = build_path(dir, prj_name);
			prj = loadmake(&makeopts, name);
			if (prj == NULL)
			{
				err = EXIT_FAILURE;
			}
			g_free(name);
			g_free(dir);
		}
		if (err == EXIT_SUCCESS)
		{
			if (!domake(prj, &makeopts))
				err = EXIT_FAILURE;
			free_project(prj);
		}
		g_free(prj_name_copy);
	}

	exec_exit();
	g_free(makeopts.directory);
	
#if DEBUG_ALLOC
	_crtexit();
#endif

	return err;
}
