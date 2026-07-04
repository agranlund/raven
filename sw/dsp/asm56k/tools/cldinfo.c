/*
  to make for most systems,

  cc -O -Iinclude cldinfo.c
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "coreaddr.h"
#include "maout.h"


/* we use this flag to tell whether or not we're working on a .cld file. */
static int not_a_cld_p;

static struct
{										/* used for loading section data */
	uint32_t block_size;
	uint32_t *mem;
} raw_data;

static uint32_t psize = 0;
static uint32_t xsize = 0;
static uint32_t ysize = 0;
static uint32_t start_address = 0;
static uint32_t emsize = 0;
static uint32_t dmsize = 0;
static uint32_t usize = 0;

/* Routines for reading headers and symbols from executable.  */

#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
static void swapem(void *uptr, size_t usiz)
{
	size_t i;
	unsigned char uc0;
	unsigned char *ucp;

	ucp = (unsigned char *) uptr;
	usiz /= 4;

	for (i = 0; i < usiz; i++, ucp += 4)
	{
		uc0 = ucp[0];
		ucp[0] = ucp[3];
		ucp[3] = uc0;
		uc0 = ucp[1];
		ucp[1] = ucp[2];
		ucp[2] = uc0;
	}
}
#else
#define swapem(uptr, usiz)
#endif



/* Read COFF file header, check magic number,
   and return number of symbols. */
static int read_file_hdr(FILE *chan, FILHDR *fil_hdr)
{
	(void) fseek(chan, 0L, 0);

	if (fread((char *) fil_hdr, FILHSZ, 1, chan) != 1)
	{
		perror("read_file_hdr");
		exit(2);
	}
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
	swapem(fil_hdr, FILHSZ);
#endif

	switch (fil_hdr->f_magic)
	{
#ifdef M566MAGIC
	case M566MAGIC:
#endif
#ifdef M568MAGIC
	case M568MAGIC:
#endif
#ifdef M563MAGIC
	case M563MAGIC:
#endif
#ifdef M16KMAGIC
	case M16KMAGIC:
#endif
#ifdef M96KMAGIC
	case M96KMAGIC:
#endif
#ifdef M56KMAGIC
	case M56KMAGIC:
#endif
#ifdef SC1MAGIC
	case SC1MAGIC:
#endif
#ifdef M567MAGIC
	case M567MAGIC:
#endif
		return fil_hdr->f_nsyms;

	default:
#ifdef BADMAG
		if (BADMAG(fil_hdr))
		{
			fprintf(stderr, "read_file_hdr() failure\n");
			exit(2);
		}

		else
		{
			return fil_hdr->f_nsyms;
		}

#else
		fprintf(stderr, "read_file_hdr() failure\n");
		exit(2);
#endif
	}
	return 0;
}

static int read_aout_hdr(FILE *chan, AOUTHDR *ao_hdr, int size)
{
	(void) fseek(chan, FILHSZ, 0);

	if (size != sizeof(AOUTHDR))
	{
		fprintf(stderr, "read_aout_hdr() failure\n");
		exit(3);
	}


	if (fread((char *) ao_hdr, size, 1, chan) != 1)
	{
		perror("read_aout_hdr");
		exit(3);
	}
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
	swapem(ao_hdr, size);
#endif

	return 0;
}

