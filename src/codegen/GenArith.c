/*

Project:    asm56k
Author:     M.Buras (sqward)


*/
#include <stdio.h>
#include <ConvertFields.h>
#include <export.h>
#include <ErrorMessages.h>
#include "GenArith.h"

int const add_patterns[] = {
	0x0,								/* add jjj,d */
	0x14080,							/* add #>xx,d */
	0x140c0,							/* add #xxxxx,d */
};

int const sub_patterns[] = {
	0x4,								/* as above */
	0x14084,
	0x140c4
};

int const cmp_patterns[] = {
	0x5,								/* as above */
	0x14085,
	0x140c5
};

int const and_patterns[] = {
	0x46,
	0x14086,
	0x140c6
};

int const eor_patterns[] = {
	0x43,
	0x14083,
	0x140c3
};

int const or_patterns[] = {
	0x42,
	0x14082,
	0x140c2
};

int const asl_patterns[] = {
	0x32,
	0xc1d00,
	0xc1e40
};

int const asr_patterns[] = {
	0x22,
	0xc1c00,
	0xc1e60
};

int const lsl_patterns[] = {
	0x33,
	0xc1e80,
	0xc1e10
};

int const lsr_patterns[] = {
	0x23,
	0xc1ec0,
	0xc1e30
};

int const incdec_patterns[] = {
	0x8,
	0xa
};


void GenAndEorOr(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move)
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
		if (src_reg == dest_reg)
		{
			yyerror(ERROR_7);
		}
		inst_code.sflag = par_move->sflag;
		inst_code.w0 = insn_patt | (ddddd_2_d_dst(dest_reg) << 3) | (ddddd_2_JJ_src(src_reg) << 4) | (par_move->w0);
		inst_code.w1 = par_move->w1;

		insert_code_w(&inst_code);
	}
}


void GenAdcSbc(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move)
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

		if (src_reg == 30)
		{
			src_reg = 0;
		} else
		{
			if (src_reg == 31)
			{
				src_reg = 1;
			} else
			{
				yyerror("In operands field: illegal source operand: X,Y only allowed.");
				src_reg = 0;
			}
		}
		dest_reg = ddddd_2_d_dst(dest_reg);

		inst_code.sflag = par_move->sflag;
		inst_code.w0 = insn_patt | (src_reg << 4) | (dest_reg << 3) | (par_move->w0);
		inst_code.w1 = par_move->w1;

		insert_code_w(&inst_code);
	}
}


void GenAddSub(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move)
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

		if (src_reg == dest_reg)
		{
			yyerror(ERROR_7);
		}

		src_reg = ddddd_2_JJJ(src_reg);
		if (src_reg == -1)
		{
			yyerror("In operands field: illegal source operand: B/A,X,Y,X0,X1,Y0,Y1 only allowed.");
			src_reg = 0;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);

		inst_code.sflag = par_move->sflag;
		inst_code.w0 = insn_patt | (src_reg << 3) | (dest_reg << 3) | (par_move->w0);
		inst_code.w1 = par_move->w1;

		insert_code_w(&inst_code);
	}
}


void GenCmp(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move)
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

		if (src_reg == dest_reg)
		{
			yyerror(ERROR_7);
		}

		src_reg = ddddd_2_JJJ2(src_reg);
		if (src_reg == -1)
		{
			yyerror("In operands field: illegal source operand: B/A,X0,Y0,X1,Y1 only allowed.");
			src_reg = 0;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);

		inst_code.sflag = par_move->sflag;
		inst_code.w0 = insn_patt | (src_reg << 4) | (dest_reg << 3) | (par_move->w0);
		inst_code.w1 = par_move->w1;

		insert_code_w(&inst_code);
	}
}


void GenAddSubEorOrLong(uint insn_patt, int val, uint dest_reg)
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
		dest_reg = ddddd_2_d_dst(dest_reg);
		inst_code.sflag = 1;
		inst_code.w0 = insn_patt | (dest_reg << 3);
		inst_code.w1 = val;
		insert_code_w(&inst_code);
	}
}


void GenAddSubEorOrShort(uint insn_patt, int val, uint dest_reg)
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
		dest_reg = ddddd_2_d_dst(dest_reg);
		if (val != (val & 0x3f))
		{
			yyerror("In operands field: Immediate value to big -> truncated.");
			val = val & 0x3f;
		}
		inst_code.w0 = insn_patt | (dest_reg << 3) | (val << 8);
		insert_code_w(&inst_code);
	}
}


