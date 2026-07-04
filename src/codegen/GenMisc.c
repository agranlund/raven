/*

Project:    asm56k
Author:     M.Buras (sqward)


*/
#include <stdio.h>
#include <stdlib.h>
#include <export.h>
#include <ConvertFields.h>
#include <ErrorMessages.h>
#include "GenMisc.h"
#include "GenBitOps.h"
#include <CodeUtils.h>
#include <PipeLineRestriction.h>
#include <SymbolTable.h>

int const jclr_patterns[] = {
	0xa0080,
	0xa8080,
	0x18080,
	0xa8080,
	0xa4080,
	0xac000
};

int const jsclr_patterns[] = {
	0xb0080,
	0xb8080,
	0x1c080,
	0xb8080,
	0xb4080,
	0xbc000
};

int const jset_patterns[] = {
	0xa00a0,
	0xa80a0,
	0x180a0,
	0xa80a0,
	0xa40a0,
	0xac020
};

int const jsset_patterns[] = {
	0xb00a0,
	0xb80a0,
	0x1c0a0,
	0xb80a0,
	0xa40a0,
	0xbc020
};


uint const jmp_pattern[] = {
	0xc0000,
	0xac080
};

uint const jcc_pattern[] = {
	0xe0000,
	0xac0a0
};

uint const jscc_pattern[] = {
	0xf0000,
	0xbc0a0
};

uint const jsr_pattern[] = {
	0xd0000,
	0xbc080
};


uint const movec_pattern1[] = {
	0x58020,
	0x5c020
};

uint const movec_pattern2[] = {
	0x50020,
	0x54020
};


uint const movem_pattern1[] = {
	0x70000,
	0x74080
};

uint const movem_pattern2[] = {
	0x78000,
	0x7c080
};


void GenOneParamParMove(uint insn_patt, uint reg, bcode *par_move)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = par_move->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		reg = ddddd_2_d_dst(reg);
		inst_code.sflag = par_move->sflag;
		inst_code.w0 = insn_patt | (reg << 3) | par_move->w0;
		inst_code.w1 = par_move->w1;
		insert_code_w(&inst_code);
	}
}


void GenBccRelTarger(uint insn_patt, raddr *rel_target)
{
	DSP56301;
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = rel_target->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		switch (rel_target->type)
		{
		case T_LONG:
			inst_code.sflag = 1;
			inst_code.w0 = 0xd0104 | insn_patt;
			inst_code.w1 = rel_target->value;
			break;
		case T_SHORT:
			inst_code.w0 = 0x50400 | ((rel_target->value & 0x1e0) << 6) | (rel_target->value & 0x1f) | (insn_patt << 11);
			break;
		case T_REGISTER:
			inst_code.w0 = 0xd1840 | (rel_target->value << 8) | insn_patt;
			break;
		}
		insert_code_w(&inst_code);
	}
}


void GenBitOpReg(uint insn_patt, int val, int dest_reg)
{
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
		if (val > 23)
		{
			yyerror("In operands filed: Immediate value to big: [0-23] range allowed.");
			val = 0;
		}
		dest_reg = ddddd_2_DDDDDD(dest_reg);
		if (dest_reg == -1)
		{
			yyerror("In operands field: Illegal destination register specified.");
			dest_reg = 4;
		}
		inst_code.w0 = insn_patt | (val) | (dest_reg << 8);
		insert_code_w(&inst_code);
	}
}


void GenBraRelTarger(raddr *rel_target)
{
	DSP56301;
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = rel_target->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		switch (rel_target->type)
		{
		case T_LONG:
			inst_code.sflag = 1;
			inst_code.w0 = 0xb10c0;
			inst_code.w1 = rel_target->value;
			break;
		case T_SHORT:
			inst_code.sflag = 0;
			inst_code.w0 = 0x50c00 | ((rel_target->value & 0x1e0) << 6) | (rel_target->value & 0x1f);
			break;
		case T_REGISTER:
			inst_code.sflag = 0;
			inst_code.w0 = 0xd18c0 | (rel_target->value << 8);
			break;
		}
		insert_code_w(&inst_code);
	}
}


void GenBrkCC(int condition)
{
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
		inst_code.w0 = 0x210 | condition;
		insert_code_w(&inst_code);
	}
}


