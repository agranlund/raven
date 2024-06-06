#include <getopt.h>
#include <string.h>
#include <stdio.h>

char *optarg;
int optind = 1;		/* next element to be processed */
int opterr = 1;
int optopt;

int getopt(int argc, char * const argv[], const char *optstring)
{
	static int sp = 1;
	char *cp;
	int c;

	if (sp == 1)
	{
		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
		{
			return -1;
		}
		else if (!strcmp(argv[optind], "--"))
		{
			optind++;
			return -1;
		}
	}

	optopt = c = argv[optind][sp];

	if (c == ':' || (cp = strchr(optstring, c)) == NULL)
	{
		if (opterr)
		{
			fprintf(stderr, "%s: illegal option -- %c\n", argv[0], c);
			sp = 1;
			return '?';
		}
		else
			optarg = argv[optind++];
		sp = 1;
	}
	else
	{
		if (argv[optind][++sp] == '\0')
		{
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return c;
}