void GenAddxSubx(uint insn_patt, uint src_reg, uint dest_reg, bcode *par_move)
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
		dest_reg = ddddd_2_d_dst(dest_reg);
		src_reg = (ddddd_2_d_src(src_reg) << 1) | dest_reg;
		if (src_reg != 1 && src_reg != 2)
		{
			yyerror("In operands field: Illegal reg. specified: A->B or B->A operations allowed.");
			src_reg = 1;
		}
		inst_code.sflag = par_move->sflag;
		inst_code.w0 = insn_patt | (src_reg << 3) | par_move->w0;
		inst_code.w1 = par_move->w1;
		insert_code_w(&inst_code);
	}
}


void GenAndiOri(uint insn_patt, int val, uint dest_reg)
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
		dest_reg = ddddd_2_EE(dest_reg);
		if (dest_reg == -1)
		{
			yyerror("In operands field: Illegal register: Control registers only allowed.");
			dest_reg = 0;
		}
		if (val != (val & 0xff))
		{
			yyerror("In operands field: Immediate value too big: 8-bit value allowed.");
			val &= 0xff;
		}
		inst_code.w0 = insn_patt | dest_reg | (val << 8);
		insert_code_w(&inst_code);
	}
}


void GenAsxImmediate(uint insn_patt, int val, uint src_reg, uint dest_reg)
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
		if ((val & 0x3f) != val)
		{
			yyerror("In operands filed: Immediate value to big: <0x0-0x3f> range allowed.");
			val &= 0x3f;
		}
		src_reg = ddddd_2_d_src(src_reg);
		dest_reg = ddddd_2_d_dst(dest_reg);
		inst_code.w0 = insn_patt | (src_reg << 7) | dest_reg | (val << 1);
		insert_code_w(&inst_code);
	}
}


void GenAsxReg(uint insn_patt, int val_reg, uint src_reg, uint dest_reg)
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
		val_reg = ddddd_2_sss(val_reg);
		if (val_reg == -1)
		{
			yyerror("In operands field: Illegal register: X0,X1,Y0,Y1,A1,B1 only allowed.");
			val_reg = 0;
		}
		src_reg = ddddd_2_d_src(src_reg);
		dest_reg = ddddd_2_d_dst(dest_reg);
		inst_code.w0 = insn_patt | (src_reg << 4) | (val_reg << 1) | dest_reg;
		insert_code_w(&inst_code);
	}
}


void GenCmpm(uint src_reg, uint dest_reg, bcode *par_move)
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

		if (src_reg == dest_reg)
		{
			yyerror(ERROR_7);
		}
		if (src_reg == 10) /* A */
			src_reg = 1;
		else if (src_reg == 11) /* B */
			src_reg = 0;
		else
			src_reg = ddddd_2_JJJ(src_reg);
		if (src_reg == -1 || src_reg == 4 || src_reg == 6)
		{
			yyerror("In operands field: Illegal source operand: B/A,X0,Y0,X1,Y1 only allowed.");
			src_reg = 0;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);

		inst_code.sflag = par_move->sflag;
		inst_code.w0 = 0x7 | (src_reg << 3) | (dest_reg << 3) | par_move->w0;
		inst_code.w1 = par_move->w1;
		insert_code_w(&inst_code);
	}
}


void GenCmpu(uint src_reg, uint dest_reg)
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
		if (src_reg == dest_reg)
		{
			yyerror(ERROR_7);
		}
		src_reg = ddddd_2_ggg(src_reg);
		if (src_reg == -1)
		{
			yyerror("In operands field: Illegal source register: A/B,X0,Y0,X1,Y1 allowed.");
			src_reg = 0;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);

		inst_code.w0 = 0xc1ff0 | (src_reg << 1) | dest_reg;
		insert_code_w(&inst_code);
	}
}


void GenIncDec(uint inst_patt, uint reg)
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
		inst_code.w0 = inst_patt | ddddd_2_d_dst(reg);
		insert_code_w(&inst_code);
	}
}


void GenDiv(uint src_reg, uint dst_reg)
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
		inst_code.w0 = 0x18040 | (ddddd_2_JJ_src(src_reg) << 4) | (ddddd_2_d_dst(dst_reg) << 3);
		insert_code_w(&inst_code);
	}
}


void GenDMac(uint modifier, uint plusminus, int val, uint dest_reg)
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
		inst_code.w0 = 0x12480 | ((modifier & 2) << 7) | ((modifier & 1) << 6) | (ddddd_2_d_dst(dest_reg) << 5) | (plusminus << 4) | val;
		insert_code_w(&inst_code);
	}
}


