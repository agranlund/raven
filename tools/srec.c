/*
        NAME
                srec - convert Motorola DSP load file records to
                       S-record format

        SYNOPSIS
                srec [-bclmqrsuwx] [-a <alen>] [-o <mem>:<offset>]
                        [-p <procno>] [-t <tlen>] <input file ... >

        DESCRIPTION
                Srec takes as input a Motorola DSP absolute load file and
                produces byte-wide Motorola S-record files suitable for
                PROM burning.  If no file is specified the standard input
                is read.  The Motorola DSP START and END records or COFF
                file header data are mapped into S0 and S7/S8/S9 records
                respectively.  All other DSP object record types are mapped
                into S1, S2, or S3-type records depending on the address
                magnitude or the target processor word size.
                Symbol and comment records are currently ignored.

                Since Motorola DSPs use different word sizes, the words
                must be split into bytes and stored in a suitable format.
                The program keeps track of the input address magnitude
                to determine the appropriate S-record format to generate.
                If the -a or -p option is selected, srec uses a format
                corresponding to the address size or processor type specified.
                For example, if the programmer entered a -p96000 option,
                srec would always produce S3/S7 records regardless of the
                input address size.

                In the default mode of operation the program writes the
                low, middle, and high bytes of each word
                consecutively to the current S1/S2/S3 record being written.
                For example, given the DSP56000 DATA record below:

                                address field
                                |
                        _DATA P 0000
                        0008F8 300000 340000 094E3E
                        |      |      |      |
                        |      |      |      fourth word
                        |      |      third word
                        |      second word
                        first word

                srec would create the following S1 record:

                          byte count field
                          | address  field              checksum field
                          | |                           |
                        S10D0000F808000000300000343E4E09F9
                                |     |     |     |
                                |     |     |     fourth word
                                |     |     third word
                                |     second word
                                first word

                Output records are written to a file named according to
                the following convention:

                        <basename>.M

                where <basename> is the filename of the input load file
                without extension and M is the memory space specifier
                (X, Y, L, P, or E) for this set of data words.  Note that a
                separate file is created for each memory space encountered
                in the input stream; thus the maximum number of output files
                in the default mode is 4.

                When the -m option is specified, srec splits each DSP source
                word into bytes and stores the bytes in parallel S1/S2/S3
                records.  For example, the following DSP56000 DATA record:

                                address field
                                |
                        _DATA P 0000
                        0008F8 300000 340000 094E3E
                        |      |      |      |
                        |      |      |      fourth word
                        |      |      third word
                        |      second word
                        first word

                would be converted by srec into the three S1 records below:

                          byte count field
                          | address  field
                          | |
                        S1070000F800003EC2 -- low  byte
                        S10700000800004EA2 -- mid  byte
                        S1070000003034098B -- high byte
                                | | | | |
                                | | | | checksum field
                                | | | fourth word
                                | | third word
                                | second word
                                first word

                The three records corresponding to the high, middle, and
                low bytes of each data word are written to separate files.
                The files are named according to the following convention:

                        <basename>.<M><#>

                where <basename> is the filename of the input load file
                without extension, <M> is the memory space specifier
                (X, Y, L, P, or E) for this set of data words, and # is
                one of the digits 0, 1, or 2 corresponding to low, middle,
                and high bytes, respectively.  If input comes from the
                standard input, the module name is used as <basename>.

                Note that a separate set of byte-wide files is created for
                each memory space encountered in the input stream.  Thus the
                number of output files generated is (number of memory spaces
                in input * size of DSP word).

                The -s option writes all information to a single file,
                storing the memory space information in the address field
                of the S0 header record.  The values stored in the address
                field and their correspondence to the DSP56000 memory
                spaces are as follows:

                        Value           DSP56000 Memory Space
                        -----           ---------------------
                          1                     X
                          2                     Y
                          3                     L
                          4                     P
                          5                     E

                When the memory space changes in a data or block data
                record, a new S0 header record is generated.  The resulting
                output file is named <basename>.s, where <basename> is
                the filename of the input load file without extension.
                The -m and -s options are mutually exclusive.

                The -q option causes srec to swap X/Y data words in L
                memory.

                The -r option causes srec to write bytes high to low rather
                than low to high in the default and -s modes.  It has no
                affect when the -m option is given.

                Address fields in DSP load records are copied as is to
                the appropriate S1, S2, or S3 record.  Subsequent S1,
                S2, or S3 record addresses are byte incremented until
                a new data record is encountered or end-of-file is reached.
                In some cases the starting S1/S2/S3 record address must
                be adjusted for byte addressing by multiplying the load
                record start address by the number of bytes in a DSP56000
                word.  When the -b option is given, any record address
                fields are adjusted to begin on a byte-multiple address.
                If the -w option is specified (the default)
                byte-incrementing is not done when generating S-record
                addresses, e.g. the S-record
                addresses are word-oriented rather than byte-oriented.
                The -b and -w options have no effect when used in conjunction
                with the -m mode, since in that case byte and word address
                mappings are 1:1.

                Data records for L space memory contain words which are loaded
                into adjacent X and Y memory locations.  In these cases
                performing the default strict word addressing may be
                inappropriate.  The -l option can be given to indicate
                that double-word addressing should be used to generate
                subsequent S1/S2/S3 addresses after the initial load address.
                In addition the -l option should be used when doing byte
                addressing since the initial load addresses must be adjusted
                to account for double-word addressing in the load file.
                In general, it is a good idea to use the -l option whenever
                the input object file contains data records which refer to
                L memory space.

                In the header record only the module id is passed as header
                data for the S0 record; the version number, revision number,
                and comment are ignored.  As noted earlier, the machine ID
                field is used to determine what type of S-records to generate
                based on the addressing range of the DSP processor.  Some
                load file generators may not produce a START record or the
                START record may not contain the processor type.  The -p
                option can be used to explicitly specify a processor load file
                format.  Note that the -p option overrides the machine ID
                given in the START record.

                The -a and -t options can be used to force overriding of
                default program behavior.  The -a argument indicates either
                a 2, 3, or 4 byte S-record address field, corresponding to
                S1/S9, S2/S8, or S3/S7.  The -t argument is the size in
                bytes of the target processor word.  Both -a and -t
                circumvent any information obtained in START records, file
                header records, or other command line options such as -p.

                The -x option splits L data records into corresponding
                X and Y data records.  The -L option is ignored when this
                option is active.  The -c option truncates all input
                words to bytes, sending only the least significant byte
                of each input word to the output file.

        OPTIONS
                -a      - <alen> indicates the size of the S-record address
                          field in bytes.  The program generates S1/S9, S2/S8,
                          or S3/S7 records depending upon whether <alen> is
                          2, 3, or 4.  This option overrides any implicit
                          or explicit processor mapping.
                -b      - use byte addressing when transferring load addresses
                          to S-record addresses.  This means that load file
                          data record start addresses are multiplied by
                          the DSP bytes/word and subsequent S1/S3 record
                          addresses are computed based on the data byte count.
                -c      - truncate input words to output bytes, e.g.
                          send only the LSB to the output file.  The
                          -m and -c options are mutually exclusive.
                -l      - use double-word addressing when transferring load
                          addresses from L space to S-record addresses.  This
                          means that load file data records for L space data
                          are moved unchanged and subsequent S1/S3 record
                          addresses are computed based on the data word count
                          divided by 2.  This option should always be used
                          when the source load file contains data records in L
                          memory space.
                -m      - split each DSP word into bytes and store
                          the bytes in parallel S-records.   Replaces
                          the -3 option.  The -m option is mutually exclusive
                          with respect to the -c and -s options.
                -o      - add <offset> (must be specified in hex) to the
                          S-record address for memory space <mem>.
                -p      - assume <procno> load file input format.  <procno>
                          is one of the Motorola DSP processor numbers, e.g.
                          56000, 96000, etc.
                -q      - do not display signon banner.
                -r      - write bytes high to low, rather than low to
                          high.  Has no effect when used with -m option.
                -s      - write data to a single file, putting memory
                          space information into the address field of the
                          S0 header record. Replaces the -1 option.
                          Bytes may be reversed with the -r option.
                          The -m and -s options are mutually exclusive.
                -t      - <tlen> indicates the size of the target processor
                          word in bytes.  This option overrides any other
                          implicit or explicit processor specification.
                -u      - swap X/Y data words in L memory.
                -w      - use word addressing when transferring load addresses
                          to S-record addresses.  This means that load file
                          data record start addresses are moved unchanged and
                          subsequent S1/S3 record addresses are computed
                          based on the data word count.
                -x      - break L space data into X and Y constituents.

        DIAGNOSTICS
                The program does some checking for invalid input record
                formats, memory spaces, and hex values.  It obviously
                cannot check for bad data values if the load file has
                been tampered with.  Both START and END records should be
                found in .LOD file input, but the program tries to be
                forgiving about this.  If a START record is not present
                then the -p option should be used.

        HISTORY
                1.0     The beginning.
                1.1     Added code to support new default mode and -r
                        option (-3 was old default mode).
                1.2     Added support for AT&T Unix System V.
                1.3     Fix for bug in default mode where S-record address
                        was not being computed correctly (e.g. not a byte
                        multiple of the 56000 word size).
                1.4     Added support for Macintosh II.
                1.5     Added separate include file, getopt () support,
                        -1 (single output file) option.
                1.6     Fix for bug in default mode when handling BLOCKDATA
                        records; repeated call to reverse bytes was reversing
                        bytes in place!
                1.7     Added -b and -w options.

                2.1     Bumped version number for distribution.

                3.0     Added DSP96000 support; replaced -1 and -3 options
                        with -s and -m respectively.
                3.1     Modified get_start() to handle old-style START
                        records (e.g. without error count, machine type,
                        assembler version, etc.).
                3.2     Added -l option.

                4.0     Added DSP56100 support.
                4.1     Fixed bug in S7/S9 generation code (get_end()) where
                        with -b option address was not transferred correctly
                        to output field, resulting in a bad S7/S9 checksum.
                4.2     Added standard input capability.
                4.3     Fixed so data field lengths can vary.
                4.4     Changed standard input to use "-" command line flag;
                        no options causes usage to be printed.  Filled out
                        option descriptions in usage message.
                4.5     Fixed bug in handling of S-record count fields to
                        deal with long values on 16-bit hosts (IBM PC).
                4.6     Name change from DSP56016 to DSP5616.
                4.7     Fixed bug in rev_bytes logic for 56100, 96000.
                4.8     Added -p option; made START/END record parsing
                        more forgiving.
                4.9     Made record type names case-insensitive.
                4.10    Fixed bugs in old-style START record logic.
                4.11    Fixed bug in get_start() where call to get_record()
                        would not terminate on EOF.

                5.0     Modifications to read COFF input files.  Added
                        -q option.
                5.1     Copyright change; minor COFF include file mods
                        due to PC filename length limitations.
                5.2     Code to trap output errors.
                5.3     Function type declaration modifications.
                5.4     56100 COFF support.
                5.5     Name change from DSP5616 to DSP56100.
                5.6     Added -q (quiet option); changed long reverse
                        flag to -u.
                5.7     Added -a, -t, and -x options.
                5.8     Added -o option.
                5.9     Changed operation to vary S-record type based on
                        address magnitude unless -a specified.
                5.10    Added upper case command line options.
                5.11    Fixed bug in check_addr() to reset S-record
                        overhead value.
                5.12    Mods for EMI memory support.
                5.13    Additions for DSP56300 (ONYX) and DSP56800 (HAWK).
                5.14    Fixed case sensitive bug in command option letters.
                5.15    Added -c option.
                5.16    Additions for DSP56600 (ONYX lite)
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#if defined (VMS)
#include <descrip.h>
#include <ssdef.h>
#include <stsdef.h>
#include <climsgdef.h>
#endif

/*
  Headers for working with COFF files
*/

