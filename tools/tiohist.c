#include <stdlib.h>
#include <stdio.h>
#include <search.h>

/* tiohist tiofilename blockfile >outputfilename */
/* This program will generate three histogram charts to the standard output by
   post processing the ".tio" file that results from the dsp simulator command
   "output t history tiofilename". 

   tiofilename - is the full name of the .tio file - for example hist1.tio

   blockfile - is the name of a text file which must contain a list of white
      space separated hexadecimal numbers that specify the start address
      of each block that will be represented in the histogram.

   The program generates three histograms.  The first is a cycle count histogram,
   indicating the number of clock cycles executed within each block; the second is
   an instruction count histogram, indicating the number of instructions executed
   within each block;  the third is a block entry histogram, indicating the number
   of entries into each block.
*/

struct hblk
{
	unsigned long startaddr;			/* start address of block */
	unsigned long cycles;				/* number of cycles executed within this block */
	unsigned long instructions;			/* number of instructions executed within this block */
	unsigned long entries;				/* number of entries into this block */
} initblk;

static int startaddr_compare(const void *_i, const void *_j)
{
	const struct hblk *i = (const struct hblk *)_i;
	const struct hblk *j = (const struct hblk *)_j;
	return ((i->startaddr) - (j->startaddr));
}


static char histbar[] = "-------------------------------------------------------------";
static int histbarmax = sizeof(histbar) - 1;


static void histout(const char *title, int numblocks, struct hblk *hb, unsigned long scaler, int fieldindex)
{
	unsigned long fieldval;
	int i;

	if (!scaler)
		scaler = 1;
	printf("\n%s", title);
	printf("\nblock     count");
	printf("\n--------- --------\n");
	for (i = 0; i < numblocks; i++)
	{
		fieldval = (fieldindex == 0) ? hb[i].cycles : (fieldindex == 1) ? hb[i].instructions : hb[i].entries;
		printf("$%08lx %08ld %s\n", hb[i].startaddr, fieldval,
			   &histbar[histbarmax - ((fieldval * histbarmax) / scaler)]);
	}
}

static void tiohist(FILE *pt, FILE *pb)
{
	int num_blocks;
	int numalloc;
	int prev_blk;
	int cur_blk;
	int fc;
	unsigned long icycle;
	unsigned long prev_icycle;
	unsigned long iaddr;
	unsigned long maxkey;
	struct hblk *blks;


	/* form blocks */
	if (!(blks = (struct hblk *) malloc(100 * sizeof(struct hblk))))
	{
		perror("tiohist");
		exit(1);
	} else
		numalloc = 100;

	for (num_blocks = 0;; num_blocks++)
	{
		if (fscanf(pb, " %lx", &initblk.startaddr) == 1)
		{
			if (num_blocks >= numalloc)
			{
				/* get another hundred blocks if you run out */
				if (!(blks = (struct hblk *) realloc(blks, (numalloc + 100) * sizeof(struct hblk))))
				{
					perror("tiohist");
					exit(1);
				} else
					numalloc += 100;
			}
			blks[num_blocks] = initblk;	/* initialize each block to all zeros except startaddr */
		} else
			break;
	}

	/* sort the blocks in order of ascending address */
	if (num_blocks)
	{
		qsort((char *) blks, num_blocks, sizeof(struct hblk), startaddr_compare);
	}

	/* load up the block data from the tio file */
	/* based on output t history filename.tio */
	/* output data is in form: 10489 P:$e267 000000        = nop */
	prev_icycle = 0;
	prev_blk = -1;

	while (fscanf(pt, " %lu P:$%lx", &icycle, &iaddr) == 2)
	{
		/* find block for addr */
		cur_blk = (prev_blk < 0) ? 0 : prev_blk;
		while ((cur_blk >= 0) && (iaddr < blks[cur_blk].startaddr))
			cur_blk--;
		while ((cur_blk < (num_blocks - 1)) && (iaddr >= blks[cur_blk + 1].startaddr))
			cur_blk++;
		if (cur_blk >= 0)
		{								/* block found */
			if (cur_blk != prev_blk)
				blks[cur_blk].entries += 1;
			blks[cur_blk].instructions += 1;
			blks[cur_blk].cycles += icycle - prev_icycle;
		}
		prev_icycle = icycle;
		prev_blk = cur_blk;
		while ((fc = getc(pt)) != '\n' && fc != EOF)
			;
	}

	/* find maximum cycles for scaling */
	for (maxkey = 0, cur_blk = 0; cur_blk < num_blocks; cur_blk++)
		if (blks[cur_blk].cycles > maxkey)
			maxkey = blks[cur_blk].cycles;

	/* output histogram based on cycles */
	histout("_______Cycle Count Histogram__________", num_blocks, blks, maxkey, 0);

	/* find maximum instructions for scaling */
	for (maxkey = 0, cur_blk = 0; cur_blk < num_blocks; cur_blk++)
		if (blks[cur_blk].instructions > maxkey)
			maxkey = blks[cur_blk].instructions;

	/* output histogram based on instructions */
	histout("_______Instruction Count Histogram__________", num_blocks, blks, maxkey, 1);


	/* find maximum entries for scaling */
	for (maxkey = 0, cur_blk = 0; cur_blk < num_blocks; cur_blk++)
		if (blks[cur_blk].entries > maxkey)
			maxkey = blks[cur_blk].entries;

	/* output histogram based on entries into block */
	histout("_______Block Entry Count Histogram__________", num_blocks, blks, maxkey, 2);
}


int main(int argc, char **argv)
{
	FILE *fptio;
	FILE *fpblk;

	if (argc != 3)
	{
		fprintf(stderr, "Version 6.3 usage: tiohist tiofile blockfile > outputfile\n");
		return 1;
	}

	if (!(fptio = fopen(argv[1], "r")))
	{
		perror("tiohist");
		return 1;
	}

	if (!(fpblk = fopen(argv[2], "r")))
	{
		perror("tiohist");
		return 1;
	}

	tiohist(fptio, fpblk);

	(void) fclose(fpblk);
	(void) fclose(fptio);

	return 0;
}
