#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DSP_WORDSIZE 3

static uint8_t binbuff[128 * 1024 * DSP_WORDSIZE];
static char *codebuf;
static char *curptr;
static char *bufend;
static size_t binindex;

/* counter to place into binbuff at  */
/* size_loc_index after block is     */
/* processed                 */
static long size_count;

#define NUMTOKENS       6
#define DSPDATA         1
#define DSPBLOCK        2
#define DSPSYMBOL       3
#define DSPEND          5

static const char *const tokens[] = {
	"START",
	"DATA",
	"BLOCKDATA",
	"SYMBOL",
	"COMMENT",
	"END"
};

static int const token_lengths[] = {
	5,
	4,
	9,
	6,
	7,
	3
};


/************************************************************************/
/* Simple string compare of current buffer pointer and table of tokens  */
/* Check for upper or lower case comparison matching					*/
/* Return 1 if strings ar equal or 0 if not								*/
/************************************************************************/

static int streq(const char *bufstr, const char *tablestr, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		if (bufstr[i] != tablestr[i] && bufstr[i] != (tablestr[i] + 32))
			return 0;
	}
	return 1;
}

/************************************************************************/
/*	Read in DSP load file.  											*/
/************************************************************************/

/************************************************************************/
/* If first char of string is '_' then we have a token					*/
/* so return 1															*/
/************************************************************************/

static int is_token(void)
{
	if (*curptr == '_')
	{
		curptr++;						/* Only bump pointer if we have a token */
		return 1;
	}
	return 0;
}

/************************************************************************/
/* Return token number associated with string pointed to by curptr		*/
/************************************************************************/

static int get_token(void)
{
	int i;

	for (i = 0; i < NUMTOKENS; i++)
	{
		if (streq(curptr, tokens[i], token_lengths[i]))
			break;
	}
	return i;
}


static int iseol(void)
{
	return *curptr == 0x0D || *curptr == 0x0A;
}


/************************************************************************/
/*	Move curptr to new line 											*/
/************************************************************************/

static void newline(void)
{
	while (curptr < bufend && !iseol())
		curptr++;
	while (curptr < bufend && iseol())
		curptr++;
}

/************************************************************************/
/* Convert next 4 to 6 ascii hex digits into a binary number	        */
/************************************************************************/

static unsigned long make_long(void)
{
	unsigned long val;
	unsigned long temp;
	int i;

	val = 0;
	for (i = 0; i < 6; i++)
	{
		if (*curptr >= '0' && *curptr <= '9')
			temp = *curptr - '0';
		else if (*curptr >= 'A' && *curptr <= 'F')
			temp = (*curptr - 'A') + 10;
		else if (*curptr >= 'a' && *curptr <= 'f')
			temp = (*curptr - 'a') + 10;
		else
			break;
		curptr++;
		val = (val << 4) | temp;
	}
	return val;
}

/************************************************************************/
/* 	Place the 3 bytes a,b,c into the binary buffer						*/
/************************************************************************/

static void put_dspword(unsigned char a, unsigned char b, unsigned char c)
{
	binbuff[binindex++] = a;
	binbuff[binindex++] = b;
	binbuff[binindex++] = c;
	size_count++;						/* counter for size of block */
}

/************************************************************************/
/*	Get new program location.  Reset size_count since we're starting 	*/
/*	a new block.  Set size_loc_index so we can stuff size_count      	*/
/*  	after we're through with the block.  Place block type, start    */
/*	location, and a dummy size placeholder into buffer.		 			*/
/* 	Block types = 0 - program, 1 - X, 2 - Y				 				*/
/************************************************************************/

static long stuff_header(char memtype)
{
	unsigned long block_loc;
	/* Place holder to put size of block */
	/* at beginning of block             */
	/* Keep track of this until we've    */
	/* actually calculated it        */
	long size_loc_index;

	/*********************************************************/
	/*  First put the block type into the buffer             */
	/*********************************************************/
	binbuff[binindex++] = 0;
	binbuff[binindex++] = 0;
	if (memtype == 'P')
	{
		binbuff[binindex++] = 0;
	} else if (memtype == 'X')
	{
		binbuff[binindex++] = 1;
	} else if (memtype == 'Y')
	{
		binbuff[binindex++] = 2;
	} else
	{
		return 0;
	}

	/*********************************************************/
	/* Next store the block location                         */
	/*********************************************************/

	block_loc = make_long();
	binbuff[binindex++] = (unsigned char) ((block_loc >> 16) & 0xff);
	binbuff[binindex++] = (unsigned char) ((block_loc >>  8) & 0xff);
	binbuff[binindex++] = (unsigned char) ((block_loc >>  0) & 0xff);

	/***********************************************************/
	/* Now setup to store the block size.  Create place holder */
	/* in buffer, init size_loc_index to binindex, and reset   */
	/* size_count                                              */
	/***********************************************************/

	size_count = 0;
	size_loc_index = binindex;
	binindex += DSP_WORDSIZE;
	newline();
	return size_loc_index;
}

