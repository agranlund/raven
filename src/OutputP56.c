/*

Project:    asm56k
Author:     M.Buras (sqward)

  P56 format:

  adr size  discr. (size: dsp word)
    0    1  memory (0=P, 1=X, 2=Y)
    1    1  block start adress
    2    1  block size (N)
    3    N  data
  N+3    1  memory (0=P, 1=X, 2=Y)
  ...


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm_types.h>
#include <export.h>
#include <Value.h>
#include "SymbolTable.h"
#include "CodeUtils.h"
#include "OutputP56.h"


static void P56_SaveData(FILE *output_file, int chunkIndex, int memtype, int offset, int skip)
{
	int j;
	unsigned char *pCode;
	unsigned char *pCodeCopy = NULL;
	unsigned char *pCodeOrig = NULL;

	pCode = chunks[chunkIndex].code_ptr + offset;

	j = (chunks[chunkIndex].code_len / 3) >> (skip ? 1 : 0);

	pCodeOrig = pCodeCopy = malloc(chunks[chunkIndex].code_len + 9);
	MTEST(pCodeCopy);

	*pCodeCopy++ = (u8) (memtype >> 16) & 0xff;
	*pCodeCopy++ = (u8) (memtype >> 8) & 0xff;
	*pCodeCopy++ = (u8) (memtype >> 0) & 0xff;

	*pCodeCopy++ = (u8) (chunks[chunkIndex].pc >> 16) & 0xff;
	*pCodeCopy++ = (u8) (chunks[chunkIndex].pc >> 8) & 0xff;
	*pCodeCopy++ = (u8) (chunks[chunkIndex].pc >> 0) & 0xff;

	*pCodeCopy++ = (u8) (j >> 16) & 0xff;
	*pCodeCopy++ = (u8) (j >> 8) & 0xff;
	*pCodeCopy++ = (u8) (j >> 0) & 0xff;

	for (; j != 0; j--)
	{
		*pCodeCopy++ = *pCode++;
		*pCodeCopy++ = *pCode++;
		*pCodeCopy++ = *pCode++;

		pCode += skip;
	}

	fwrite(pCodeOrig, chunks[chunkIndex].code_len + 9, 1, output_file);

	free(pCodeOrig);
}


void SaveFileP56(const char *name)
{
	FILE *output_file;
	int i;

	if (num_chunks == 0)
	{
		yywarning("Can't write null P56 file. ");
		return;
	}

	output_file = fopen(name, "wb");

	if (output_file <= 0)
	{
		printf("error while opening file: %s for write.\n", name);
		return;
	}

	for (i = 0; i != num_chunks2; i++)
	{
		int j, num_zeros = 0;

		for (j = 0; j < chunks[i].code_len; j++)
		{
			if (chunks[i].code_ptr[j] == 0)
			{
				num_zeros++;
			}
		}

		if (num_zeros != chunks[i].code_len || g_write_zero_sections)
		{
			switch (chunks[i].mem_type)
			{
			case P_MEM:
			case X_MEM:
			case Y_MEM:
				P56_SaveData(output_file, i, chunks[i].mem_type, 0, 0);
				break;
			case L_MEM:
				P56_SaveData(output_file, i, X_MEM, 3, 3);
				P56_SaveData(output_file, i, Y_MEM, 0, 3);
				break;
			}
		}

	}

	fclose(output_file);
}
