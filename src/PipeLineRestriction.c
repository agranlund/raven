/*

   Project:    asm56k
   Author:     M.Buras (sqward)

   Early pipeline restriction checks

 */

#include <string.h>
#include <asm_types.h>
#include <export.h>
#include <CodeUtils.h>
#include <ConvertFields.h>
#include "PipeLineRestriction.h"

#define STORE_ADDR_PIPLINE_REGS 8
uint g_prevInstrRegs[STORE_ADDR_PIPLINE_REGS];	/* store 16  registers */
uint g_prevInstrRegsNum = 0;
uint g_currInstrRegs[STORE_ADDR_PIPLINE_REGS];	/* store 16  registers */
uint g_currInstrRegsNum = 0;


void PipeLineReset(void)
{
	g_currInstrRegsNum = 0;
	g_prevInstrRegsNum = 0;
}


void PipeLineNewInst(void)
{
	memcpy(g_prevInstrRegs, g_currInstrRegs, sizeof(g_currInstrRegs));
	g_prevInstrRegsNum = g_currInstrRegsNum;
	g_currInstrRegsNum = 0;
}


void PipeLineNewSrcAguReg(uint reg)
{
	uint i;

	if (isAguReg(reg))
	{

		for (i = 0; i < g_prevInstrRegsNum; i++)
		{
			if (reg == g_prevInstrRegs[i])
			{
				yywarning("Pipeline restriction violation: register %s used as target in previous instruction.", getRegName(reg));
			}
		}
	}
}


void PipeLineNewDstAguReg(uint reg)
{
	if (isAguReg(reg))
	{
		g_currInstrRegs[g_currInstrRegsNum] = reg;

		if (g_currInstrRegsNum < STORE_ADDR_PIPLINE_REGS)	/* this should never happen but just in case.. */
		{
			g_currInstrRegsNum++;
		}
	}
}


void PipeLineNewSrcEA(bcode *reg)
{
	uint code = reg->w0 & 0xf8;

	if (code == 0 || code == (0x1 << 3) || code == 0x28)
	{
		PipeLineNewSrcAguReg((reg->w0 & 0x7) + 12);	/* Rx is in use */
		PipeLineNewSrcAguReg((reg->w0 & 0x7) + 20);	/* Nx is in use */
	} else if (code != 0x30)
	{
		PipeLineNewSrcAguReg((reg->w0 & 0x7) + 12);	/* Rx is in use */
	}
}


void PipeLineNewDstEA(bcode *reg)
{
	PipeLineNewSrcEA(reg);
#if 0
	uint code = reg->w0 & 0xf8;

	if (code == 0 || code == (0x1 << 3) || code == 0x28)
	{
		PipeLineNewDstAguReg((reg->w0 & 0x7) + 12);     /* Rx is in use */
	    PipeLineNewDstAguReg((reg->w0 & 0x7) + 20);     /* Nx is in use */
	} else if (code != 0x30)
	{
	    PipeLineNewDstAguReg((reg->w0 & 0x7) + 12);     /* Rx is in use */
	}
#endif
}
