/*

Project:    asm56k
Author:     M.Buras (sqward)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm_types.h>
#include "ErrorMessages.h"
#include <export.h>
#include "CodeUtils.h"


chunk chunks[1024];
int num_chunks;							/* pass 1 */
int num_chunks2;						/* pass 2 */

int pc;
int mem_space;
int in_section;
static unsigned char *c_ptr;


void allocate_chunk(int type)
{
	unsigned char *pNewChunkMem;

	if (in_section)
	{
		chunks[num_chunks2].code_len2 = (c_ptr - chunks[num_chunks2].code_ptr);
		debugprint("d2 %p, 0x%x, 0x%x\n", c_ptr, chunks[num_chunks2].code_len2, chunks[num_chunks2].code_len);

		if (chunks[num_chunks2].code_len2 == 0)
		{
			if (g_passNum != 0)
			{
				/* yywarning("Empty section defined."); */
			}
		}
		num_chunks2++;
	}

	pNewChunkMem = (unsigned char *) malloc(chunks[num_chunks2].code_len);
	MTEST(pNewChunkMem);
	c_ptr = chunks[num_chunks2].code_ptr = pNewChunkMem;
	chunks[num_chunks2].mem_type = type;
	chunks[num_chunks2].pc = pc;
	chunks[num_chunks2].hasdata = FALSE;
	in_section = TRUE;
}


static int GetCurrentChunkIndex(void)
{
	if (g_passNum != 0)
		return num_chunks2;
	return num_chunks;
}


int GetCurrentMemType(void)
{
	return chunks[GetCurrentChunkIndex()].mem_type;
}


void CheckCodeInLMem(void)
{
	if (GetCurrentMemType() == L_MEM)
		yyerror("No code in L: memory allowed.");
}


int GetCurrentPC(void)
{
	return pc;
}


int GetCurrentChunkBegin(void)
{
	return chunks[GetCurrentChunkIndex()].pc;
}


void close_chunk(void)
{
	if (in_section)
	{
		chunks[num_chunks2].code_len2 = (c_ptr - chunks[num_chunks2].code_ptr);
		debugprint("d2 %p, 0x%x, 0x%x\n", c_ptr, chunks[num_chunks2].code_len2, chunks[num_chunks2].code_len);
		if (chunks[num_chunks2].code_len2 == 0)
		{
			/* yywarning("Empty section defined."); */
		}
		num_chunks2++;
	}
}


void allocate_vchunk(int type)
{
	mem_space = type;

	if (in_section)
	{
		chunks[num_chunks].code_len = (long) c_ptr;
		num_chunks++;
	}
	c_ptr = 0;

	chunks[num_chunks].mem_type = type;
	chunks[num_chunks].pc = pc;
	chunks[num_chunks].hasdata = FALSE;

	in_section = TRUE;
}


void close_vchunk(void)
{
	if (in_section)
	{
		chunks[num_chunks].code_len = (long) c_ptr;
		num_chunks++;
	}
}


void GenOrg(uint memSpace, uint address)
{
	if (g_passNum == 0)
	{
		pc = address;
		allocate_vchunk(memSpace);
	} else
	{
		pc = address;
		allocate_chunk(memSpace);
	}
}


void GenDc(Value data)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 0;
		if (GetCurrentMemType() == L_MEM)
			inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		if (GetCurrentMemType() == L_MEM)
		{
			u64 raw_val = Val_GetAsFract48(data);

			inst_code.sflag = 1;
			inst_code.w0 = raw_val & 0xffffff;
			inst_code.w1 = raw_val >> 24;
		} else
		{
			u32 raw_val = Val_GetAsFract24(data);

			inst_code.w0 = raw_val;
		}
		insert_code_w(&inst_code);
	}
}


void GenDS(Value val1)
{
	int lmem = 1;
	int size;
	int val;

	if (Val_CheckResolved(val1))
	{
		yyerror("Forward referenced symbols not allowed for DS statement.");
		return;
	}

	val = Val_GetAsInt(val1);

	if (GetCurrentMemType() == L_MEM)
	{
		lmem = 2;
	}

	size = val * lmem * 3;

	pc += val * lmem;

	if (g_passNum != 0)
	{
		int i;

		for (i = 0; i < size; i++)
		{
			*c_ptr++ = 0;
		}
	} else
	{
		c_ptr += size;
	}
}