void GenBscc(uint condition, raddr *rel_target)
{
	DSP56301;
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = rel_target->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		switch (rel_target->type)
		{
		case T_LONG:
			inst_code.sflag = 1;
			inst_code.w0 = 0xb1000 | condition;
			inst_code.w1 = rel_target->value;
			break;
		case T_SHORT:
			inst_code.sflag = 0;
			inst_code.w0 = 0x50000 | ((rel_target->value & 0x1e0) << 6) | (rel_target->value & 0x1f) | (condition << 12);
			break;
		case T_REGISTER:
			inst_code.sflag = 0;
			inst_code.w0 = 0xd1800 | (rel_target->value << 8) | condition;
			break;
		}
		insert_code_w(&inst_code);
	}
}


void GenBsr(raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		DSP56301;
		inst_code.sflag = rel_target->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		switch (rel_target->type)
		{
		case T_LONG:
			inst_code.sflag = 1;
			inst_code.w0 = 0xd1080;
			inst_code.w1 = rel_target->value;
			break;
		case T_SHORT:
			inst_code.sflag = 0;
			inst_code.w0 = 0x50800 | ((rel_target->value & 0x1e0) << 6) | (rel_target->value & 0x1f);
			break;
		case T_REGISTER:
			inst_code.sflag = 0;
			inst_code.w0 = 0xd1880 | (rel_target->value << 8);
			break;
		}
		insert_code_w(&inst_code);
	}
}


void GenClb(uint src_reg, uint dest_reg)
{
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
		inst_code.w0 = 0xc1e00 | (ddddd_2_d_src(src_reg) << 1) | ddddd_2_d_dst(dest_reg);
		insert_code_w(&inst_code);
	}
}


void GenDebug(uint inst_patt, uint condition)
{
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
		inst_code.w0 = inst_patt | condition;
		insert_code_w(&inst_code);
	}
}


void GenDo1(uint xory, bcode *ea, raddr *rel_target)
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

		if (ea->sflag == 1)
		{
			yyerror("In operands field: Illegal addressing mode (Try to  force short).");
			ea->sflag = 0;
			ea->w0 = 0;
		}
		if (rel_target->type == T_REGISTER)
		{
			yyerror("In operands field: Only absolute address is valid.");
			rel_target->type = T_LONG;
			rel_target->abs_value = 0;
		}
		if (ea->sflag == 2)
		{
			if (ea->w1 != (ea->w1 & 0x3f))
			{
				yyerror("In operands field: Short addressing mode out of range.");
				ea->w1 &= 0x3f;
			}
			inst_code.w0 = 0x60000 | (ea->w1 << 8) | (xory << 6);
		} else
		{
			inst_code.w0 = 0x64000 | (ea->w0 << 8) | (xory << 6);
		}
		inst_code.sflag = 1;
		inst_code.w1 = rel_target->abs_value - 1;
		insert_code_w(&inst_code);
	}
}


void GenDo2(int val, raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		if (rel_target->type == T_REGISTER)
		{
			yyerror("In operands field: Only absolute address is valid.");
			rel_target->type = T_LONG;
			rel_target->abs_value = 0;
		}
		if (val != (val & 0xfff))
		{
			yyerror("In operands field: Immediate value out of range");
			val &= 0xfff;
		}
		inst_code.sflag = 1;
		inst_code.w0 = 0x60080 | ((val & 0xff) << 8) | ((val >> 8) & 0xf);
		inst_code.w1 = rel_target->abs_value - 1;
		insert_code_w(&inst_code);
	}
}


void GenDo3(uint src_reg, raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		if (rel_target->type == T_REGISTER)
		{
			yyerror("In operands field: Only absolute address is valid.");
			rel_target->type = T_LONG;
			rel_target->abs_value = 0;
		}
		src_reg = ddddd_2_DDDDDD(src_reg);
		if (src_reg == -1)
		{
			yyerror("In operands field: Illegal register");
			src_reg = 0;
		}
		inst_code.sflag = 1;
		inst_code.w0 = 0x6c000 | (src_reg << 8);
		inst_code.w1 = rel_target->abs_value - 1;
		insert_code_w(&inst_code);
	}
}


void GenDoForever(raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		if (rel_target->type == T_REGISTER)
		{
			yyerror("In operands field: Only absolute address is valid.");
			rel_target->type = T_LONG;
			rel_target->abs_value = 0;
		}
		inst_code.sflag = 1;
		inst_code.w0 = 0x00203;
		inst_code.w1 = rel_target->abs_value - 1;
		insert_code_w(&inst_code);
	}
}


