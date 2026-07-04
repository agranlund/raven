/*
	NAME
		strip - Motorola DSP COFF file strip utility

	SYNOPSIS
		strip files

	DESCRIPTION

		This program strips line number and symbol table
		information from Motorola DSP COFF object files.

	OPTIONS
		q	- do not display signon banner.

		Error messages are sent to the standard error output when
		files cannot be open.

	HISTORY
		1.0	The beginning.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

/*
  Headers for working with COFF files
*/

#include "coreaddr.h"
#include "maout.h"
#include "dspext.h"

/*
  General-purpose defines
*/

#define TRUE	1
#define FALSE	0
#define EOS	'\0'						/* end-of-string constant */
#define MAXSTR	512						/* maximum string size + 1 */
#define MAXBUF	512						/* maximum buffer size */
#define STREQ(a,b)	(*(a) == *(b) && strcmp((a), (b)) == 0)
#define BUFLEN	(MAXBUF * sizeof (long))	/* length of data buffer */

union wrd
{										/* word union for byte swapping */
	uint32_t l;
	unsigned char b[4];
};

/*
  Global variables
*/

static char const Progname[] = "strip";				/* program default name */
static char const Progtitle[] = "DSP COFF File Strip Utility";

/*
  Put an extra space after the version number so that it is not
  compressed in the object file.  That way the strings program
  can find it.
*/
static char const Version[] = "Version 6.3 ";		/* strip version number */
static char const Copyright[] = "(C) Copyright Motorola, Inc. 1991-1996.  All rights reserved.";

static char *coptarg = NULL;				/* command argument pointer */
static int coptind = 0;					/* command argument index */

static FILE *ifile = NULL;				/* file pointer for input file */
static char *ifn = NULL;				/* pointer to input file name */
static FILE *ofile = NULL;				/* file pointer for output file */
static const char *ofn = NULL;			/* pointer to output file name */
static const char *tfn = NULL;			/* pointer to temporary file name */
static int quiet = FALSE;				/* signon banner flag */


__attribute__((format(printf, 1, 2)))
static void error(const char *fmt, ...)		/* display error on stderr, exit nonzero */
{
	va_list ap;
	int err = errno;

	va_start(ap, fmt);
	(void) fprintf(stderr, "%s: ", Progname);
	(void) vfprintf(stderr, fmt, ap);
	(void) fprintf(stderr, "\n");
	va_end(ap);
	if (err)
	{
		errno = err;
		perror(Progname);
	}
	unlink(ofn);
	exit(1);
}


/**
* name		setfile --- set the file type and creator if necessary
*
* synopsis	yn = setfile (fn, type, creator)
*		int yn;		TRUE on success, FALSE on failure
*		char *fn;	pointer to file name
*		char *type;	pointer to file type
*		char *creator;	pointer to file creator
*
* description	Sets the file type and creator for newly-created Macintosh
*		output files.  Simply returns TRUE on other hosts.
*
**/
#if defined (MPW)
static int setfile(const char *fn, const char *type, const char *creator)
{
	int i;
	short status;
	struct FInfo finfo;
	OSType val;
	static char buf[256];

	(void) strcpy(buf, fn);
	c2pstr(buf);
	if ((status = GetFInfo((unsigned char *) &buf[0], (short) 0, &finfo)) != 0)
		return FALSE;
	i = 0;
	val = (OSType) 0;
	while (i < sizeof(OSType))
	{
		val <<= (OSType) 8;
		val += (OSType) type[i++];
	}
	finfo.fdType = val;
	i = 0;
	val = (OSType) 0;
	while (i < sizeof(OSType))
	{
		val <<= (OSType) 8;
		val += (OSType) creator[i++];
	}
	finfo.fdCreator = val;
	if ((status = SetFInfo((unsigned char *) &buf[0], (short) 0, &finfo)) != 0)
		return FALSE;
	return TRUE;
}
#else
#define setfile(fn, type, creator)
#endif