int GenAlign(Value val1)
{
	int old_pc = pc;
	int val;

	if (Val_CheckResolved(val1))
	{
		yyerror("Forward referenced symbols not allowed for ALIGN statement.");
		return 0;
	}

	val = Val_GetAsInt(val1);

	if (GetCurrentMemType() == L_MEM)
		val <<= 1;

	for (;;)
	{
		if (pc % val == 0)
		{
			break;
		}

		if (g_passNum != 0)
		{
			*c_ptr++ = 0;
			*c_ptr++ = 0;
			*c_ptr++ = 0;
		} else
		{
			c_ptr += 3;
		}
		pc++;
	}

	return pc - old_pc;
}


void GenDSM(hs *pLabel, Value val1)
{
	uint wasted;
	u32 align_mask, align_val;
	uint val;

	if (Val_CheckResolved(val1))
	{
		yyerror("Forward referenced symbols not allowed for DSM statement.");
		return;
	}

	val = Val_GetAsInt(val1);

	if (val == 0)
	{
		yyerror("Zero size DSM buffer not allowed.");
		return;
	}
	if (val >= 0x10000)
	{
		yyerror(ERROR_9);
		return;
	}

	val--;
	align_mask = 0;
	align_val = 1;
	for (;;)
	{
		if ((val & align_mask) == val)
		{
			break;
		}
		align_val <<= 1;
		align_mask <<= 1;
		align_mask |= 1;
	}

	if ((pc & ~align_mask) != pc)
	{
		wasted = GenAlign(Val_CreateInt(align_val));

		if (pLabel)
		{
			SymSetValue(pLabel, T_PTR, Val_CreateInt(pc));	/* reset label address value */
		}

		if (wasted != 0 && g_passNum != 0)
		{
			yywarning("DSM statement wasted %u words of memory.", wasted);
		}
	}

	GenDS(Val_CreateInt(val + 1));
}


void verify_code(void)
{
	int a;

	if (num_chunks != num_chunks2)
	{
		yyerror("Internal error #1.");
	} else
	{
		for (a = 0; a < num_chunks; a++)
		{
			if (chunks[a].code_len2 != chunks[a].code_len)
			{
				yyerror("Internal error #2: mem: %d, org: 0x%x, skew: %d", chunks[a].mem_type, chunks[a].pc,
						(chunks[a].code_len2 - chunks[a].code_len) / 3);
			}
		}
	}
}


void insert_code_w(bcode *inst_code)
{
	*c_ptr++ = (unsigned char) (inst_code->w0 >> 16);
	*c_ptr++ = (unsigned char) (inst_code->w0 >> 8);
	*c_ptr++ = (unsigned char) (inst_code->w0);
	pc++;

	if (inst_code->sflag)
	{

		*c_ptr++ = (unsigned char) (inst_code->w1 >> 16);
		*c_ptr++ = (unsigned char) (inst_code->w1 >> 8);
		*c_ptr++ = (unsigned char) (inst_code->w1);
		pc++;
	}

	/* This is required for empty segment detection */
	chunks[GetCurrentChunkIndex()].hasdata = TRUE;
}


void InsertString(const char *pString, int str_len)
{
	int wordLength;

	str_len++;							/* take the termination character into account */

	wordLength = (str_len / 3) + ((str_len % 3) > 0 ? 1 : 0);

	pc += wordLength;

	if (g_passNum != 0)
	{
		int i = (wordLength * 3) % str_len;

		strcpy((char *) c_ptr, pString);

		c_ptr += str_len;

		for (; i != 0; i--)
		{
			*c_ptr++ = 0;				/* pad out the remaining bytes */
		}
	} else
	{
		c_ptr += wordLength * 3;
	}

	/* This is required for empty segment detection */
	chunks[GetCurrentChunkIndex()].hasdata = TRUE;
}


void insert_vcode_w(const bcode *inst_code)
{
	if (in_section)
	{
		if (inst_code->sflag == 0)
		{
			c_ptr += 3;
			pc++;
		} else
		{
			c_ptr += 6;
			pc += 2;
		}
	} else
	{
		yyerror("No output memory section defined.");
		asm_abort();
	}

	/* This is required for empty segment detection */
	chunks[GetCurrentChunkIndex()].hasdata = TRUE;
}


void retInit(raddr *ret)
{
	ret->sflag = 0;
	ret->abs_value = 0;
	ret->type = 0;
	ret->value = 0;
}