void GenDor1(uint xory, bcode *ea, raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		if (ea->sflag == 1)
		{
			yyerror("In operands field: Illegal addressing mode (Try to force short).");
			ea->sflag = 0;
			ea->w0 = 0;
		}
		if (rel_target->type != T_LONG)
		{
			yyerror("In operands field: Illegal destination address.");
			rel_target->type = T_LONG;
			rel_target->value = 0;
		}

		if (ea->sflag == 2)
		{
			if (ea->w1 <= 0x3f)
			{
				yyerror("In operands field: Short addressing mode out  of range.");
				ea->w1 &= 0x3f;
			}
			inst_code.w0 = 0x60010 | (ea->w1 << 8) | (xory << 6);
		} else
		{
			inst_code.w0 = 0x64010 | (ea->w0 << 8) | (xory << 6);
		}
		inst_code.sflag = 1;
		inst_code.w1 = rel_target->value - 1;
		insert_code_w(&inst_code);
	}
}


void GenDor2(int val, raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		if (rel_target->type != T_LONG)
		{
			yyerror("In operands field: Illegal destination address.");
			rel_target->type = T_LONG;
			rel_target->value = 0;
		}
		if (val != (val & 0xfff))
		{
			yyerror("In operands field: Immediate value out of range");
			val &= 0xfff;
		}
		inst_code.sflag = 1;
		inst_code.w0 = 0x60090 | ((val & 0xff) << 8) | ((val >> 8) & 0xf);
		inst_code.w1 = rel_target->value - 1;
		insert_code_w(&inst_code);
	}
}


void GenDor3(uint src_reg, raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		if (rel_target->type != T_LONG)
		{
			yyerror("In operands field: Illegal destination address.");
			rel_target->type = T_LONG;
			rel_target->value = 0;
		}
		src_reg = ddddd_2_DDDDDD(src_reg);
		if (src_reg == -1)
		{
			yyerror("In operands field: Illegal register");
			src_reg = 0;
		}
		inst_code.sflag = 1;
		inst_code.w0 = 0x6c010 | (src_reg << 8);
		inst_code.w1 = rel_target->value - 1;
		insert_code_w(&inst_code);
	}
}


void GenDorForever(raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		if (rel_target->type != T_LONG)
		{
			yyerror("In operands field: Illegal destination address.");
			rel_target->type = T_LONG;
			rel_target->value = 0;
		}
		inst_code.sflag = 1;
		inst_code.w0 = 0x00202;
		inst_code.w1 = rel_target->value - 1;
		insert_code_w(&inst_code);
	}
}


void GenEnddo(void)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 0;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0x8c;
		inst_code.w1 = 0;
		insert_code_w(&inst_code);
	}
}


void GenIllegal(void)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 0;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0x5;
		inst_code.w1 = 0;
		insert_code_w(&inst_code);
	}
}


void GenJmpJsrJsccJcc(const uint *insn_patt, uint condition, bcode *ea)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = ea->sflag;
		if (inst_code.sflag == 2)
			inst_code.sflag = 0;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		if (ea->sflag == 2)
		{
			if (ea->w1 > 4095)
			{
				yyerror(ERROR_5);
				ea->w1 &= 0xfff;
			}
			inst_code.sflag = 0;
			inst_code.w0 = insn_patt[0] | (condition << 12) | ea->w1;
			inst_code.w1 = 0;
		} else
		{
			inst_code.sflag = ea->sflag;
			inst_code.w0 = insn_patt[1] | (ea->w0 << 8) | condition;
			inst_code.w1 = ea->w1;
		}
		insert_code_w(&inst_code);
	}
}


void GenJccBitRelReg(int insn_patt, int val, int dest_reg, raddr *rel_target)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

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
		dest_reg = ddddd_2_DDDDDD(dest_reg);
		if (dest_reg == -1)
		{
			yyerror("In operands field: Illegal destination register specified.");
			dest_reg = 4;
		}
		inst_code.sflag = 1;
		inst_code.w0 = insn_patt | (val) | (dest_reg << 8);
		inst_code.w1 = rel_target->value;
		insert_code_w(&inst_code);
	}
}