void GenLsxImmediate(const int *insn_patt, int val, uint dest_reg)
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
		if (val > 16)
		{
			yyerror(ERROR_9);
			val &= 0x1f;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);
		inst_code.w0 = insn_patt[1] | (val << 1) | dest_reg;
		insert_code_w(&inst_code);
	}
}


void GenLsxReg(const int *insn_patt, int src_reg, uint dest_reg)
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
		inst_code.w0 = insn_patt[2] | (src_reg << 1) | dest_reg;
		insert_code_w(&inst_code);
	}
}


uint const mac_pattern[] = {
	0x82,
	0x100c2,
	0x141c2,
	0x12680
};

uint const mpy_pattern[] = {
	0x80,
	0x100c0,
	0x141c0,
	0x12780
};

uint const macr_pattern[] = {
	0x83,
	0x100c3,
	0x141c3
};

uint const mpyr_pattern[] = {
	0x81,
	0x100c1,
	0x141c1
};

void GenMul1(const uint *insn_patt, uint plusminus, uint reg_pair, uint dest_reg, bcode *par_move)
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
		if (reg_pair == -1)
		{
			yyerror(ERROR_11);
			reg_pair = 0;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);
		inst_code.w0 = insn_patt[0] | (reg_pair << 4) | (dest_reg << 3) | (plusminus << 2) | par_move->w0;
		inst_code.w1 = par_move->w1;
		inst_code.sflag = par_move->sflag;
		insert_code_w(&inst_code);
	}
}


void GenMul2(const uint *insn_patt, uint plusminus, uint src_reg, int val, uint dest_reg)
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
		src_reg = ddddd_2_QQ(src_reg);
		if (src_reg == -1)
		{
			yyerror(ERROR_12);
			src_reg = 0;
		}
		val = exp_2_ssss(val);
		if (val == -1)
		{
			yyerror("Immediate value must be of the following form: 2^n.");
			val = 0;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);
		inst_code.w0 = insn_patt[1] | (val << 8) | (src_reg << 4) | (dest_reg << 3) | (plusminus << 2);
		insert_code_w(&inst_code);
	}
}


void GenMuli(const uint *insn_patt, uint plusminus, int val, uint src_reg, uint dest_reg)
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
		src_reg = ddddd_2_qq(src_reg);
		if (src_reg == -1)
		{
			yyerror(ERROR_12);
			src_reg = 0;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);
		inst_code.w0 = insn_patt[2] | (src_reg << 4) | (dest_reg << 3) | (plusminus << 2);
		inst_code.w1 = val;
		inst_code.sflag = 1;
		insert_code_w(&inst_code);
	}
}


void GenMulxx(const uint *insn_patt, uint code, uint plusminus, uint reg_pair, uint dest_reg)
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
		if (reg_pair == -1)
		{
			yyerror(ERROR_11);
			reg_pair = 0;
		}
		dest_reg = ddddd_2_d_dst(dest_reg);
		inst_code.w0 = insn_patt[3] | (code << 6) | (dest_reg << 5) | (plusminus << 4) | reg_pair;
		insert_code_w(&inst_code);
	}
}


uint const max_pattern[] = {
	0x1d,
	0x15
};

void GenMax(uint insn_patt, uint src_reg, uint dst_reg, bcode *par_move)
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
		if (src_reg != 10)
		{
			yyerror("Illegal source register: A is allowed.");
		}
		if (dst_reg != 11)
		{
			yyerror("Illegal destination register: B is allowed.");
		}
		inst_code.w0 = insn_patt | par_move->w0;
		inst_code.w1 = par_move->w1;
		inst_code.sflag = par_move->sflag;
		insert_code_w(&inst_code);
	}
}


void GenNorm(uint src_reg, uint dst_reg)
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
		src_reg = ddddd_2_RRR(src_reg);
		if (src_reg == -1)
		{
			yyerror("Illegal source register: Rn regs are only allowed.");
			src_reg = 0;
		}
		dst_reg = ddddd_2_d_dst(dst_reg);
		inst_code.w0 = 0x1d815 | (src_reg << 8) | (dst_reg << 3);
		insert_code_w(&inst_code);
	}
}


void GenNormf(uint src_reg, uint dst_reg)
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
		src_reg = ddddd_2_sss(src_reg);
		if (src_reg == -1)
		{
			yyerror(ERROR_10);
			src_reg = 0;
		}
		dst_reg = ddddd_2_d_dst(dst_reg);
		inst_code.w0 = 0xc1e20 | (src_reg << 1) | dst_reg;
		insert_code_w(&inst_code);
	}
}
