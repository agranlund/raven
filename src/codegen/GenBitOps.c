/*

Project:    asm56k
Author:     M.Buras (sqward)


*/
#include <stdio.h>
#include <ConvertFields.h>
#include <export.h>
#include <ErrorMessages.h>
#include "GenBitOps.h"
#include "GenMisc.h"

int const bchg_patterns[] = {
	0xb0000,
	0xb8000,
	0x14000,
	0xb8000,
	0xb4000,
	0xbc040
};

int const bclr_patterns[] = {
	0xa0000,
	0xa8000,
	0x10000,
	0xa8000,
	0xa4000,
	0xac040
};

int const bset_patterns[] = {
	0xa0020,
	0xa8020,
	0x10020,
	0xa8020,
	0xa4020,
	0xac060
};

int const btst_patterns[] = {
	0xb0020,
	0xb8020,
	0x14020,
	0xb8020,
	0xb4060,
	0xbc060
};

int const brclr_patterns[] = {
	0xc8080,
	0xcc000,
	0x48000,
	0xcc000,
	0xc8000,
	0xcc080
};

int const brset_patterns[] = {
	0xc80a0,
	0xcc020,
	0x48020,
	0xcc020,
	0xc8020,
	0xcc0a0
};

int const bsclr_patterns[] = {
	0xd8080,
	0xdc000,
	0x48080,
	0xdc000,
	0xd8000,
	0xdc080
};

int const bsset_patterns[] = {
	0xd80a0,
	0xdc020,
	0x480a0,
	0xdc020,
	0xd8020,
	0xdc0a0
};

int const bra_patterns[] = {
	0xb10c0,
	0x50c00,
	0xd18c0,
};


int const extract_patterns[] = {
	0xc1a00,
	0xc1800,
	0xc1a80,
	0xc1880,
	0xc1b00,
	0xc1900
};


void GenBitOp(const int *insn_patt, int val, int xory, bcode *ea)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = ea->sflag;
		if (inst_code.sflag == 2)
		{
			inst_code.sflag = 0;
		}
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		if (val > 23)
		{
			yyerror("In operands filed: Immediate value to big: <0-23> range allowed.");
			val = 0;
		}
		if (ea->sflag == 2)
		{
			if (ea->w1 < 64)
			{
				inst_code.w0 = insn_patt[0] | (ea->w1 << 8) | (val) | (xory << 6);
			} else
			{
				ea->w1 |= 0xff0000;
				if (ea->w1 >= 0xffffc0 && ea->w1 <= 0xffffff)
				{
					inst_code.w0 = insn_patt[1] | ((ea->w1 - 0xffffc0) << 8) | (val) | (xory << 6);
				} else
				{
					if (ea->w1 >= 0xffff80 && ea->w1 <= 0xffffbf)
					{
						inst_code.w0 = insn_patt[2] | ((ea->w1 - 0xffff80) << 8) | (val) | (xory << 6);
					} else
					{
						yyerror("Destination address out of range.");
						inst_code.w0 = insn_patt[3] | (val) | (xory << 6);
					}
				}
			}
		} else
		{
			inst_code.sflag = ea->sflag;
			inst_code.w0 = insn_patt[4] | (val) | (xory << 6) | (ea->w0 << 8);
			inst_code.w1 = ea->w1;
		}
		insert_code_w(&inst_code);
	}
}


void GenJccBitRel(const int *insn_patt, int val, int xory, bcode *ea, raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;

		if (val > 23)
		{
			yyerror("In operands filed: Immediate value to big: [0-23] range allowed.");
			val = 0;
		}

		if (rel_target->type != T_LONG)
		{
			yyerror("In operands field: Only long branches accepted.");
			rel_target->value = 0;
		}

		if (ea->sflag == 1)
		{
			yyerror("In operands field: Illegal effective address specified.");
			ea->w0 = 0;
			ea->sflag = 0;
		}

		inst_code.w1 = rel_target->value;

		if (ea->sflag == 2)
		{
			if (ea->w1 < 64)
			{
				inst_code.w0 = insn_patt[0] | (ea->w1 << 8) | (val) | (xory << 6);
			} else
			{
				if (ea->w1 >= 0xffffc0 && ea->w1 <= 0xffffff)
				{
					inst_code.w0 = insn_patt[1] | ((ea->w1 - 0xffffc0) << 8) | (val) | (xory << 6);
				} else
				{
					if (ea->w1 >= 0xffff80 && ea->w1 <= 0xffffbf)
					{
						inst_code.w0 = insn_patt[2] | ((ea->w1 - 0xffff80) << 8) | (val) | (xory << 6);
					} else
					{
						yyerror("Destenation address out of range.");
						inst_code.w0 = insn_patt[3] | (val) | (xory << 6);
					}
				}
			}
		} else
		{
			inst_code.w0 = insn_patt[4] | (val) | (xory << 6) | (ea->w0 << 8);
		}
		inst_code.sflag = 1;
		insert_code_w(&inst_code);
	}
}


void GenInsExt1(uint insn_patt, uint reg1, uint src_reg, uint dest_reg)
{
	DSP56301;
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 0;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		reg1 = ddddd_2_sss(reg1);
		if (reg1 == -1)
		{
			yyerror("Illegal source register: X0-1,Y0-1,A1,B1 are allowed.");
			reg1 = 0;
		}
		inst_code.w0 = 0xc1a00 | (ddddd_2_d_src(src_reg) << 4) | (reg1 << 1) | ddddd_2_d_dst(dest_reg);
		insert_code_w(&inst_code);
	}
}


void GenInsExt2(uint insn_patt, uint val, uint src_reg, uint dest_reg)
{
	DSP56301;
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		inst_code.w0 = insn_patt | (ddddd_2_d_src(src_reg) << 4) | ddddd_2_d_dst(dest_reg);
		inst_code.w1 = val;
		inst_code.sflag = 1;
		insert_code_w(&inst_code);
	}
}