#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
/**
*
* name		swapw - swap bytes in words
*
* synopsis	swapw (ptr, size, nitems)
*		char *ptr;		pointer to buffer
*		int size;		size of buffer
*		int nitems;		number of items to write
*
* description	Treats ptr as reference to union array; if necessary,
*		swaps bytes to maintain base format byte ordering
*		(big endian).
*
**/
static void swapw(char *ptr, int size, int nitems)
{
	union wrd *w;
	union wrd *end = (union wrd *) ptr + ((size * nitems) / sizeof(union wrd));
	unsigned i;

	for (w = (union wrd *) ptr; w < end; w++)
	{
		i = w->b[0];
		w->b[0] = w->b[3];
		w->b[3] = i;
		i = w->b[1];
		w->b[1] = w->b[2];
		w->b[2] = i;
	}
}
#else
#define swapw(ptr, size, nitems)
#endif

/**
*
* name		freads - swap bytes and read
*
* synopsis	freads (ptr, size, nitems, stream)
*		char *ptr;		pointer to buffer
*		int size;		size of buffer
*		int nitems;		number of items to read
*		FILE *stream;		file pointer
*
* description	Treats ptr as reference to union array; if necessary,
*		swaps bytes to maintain base format byte ordering
*		(big endian).  Calls fread to do I/O.
*
**/
static int freads(char *ptr, int size, int nitems, FILE *stream)
{
	int rc;

	rc = fread(ptr, size, nitems, stream);
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
	swapw(ptr, size, nitems);
#endif
	return rc;
}


/**
*
* name		fwrites - swap bytes and write
*
* synopsis	fwrites (ptr, size, nitems, stream)
*		char *ptr;		pointer to buffer
*		int size;		size of buffer
*		int nitems;		number of items to write
*		FILE *stream;		file pointer
*
* description	Treats ptr as reference to union array; if necessary,
*		swaps bytes to maintain base format byte ordering
*		(big endian).  Calls fwrite to do I/O.
*
**/
static int fwrites(char *ptr, int size, int nitems, FILE *stream)
{
	swapw(ptr, size, nitems);
	return fwrite(ptr, size, nitems, stream);
}


static void strip_file(void)
{
	FILHDR file_header;
	AOUTHDR opt_header;
	XCNHDR *scn_header;
	XCNHDR *sh;
	XCNHDR *new_scn_header;
	long opthdr;
	long nscns;
	long dsize = 0;
	long sec_data_offset;
	int i;

	/* read and write file headers */
	if (freads((char *) &file_header, sizeof(FILHDR), 1, ifile) != 1)
		error("cannot read file header");
	errno = 0;							/* set errno here for non-system errors */
	if (!(file_header.f_flags & F_RELFLG))	/* must be absolute object */
		error("invalid object file type");
	if (!file_header.f_nsyms)			/* already stripped */
		error("%s: already stripped", ifn);
	file_header.f_flags |= F_LNNO;		/* indicate line numbers stripped */
	file_header.f_nsyms = 0L;			/* zero count of symbol entries */
	/*
	   We have to save the next two fields before the write because
	   bytes may be swapped on the write and they are swapped *in place*.
	 */
	opthdr = file_header.f_opthdr;		/* save optional header length */
	nscns = file_header.f_nscns;		/* save number of sections */
	if (fwrites((char *) &file_header, sizeof(FILHDR), 1, ofile) != 1)
		error("cannot write file header");
	if (file_header.f_opthdr)
	{									/* optional header present */
		if (freads((char *) &opt_header, (int) opthdr, 1, ifile) != 1)
			error("cannot read optional header");
		if (fwrites((char *) &opt_header, (int) opthdr, 1, ofile) != 1)
			error("cannot read optional header");
	}

	/* read and write section headers */
	if ((scn_header = (XCNHDR *) malloc((unsigned) (nscns * sizeof(XCNHDR)))) == NULL)
		error("cannot allocate section headers");
	if ((new_scn_header = (XCNHDR *) malloc((unsigned) (nscns * sizeof(XCNHDR)))) == NULL)
		error("cannot allocate new section headers");
	if (fseek(ifile, (long) (FILHSZ + opthdr), 0) != 0)
	{
		error("cannot seek to section headers");
	}
	if (freads((char *) scn_header, (int) (nscns * sizeof(XCNHDR)), 1, ifile) != 1)
		error("cannot read section headers");
	(void) memcpy(new_scn_header, scn_header, (int) (nscns * sizeof(XCNHDR)));
	sec_data_offset = (long) (FILHSZ + opthdr + (nscns * sizeof(XCNHDR)));

	for (i = 0, sh = new_scn_header; i < nscns; i++, sh++)
	{
		if (sh->_s.s_scnptr)
		{
			/* real data exists */
			sh->_s.s_scnptr = sec_data_offset;
			dsize += sh->_s.s_size;		/* cumulate data size */
			sec_data_offset += sh->_s.s_size * sizeof(long);
		}
		sh->_s.s_nreloc = sh->_s.s_nlnno = 0L;
	}
	if (fwrites((char *) new_scn_header, (int) (nscns * sizeof(XCNHDR)), 1, ofile) != 1)
		error("cannot write section headers");
	free((char *) new_scn_header);

	/* read and write section data */
	for (i = 0, sh = scn_header; i < nscns; i++, sh++)
	{
		if (sh->_s.s_scnptr && sh->_s.s_size)
		{
			long *raw_data;

			raw_data = (long *) malloc((unsigned) (sh->_s.s_size * sizeof(long)));
			if (!raw_data)
			{
				error("cannot allocate raw data");
			}
			if (fseek(ifile, sh->_s.s_scnptr, 0) != 0)
			{
				error("cannot seek to raw data");
			}
			if (freads((char *) raw_data, (int) sh->_s.s_size, sizeof(long), ifile) != sizeof(long))
			{
				error("cannot read raw data ");
			}
			if (fwrites((char *) raw_data, (int) sh->_s.s_size, sizeof(long), ofile) != sizeof(long))
			{
				error("cannot write raw data");
			}
			free((char *) raw_data);
		}
	}
	free((char *) scn_header);
}