/*
0xa0080,
0xa8080,
0x18080,
0xa8080,
0xa4080,
0xac000
*/
void GenJccBitAbs(const int *insn_patt, int val, int xory, bcode *ea, raddr *rel_target)
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
			yyerror(ERROR_6);
			val = 0;
		}

		if (ea->sflag == 1)
		{
			yyerror(ERROR_4);
			ea->w0 = 0;
			ea->sflag = 0;
		}

		inst_code.w1 = rel_target->abs_value;

		if (ea->sflag == 2)
		{
			if (ea->w1 < 64)
			{
				inst_code.w0 = insn_patt[0] | (ea->w1 << 8) | (val) | (xory << 6);
			} else
			{
				ea->w1 |= 0xffff00;
				if (ea->w1 >= 0xffffc0 && ea->w1 <= 0xffffff)
				{
					inst_code.w0 = insn_patt[1] | ((ea->w1 - 0xffffc0) << 8) | (val) | (xory << 6);
				} else
				{
					if (ea->w1 >= 0xffff80 && ea->w1 <= 0xffffbf)
					{
						/* 56301 only! */
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
			inst_code.w0 = insn_patt[4] | (val) | (xory << 6) | (ea->w0 << 8);
		}
		inst_code.sflag = 1;
		insert_code_w(&inst_code);
	}
}


void GenJccBitAbsReg(int insn_patt, int val, uint dest_reg, raddr *rel_target)
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
			yyerror(ERROR_6);
			val = 0;
		}
		dest_reg = ddddd_2_DDDDDD(dest_reg);
		if (dest_reg == -1)
		{
			yyerror(ERROR_8);
			dest_reg = 4;
		}
		inst_code.sflag = 1;
		inst_code.w0 = insn_patt | (val) | (dest_reg << 8);
		inst_code.w1 = rel_target->abs_value;
		insert_code_w(&inst_code);
	}
}


void GenLra(raddr *rel_target, uint dest_reg)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 1;
		if (rel_target->type == T_REGISTER)
			inst_code.sflag = 0;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		dest_reg = ddddd_2_ddddd(dest_reg);
		if (dest_reg == -1)
		{
			yyerror(ERROR_8);
			dest_reg = 0;
		}
		switch (rel_target->type)
		{
		case T_LONG:
		case T_SHORT:
			inst_code.sflag = 1;
			inst_code.w0 = 0x44040 | dest_reg;
			inst_code.w1 = rel_target->value;
			break;
		case T_REGISTER:
			inst_code.sflag = 0;
			inst_code.w0 = 0x4c000 | (rel_target->value << 8) | dest_reg;
			break;
		default:
			yyerror("Unsupported addressing mode.");
			break;
		}
		insert_code_w(&inst_code);
	}
}


void GenLua1(uint mmrrr, uint dest_reg)
{
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
		dest_reg = ddddd_2_dddd(dest_reg);
		if (dest_reg == -1)
		{
			yyerror(ERROR_8);
			dest_reg = 0;
		}
		inst_code.w0 = 0x44010 | (mmrrr << 8) | dest_reg;
		insert_code_w(&inst_code);
	}
}


void GenLua2(uint rreg, int val, uint dest_reg)
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
		if (abs(val) > 63)
		{
			yyerror(ERROR_9);
			val = 0;
		}
		dest_reg = ddddd_2_dddd(dest_reg);
		if (dest_reg == -1)
		{
			yyerror(ERROR_8);
			dest_reg = 0;
		}
		inst_code.w0 = 0x40000 | ((val & 0x70) << 7) | (rreg << 8) | ((val & 0xf) << 4) | dest_reg;
		insert_code_w(&inst_code);
	}
}


void GenMerge(uint src_reg, uint dest_reg)
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
		src_reg = ddddd_2_sss(src_reg);
		if (src_reg == -1)
		{
			yyerror(ERROR_10);
			src_reg = 0;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);
		inst_code.w0 = 0xc1b800 | (src_reg << 1) | dest_reg;
		insert_code_w(&inst_code);
	}
}


void GenMove(bcode *par_move)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = par_move->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		inst_code.sflag = par_move->sflag;
		inst_code.w0 = par_move->w0;
		inst_code.w1 = par_move->w1;
		insert_code_w(&inst_code);
	}
}


