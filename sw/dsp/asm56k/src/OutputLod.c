/*

Project:    asm56k
Author:     M.Buras (sqward)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm_types.h>
#include <export.h>
#include <Value.h>
#include "SymbolTable.h"
#include "CodeUtils.h"
#include "OutputLod.h"

#define L_EQU (L_MEM + 1)

/*
 * Generate output code in LOD format
 */


static int symbol_value(const hs *sym)
{
	if (sym->type == T_VALUE && (sym->m_val.m_type == kFract || sym->m_val.m_type == kFloat))
		return Val_CastToInt(sym->m_val).m_value.m_int;
	return Val_GetAsInt(sym->m_val);
}


static int symbol_cmp(const void *_s1, const void *_s2)
{
	const hs *const *s1 = (const hs *const *)_s1;
	const hs *const *s2 = (const hs *const *)_s2;
	int val1 = symbol_value(*s1);
	int val2 = symbol_value(*s2);
	return val1 - val2;
}


static void LOD_OutputSymbols(FILE *output_file, int memspace, char type)
{
	size_t i;
	size_t count;
	hs **symbols;
	size_t len, maxlen;
	
	count = 0;
	maxlen = 0;
	for (i = 0; i < HASH_SIZE; i++)
	{
		hs *pSymbol = hash_tab[i].pHead;

		while (pSymbol != NULL)
		{
			if ((pSymbol->mem_space == memspace && (pSymbol->type == T_PTR || memspace == L_MEM)) ||
				(memspace == L_EQU && pSymbol->type == T_VALUE))
			{
				count++;
				len = strlen(pSymbol->pString);
				if (len > maxlen)
					maxlen = len;
			}
			pSymbol = pSymbol->pNext;
		}
	}

	if (count == 0)
		return;
	
	symbols = (hs **)malloc(count * sizeof(*symbols));
	MTEST(symbols);
	count = 0;
	for (i = 0; i < HASH_SIZE; i++)
	{
		hs *pSymbol = hash_tab[i].pHead;

		while (pSymbol != NULL)
		{
			if ((pSymbol->mem_space == memspace && (pSymbol->type == T_PTR || memspace == L_MEM)) ||
				(memspace == L_EQU && pSymbol->type == T_VALUE))
			{
				symbols[count++] = pSymbol;
			}
			pSymbol = pSymbol->pNext;
		}
	}
	qsort(symbols, count, sizeof(*symbols), symbol_cmp);
	fprintf(output_file, "_SYMBOL %c\r\n", type);
	for (i = 0; i < count; i++)
	{
		hs *pSymbol = symbols[i];
		fprintf(output_file, "%-*s  I %.6X\r\n", (int)maxlen, pSymbol->pString, symbol_value(pSymbol));
	}
	free(symbols);
}


/*
 * offset and skip are used for L memory only
 */
static void LOD_SaveData(FILE *output_file, int chunkIndex, const char *MemType, int offset, int skip)
{
	int j, code_word;
	int mod_cnt;
	unsigned char *code;

	if (g_dsp_cpu >= 56301)
		fprintf(output_file, "_DATA %s %.6X\r\n", MemType, chunks[chunkIndex].pc);
	else
		fprintf(output_file, "_DATA %s %.4X\r\n", MemType, chunks[chunkIndex].pc);

	mod_cnt = 0;
	code = chunks[chunkIndex].code_ptr + offset;

	debugprint("codelen %d, %d\r\n", chunks[chunkIndex].code_len / 3, chunks[chunkIndex].code_len);

	j = (chunks[chunkIndex].code_len / 3) >> (skip ? 1 : 0);

	for (; j != 0; j--)
	{
		code_word = *code++;
		code_word <<= 8;
		code_word |= *code++;
		code_word <<= 8;
		code_word |= *code++;

		code += skip;

		if (mod_cnt == 7 || j == 1)
			fprintf(output_file, "%.6X\r\n", code_word);
		else
			fprintf(output_file, "%.6X ", code_word);

		mod_cnt++;
		mod_cnt &= 0x7;
	}
}


void SaveFileLod(const char *name, const char *iname)
{
	FILE *output_file;
	int i;

	output_file = fopen(name, "wb");
	if (output_file <= 0)
	{
		printf("error while opening file: %s for write.\n", name);
		return;
	}

	/* write out LOD header: */
	fprintf(output_file, "_START %s 0000 0000 0000 asm56k v0.1a\r\n\r\n", iname);

	if (num_chunks == 0)
	{
		return;
	}

	for (i = 0; i != num_chunks2; i++)
	{
		int j;
		int num_zeros = 0;

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
				LOD_SaveData(output_file, i, "P", 0, 0);
				break;
			case X_MEM:
				LOD_SaveData(output_file, i, "X", 0, 0);
				break;
			case Y_MEM:
				LOD_SaveData(output_file, i, "Y", 0, 0);
				break;
			case L_MEM:
				LOD_SaveData(output_file, i, "X", 3, 3);
				LOD_SaveData(output_file, i, "Y", 0, 3);
				break;
			}
		}
	}

	if (g_output_symbols)
	{
		LOD_OutputSymbols(output_file, P_MEM, 'P');
		LOD_OutputSymbols(output_file, X_MEM, 'X');
		LOD_OutputSymbols(output_file, Y_MEM, 'Y');
		LOD_OutputSymbols(output_file, L_MEM, 'L');
		LOD_OutputSymbols(output_file, L_EQU, 'N');
	}

	fprintf(output_file, "_END 0000\r\n");

	fclose(output_file);
}