#include "coreaddr.h"
#include "maout.h"
#include "dspext.h"


/**
*	SREC Definitions
**/

#define TRUE		1
#define FALSE		0
#define EOS		'\0'					/* end-of-string constant */
#define MAXSTR		512					/* maximum string size + 1 */
#define MAXFLD		16					/* maximum field value size */
#define MAXEXTLEN	4					/* longest filename extension */

#if defined (VMS)						/* exit status values */
#define OK	0x18000001L
#define	ERR	0x1800FFFBL
#define CLI_ABSENT	CLI$_ABSENT
#else
#define OK	0
#define ERR	(-1)
#define CLI_ABSENT	0
#endif

#define DSP56000	1					/* DSP56000 flag constant */
#define DSP96000	2					/* DSP96000 flag constant */
#define DSP56100	3					/* DSP56100 flag constant */
#define DSP56300	4					/* DSP56300 flag constant */
#define DSP56800	5					/* DSP56800 flag constant */
#define DSP56600        6				/* DSP56600 flag constang */
#define D100            7				/* Star*Core 100 flag constang */
#define DSP56700        8				/* DSP56700 flag constang */

#define ASIZE1		2					/* S1/S9 record address size */
#define ASIZE2		3					/* S2/S8 record address size */
#define ASIZE3		4					/* S3/S7 record address size */

#define WSIZE2		2					/* 2 byte DSP word size */
#define WSIZE3		3					/* 3 byte DSP word size */
#define WSIZE4		4					/* 4 byte DSP word size */

#define MASK2		0xFFFFL				/* 2 byte word mask */
#define MASK3		0xFFFFFFL			/* 3 byte word mask */
#define MASK4		0xFFFFFFFFL			/* 4 byte word mask */

#define FMT4		"%04lx"				/* 4-digit word/addr format string */
#define FMT6		"%06lx"				/* 6-digit word/addr format string */
#define FMT8		"%08lx"				/* 8-digit word/addr format string */

#define S1FMT		"S1%02lx%04lx%s%02x\n"	/* S1 format string */
#define S2FMT		"S2%02lx%06lx%s%02x\n"	/* S2 format string */
#define S3FMT		"S3%02lx%08lx%s%02x\n"	/* S3 format string */
#define S7FMT		"S7%02lx%s%02x\n"	/* S7 format string */
#define S8FMT		"S8%02lx%s%02x\n"	/* S8 format string */
#define S9FMT		"S9%02lx%s%02x\n"	/* S9 format string */

#define S0OVRHD		3					/* S0 record overhead */
#define MAXBYTE		30					/* max data bytes per S-record */
#define MAXOVRHD	8					/* maximum S-record overhead */
#define MAXBUF	(MAXBYTE + MAXOVRHD) * 2	/* maximum byte buffer size */
#define CSMASK		0xFF				/* checksum mask */

#define MSPACES	6						/* number of memory spaces */
#define XMEM	0						/* memory space array offsets */
#define YMEM	1
#define LMEM	2
#define PMEM	3
#define EMEM	4
#define DMEM	5

#define NONE	0						/* OMF record codes */
#define START	1
#define END	2
#define DATA	3
#define BDATA	4
#define SYMBOL	5
#define COMMENT	6

#define RECORD	1						/* OMF field types */
#define HEXVAL	2

#define NEWREC	'_'						/* new record indicator */

/*	File type designations	*/
#define FT_UNKNOWN	(-1)
#define FT_NONE		0
#define FT_LOD		1
#define FT_CLD		2

struct srec
{										/* S-record structure */
	FILE *fp;
	unsigned checksum;
	char *p;
	char buf[MAXBUF + 1];
};

union wrd
{										/* word union for byte swapping */
	uint32_t l;
	unsigned char b[4];
};



static char const Progname[] = "srec";				/* program default name */
char Progtitle[] = "DSP S-Record Conversion Utility";

/*
  Put an extra space after the version number so that it is not
  compressed in the object file.  That way the strings program
  can find it.
*/
char Version[] = "Version 6.3.1 ";		/* srec version number */
char Copyright[] = "(C) Copyright Motorola, Inc. 1987-1996.  All rights reserved.";

static char *coptarg = NULL;			/* command argument pointer */
static int coptind = 0;					/* command argument index */

static char *ifname = NULL;				/* pointer to input file name */
static FILE *ifile = NULL;				/* file pointer for input file */
static int ftype = FT_NONE;				/* input file type designation */

static int dclflag = FALSE;				/* VMS DCL flag */
static int opts = FALSE;				/* single file option flag */
static int optm = FALSE;				/* multiple file option flag */
static int reverse = FALSE;				/* reverse byte option flag */
static int revlong = FALSE;				/* reverse word option flag */
static int baddr = FALSE;				/* byte address flag */
static int waddr = FALSE;				/* word address flag */
static int lmem = FALSE;				/* L memory flag */
static int quiet = FALSE;				/* signon banner flag */
static int xyflag = FALSE;				/* L data -> X/Y data */
static int trunc = FALSE;				/* truncate words to bytes */

static unsigned linecount = 1;			/* input file line count */
static char fldbuf[MAXSTR] = { EOS };	/* Object file field buffer */
static char lfldbuf[MAXSTR] = { EOS };	/* Object file long field buffer */
static char strbuf[MAXSTR] = { EOS };	/* Global string buffer */

static char s0buf[MAXSTR + 40];				/* S0 record string */
static unsigned s0cnt = S0OVRHD;		/* S0 byte count */
static unsigned s0addr = 0;				/* S0 address */
static unsigned s0sum = 0;				/* S0 checksum */
static int mach = NONE;					/* source machine type */
static int wsize = 0;					/* bytes per DSP word */
static int wsizee = 0;					/* bytes per emem word */
static int wsizep = 0;					/* bytes per DSP word program mem */
static int emem_p = 0;					/* flag indicating P size emi memory words */
static int wsized = 0;					/* bytes per DSP word data mem */
static int asize = 0;					/* size of SREC address */
static int ovrhd = 0;					/* overhead bytes in S-record */
static uint32_t wmask = 0;				/* word mask */
static uint32_t wmaskp = 0;				/* program word mask */
static uint32_t wmaskd = 0L;			/* data word mask */
static uint32_t amask = 0L;				/* address mask */
static int alen = 0;					/* SREC addr. length argument */
static int tlen = 0;					/* target word length argument */
static char *wrdfmt = NULL;				/* word format string */
static char *wrdfmtp = NULL;			/* word format string program memory */
static char *wrdfmtd = NULL;			/* word format string data memory */
static char *adrfmt = NULL;				/* address format string */
static char *s123fmt = NULL;			/* S-record start format string */
static char *s789fmt = NULL;			/* S-record end format string */
static FILHDR file_header;				/* file header structure */
static AOUTHDR opt_header;				/* optional header structure */

/*
	pointers to S-record structure arrays
*/
static struct srec *rec[] = { NULL, NULL, NULL, NULL, NULL, NULL };

static const char *memstr = "xylpedXYLPED";
static unsigned long memoff[] = { 0, 0, 0, 0, 0, 0 };

static char dclbuf[MAXSTR];

#if defined (VMS)						/* VMS DCL cmd line opt descriptors */
$DESCRIPTOR(arg_desc, dclbuf);
$DESCRIPTOR(lod_desc, "LOAD");
$DESCRIPTOR(byt_desc, "BYTE");
$DESCRIPTOR(lng_desc, "LONG");
$DESCRIPTOR(mlt_desc, "MULT");
$DESCRIPTOR(prc_desc, "PROC");
$DESCRIPTOR(rev_desc, "REVERSE");
$DESCRIPTOR(rev_desc, "REVLONG");
$DESCRIPTOR(sgl_desc, "SINGLE");
$DESCRIPTOR(wrd_desc, "WORD");
#else
int lod_desc;
#endif


static void error(const char *str)
{
	if (ftype == FT_LOD)
		(void) fprintf(stderr, "%s: at line %d: %s\n", Progname, linecount, str);
	else
		(void) fprintf(stderr, "%s: %s\n", Progname, str);
	exit(1);
}