void GenMovec1(const uint *insn_patt, uint dir, uint xory, bcode *ea, uint dest_reg)
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
		PipeLineNewDstAguReg(dest_reg);

		dest_reg = ddddd_2_ddddd2(dest_reg);
		if (dest_reg == -1)
		{
			yyerror(dir ? ERROR_16 : ERROR_8);
			dest_reg = 0;
		}
		if (ea->sflag == 2)
		{
			if (ea->w1 != (ea->w1 & 0x3f))
			{
				yyerror(ERROR_9);
				ea->w1 &= 0x3f;
			}
			inst_code.sflag = 0;
			inst_code.w0 = insn_patt[0] | (ea->w1 << 8) | (xory << 6) | dest_reg;
		} else
		{
			inst_code.sflag = ea->sflag;
			inst_code.w0 = insn_patt[1] | (ea->w0 << 8) | (xory << 6) | dest_reg;
			inst_code.w1 = ea->w1;
		}
		insert_code_w(&inst_code);
	}
}


void GenMovec2(uint src_reg, uint dest_reg)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 0;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;
		int new_src, new_dest;
		
		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;

		PipeLineNewSrcAguReg(src_reg);
		PipeLineNewDstAguReg(dest_reg);

		new_src = ddddd_2_ddddd2(src_reg);
		new_dest = ddddd_2_eeeeee(dest_reg);

		if (new_src != -1 && new_dest != -1)
		{
			inst_code.w0 = 0x440a0 | (new_dest << 8) | new_src;
		} else
		{
			int new_src = ddddd_2_eeeeee(src_reg);
			int new_dest = ddddd_2_ddddd2(dest_reg);

			if (new_src != -1 && new_dest != -1)
			{
				inst_code.w0 = 0x4c0a0 | (new_src << 8) | new_dest;
			} else
			{
				yyerror("Illegal register combination.");
			}
		}
		insert_code_w(&inst_code);
	}
}


void GenMovec3(uint sh, int val, uint dest_reg)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = sh;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		PipeLineNewDstAguReg(dest_reg);

		dest_reg = ddddd_2_ddddd2(dest_reg);
		if (dest_reg == -1)
		{
			yyerror(ERROR_8);
			dest_reg = 0;
		}
		if (sh == 1)
		{
			inst_code.w0 = 0x5c020 | (0x34 << 8) | dest_reg;
			inst_code.w1 = val;
		} else
		{
			if (val != (val & 0xff))
			{
				yyerror(ERROR_9);
				val &= 0xff;
			}
			inst_code.w0 = 0x500a0 | (val << 8) | dest_reg;
		}
		inst_code.sflag = sh;
		insert_code_w(&inst_code);
	}
}


void GenMovem(const uint *insn_patt, uint dir, bcode *ea, uint reg)
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
		reg = ddddd_2_DDDDDD(reg);
		if (reg == -1)
		{
			yyerror(dir ? ERROR_16 : ERROR_8);
			reg = 0;
		}
		if (ea->sflag == 2)
		{
			if (ea->w1 != (ea->w1 & 0x3f))
			{
				yyerror(ERROR_9);
				ea->w1 = 0;
			}
			inst_code.w0 = insn_patt[0] | (ea->w1 << 8) | reg;
			inst_code.sflag = 0;
		} else
		{
			inst_code.w0 = insn_patt[1] | (ea->w0 << 8) | reg;
			inst_code.w1 = ea->w1;
			inst_code.sflag = ea->sflag;
		}
		insert_code_w(&inst_code);
	}
}


void GenMovep(uint src_xory, bcode *src_ea, uint dst_xory, bcode *dst_ea)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = 0;
		if (src_ea->sflag != dst_ea->sflag)
		{
			if (src_ea->sflag == 1 || dst_ea->sflag == 1)
			{
				inst_code.sflag = 1;
			}
		}
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		/* test which case */
		if (src_ea->sflag == dst_ea->sflag)
		{
			if (src_ea->sflag != 2 || dst_ea->sflag != 2 || (src_ea->sflag == 2 && dst_ea->sflag == 2))
			{
				yyerror("Invalid source/destination specified.");
			}
		} else
		{								/* movep short_addressing , ea */
			uint dir = 0;
			int new_src_addr;

			if (src_ea->sflag != 2)
			{
				uint tmp = src_xory;

				src_xory = dst_xory;
				dst_xory = tmp;
				{
					bcode *tmp_ea = src_ea;

					src_ea = dst_ea;
					dst_ea = tmp_ea;
					dir = 1;
				}
			}
			new_src_addr = src_ea->w1 | 0xffff00;
			if (new_src_addr >= 0xffffc0 && new_src_addr <= 0xffffff)
			{
				inst_code.w0 = 0x84080 | (src_xory << 16) | (dir << 15) | (dst_ea->w0 << 8) | (dst_xory << 6) | (new_src_addr - 0xffffc0);
			} else
			{
				if (new_src_addr >= 0xffff80 && new_src_addr <= 0xffffbf)
				{
					if (src_xory == 0)
					{					/* x mem */
						inst_code.w0 =
							0x74000 | (dir << 15) | (dst_ea->w0 << 8) | (dst_xory << 6) | (new_src_addr - 0xffff80);
					} else
					{					/* y mem */
						inst_code.w0 =
							0x70080 | (dir << 15) | (dst_ea->w0 << 8) | (dst_xory << 6) | (new_src_addr - 0xffff80);
					}
				} else
				{
					yyerror("Source address out of range.");
					inst_code.w0 = 0;
				}
			}
			inst_code.sflag = dst_ea->sflag;
			inst_code.w1 = dst_ea->w1;
		}
		insert_code_w(&inst_code);
	}
}