/************************************************************************/
/*	Take the next 6 ascii hex digits and convert to a 24 bit DSP	 	*/
/*	word. (by creating 3 consecutive bytes								*/
/************************************************************************/

static void make_dspword(void)
{
	int i, j;
	unsigned char ch[DSP_WORDSIZE];
	char mult;
	char temp;
	char val;

	for (i = 0; i < DSP_WORDSIZE; i++)				/* For the next 3 bytes - 24 bits */
	{
		val = 0;
		mult = 16;
		for (j = 0; j < 2; j++)			/* 2 characters to a byte */
		{
			if (*curptr >= '0' && *curptr <= '9')
				temp = (*curptr - '0');
			else if (*curptr >= 'A' && *curptr <= 'F')
				temp = (*curptr - 'A') + 10;
			else
				temp = (*curptr - 'a') + 10;
			curptr++;
			val += temp * mult;
			mult /= 16;
		}
		ch[i] = val;
	}
	put_dspword(ch[0], ch[1], ch[2]);
}

/************************************************************************/
/* 	Convert a line of ascii hex into binary DSP code					*/
/************************************************************************/

static void convert_line(void)
{
	while (curptr < bufend && !iseol())
	{
		while (curptr < bufend && *curptr == ' ')
			curptr++;
		if (curptr < bufend && !iseol())			/* If not end of line then, */
			make_dspword();				/* it must be a data word */
	}
}


/* Called when a DATA token is found */
static int do_convert(void)
{
	char memtype;						/* X, Y, or P memory */
	long size_loc_index;

	curptr += token_lengths[DSPDATA];
	while (curptr < bufend && *curptr == ' ')				/* Get rid of spaces between "DATA" and */
		curptr++;											/* Memory type "X,Y,L, or P" */
	memtype = *curptr++;									/* Get memory type */
	while (curptr < bufend && *curptr == ' ')
		curptr++;
	size_loc_index = stuff_header(memtype);					/* Setup Block's header info */
	if (size_loc_index == 0)
		return 0;
	while (curptr < bufend && !is_token())					/* Convert all lines of this section to bin */
	{
		if (iseol())
			newline();
		else
			convert_line();
	}

	/****************************************************************/
	/*  Now that we've completed the block and know its size, put   */
	/*  it into the blocks header that's at the beginning of the    */
	/*  block.                                                      */

	binbuff[size_loc_index++] = (unsigned char) ((size_count >> 16) & 0xff);
	binbuff[size_loc_index++] = (unsigned char) ((size_count >>  8) & 0xff);
	binbuff[size_loc_index++] = (unsigned char) ((size_count >>  0) & 0xff);

	return 1;
}


static int convert_file(void)
{
	int dsptoken;

	curptr = codebuf;
	binindex = 0;
	while (curptr < bufend && !is_token())					/* Look for first token, prime pump */
		newline();
	while (curptr < bufend && (dsptoken = get_token()) != DSPEND)
	{
		switch (dsptoken)
		{
		case DSPDATA:
			if (do_convert() == 0)
				return 0;
			break;
#if 0
		case DSPSYMBOL:
			while (get_token() != DSPEND)
			{
				printf("%i\n", get_token());
				while (curptr < bufend && !is_token())
					newline();
			}
			break;
#endif
		default:
			newline();					/* Blow away token line */
			while (curptr < bufend && !is_token())			/* Find next token */
				newline();
			break;
		}
	}
	return 1;
}


int main(int argc, char **argv)
{
	FILE *fi;
	FILE *fo;
	size_t fisize;
	size_t fosize;
	int exitcode = 0;
	
	if (argc != 3)
	{
		fprintf(stderr, "LOD to P56 converter\n");
		fprintf(stderr, "Usage: %s <LOD file> <P56 file>\n", argv[0]);
		return 1;
	}

	if ((fi = fopen(argv[1], "rb")) == NULL)
	{
		fprintf(stderr, "error reading from input file\n");
		return 1;
	}

	fseek(fi, 0L, SEEK_END);
	fisize = ftell(fi);
	rewind(fi);
	codebuf = (char *) malloc(fisize);
	fread(codebuf, 1, fisize, fi);
	fclose(fi);
	bufend = codebuf + fisize;

	if (convert_file())
	{
		fosize = binindex / DSP_WORDSIZE;
		if ((fo = fopen(argv[2], "wb")) == NULL)
		{
			fprintf(stderr, "error writing to output file\n");
			return 1;
		}
		fwrite(binbuff, 3, fosize, fo);
		fclose(fo);
	} else
	{
		fprintf(stderr, "error during conversion\n");
		exitcode = 1;
	}

	free(codebuf);
	return exitcode;
}