static void error2(const char *fmt, const char *str)
{
	if (ftype == FT_LOD)
		(void) fprintf(stderr, "%s: at line %d: ", Progname, linecount);
	else
		(void) fprintf(stderr, "%s: ", Progname);
	(void) fprintf(stderr, fmt, str);
	(void) fprintf(stderr, "\n");
	exit(1);
}


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


static int get_ftype(const char *fname)		/* try to determine file type by file extension */
{
	const char *fn = mybasename(fname);
	char ext[MAXEXTLEN + 1];
	char *ep;
	int i;

	if (!fn || (ep = strrchr(fn, '.')) == NULL)
		return FT_NONE;
	for (i = 0; i < MAXEXTLEN; i++)
		ext[i] = isupper(ep[i]) ? tolower(ep[i]) : ep[i];
	ext[i] = EOS;
	if (strncmp(ext, ".lod", MAXEXTLEN) == 0)
		return FT_LOD;
	if (strncmp(ext, ".cld", MAXEXTLEN) == 0)
		return FT_CLD;
	return FT_UNKNOWN;
}


static const char *fix_fname(char *fn, const char *ext)	/* add extension to file name if reqd */
{
	const char *p;
	char *np;
	char *ep;
	int added = FALSE;

	p = mybasename(fn);					/* get file base name */
	np = fn + strlen(fn);				/* compute end of string */
	if ((ep = strrchr(fn, '.')) == NULL || ep < p)
		ep = np;						/* no extension */
	if (strncmp(ep, ext, MAXEXTLEN) != 0)
	{
		(void) strcpy(ep, ext);
		added = TRUE;
	}
	return added ? ep : NULL;
}


static int open_ifile(char *fn)			/* open input file, return file type */
{
	int ft;
	char *mode;
	char fname[MAXSTR];

	(void) strcpy(fname, fn);
	ft = get_ftype(fn);
	mode = ft == FT_LOD ? "r" : "rb";
	if ((ifile = fopen(fn, mode)) != NULL)
		return ft;
	if (ftype != FT_NONE)
		error2("cannot open input file %s", fname);
	fix_fname(fn, ".cld");
	if ((ifile = fopen(fn, "rb")) != NULL)
		return FT_CLD;
	fix_fname(fn, ".lod");
	if ((ifile = fopen(fn, "r")) != NULL)
		return FT_LOD;
	error2("cannot open input file %s", fname);
	return FT_NONE;
}


static char *strup(char *str)			/* convert all alpha chars in str to upper */
{
	char *p = str;

	while (*p)
	{
		if (isalpha(*p) && islower(*p))
			*p = toupper(*p);
		p++;
	}
	return str;
}


static void bld_s0(unsigned int space)
{
	static char s0name[MAXSTR];

	if (space == NONE)
		(void) strcpy(s0name, s0buf);
	(void) sprintf(s0buf, "S0%02x%04x%s%02x\n", s0cnt, space, s0name, ~(s0sum + s0cnt + space) & CSMASK);
	(void) strup(s0buf);
}