void GenMovep2(uint rw, bcode *src_ea, uint dst_xory, bcode *dst_ea)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = src_ea->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		if (dst_ea->sflag == 2)
		{
			dst_ea->w1 |= 0xffff00;
			if (dst_ea->w1 >= 0xffffc0 && dst_ea->w1 <= 0xffffff)
			{
				inst_code.w0 = 0x84040 | (dst_xory << 16) | (rw << 15) | (src_ea->w0 << 8) | (dst_ea->w1 - 0xffffc0);
			} else
			{
				if (dst_ea->w1 >= 0xffff80 && dst_ea->w1 <= 0xffffbf)
				{
					inst_code.w0 = 0x08000 | (rw << 14) | (src_ea->w0 << 8) | (dst_xory << 6) | (dst_ea->w1 - 0xffff80);
				} else
				{
					yyerror(ERROR_5);
					inst_code.w0 = 0;
				}
			}
		} else
		{
			yyerror(ERROR_4);
		}

		inst_code.sflag = src_ea->sflag;
		inst_code.w1 = src_ea->w1;
		insert_code_w(&inst_code);
	}
}


void GenMovep3(int val, uint dst_xory, bcode *dst_ea)
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
		inst_code.sflag = 1;
		if (dst_ea->sflag == 2)
		{
			dst_ea->w1 |= 0xffff00;

			if (dst_ea->w1 >= 0xffffc0 && dst_ea->w1 <= 0xffffff)
			{
				inst_code.w0 = 0x84080 | (1 << 15) | (0x34 << 8) | (dst_xory << 16) | (dst_ea->w1 - 0xffffc0);
			} else
			{
				if (dst_ea->w1 >= 0xffff80 && dst_ea->w1 <= 0xffffbf)
				{
					inst_code.w0 = 0x074000 | (1 << 15) | (0x34 << 8) | (dst_ea->w1 - 0xffff80);
					if (dst_xory == 1)
					{
						inst_code.w0 = inst_code.w0 ^ 0x4080;
					}
				} else
				{
					yyerror(ERROR_5);
					inst_code.w0 = 0;
				}
			}
		} else
		{
			yyerror(ERROR_5);
		}
		inst_code.w1 = val;

		insert_code_w(&inst_code);
	}
}


void GenMovep4(uint rw, uint xory, bcode *ea, uint reg)
{
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
		reg = ddddd_2_DDDDDD(reg);
		if (reg == -1)
		{
			yyerror(ERROR_8);
		} else
		{
			if (ea->sflag == 2)
			{
				ea->w1 |= 0xffff00;

				if (ea->w1 >= 0xffffc0 && ea->w1 <= 0xffffff)
				{
					inst_code.w0 = 0x84000 | (xory << 16) | (rw << 15) | (reg << 8) | (ea->w1 - 0xffffc0);
				} else
				{
					if (ea->w1 >= 0xffff80 && ea->w1 <= 0xffffbf)
					{
						inst_code.w0 = 0x044080 | (rw << 15) | (reg << 8) | (((ea->w1 - 0xffff80) & 0x20) << 1) | ((ea->w1 - 0xffff80) & 0x1f);
						if (xory == 1)
						{
							inst_code.w0 = inst_code.w0 ^ 0xa;
						}
					} else
					{
						if (!rw)
							yyerror("Source operand error.");
						else
							yyerror(ERROR_5);

						inst_code.w0 = 0;
					}
				}
			} else
			{
				yyerror(ERROR_5);
			}
		}
		insert_code_w(&inst_code);
	}
}