/**
*
* name		cmdarg --- scan command line for argument
*
* synopsis	argp = cmdarg (arg, argc, argv)
*		char *argp;	pointer to argument
*		char arg;	argument to find
*		int argc;	count of command line arguments
*		char **argv;	pointer to command line arguments
*
* description	Takes a pointer to and count of the command line arguments.
*		Scans command line looking for argument character.  Returns
*		pointer to argument if found, NULL otherwise.
*
**/
static char *cmdarg(char arg, int argc, char **argv)
{
	--argc;								/* skip over program name */
	++argv;
	while (argc > 0)
	{									/* scan args */
		if (**argv == '-')
		{
			char *p;

			for (p = *argv + 1; *p; p++)
				if ((isupper(*p) ? tolower(*p) : *p) == arg)
					return *argv;
		}
		--argc;
		++argv;
	}
	return NULL;
}


/* get option letter from argv */
static int getopts(int argc, char *argv[], char *optstring)
{
	char c;
	char *place;
	static char *scan = NULL;

	coptarg = NULL;

	if (scan == NULL || *scan == '\0')
	{
		if (coptind == 0)
			coptind++;

		if (coptind >= argc || argv[coptind][0] != '-' || argv[coptind][1] == '\0')
			return EOF;
		if (strcmp(argv[coptind], "--") == 0)
		{
			coptind++;
			return EOF;
		}

		scan = argv[coptind] + 1;
		coptind++;
	}

	c = *scan++;
	place = strchr(optstring, c);

	if (place == NULL || c == ':')
	{
		(void) fprintf(stderr, "%s: unknown option -%c\n", Progname, c);
		return '?';
	}

	place++;
	if (*place == ':')
	{
		if (*scan != '\0')
		{
			coptarg = scan;
			scan = NULL;
		} else if (coptind < argc)
		{
			coptarg = argv[coptind];
			coptind++;
		} else
		{
			(void) fprintf(stderr, "%s: -%c argument missing\n", Progname, c);
			return '?';
		}
	}

	return c;
}


#if defined (APOLLO)
/**
*
* name		tmpnam - create temporary file name
*
* synopsis	fn = tmpnam (template)
*		char *fn;		pointer to temporary file name
*		char *template;		template for temporary file name
*
* description	Creates a temporary file name based on template.  Template
*		has the form yyyXXXXXX, where yyy is arbitrary text and
*		the literal XXXXXX substring is replaced by a letter
*		and a five digit number.  If a duplicate identifier is
*		formed in the same process, the routine substitutes the
*		next alphabetic character in sequence for the leading letter.
*		Returns pointer to revised template on success, NULL if
*		template is badly formed or other error.
*
**/
char *tmpnam(char *template)
{
	static char oldfn[MAXSTR] = "";
	static char subpat[] = "XXXXXX";
	char *p;
	chart *subp;
	char tbuf[32];
	long t;
	int i;

	if ((subp = strchr(template, subpat[0])) == NULL)
		return NULL;

	if (strcmp(subp, subpat) != 0)
		return NULL;

	(void) time(&t);
	if (t < 0L)
		t *= -1L;
	sprintf(tbuf, "%05ld", t);
	for (i = strlen(subpat) - 1, p = tbuf + strlen(tbuf); i > 0; i--, p--)
		;
	i = 'a';
	do
	{
		sprintf(subp, "%c%s", i++, p);
	} while (i <= 'z' && strcmp(oldfn, template) == 0);
	if (i > 'z')
		return NULL;

	strcpy(oldfn, template);
	return template;
}
#endif