static int get_memch(int spc)			/* return memory space character */
{
	switch (spc)
	{
	case XMEM:
		return 'x';
	case YMEM:
		return 'y';
	case LMEM:
		return 'l';
	case PMEM:
		return 'p';
	case EMEM:
		return 'e';
	case DMEM:
		return 'd';
	default:
		return 0;
	}
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


static void open_ofiles(int space)		/* open S-record ouput files */
{
	struct srec *drec;
	char fn[MAXSTR];
	char *p = NULL;
	int i;
	char c;

/*
	allocate S-record structure array; construct output file name
*/

	wsize = (space == EMEM) ? wsizee : (space == PMEM) ? wsizep : wsized;
	if ((drec = (struct srec *) calloc(optm ? (unsigned) wsize : (unsigned) 1, sizeof(struct srec))) == NULL)
		error("cannot allocate S-record structure");
	rec[space] = drec;					/* squirrel pointer away */
	if (ifname)
	{									/* if explicit input file */
		c = opts ? 's' : get_memch(space);
		(void) sprintf(fn, "%s.%c", ifname, c);
		p = fn + strlen(fn);
	}

	/*
	   loop to open all files, write out S0 record
	 */

	if (!optm)
	{									/* don't need multiple files */
		if (!ifname)
		{
			drec->fp = stdout;			/* if all else fails... */
		} else
		{
			if ((drec->fp = fopen(fn, "w")) == NULL)
				error2("cannot open output file %s", fn);
			setfile(fn, "TEXT", "MPS ");
		}
		if (!opts)
			if (fputs(s0buf, drec->fp) == EOF)
				error("cannot write S0 record");
	} else
	{
		for (i = wsize - 1, rec[space] = drec; i >= 0; i--, drec++)
		{
			if (!ifname)
				drec->fp = stdout;
			else
			{
				*p = (char) (i + '0');
				*(p + 1) = EOS;
				if ((drec->fp = fopen(fn, "w")) == NULL)
					error2("cannot open output file %s", fn);
				setfile(fn, "TEXT", "MPS ");
			}
			if (fputs(s0buf, drec->fp) == EOF)
				error("cannot write S0 record");
		}
	}
}



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


static void rev_bytes(char *buf, int mem)	/* reverse the bytes in buf */
{
	char c;

	wsize = mem == PMEM || emem_p ? wsizep : wsized;
	switch (wsize)
	{
	case WSIZE4:
		c = buf[0];
		buf[0] = buf[6];
		buf[6] = c;
		c = buf[1];
		buf[1] = buf[7];
		buf[7] = c;
		c = buf[2];
		buf[2] = buf[4];
		buf[4] = c;
		c = buf[3];
		buf[3] = buf[5];
		buf[5] = c;
		break;
	case WSIZE2:
		c = buf[0];
		buf[0] = buf[2];
		buf[2] = c;
		c = buf[1];
		buf[1] = buf[3];
		buf[3] = c;
		break;
	case WSIZE3:
	default:
		c = buf[0];
		buf[0] = buf[4];
		buf[4] = c;
		c = buf[1];
		buf[1] = buf[5];
		buf[5] = c;
		break;
	}
}


/* reset S-record type based on address value */
static void check_addr(uint32_t addr)
{
	if (addr <= (uint32_t) MASK2)
	{
		if (asize != ASIZE1)
		{
			asize = ASIZE1;
			amask = MASK2;
			adrfmt = FMT4;
			s123fmt = S1FMT;
			s789fmt = S9FMT;
		}
		ovrhd = asize + 1;
		return;
	}
	if (addr <= (uint32_t) MASK3)
	{
		if (asize != ASIZE2)
		{
			asize = ASIZE2;
			amask = MASK3;
			adrfmt = FMT6;
			s123fmt = S2FMT;
			s789fmt = S8FMT;
		}
		ovrhd = asize + 1;
		return;
	}
	if (asize != ASIZE3)
	{
		asize = ASIZE3;
		amask = MASK4;
		adrfmt = FMT8;
		s123fmt = S3FMT;
		s789fmt = S7FMT;
	}
	ovrhd = asize + 1;
}


static int sum_addr(uint32_t addr)	/* sum bytes of address */
{
	int i;
	int sum;

	for (i = 0, sum = 0; i < sizeof(addr); i++)
	{
		sum += addr & CSMASK;
		addr >>= 8;
	}
	return sum;
}


/* flush S1/S2/S3 record to appropriate file */
static void flush_rec(int space, uint32_t addr, uint32_t count)
{
	int i;
	struct srec *drec;

	if (!alen)							/* address size not explicitly set */
		check_addr(addr);
	count += ovrhd;
	wsize = ((space == PMEM) || (emem_p)) ? wsizep : wsized;
	for (i = 0, drec = rec[space]; i < (optm ? wsize : 1); i++, drec++)
	{
		drec->checksum += (unsigned) sum_addr(addr & amask);
		(void) sprintf(strbuf, s123fmt, count, addr & amask, drec->buf, ~(drec->checksum + count) & CSMASK);
		if (fputs(strup(strbuf), drec->fp) == EOF)
			error("cannot write start S-record");
		drec->checksum = 0;
		drec->p = drec->buf;
	}
}


/* move bytes from field buffer into record buffer */
static void get_bytes(int space, char *fbp)
{
	int i, j;
	char *p;
	struct srec *drec;

	/* loop to move bytes, sum for checksum */
	wsize = ((space == PMEM) || (emem_p)) ? wsizep : wsized;
	for (i = 0, drec = rec[space]; i < wsize; i++, drec += optm ? 1 : 0)
	{
		if (trunc && i < wsize - 1)
		{
			fbp += 2;
			continue;					/* skip high order bytes */
		}
		p = drec->p;
		*p++ = *fbp++;
		*p++ = *fbp++;
		*p = EOS;
		(void) sscanf(drec->p, "%x", &j);
		drec->checksum += (unsigned) j;
		drec->p = p;
	}
}


static void read_data(XCNHDR *sh, int sn, int spc, int mem)	/* read COFF section data */
{
	uint32_t *raw_data;
	int space = mem;
	int i;
	int mask = revlong ? !!mem : !mem;
	uint32_t addr;
	uint32_t count = 0;
	uint32_t addr_incr;
	struct srec *drec;
	char snstr[MAXFLD];

	if (opts)
	{									/* single file option */
		i = space + 1;
		space = NONE;
		if (s0addr != (unsigned) i)
		{
			bld_s0(i);
			if (fputs(s0buf, rec[0]->fp) == EOF)
				error("cannot write S0 record");
			s0addr = (unsigned) i;
		}
	} else if (!rec[space])				/* see if files are open */
		open_ofiles(space);
	addr = CORE_ADDR_ADDR(sh->_s.s_paddr) + memoff[mem];
	emem_p = ((file_header.f_magic == M566MAGIC) && (sh->_s.s_flags & STYP_OVERLAYP));
	wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
	addr_incr = ((file_header.f_magic == M566MAGIC) && (mem == EMEM)) ? wsizee : wsize;
	if (!optm && baddr)
		addr *= addr_incr;				/* adjust address for serial bytes */
	if (lmem && mem == LMEM && baddr)
		addr *= 2;						/* adjust address for long memory */

	/* read in section data block */
	(void) sprintf(snstr, "%d", sn);
	if ((raw_data = (uint32_t *) malloc(sh->_s.s_size * sizeof(*raw_data))) == NULL)
		error2("cannot allocate data block for section %s", snstr);
	if (fseek(ifile, sh->_s.s_scnptr, 0) != 0)
		error2("cannot seek to raw data in section %s", snstr);
	if (freads((char *) raw_data, (int) sh->_s.s_size, sizeof(uint32_t), ifile) != sizeof(uint32_t))
		error2("cannot read raw data in section %s", snstr);

	/* initialize data record fields */
	wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;

	for (i = 0, drec = rec[space]; i < (optm ? wsize : 1); i++, drec++)
	{
		drec->checksum = 0;
		drec->p = drec->buf;
	}

	/* loop to pick up data */
	for (i = 0; i < sh->_s.s_size; i++)
	{

		if (spc != mem && (i ^ mask) & 1)
		{
			raw_data++;					/* alternate if splitting L space */
			continue;
		}
		wmask = ((mem == PMEM) || (emem_p)) ? wmaskp : wmaskd;
		wrdfmt = ((mem == PMEM) || (emem_p)) ? wrdfmtp : wrdfmtd;
		(void) sprintf(fldbuf, wrdfmt, *raw_data++ & wmask);
		(void) strup(fldbuf);
		wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
		if ((int) strlen(fldbuf) != wsize * 2)
			error("improper number of bytes in word");
		if (!reverse && !optm)
			rev_bytes(fldbuf, mem);
		if (mem != LMEM)
			(void) strcpy(lfldbuf, fldbuf);
		else
		{
			(void) sprintf(lfldbuf, wrdfmt, *raw_data++ & wmask);
			(void) strup(lfldbuf);
			if ((int) strlen(lfldbuf) != wsize * 2)
				error("improper number of bytes in word");
			if (!reverse && !optm)
				rev_bytes(lfldbuf, mem);
			i++;						/* increment data counter */
		}
		get_bytes(space, revlong ? fldbuf : lfldbuf);
		if (mem == LMEM)
			get_bytes(space, revlong ? lfldbuf : fldbuf);

		/* if max record count reached, print out current record */
		if (mem == LMEM)
			count += optm || trunc ? 2L : wsize * 2;
		else
			count += optm || trunc ? 1L : wsize;
		if (!(count & 1L) && count >= MAXBYTE)
		{
			flush_rec(space, addr, count);
			if ((!baddr) && !(optm || trunc))
			{
				if (lmem && mem == LMEM)
					addr += (count / addr_incr) / 2L;
				else
					addr += count / addr_incr;
			} else
				addr += count;
			count = 0L;
		}
	}
	free((char *) (raw_data - i));		/* free data block */

	/* new record or EOF encountered; flush out current record */
	if (rec[space]->p != rec[space]->buf)
		flush_rec(space, addr, count);
}


/* read COFF block section data */
static void read_bdata(XCNHDR *sh, int sn, int spc, int mem)
{
	uint32_t raw_data[2];
	int space = mem;
	int i;
	int index = 0;
	uint32_t addr;
	uint32_t repeat;
	uint32_t count = 0;
	uint32_t addr_incr;
	struct srec *drec;
	char snstr[MAXFLD];

	if (opts)
	{									/* single file option */
		i = space + 1;
		space = NONE;
		if (s0addr != (unsigned) i)
		{
			bld_s0(i);
			if (fputs(s0buf, rec[0]->fp) == EOF)
				error("cannot write S0 record");
			s0addr = (unsigned) i;
		}
	} else if (!rec[space])				/* see if files are open */
		open_ofiles(space);
	addr = CORE_ADDR_ADDR(sh->_s.s_paddr) + memoff[mem];

	emem_p = ((file_header.f_magic == M566MAGIC) && (sh->_s.s_flags & STYP_OVERLAYP));
	wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
	addr_incr = ((file_header.f_magic == M566MAGIC) && (mem == EMEM)) ? wsizee : wsize;
	if (!optm && baddr)
		addr *= addr_incr;				/* adjust address for serial bytes */
	if (lmem && mem == LMEM && baddr)
		addr *= 2;						/* adjust address for long memory */
	repeat = CORE_ADDR_ADDR(sh->_s.s_vaddr);

	/* read in section data block (read in again even if xyflag) */
	(void) sprintf(snstr, "%d", sn);
	if (fseek(ifile, sh->_s.s_scnptr, 0) != 0)
		error2("cannot seek to raw data in section %s", snstr);
	if (freads((char *) raw_data, (int) sh->_s.s_size, sizeof(uint32_t), ifile) != sizeof(uint32_t))
		error2("cannot read raw data in section %s", snstr);

	/* pick up data value */
	if (spc != mem)						/* change index if splitting L space */
		index = revlong ? !mem : !!mem;

	wmask = ((mem == PMEM) || (emem_p)) ? wmaskp : wmaskd;
	wrdfmt = ((mem == PMEM) || (emem_p)) ? wrdfmtp : wrdfmtd;
	(void) sprintf(fldbuf, wrdfmt, raw_data[index] & wmask);
	(void) strup(fldbuf);
	wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
	if ((int) strlen(fldbuf) != wsize * 2)
		error("improper number of bytes in word");
	if (!reverse && !optm)
		rev_bytes(fldbuf, mem);
	if (mem != LMEM)
		(void) strcpy(lfldbuf, fldbuf);
	else
	{									/* L memory; get next word */
		(void) sprintf(lfldbuf, wrdfmt, raw_data[1] & wmask);
		(void) strup(lfldbuf);
		if ((int) strlen(lfldbuf) != wsize * 2)
			error("improper number of bytes in word");
		if (!reverse && !optm)
			rev_bytes(lfldbuf, mem);
	}

	/* initialize data record fields */
	wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
	for (i = 0, drec = rec[space]; i < (optm ? wsize : 1); i++, drec++)
	{
		drec->checksum = 0;
		drec->p = drec->buf;
	}

	/* loop to generate data records */
	for (i = 0; i < (int) repeat; i++)
	{

		get_bytes(space, revlong ? fldbuf : lfldbuf);
		if (mem == LMEM)
			get_bytes(space, revlong ? lfldbuf : fldbuf);

		/* if max record count reached, print out current record */
		wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
		addr_incr = ((file_header.f_magic == M566MAGIC) && (mem == EMEM)) ? wsizee : wsize;
		if (mem == LMEM)
			count += optm || trunc ? 2L : wsize * 2;
		else
			count += optm || trunc ? 1L : wsize;
		if (!(count & 1L) && count >= MAXBYTE)
		{
			flush_rec(space, addr, count);
			if ((!baddr) && !(optm || trunc))
			{
				if (lmem && spc == LMEM)
					addr += (count / addr_incr) / 2L;
				else
					addr += count / addr_incr;
			} else
				addr += count;
			count = 0L;
		}
	}

	/* new record or EOF encountered; flush out current record */
	if (rec[space]->p != rec[space]->buf)
		flush_rec(space, addr, count);
}


static int get_mem(enum memory_map mem)	/* return memory space from enumeration type */
{
	switch (mem)
	{

	case memory_map_x:
	case memory_map_xi:
	case memory_map_xe:
	case memory_map_xr:
		return XMEM;
	case memory_map_xa:
	case memory_map_xb:
		if (mach != DSP96000)
			return ERR;
		return XMEM;

	case memory_map_y:
	case memory_map_yi:
	case memory_map_ye:
	case memory_map_yr:
		if (mach != DSP56000 && mach != DSP56300 && mach != DSP56600 && mach != DSP96000)
			return ERR;
		return YMEM;
	case memory_map_ya:
	case memory_map_yb:
		if (mach != DSP96000)
			return ERR;
		return YMEM;

	case memory_map_l:
	case memory_map_li:
	case memory_map_le:
		if (mach != DSP56000 && mach != DSP56300 && mach != DSP56600 && mach != DSP96000)
			return ERR;
		return LMEM;
	case memory_map_laa:
	case memory_map_lab:
	case memory_map_lba:
	case memory_map_lbb:
		if (mach != DSP96000)
			return ERR;
		return LMEM;

	case memory_map_p:
	case memory_map_pi:
	case memory_map_pe:
	case memory_map_pr:
	case memory_map_pa:
	case memory_map_pb:
		return PMEM;

	case memory_map_emi:
	case memory_map_e0:
	case memory_map_e1:
	case memory_map_e2:
	case memory_map_e3:
	case memory_map_e4:
	case memory_map_e5:
	case memory_map_e6:
	case memory_map_e7:
	case memory_map_e8:
	case memory_map_e9:
	case memory_map_e10:
	case memory_map_e11:
	case memory_map_e12:
	case memory_map_e13:
	case memory_map_e14:
	case memory_map_e15:
	case memory_map_e16:
	case memory_map_e17:
	case memory_map_e18:
	case memory_map_e19:
	case memory_map_e20:
	case memory_map_e21:
	case memory_map_e22:
	case memory_map_e23:
	case memory_map_e24:
	case memory_map_e25:
	case memory_map_e26:
	case memory_map_e27:
	case memory_map_e28:
	case memory_map_e29:
	case memory_map_e30:
	case memory_map_e31:
	case memory_map_e32:
	case memory_map_e33:
	case memory_map_e34:
	case memory_map_e35:
	case memory_map_e36:
	case memory_map_e37:
	case memory_map_e38:
	case memory_map_e39:
	case memory_map_e40:
	case memory_map_e41:
	case memory_map_e42:
	case memory_map_e43:
	case memory_map_e44:
	case memory_map_e45:
	case memory_map_e46:
	case memory_map_e47:
	case memory_map_e48:
	case memory_map_e49:
	case memory_map_e50:
	case memory_map_e51:
	case memory_map_e52:
	case memory_map_e53:
	case memory_map_e54:
	case memory_map_e55:
	case memory_map_e56:
	case memory_map_e57:
	case memory_map_e58:
	case memory_map_e59:
	case memory_map_e60:
	case memory_map_e61:
	case memory_map_e62:
	case memory_map_e63:
	case memory_map_e64:
	case memory_map_e65:
	case memory_map_e66:
	case memory_map_e67:
	case memory_map_e68:
	case memory_map_e69:
	case memory_map_e70:
	case memory_map_e71:
	case memory_map_e72:
	case memory_map_e73:
	case memory_map_e74:
	case memory_map_e75:
	case memory_map_e76:
	case memory_map_e77:
	case memory_map_e78:
	case memory_map_e79:
	case memory_map_e80:
	case memory_map_e81:
	case memory_map_e82:
	case memory_map_e83:
	case memory_map_e84:
	case memory_map_e85:
	case memory_map_e86:
	case memory_map_e87:
	case memory_map_e88:
	case memory_map_e89:
	case memory_map_e90:
	case memory_map_e91:
	case memory_map_e92:
	case memory_map_e93:
	case memory_map_e94:
	case memory_map_e95:
	case memory_map_e96:
	case memory_map_e97:
	case memory_map_e98:
	case memory_map_e99:
	case memory_map_e100:
	case memory_map_e101:
	case memory_map_e102:
	case memory_map_e103:
	case memory_map_e104:
	case memory_map_e105:
	case memory_map_e106:
	case memory_map_e107:
	case memory_map_e108:
	case memory_map_e109:
	case memory_map_e110:
	case memory_map_e111:
	case memory_map_e112:
	case memory_map_e113:
	case memory_map_e114:
	case memory_map_e115:
	case memory_map_e116:
	case memory_map_e117:
	case memory_map_e118:
	case memory_map_e119:
	case memory_map_e120:
	case memory_map_e121:
	case memory_map_e122:
	case memory_map_e123:
	case memory_map_e124:
	case memory_map_e125:
	case memory_map_e126:
	case memory_map_e127:
	case memory_map_e128:
	case memory_map_e129:
	case memory_map_e130:
	case memory_map_e131:
	case memory_map_e132:
	case memory_map_e133:
	case memory_map_e134:
	case memory_map_e135:
	case memory_map_e136:
	case memory_map_e137:
	case memory_map_e138:
	case memory_map_e139:
	case memory_map_e140:
	case memory_map_e141:
	case memory_map_e142:
	case memory_map_e143:
	case memory_map_e144:
	case memory_map_e145:
	case memory_map_e146:
	case memory_map_e147:
	case memory_map_e148:
	case memory_map_e149:
	case memory_map_e150:
	case memory_map_e151:
	case memory_map_e152:
	case memory_map_e153:
	case memory_map_e154:
	case memory_map_e155:
	case memory_map_e156:
	case memory_map_e157:
	case memory_map_e158:
	case memory_map_e159:
	case memory_map_e160:
	case memory_map_e161:
	case memory_map_e162:
	case memory_map_e163:
	case memory_map_e164:
	case memory_map_e165:
	case memory_map_e166:
	case memory_map_e167:
	case memory_map_e168:
	case memory_map_e169:
	case memory_map_e170:
	case memory_map_e171:
	case memory_map_e172:
	case memory_map_e173:
	case memory_map_e174:
	case memory_map_e175:
	case memory_map_e176:
	case memory_map_e177:
	case memory_map_e178:
	case memory_map_e179:
	case memory_map_e180:
	case memory_map_e181:
	case memory_map_e182:
	case memory_map_e183:
	case memory_map_e184:
	case memory_map_e185:
	case memory_map_e186:
	case memory_map_e187:
	case memory_map_e188:
	case memory_map_e189:
	case memory_map_e190:
	case memory_map_e191:
	case memory_map_e192:
	case memory_map_e193:
	case memory_map_e194:
	case memory_map_e195:
	case memory_map_e196:
	case memory_map_e197:
	case memory_map_e198:
	case memory_map_e199:
	case memory_map_e200:
	case memory_map_e201:
	case memory_map_e202:
	case memory_map_e203:
	case memory_map_e204:
	case memory_map_e205:
	case memory_map_e206:
	case memory_map_e207:
	case memory_map_e208:
	case memory_map_e209:
	case memory_map_e210:
	case memory_map_e211:
	case memory_map_e212:
	case memory_map_e213:
	case memory_map_e214:
	case memory_map_e215:
	case memory_map_e216:
	case memory_map_e217:
	case memory_map_e218:
	case memory_map_e219:
	case memory_map_e220:
	case memory_map_e221:
	case memory_map_e222:
	case memory_map_e223:
	case memory_map_e224:
	case memory_map_e225:
	case memory_map_e226:
	case memory_map_e227:
	case memory_map_e228:
	case memory_map_e229:
	case memory_map_e230:
	case memory_map_e231:
	case memory_map_e232:
	case memory_map_e233:
	case memory_map_e234:
	case memory_map_e235:
	case memory_map_e236:
	case memory_map_e237:
	case memory_map_e238:
	case memory_map_e239:
	case memory_map_e240:
	case memory_map_e241:
	case memory_map_e242:
	case memory_map_e243:
	case memory_map_e244:
	case memory_map_e245:
	case memory_map_e246:
	case memory_map_e247:
	case memory_map_e248:
	case memory_map_e249:
	case memory_map_e250:
	case memory_map_e251:
	case memory_map_e252:
	case memory_map_e253:
	case memory_map_e254:
	case memory_map_e255:
		if (mach != DSP56000 && mach != DSP56300 && mach != DSP56600)
			return ERR;
		return EMEM;

	case memory_map_dm:
		if (mach != DSP56000)
			return ERR;
		return DMEM;

	case memory_map_none:
		return NONE;

	case memory_map_error:
	default:
		return ERR;
	}
}


static int setup_mach(const char *buf)
{
	int mch = NONE;

	if (strcmp(buf, "56000") == 0)
	{
		mch = DSP56000;
		wsize = wsizep = wsized = WSIZE3;
		asize = ASIZE1;
		wmask = wmaskd = wmaskp = MASK3;
		amask = MASK2;
		wrdfmt = wrdfmtd = wrdfmtp = FMT6;
		adrfmt = FMT4;
		s123fmt = S1FMT;
		s789fmt = S9FMT;
	} else if (strcmp(buf, "96000") == 0)
	{
		mch = DSP96000;
		wsize = wsizep = wsized = WSIZE4;
		asize = ASIZE3;
		wmask = wmaskd = wmaskp = MASK4;
		amask = MASK4;
		wrdfmt = wrdfmtd = wrdfmtp = FMT8;
		adrfmt = FMT8;
		s123fmt = S3FMT;
		s789fmt = S7FMT;
	} else if (strcmp(buf, "100") == 0)
	{
		mch = D100;
		wsize = wsizep = wsized = WSIZE2;
		asize = ASIZE3;
		wmask = wmaskd = wmaskp = MASK2;
		amask = MASK4;
		wrdfmt = wrdfmtd = wrdfmtp = FMT4;
		adrfmt = FMT8;
		s123fmt = S3FMT;
		s789fmt = S9FMT;
	} else if (strcmp(buf, "56700") == 0)
	{
		mch = DSP56700;
		wsize = wsizep = wsized = WSIZE2;
		asize = ASIZE3;
		wmask = wmaskd = wmaskp = MASK2;
		amask = MASK4;
		wrdfmt = wrdfmtd = wrdfmtp = FMT4;
		adrfmt = FMT8;
		s123fmt = S3FMT;
		s789fmt = S9FMT;
	} else if (strcmp(buf, "5616") == 0 || strcmp(buf, "56100") == 0)
	{
		mch = DSP56100;
		wsize = wsizep = wsized = WSIZE2;
		asize = ASIZE1;
		wmask = wmaskd = wmaskp = MASK2;
		amask = MASK2;
		wrdfmt = wrdfmtd = wrdfmtp = FMT4;
		adrfmt = FMT4;
		s123fmt = S1FMT;
		s789fmt = S9FMT;
	} else if (strcmp(buf, "56300") == 0)
	{
		mch = DSP56300;
		wsize = wsizep = wsized = WSIZE3;
		asize = ASIZE2;
		wmask = wmaskd = wmaskp = MASK3;
		amask = MASK3;
		wrdfmt = wrdfmtd = wrdfmtp = FMT6;
		adrfmt = FMT6;
		s123fmt = S2FMT;
		s789fmt = S8FMT;
	} else if (strcmp(buf, "56800") == 0)
	{
		mch = DSP56800;
		wsize = wsizep = wsized = WSIZE2;
		asize = ASIZE2;
		wmask = wmaskd = wmaskp = MASK2;
		amask = MASK2;
		wrdfmt = wrdfmtd = wrdfmtp = FMT4;
		adrfmt = FMT4;
		s123fmt = S1FMT;
		s789fmt = S9FMT;
	} else if (strcmp(buf, "56600") == 0)
	{
		mch = DSP56600;
		wsizee = 1;
		wsize = wsized = WSIZE2;
		wsizep = WSIZE3;
		asize = ASIZE2;
		wmask = wmaskd = MASK2;
		wmaskp = MASK3;
		amask = MASK2;
		wrdfmt = wrdfmtd = FMT4;
		wrdfmtp = FMT6;
		adrfmt = FMT4;
		s123fmt = S1FMT;
		s789fmt = S9FMT;
	}
	switch (alen)
	{									/* override address size settings */
	case ASIZE1:
		asize = alen;
		amask = MASK2;
		adrfmt = FMT4;
		s123fmt = S1FMT;
		s789fmt = S9FMT;
		break;
	case ASIZE2:
		asize = alen;
		amask = MASK3;
		adrfmt = FMT6;
		s123fmt = S2FMT;
		s789fmt = S8FMT;
		break;
	case ASIZE3:
		asize = alen;
		amask = MASK4;
		adrfmt = FMT8;
		s123fmt = S3FMT;
		s789fmt = S7FMT;
		break;
	default:
		break;
	}
	switch (tlen)
	{									/* override word size settings */
	case WSIZE2:
		wsize = wsized = wsizep = tlen;
		wmask = wmaskd = wmaskp = MASK2;
		wrdfmt = wrdfmtd = wrdfmtp = FMT4;
		break;
	case WSIZE3:
		wsize = wsized = wsizep = tlen;
		wmask = wmaskd = wmaskp = MASK3;
		wrdfmt = wrdfmtd = wrdfmtp = FMT6;
		break;
	case WSIZE4:
		wsize = wsized = wsizep = tlen;
		wmask = wmaskd = wmaskp = MASK4;
		wrdfmt = wrdfmtd = wrdfmtp = FMT8;
		break;
	default:
		break;
	}
	ovrhd = asize + 1;					/* set overhead value */
	return mch;
}


static void read_headers(FILE *fp)		/* read COFF file and optional headers */
{
	char *mstr = NULL;

	if (freads((char *) &file_header, sizeof(FILHDR), 1, fp) != 1)
		error("cannot read file header");
	if (!(file_header.f_flags & F_RELFLG))
		error("invalid object file type");

	/* Determine machine type */
	switch (file_header.f_magic)
	{
	case M56KMAGIC:
		mstr = "56000";
		break;
	case M96KMAGIC:
		mstr = "96000";
		break;
	case M16KMAGIC:
		mstr = "56100";
		break;
	case M563MAGIC:
		mstr = "56300";
		break;
	case M568MAGIC:
		mstr = "56800";
		break;
	case M566MAGIC:
		mstr = "56600";
		break;
	case SC1MAGIC:
		mstr = "100";
		break;
	case M567MAGIC:
		mstr = "56700";
		break;
	default:
		error("invalid machine type");
	}
	if (!mach && (mach = setup_mach(mstr)) == NONE)
		error("invalid machine type");

	if (file_header.f_opthdr)			/* optional header present */
		if (freads((char *) &opt_header, (int) file_header.f_opthdr, 1, fp) != 1)
			error("cannot read optional header");

	/* Build base S0 record */
	bld_s0(NONE);
	if (opts)							/* single file option */
		open_ofiles(NONE);
}


static void do_end(void)				/* generate S9 record(s) and clean up */
{
	int i;
	int space;
	uint32_t count;				/* always a checksum byte */
	uint32_t addr;
	uint32_t checksum;
	struct srec *drec;

	addr = CORE_ADDR_ADDR(opt_header.entry) + memoff[PMEM];
	wsize = wsizep;
	if (!optm && baddr)
		addr *= wsize;					/* adjust address for serial bytes */
	(void) sprintf(fldbuf, adrfmt, addr & amask);
	checksum = (unsigned) sum_addr(addr & amask);
	count = ovrhd;
	if (!alen)							/* address size not explicitly set */
		check_addr(addr);

	for (space = 0; space < (opts ? 1 : MSPACES); space++)
	{
		if (rec[space])
		{
			for (i = 0, drec = opts ? rec[0] : rec[space]; i < (optm ? wsize : 1); i++, drec++)
			{
				(void) sprintf(strbuf, s789fmt, count, fldbuf, ~(checksum + count) & CSMASK);
				if (fputs(strup(strbuf), drec->fp) == EOF)
					error("cannot write end S-record");
				(void) fclose(drec->fp);
			}
			free((char *) rec[space]);
			rec[space] = NULL;
		}
	}
}


static void do_coff(FILE *fp)			/* process COFF file records */
{
	int i;
	int spc;
	int32_t nscns;
	XCNHDR *scn_header;
	XCNHDR *sh;
	void (*read_rtn) (XCNHDR *, int, int, int);

	read_headers(fp);
	nscns = file_header.f_nscns;
	if ((scn_header = (XCNHDR *) malloc((unsigned) (nscns * sizeof(XCNHDR)))) == NULL)
		error("cannot allocate section headers");
	if (fseek(fp, (long) (FILHSZ + file_header.f_opthdr), 0) != 0)
		error("cannot seek to section headers");
	if (freads((char *) scn_header, (int) (nscns * sizeof(XCNHDR)), 1, fp) != 1)
		error("cannot read section headers");
	for (i = 1, sh = scn_header; i <= nscns; i++, sh++)
	{
		if (!sh->_s.s_scnptr || !sh->_s.s_size)
			continue;					/* no data */
		/* get section memory space and start address */
		if ((spc = get_mem(CORE_ADDR_MAP(sh->_s.s_paddr))) < 0)
			error("invalid memory space specifier");
		read_rtn = sh->_s.s_flags & STYP_BLOCK ? read_bdata : read_data;
		if (!(spc == LMEM && xyflag))
			(*read_rtn) (sh, i, spc, spc);
		else
		{
			(*read_rtn) (sh, i, spc, XMEM);
			(*read_rtn) (sh, i, spc, YMEM);
		}
	}
	free((char *) scn_header);
	do_end();
}


static int fldhex(void)					/* insure fldbuf contains hex value */
{
	char *p = fldbuf;

	while (*p)
	{
		if (!isxdigit(*p))
			break;
		p++;
	}
	if (!*p)
		return TRUE;
	else
		return FALSE;
}


static int get_field(void)				/* get next field from ifile; put in fldbuf */
{
	int c;
	char *p;

	while ((c = fgetc(ifile)) != EOF && isspace(c))
		if (c == '\n')
		{
			linecount++;
		}

	if (c == EOF)						/* end of object file */
		return EOF;

	for (p = fldbuf, *p++ = c;			/* loop to pick up field value */
		 (c = fgetc(ifile)) != EOF && !isspace(c); *p++ = c)
		if (p - fldbuf >= sizeof(fldbuf) - 2)
		{
			*p++ = c;					/* save character */
			break;						/* don't overrun field buffer */
		}
	*p = EOS;							/* null at end of value */
	if (c != EOF)
		(void) ungetc(c, ifile);		/* put back last char. if not EOF */

	return *fldbuf == NEWREC ? 1 : 0;	/* let caller know if new record */
}


static void get_bdata(int spc, int mem)	/* process BLOCKDATA records */
{
	int space = spc = mem;
	int i;
	int j;
	unsigned long addr;
	unsigned long val;
	uint32_t count = 0;
	uint32_t addr_incr;
	unsigned int repeat;
	struct srec *drec;

	if (opts)
	{									/* single file option */
		i = space + 1;
		space = NONE;
		if (s0addr != (unsigned) i)
		{
			bld_s0(i);
			if (fputs(s0buf, rec[0]->fp) == EOF)
				error("cannot write S0 record");
			s0addr = (unsigned) i;
		}
	} else if (!rec[space])				/* see if files are open */
		open_ofiles(space);

	if (get_field() < 0)				/* read in address field */
		error("invalid BLOCKDATA record");
	if (!fldhex())
		error("invalid address value");
	(void) sscanf(fldbuf, "%lx", &addr);	/* convert address */
	addr += memoff[mem];				/* add in memory offset */
	wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
	addr_incr = ((file_header.f_magic == M566MAGIC) && (mem == EMEM)) ? wsizee : wsize;
	if (!optm && baddr)
		addr *= addr_incr;				/* adjust address for serial bytes */
	if (lmem && mem == LMEM && baddr)
		addr *= 2;						/* adjust address for long memory */

	if (get_field() < 0)				/* read in repeat field */
		error("invalid BLOCKDATA record");
	if (!fldhex())
		error("invalid count value");
	(void) sscanf(fldbuf, "%x", &repeat);	/* save repeat value */

	if (get_field() < 0)				/* read in value field */
		error("invalid BLOCKDATA record");
	if (!fldhex())
		error("invalid data value");
	(void) sscanf(fldbuf, "%lx", &val);
	wmask = ((mem == PMEM) || (emem_p)) ? wmaskp : wmaskd;
	wrdfmt = ((mem == PMEM) || (emem_p)) ? wrdfmtp : wrdfmtd;
	(void) sprintf(fldbuf, wrdfmt, val & wmask);
	(void) strup(fldbuf);
	wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
	if ((int) strlen(fldbuf) != wsize * 2)
		error("improper number of bytes in word");
	if (!fldhex())
		error("invalid data value");
	if (!reverse && !optm)
		rev_bytes(fldbuf, mem);

/*
	initialize data record fields
*/

	for (i = 0, drec = rec[space]; i < (optm ? wsize : 1); i++, drec++)
	{
		drec->checksum = 0;
		drec->p = drec->buf;
	}

/*
	loop to generate data records
*/

	wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
	addr_incr = ((file_header.f_magic == M566MAGIC) && (mem == EMEM)) ? wsizee : wsize;
	for (j = 0; j < (int) repeat; j++)
	{

		get_bytes(space, fldbuf);		/* extract bytes from field */

/*
	if max record count reached, print out current record
*/

		count += optm || trunc ? 1L : wsize;
		if (!(count & 1L) && count >= MAXBYTE)
		{
			flush_rec(space, addr, count);
			if ((!baddr) && !(optm || trunc))
			{
				if (lmem && mem == LMEM)
					addr += (count / addr_incr) / 2L;
				else
					addr += count / addr_incr;
			} else
				addr += count;
			count = 0L;
		}
	}

/*
	new record or EOF encountered; flush out current record
*/

	if (rec[space]->p != rec[space]->buf)
		flush_rec(space, addr, count);
}


static void get_data(int spc, int mem)	/* process DATA records */
{
	int space = mem;
	int i;
	int mask = revlong ? !!mem : !mem;
	unsigned long addr;
	unsigned long val;
	uint32_t count = 0;
	uint32_t addr_incr;
	struct srec *drec;

	if (opts)
	{									/* single file option */
		i = space + 1;
		space = NONE;
		if (s0addr != (unsigned) i)
		{
			bld_s0(i);
			if (fputs(s0buf, rec[0]->fp) == EOF)
				error("cannot write S0 record");
			s0addr = (unsigned) i;
		}
	} else if (!rec[space])				/* see if files are open */
		open_ofiles(space);

	if (get_field() < 0)				/* read in address field */
		error("invalid DATA record");
	if (!fldhex())
		error("invalid address value");
	(void) sscanf(fldbuf, "%lx", &addr);	/* convert address */
	addr += memoff[mem];				/* add in memory offset */
	wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
	addr_incr = ((file_header.f_magic == M566MAGIC) && (mem == EMEM)) ? wsizee : wsize;
	if (!optm && baddr)
		addr *= addr_incr;				/* adjust address for serial bytes */
	if (lmem && mem == LMEM && baddr)
		addr *= 2;						/* adjust address for long memory */

/*
	initialize data record fields
*/

	for (i = 0, drec = rec[space]; i < (optm ? wsize : 1); i++, drec++)
	{
		drec->checksum = 0;
		drec->p = drec->buf;
	}

/*
	loop to pick up data
*/

	for (i = 0; get_field() == 0; i++)
	{									/* get next data field */

		if (spc != mem && (i ^ mask) & 1)
			continue;					/* alternate if splitting L space */
		if (!fldhex())
			error("invalid data value");
		(void) sscanf(fldbuf, "%lx", &val);
		wmask = ((mem == PMEM) || (emem_p)) ? wmaskp : wmaskd;
		wrdfmt = ((mem == PMEM) || (emem_p)) ? wrdfmtp : wrdfmtd;
		(void) sprintf(fldbuf, wrdfmt, val & wmask);
		(void) strup(fldbuf);
		wsize = ((mem == PMEM) || (emem_p)) ? wsizep : wsized;
		addr_incr = ((file_header.f_magic == M566MAGIC) && (mem == EMEM)) ? wsizee : wsize;
		if ((int) strlen(fldbuf) != wsize * 2)
			error("improper number of bytes in word");
		if (!reverse && !optm)
			rev_bytes(fldbuf, mem);
		(void) strcpy(lfldbuf, fldbuf);
		if (mem == LMEM)
		{
			if (get_field() != 0)
				error("data synchronization error");
			if (!fldhex())
				error("invalid data value");
			(void) sscanf(fldbuf, "%lx", &val);
			(void) sprintf(fldbuf, wrdfmt, val & wmask);
			(void) strup(fldbuf);
			if ((int) strlen(fldbuf) != wsize * 2)
				error("improper number of bytes in word");
			if (!reverse && !optm)
				rev_bytes(fldbuf, mem);
		}
		get_bytes(space, revlong ? fldbuf : lfldbuf);
		if (mem == LMEM)
			get_bytes(space, revlong ? lfldbuf : fldbuf);

/*
	if max record count reached, print out current record
*/

		if (mem == LMEM)
			count += optm || trunc ? 2L : wsize * 2;
		else
			count += optm || trunc ? 1L : wsize;
		if (!(count & 1L) && count >= MAXBYTE)
		{
			flush_rec(space, addr, count);
			if ((!baddr) && !(optm || trunc))
			{
				if (lmem && mem == LMEM)
					addr += (count / addr_incr) / 2L;
				else
					addr += count / addr_incr;
			} else
				addr += count;
			count = 0L;
		}
	}

/*
	new record or EOF encountered; flush out current record
*/

	if (rec[space]->p != rec[space]->buf)
		flush_rec(space, addr, count);
}


static void get_end(void)				/* process END record and clean up */
{
	int i;
	int field;
	int space;
	uint32_t count = 1;			/* always a checksum byte */
	unsigned long addr;
	uint32_t checksum = 0;
	struct srec *drec;

	if ((field = get_field()) > 0)		/* try to get address field */
		error("invalid END record");

	if (field == 0)
	{
		if (!fldhex())
			error("invalid address value");
		(void) sscanf(fldbuf, "%lx", &addr);	/* convert address */
		addr += memoff[PMEM];			/* add in memory offset */
		wsize = wsizep;
		if (!optm && baddr)
			addr *= wsize;				/* adjust address for serial bytes */
		(void) sprintf(fldbuf, adrfmt, addr & amask);
		checksum = (unsigned) sum_addr(addr & amask);
		count = ovrhd;
		if (!alen)						/* address size not explicitly set */
			check_addr(addr);
	}

	for (space = 0; space < (opts ? 1 : MSPACES); space++)
	{
		if (rec[space])
		{

			wsize = ((space == PMEM) || (emem_p)) ? wsizep : wsized;
			for (i = 0, drec = opts ? rec[0] : rec[space]; i < (optm ? wsize : 1); i++, drec++)
			{
				(void) sprintf(strbuf, s789fmt, count, field == 0 ? fldbuf : "", ~(checksum + count) & CSMASK);
				if (fputs(strup(strbuf), drec->fp) == EOF)
					error("cannot write end S-record");
				(void) fclose(drec->fp);
			}
			free((char *) rec[space]);
			rec[space] = NULL;
		}
	}
}


static char *scan_field(char *bp)	/* scan next field in strbuf; put in fldbuf */
{
	char *p;

	while (*bp && isspace(*bp))
		bp++;

	if (!*bp)							/* end of line */
		return NULL;

	for (p = fldbuf; *bp && !isspace(*bp); *p++ = *bp++)
		;								/* loop to pick up field value */
	*p = EOS;							/* null at end of value */

	return bp;							/* return current line pointer */
}


static int get_comment(void)			/* get comment from load file; put in fldbuf */
{
	int c;
	char *p;

	while ((c = fgetc(ifile)) != EOF && c != '\n' && isspace(c))
		;								/* skip white space (except newline) */

	if (c == EOF || c != '\n')			/* end of file or synch error */
		return EOF;

	linecount++;
	for (p = fldbuf; (c = fgetc(ifile)) != EOF && c != '\n'; *p++ = c)
		if (p - fldbuf >= sizeof(fldbuf) - 2)
		{
			*p++ = c;					/* save character */
			break;						/* don't overrun field buffer */
		}
	if (c == '\n')
		linecount++;
	*p = EOS;							/* null at end of comment */

	return 0;							/* good return */
}


static int get_line(void)				/* buffer line in strbuf */
{
	char *p;
	int c;

	for (p = strbuf; (c = fgetc(ifile)) != EOF && c != '\n'; p++)
		*p = c;
	if (c == EOF || c != '\n')			/* end of file or synch error */
		return EOF;
	*p = EOS;
	(void) ungetc(c, ifile);			/* put back new line */
	return 0;
}


static int get_record(void)				/* look for next record in load file input */
{
	int field = 0;
	char *fbp = fldbuf + 1;

	while (fldbuf[0] != NEWREC && (field = get_field()) == 0)
		;

	if (field < 0)
		return field;

	(void) strup(fbp);
	if (strcmp(fbp, "DATA") == 0)
		return DATA;
	if (strcmp(fbp, "BLOCKDATA") == 0)
		return BDATA;
	if (strcmp(fbp, "START") == 0)
		return START;
	if (strcmp(fbp, "END") == 0)
		return END;
	if (strcmp(fbp, "SYMBOL") == 0)
		return SYMBOL;
	if (strcmp(fbp, "COMMENT") == 0)
		return COMMENT;

	return NONE;
}


static int get_start(FILE *fp)			/* process START record */
{
	int type;
	int old = FALSE;
	char *fbp = fldbuf;
	char *sbp = s0buf;
	char *lbp = strbuf;
	char *p;

/*
	locate START record
*/

	while ((type = get_record()) <= 0 && type != EOF)
		;								/* look for record identifier */

	if (type == EOF)
		return FALSE;

	if (type != START)
	{									/* be forgiving */
		if (!mach && (mach = setup_mach("56000")) == NONE)
			error("cannot initialize machine type");
		bld_s0(NONE);
		if (opts)						/* single file option */
			open_ofiles(NONE);
		return TRUE;
	}

	if (get_line() < 0)					/* buffer entire line */
		error("invalid START record");

/*
	get program name; loop to put into header string
*/

	if ((lbp = scan_field(lbp)) == NULL)	/* pick up program name */
		error("invalid START record");	/* got to have a name */

	if (fp == stdin)
	{									/* use module name for output */
		fix_fname(fldbuf, ".lod");
		if ((p = strrchr(ifname, '.')) != NULL)
			*p = EOS;					/* strip extension */
	}

	while (*fbp)
	{
		(void) sprintf(sbp, "%02x", *fbp);
		s0sum += (unsigned) (*fbp++ & CSMASK);
		sbp += 2;
		s0cnt++;
	}

/*
	build S0 record
*/

	bld_s0(NONE);

	if (((lbp = scan_field(lbp)) == NULL) ||	/* skip version number */
		((lbp = scan_field(lbp)) == NULL) ||	/* skip revision number */
		((lbp = scan_field(lbp)) == NULL))	/* skip error count */
		old = TRUE;						/* wierd or old-style START record */

	if (old)
	{									/* look for machine type */
		if (!mach && (mach = setup_mach("56000")) == NONE)
			error("cannot initialize machine type");
	} else
	{
		/* already got error count in last scan_field() call.... */

		if ((lbp = scan_field(lbp)) == NULL)	/* machine ID */
			error("invalid START record");

		if (strncmp(fldbuf, "DSP", 3) != 0)
			error("invalid START record");

		if (!mach && (mach = setup_mach(&fldbuf[3])) == NONE)
			error("invalid machine type");

		if ((lbp = scan_field(lbp)) == NULL)	/* skip asm version */
			error("invalid START record");
	}

	if (get_comment() < 0)				/* skip comment */
		error("invalid START record");

	if (opts)							/* single file option */
		open_ofiles(NONE);

	return TRUE;
}


static int get_space(void)				/* return memory space attribute */
{
	switch (fldbuf[0])
	{
	case 'X':
	case 'x':
		return XMEM;
	case 'Y':
	case 'y':
		return YMEM;
	case 'L':
	case 'l':
		return LMEM;
	case 'P':
	case 'p':
		return PMEM;
	case 'E':
	case 'e':
		return EMEM;
	case 'D':
	case 'd':
		return DMEM;
	default:
		return -1;
	}
}


static void do_lod(FILE *fp)			/* process load file records */
{
	int rtype;
	int spc;
	long reset;
	void (*get_rtn)(int, int);

	if (!get_start(fp))					/* no START record */
		error("no START record");

	while (!feof(fp))
	{									/* loop while not end-of-file */

		rtype = get_record();
		switch (rtype)
		{
		case START:
			error("duplicate START record");
		case END:
			get_end();
			return;
		case DATA:
		case BDATA:
			get_rtn = rtype == DATA ? get_data : get_bdata;
			if (get_field() < 0)		/* pick up memory space */
				error("invalid DATA/BLOCKDATA record");
			if ((spc = get_space()) < 0)
				error("invalid memory space specifier");
			if (!(spc == LMEM && xyflag))
				(*get_rtn) (spc, spc);
			else
			{
				reset = ftell(ifile);
				(*get_rtn) (spc, XMEM);
				if (fseek(ifile, reset, 0) != 0)
					error("cannot reset data pointer in L memory");
				(*get_rtn) (spc, YMEM);
			}
			break;
		case COMMENT:
			(void) get_comment();
			break;
		case SYMBOL:
			fldbuf[0] = EOS;			/* skip SYMBOL records */
			break;
		default:
			if (!feof(fp))
				error("invalid record type");
		}
	}
	/* should never get here if END record found */
	if (feof(fp))
		get_end();						/* be forgiving */
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

/**
*
* name		dcl_getval - get DCL command line value
*
* synopsis	status = dcl_getval (opt)
*		int status;	return status from cli$get_value
*		struct dsc$descriptor_s *opt;	pointer to command line option
*
* description	Calls the VMS DCL routine cli$get_value to return values
*		for the command line option referenced by opt.  The values
*		are returned in the global dclbuf array.  The length returned
*		from cli$get_value is used to terminate the string.
*
**/
static int
#if defined (VMS)
dcl_getval(struct dsc$descriptor_s *opt)
#else
dcl_getval(int *opt)
#endif
{
#if defined (VMS)
	unsigned int status;
	unsigned int len = 0;

	status = cli$get_value(opt, &arg_desc, &len);
	if (status != CLI$_ABSENT)
		dclbuf[len] = EOS;
	return status;
#else
	return *opt;
#endif
}


#if defined (VMS)
/**
*
* name		dcl_reset - reset DCL command line
*
* synopsis	yn = dcl_reset ()
*		int yn;		TRUE/FALSE for errors
*
* description	Resets the DCL command line so that it may be reparsed.
*		Returns TRUE if command line is reset, FALSE on error.
*
**/
static int dcl_reset(void)
{
	return cli$dcl_parse(NULL, NULL) == CLI$_NORMAL;
}
#endif


static void usage(void)					/* display usage on stderr, exit nonzero */
{
#if defined (__MSDOS__) || defined (__TOS__) || defined (__atarist__)
	if (quiet)
#endif
		(void) fprintf(stderr, "%s  %s\n%s\n", Progtitle, Version, Copyright);
	(void) fprintf(stderr,
				   "Usage:  %s [-blmqrsuwx] [-a <alen>] [-o <mem>:<offset>] [-p <procno>] [-t <tlen>] <input file ... >\n",
				   Progname);
	(void) fprintf(stderr, "        a - <alen> S-record address length\n");
	(void) fprintf(stderr, "        b - byte addressing\n");
	(void) fprintf(stderr, "        c - truncate words to bytes\n");
	(void) fprintf(stderr, "        l - long (double-word) addressing\n");
	(void) fprintf(stderr, "        m - multiple output files\n");
	(void) fprintf(stderr, "        o - add <offset> to <mem> addresses\n");
	(void) fprintf(stderr, "        p - <procno> load file format\n");
	(void) fprintf(stderr, "        q - do not display signon banner\n");
	(void) fprintf(stderr, "        r - reverse bytes in word\n");
	(void) fprintf(stderr, "        s - single output file\n");
	(void) fprintf(stderr, "        t - <tlen> target word length\n");
	(void) fprintf(stderr, "        u - reverse words in L memory\n");
	(void) fprintf(stderr, "        w - word addressing\n");
	(void) fprintf(stderr, "        x - convert L records to X and Y\n");
	exit(1);
}


static void do_srec(FILE *fp)			/* determine type of object file */
{
	int c;

	if (ftype == FT_CLD)
	{
		do_coff(fp);
	} else if (ftype == FT_LOD)
	{
		do_lod(fp);
	} else
	{									/* try to guess type from file contents */
		while ((c = fgetc(fp)) != EOF)
			if (c == NEWREC || c == EOS)
				break;
		rewind(fp);						/* restart file */
		if (c == EOF)					/* end of file with no flag character */
			error("invalid object file type");
		if (c == EOS)					/* simple-minded binary assumption */
			do_coff(fp);
		else							/* assume we've got a .LOD file */
			do_lod(fp);
	}
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
		(void) fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
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
			(void) fprintf(stderr, "%s: -%c argument missing\n", argv[0], c);
			return '?';
		}
	}

	return c;
}