void GenNoArgOpcode(uint opcode)
{
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
		inst_code.w0 = opcode;
		insert_code_w(&inst_code);
	}
}



void GenPlockPunloc(uint opcode, bcode *ea)
{
	DSP56301;
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = ea->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		inst_code.w0 = opcode | (ea->w0) << 8;
		inst_code.w1 = ea->w1;
		inst_code.sflag = ea->sflag;
		insert_code_w(&inst_code);
	}
}


void GenPlockrPunlockr(uint opcode, raddr *rel_target)
{
	DSP56301;
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = rel_target->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		if (rel_target->type == T_REGISTER)
		{
			yyerror(ERROR_4);
		}

		inst_code.w0 = opcode;
		inst_code.w1 = rel_target->value;
		inst_code.sflag = rel_target->sflag;
		insert_code_w(&inst_code);
	}
}


void GenRep1(uint xory, bcode *ea)
{
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
		if (ea->w0 == 0x30)
		{
			if (ea->sflag != 2)
			{
				yyerror(ERROR_4);
				ea->w0 = 0;
			} else
			{
				if (ea->w1 <= 0x3f)
				{
					inst_code.w0 = 0x60020 | (ea->w1 << 8) | (xory << 6);
				} else
				{
					yyerror(ERROR_9);
					inst_code.w0 = 0;
				}
			}
		} else
		{
			inst_code.w0 = 0x64020 | (ea->w0 << 8) | (xory << 6);
		}
		insert_code_w(&inst_code);
	}
}


void GenRep2(int exp)
{
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
		if (exp > 0xfff)
		{
			yyerror(ERROR_9);
			exp = 0;
		}
		inst_code.w0 = 0x600a0 | (exp >> 8) | ((exp & 0xff) << 8);
		insert_code_w(&inst_code);
	}
}


void GenRep3(uint reg)
{
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
		reg = ddddd_2_DDDDDD(reg);
		if (reg == -1)
		{
			yyerror("Illegal register.");
			reg = 0;
		}
		inst_code.w0 = 0x6c020 | (reg << 8);
		insert_code_w(&inst_code);
	}
}


void GenTcc1(uint condition, uint src_reg, uint dst_reg)
{
	int new_src_reg;

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
		if (src_reg == 10 || src_reg == 11) /* A/B */
			new_src_reg = 0;
		else
			new_src_reg = ddddd_2_JJJ(src_reg);
		if (new_src_reg != -1)
		{
			if (src_reg == dst_reg)
			{
				yyerror(ERROR_7);
				new_src_reg = 0;
			} else if (new_src_reg == 4 || new_src_reg == 6)
			{
				yyerror(ERROR_16);
				new_src_reg = 0;
			}
			dst_reg = ddddd_2_d_dst(dst_reg);
			inst_code.w0 = 0x20000 | (condition << 12) | (new_src_reg << 3) | (dst_reg << 3);
		} else
		{								/* 56301 extension  */
			DSP56301;

			src_reg = ddddd_2_RRR(src_reg);
			if (src_reg == -1)
			{
				yyerror(ERROR_16);
				src_reg = 0;
			}
			dst_reg = ddddd_2_RRR(dst_reg);
			if (dst_reg == -1)
			{
				yyerror(ERROR_8);
				dst_reg = 0;
			}
			inst_code.w0 = 0x20800 | (condition << 12) | (src_reg << 8) | dst_reg;
		}
		insert_code_w(&inst_code);
	}
}


void GenTcc2(uint condition, uint src_reg1, uint dst_reg1, uint src_reg2, uint dst_reg2)
{
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
		if (src_reg1 == dst_reg1)
		{
			yyerror(ERROR_7);
			src_reg1 = 0;
		}
		if (src_reg1 == 10 || src_reg1 == 11) /* A/B */
			src_reg1 = 0;
		else
			src_reg1 = ddddd_2_JJJ(src_reg1);
		if (src_reg1 == -1 || src_reg1 == 4 || src_reg1 == 6)
		{
			yyerror(ERROR_16);
			src_reg1 = 0;
		}
		dst_reg1 = ddddd_2_d_dst(dst_reg1);

		src_reg2 = ddddd_2_RRR(src_reg2);
		if (src_reg2 == -1)
		{
			yyerror(ERROR_16);
			src_reg2 = 0;
		}
		dst_reg2 = ddddd_2_RRR(dst_reg2);
		if (dst_reg2 == -1)
		{
			yyerror(ERROR_8);
			dst_reg2 = 0;
		}
		inst_code.w0 = 0x30000 | (condition << 12) | (src_reg1 << 3) | (dst_reg1 << 3) | (src_reg2 << 8) | dst_reg2;
		insert_code_w(&inst_code);
	}
}