static void read_section_contents(FILE *chan)
{
	FILHDR fl_hdr;
	AOUTHDR ao_hdr;
	SCNHDR section_hdr;
	uint32_t i;
	enum memory_map memtype;
	uint32_t section_length;

	read_file_hdr(chan, &fl_hdr);

	if (fl_hdr.f_opthdr != sizeof(AOUTHDR))
	{
		/* this isn't a .cld file. make a note, and continue on. */
		not_a_cld_p = 1;
	} else
	{
		read_aout_hdr(chan, &ao_hdr, (int) sizeof(AOUTHDR));
	}

	for (i = 0; i < fl_hdr.f_nscns; ++i)
	{
		if (fseek(chan, FILHSZ + fl_hdr.f_opthdr + (i * SCNHSZ), 0) != 0)
		{
			perror("read_section_contents 1");
			exit(5);
		}

		if (fread((char *) &section_hdr, SCNHSZ, 1, chan) != 1)
		{
			perror("read_section_contents 2");
			exit(5);
		}
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
		swapem(&section_hdr, SCNHSZ);
		swapem(section_hdr.s_name, 8);
#endif

		/* we want to summarize all of the sections *except* for ".text"
		   and ".data"; these themselves are summaries of a sort that are
		   needed by the debugger. including them would incorrectly count
		   all object twice (and then some). */
		if (strncmp(section_hdr.s_name, ".text", 8) && strncmp(section_hdr.s_name, ".data", 8))
		{
			/* load memory block */
			if (section_hdr.s_size > raw_data.block_size)
			{
				if (raw_data.mem)
				{
					free(raw_data.mem);
				}
				if (!(raw_data.mem = (uint32_t *) malloc((size_t) ((section_hdr.s_size) * sizeof(uint32_t)))))
				{
					perror("malloc error");
					exit(5);
				}

				raw_data.block_size = section_hdr.s_size;
			}

			if (fseek(chan, section_hdr.s_scnptr, 0) != 0)
			{
				perror("read_section_contents 3");
				exit(5);
			}

			if (section_hdr.s_size && (fread((char *) raw_data.mem,
											 (section_hdr.s_size * sizeof(uint32_t)), 1, chan) != 1))
			{
				perror("read_section_contents 4");
				exit(5);
			}
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
			swapem(raw_data.mem, section_hdr.s_size * sizeof(uint32_t));
#endif
			switch (memtype = CORE_ADDR_MAP(section_hdr.s_paddr))
			{
			case memory_map_pa:
			case memory_map_pb:
			case memory_map_pe:
			case memory_map_pi:
			case memory_map_pr:
			case memory_map_p8:
				memtype = memory_map_p;
				break;

			case memory_map_xa:
			case memory_map_xb:
			case memory_map_xe:
			case memory_map_xi:
			case memory_map_xr:
				memtype = memory_map_x;
				break;

			case memory_map_ya:
			case memory_map_yb:
			case memory_map_ye:
			case memory_map_yi:
			case memory_map_yr:
				memtype = memory_map_y;
				break;

			case memory_map_laa:
			case memory_map_li:
			case memory_map_lab:
			case memory_map_lba:
			case memory_map_lbb:
			case memory_map_le:
				memtype = memory_map_l;
				break;

			case memory_map_u8:
			case memory_map_u16:
				memtype = memory_map_u;
				break;
			default:
				break;
			}

			if (section_hdr.s_flags & STYP_BLOCK)
			{
				section_length = CORE_ADDR_ADDR(section_hdr.s_vaddr);

				if (memtype == memory_map_l)
				{
					xsize += section_length;
					ysize += section_length;
				} else if (memtype == memory_map_y)
				{
					ysize += section_length;
				} else if (memtype == memory_map_p)
				{
					psize += section_length;
				} else if (memtype == memory_map_x)
				{
					xsize += section_length;
				} else if (memtype == memory_map_dm)
				{
					dmsize += section_length;
				} else if (memtype >= memory_map_emi && memtype <= memory_map_e255)
				{
					emsize += section_length;
				} else if (memtype == memory_map_u)
				{
					usize += section_length;
				}
			} else
			{
				section_length = section_hdr.s_size;
				if (memtype == memory_map_l)
				{
					section_length >>= 1;
					xsize += section_length;
					ysize += section_length;
				} else if (memtype == memory_map_y)
				{
					ysize += section_length;
				} else if (memtype == memory_map_p)
				{
					psize += section_length;
				} else if (memtype == memory_map_x)
				{
					xsize += section_length;
				} else if (memtype == memory_map_dm)
				{
					dmsize += section_length;
				} else if (memtype >= memory_map_emi && memtype <= memory_map_e255)
				{
					emsize += section_length;
				} else if (memtype == memory_map_u)
				{
					usize += section_length;
				}
			}
		}
	}

	if (!not_a_cld_p)
	{
		start_address = CORE_ADDR_ADDR(ao_hdr.entry);
	}

	if (raw_data.mem)
	{
		free(raw_data.mem);
		raw_data.block_size = 0;
		raw_data.mem = 0;
	}
}


static void cld_info(FILE *execchan, long *xs, long *ys, long *ps, long *sa, long *ems, long *dms, long *us)
{
	/* Now open and digest the file the user requested, if any.  */
	read_section_contents(execchan);

	*xs = xsize;
	*ps = psize;
	*ys = ysize;
	*ems = emsize;
	*dms = dmsize;
	*us = usize;
	if (!not_a_cld_p)
	{
		*sa = start_address;
	}
}


int main(int argc, char **argv)
{
	long xs;
	long ys;
	long ps;
	long sa = 0;
	long ems;
	long dms;
	long us;
	FILE *ifile;						/* file pointer for input file */

	ifile = NULL;

	if (argc != 2)
	{
		fprintf(stderr, "Version 6.3 usage: cldinfo file\n");
		return 1;
	}
	if ((ifile = fopen(argv[1], "rb")) == NULL)
	{
		perror(argv[1]);
		return 1;
	}

	cld_info(ifile, &xs, &ys, &ps, &sa, &ems, &dms, &us);

	(void) fclose(ifile);

	fprintf(stdout, "filename: %s\n", argv[1]);

	if (not_a_cld_p)
	{
		fprintf(stdout,
				"\txsize: %ld, ysize: %ld, psize: %ld, emsize: %ld, dmsize: %ld, usize: %ld\n",
				xs, ys, ps, ems, dms, us);
	} else
	{
		fprintf(stdout,
				"\txsize: %ld, ysize: %ld, psize: %ld, emsize: %ld, dmsize: %ld, usize: %ld, start addr: %lx\n",
				xs, ys, ps, ems, dms, us, sa);
	}

	return 0;
}