int main(int argc, char *argv[])
{
	int c;
	char *fn;
	char *p;
	char *procno = NULL;

/*
	check for command line options
*/

#if defined (VMS)
	dclflag = getenv(p) == NULL;
#endif
	/* scan for quiet flag on command line */
	quiet = cmdarg('q', argc, argv) != 0;

#if defined (__MSDOS__) || defined (__TOS__) || defined (__atarist__)
	if (!quiet)
		(void) fprintf(stderr, "%s  %s\n%s\n", Progtitle, Version, Copyright);
#endif

	if (!dclflag)
	{									/* not using VMS DCL */

		coptarg = NULL;					/* command argument pointer */
		coptind = 0;						/* command argument index */
		while ((c = getopts(argc, argv, "A:a:BbCcLlMmO:o:P:p:QqRrSsT:t:UuW?w?Xx")) != EOF)
		{
			if (isupper(c))
				c = tolower(c);
			switch (c)
			{
			case 'a':
				if (!sscanf(coptarg, "%d", &alen))
					usage();
				break;
			case 'b':
				baddr = TRUE;
				break;
			case 'c':
				trunc = TRUE;
				break;
			case 'l':
				lmem = TRUE;
				break;
			case 'm':
				optm = TRUE;
				break;
			case 'o':
				if ((p = strchr(memstr, coptarg[0])) == NULL || coptarg[1] != ':')
					usage();
				if (isupper(*p))
					p -= MSPACES;
				if (!sscanf(&coptarg[2], "%lx", &memoff[p - memstr]))
					usage();
				break;
			case 'p':
				procno = coptarg;
				break;
			case 'q':
				quiet = TRUE;
				break;
			case 'r':
				reverse = TRUE;
				break;
			case 's':
				opts = TRUE;
				break;
			case 't':
				if (!sscanf(coptarg, "%d", &tlen))
					usage();
				break;
			case 'u':
				revlong = TRUE;
				break;
			case 'w':
				waddr = TRUE;
				break;
			case 'x':
				xyflag = TRUE;
				break;
			case '?':
			default:
				usage();
				break;
			}
		}
		argc -= coptind;				/* adjust argument count */
		argv = &argv[coptind];			/* reset argv pointer */
		if (argc <= 0 || (opts && optm) || (trunc && optm) || (baddr && waddr))
			usage();					/* no more args or mutually exclusive args */

	} else
	{									/* using VMS DCL */
#if defined (VMS)
		if (cli$present(&byt_desc) == CLI$_PRESENT)
			baddr = TRUE;
		if (cli$present(&lng_desc) == CLI$_PRESENT)
			lmem = TRUE;
		if (cli$present(&mlt_desc) == CLI$_PRESENT)
			optm = TRUE;
		if (dcl_getval(&prc_desc) != CLI_ABSENT)
			procno = dclbuf;
		if (cli$present(&rev_desc) == CLI$_PRESENT)
			reverse = TRUE;
		if (cli$present(&sgl_desc) == CLI$_PRESENT)
			opts = TRUE;
		if (cli$present(&wrd_desc) == CLI$_PRESENT)
			waddr = TRUE;
#endif
	}

/*
	Set processor type if necessary
*/

	if (procno && (mach = setup_mach(procno)) == NONE)
		error("invalid machine type");
	if (alen && (alen < ASIZE1 || alen > ASIZE3))
		error("invalid -a command line argument");
	if (tlen && (tlen < WSIZE2 || tlen > WSIZE4))
		error("invalid -t command line argument");
	if (lmem && xyflag)
		lmem = FALSE;					/* reset long memory address flag */

/*
	Loop to process files
*/

	while (dclflag ? dcl_getval(&lod_desc) != CLI_ABSENT : argc)
	{
		fn = dclflag ? dclbuf : *argv;
		if (strcmp(fn, "-") == 0)
		{
			if ((ifname = malloc((unsigned) (MAXSTR))) == NULL)
				error("cannot allocate file name");
			ifile = stdin;				/* use standard input */
			do_srec(ifile);				/* process records */
#if defined (VAX)
		} else if (strcmp(strup(fn), "SYS$INPUT") == 0)
		{
			if ((ifname = malloc((unsigned) (MAXSTR))) == NULL)
				error("cannot allocate file name");
			ifile = stdin;				/* use standard input */
			do_srec(ifile);				/* process records */
#endif
		} else
		{
			if ((ifname = malloc((unsigned) (strlen(fn) + MAXEXTLEN + 1))) == NULL)
				error("cannot allocate file name");
			(void) strcpy(ifname, fn);
			ftype = open_ifile(ifname);
			if ((p = strrchr(ifname, '.')) != NULL)
				*p = EOS;				/* strip extension */
			do_srec(ifile);				/* process records */
			(void) fclose(ifile);		/* close input file */
		}
		free(ifname);					/* free file name */
		if (!dclflag)
		{
			argv++;						/* bump arg pointer */
			argc--;						/* decrement arg counter (MPW 3.0 kludge) */
		}
	}

	return 0;
}