void GenTfr(uint src_reg, uint dst_reg, bcode *par_move)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = par_move->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		if (src_reg == dst_reg)
		{
			yyerror(ERROR_8);
			src_reg = 0;
		}
		src_reg = ddddd_2_JJJ2(src_reg);
		if (src_reg == -1)
		{
			yyerror(ERROR_16);
			src_reg = 0;
		}
		dst_reg = ddddd_2_d_dst(dst_reg);
		inst_code.w0 = 0x1 | (src_reg << 4) | (dst_reg << 3) | par_move->w0;
		inst_code.w1 = par_move->w1;
		inst_code.sflag = par_move->sflag;
		insert_code_w(&inst_code);
	}
}


void GenTrapcc(uint condition)
{
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
		inst_code.w0 = 0x10 | condition;
		insert_code_w(&inst_code);
	}
}


void GenVsl(uint reg, int val, bcode *ea)
{
	if (g_passNum == 0)
	{
		bcode inst_code;

		inst_code.sflag = ea->sflag;
		insert_vcode_w(&inst_code);
	} else
	{
		bcode inst_code;

		inst_code.sflag = 0;
		inst_code.w0 = 0;
		inst_code.w1 = 0;
		reg = ddddd_2_d_src(reg);
		if (val != 0 && val != 1)
		{
			yyerror(ERROR_9);
			val = 0;
		}
		inst_code.w0 = 0xac0c0 | (reg << 16) | (ea->w0 << 8) | (val << 4);
		inst_code.sflag = ea->sflag;
		insert_code_w(&inst_code);
	}
}


bcode GenEA1(uint opcode, uint addr_reg, uint offs_reg)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
	} else
	{
		if (addr_reg != offs_reg)
		{
			yyerror("Nx register must match Rx register.");
		} else
		{
			move.w0 = opcode | addr_reg;
		}
	}
	return move;
}

bcode GenEA2(uint opcode, uint addr_reg)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
	} else
	{
		move.w0 = opcode | addr_reg;
	}
	return move;
}

bcode GenImmLong(uint opcode, uint addr)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
		move.sflag = 1;
	} else
	{
		move.sflag = 1;
		move.w0 = opcode;
		move.w1 = addr & 0xffffff;
	}
	return move;
}

bcode GenImmShortIO(uint opcode, uint addr)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
		move.sflag = 2;
	} else
	{
		move.sflag = 2;
		move.w0 = opcode;
		move.w1 = addr & 0xffffff;
	}
	return move;
}

bcode GenImmShortAbs(uint opcode, uint addr)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
		move.sflag = 2;
	} else
	{
		move.sflag = 2;
		move.w0 = opcode;
		move.w1 = addr & 0xffffff;
	}
	return move;
}


raddr GenRelAddrLong(int addr)
{
	raddr ret;

	retInit(&ret);
	ret.sflag = 1;

	if (g_passNum != 0)
	{
		int btarget;

		ret.abs_value = addr;
		btarget = ret.abs_value - pc;

		ret.type = T_LONG;
		ret.value = btarget;
	}
	return ret;
}


raddr GenRelAddrShort(int addr)
{
	raddr ret;

	retInit(&ret);
	ret.sflag = 0;

	if (g_passNum != 0)
	{
		int btarget;

		ret.abs_value = addr;

		btarget = ret.abs_value - pc;

		if (btarget < -0xff || btarget > 0xff)
		{
			yyerror("Short addressing out of range.");
		}

		ret.type = T_SHORT;
		ret.value = btarget;
	}
	return ret;
}


raddr GenRelAddrReg(uint reg)
{
	raddr ret;

	retInit(&ret);
	ret.sflag = 0;

	if (g_passNum != 0)
	{
		ret.type = T_REGISTER;
		ret.value = reg;
	}
	return ret;
}