/* return base part of file name in str */
static const char *mybasename(const char *str)
{
	const char *p;

	if (!str)							/* empty input */
		return NULL;

	for (p = str + strlen(str); p >= str; --p)
#if defined (__MSDOS__) || defined (__TOS__) || defined (__atarist__)
		if (*p == '\\' || *p == '/' || *p == ':')
#endif
#if defined (VMS)
			if (*p == ']' || *p == ':')
#endif
#if defined (UNIX) || defined (MACH)
				if (*p == '/')
#endif
#if defined (MPW)
					if (*p == ':')
#endif
						break;

	return p < str ? str : ++p;
}


static void usage(void)					/* display usage on stderr, exit nonzero */
{
#if defined (__MSDOS__) || defined (__TOS__) || defined (__atarist__)
	if (quiet)
#endif
		(void) fprintf(stderr, "%s  %s\n%s\n", Progtitle, Version, Copyright);
	(void) fprintf(stderr, "Usage:  %s [-q] files\n", Progname);
	(void) fprintf(stderr, "        q - do not display signon banner\n");
	exit(1);
}



int main(int argc, char *argv[])
{
	int c;								/* option character buffer */
	char ofb[L_tmpnam + 1];				/* output file name buffer */
	char tfb[L_tmpnam + 1];				/* temporary file name buffer */

/*
	check for command line options
*/

	/* scan for quiet flag on command line */
	quiet = cmdarg('q', argc, argv) != 0;

#if defined (__MSDOS__) || defined (__TOS__) || defined (__atarist__)
	if (!quiet)
		(void) fprintf(stderr, "%s  %s\n%s\n", Progtitle, Version, Copyright);
#endif
	while ((c = getopts(argc, argv, "Qq")) != EOF)
	{
		if (isupper(c))
			c = tolower(c);
		switch (c)
		{
		case 'q':
			quiet = TRUE;
			break;
		case '?':
		default:
			usage();
			break;
		}
	}

	/* no more args?  error */
	if (coptind >= argc)
		usage();

	/* dash for input filename? use stdio */
	if (*argv[coptind] == '-')
	{

		ifn = "stdin";
		ifile = stdin;
		ofn = "stdout";
		ofile = stdout;
		strip_file();
	} else
	{
		while (coptind < argc)
		{								/* process input files */

			/* open input and scratch files */
			ifn = argv[coptind++];
			if (!(ifile = fopen(ifn, "rb")))
				error("cannot open input file %s", ifn);
#if defined (APOLLO)
			(void) strcpy(ofb, "stXXXXXX");
#endif
			if ((ofn = mybasename(tmpnam(ofb))) == NULL)
				error("cannot create output file name");
#if defined (UNIX)
			(void) strcat(tfb, "a");	/* to insure uniqueness */
#endif
			if (!(ofile = fopen(ofn, "wb")))
				error("cannot open output file %s", ofn);
			setfile(ofn, "COFF", "MPS ");

			strip_file();

			/* rename scratch file to original file name */
			(void) fclose(ifile);
			(void) fclose(ofile);
#if defined (APOLLO)
			(void) strcpy(tfb, "stXXXXXX");
#endif
			if ((tfn = mybasename(tmpnam(tfb))) == NULL)
				error("cannot create temporary file name");
#if defined (UNIX)
			(void) strcat(tfb, "b");	/* to insure uniqueness */
#endif
			if (rename(ifn, tfn) != 0)
				error("cannot rename input file %s", ifn);
			if (rename(ofn, ifn) != 0)
			{
				if (rename(tfn, ifn) != 0)
					error("cannot undo rename of %s to %s", ifn, tfn);
				error("cannot rename output file %s", ofn);
			}
			if (unlink(tfn) != 0)
				error("cannot remove renamed input file %s", tfn);
		}
	}
	return 0;
}
