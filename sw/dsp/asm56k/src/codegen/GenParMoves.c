/*

Project:    asm56k
Author:     M.Buras (sqward)


*/
#include <stdio.h>
#include "GenParMoves.h"
#include <ConvertFields.h>
#include <PipeLineRestriction.h>
#include <export.h>
#include <ErrorMessages.h>

uint const xmem_reg_pattern1[] = {
	0x408000,
	0x40c000
};

uint const xmem_reg_pattern2[] = {
	0x400000,
	0x404000
};

uint const ymem_reg_pattern1[] = {
	0x488000,
	0x48c000
};

uint const ymem_reg_pattern2[] = {
	0x480000,
	0x484000
};


uint const lmem_reg_pattern1[] = {
	0x408000,
	0x40c000
};

uint const lmem_reg_pattern2[] = {
	0x400000,
	0x404000
};


uint const XRegYReg_pattern[] = {
	0xc08000,
	0x808000,
	0xc00000,
	0x800000
};



bcode GenParIFcc(uint condition, uint opcode)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
	} else
	{
		DSP56301;
		move.w0 = condition | (opcode << 8);
	}
	return move;
}


bcode GenParExpReg(int val, uint dst_reg)
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
		PipeLineNewDstAguReg(dst_reg);

		dst_reg = ddddd_2_ddddd(dst_reg);
		if (dst_reg == -1)
		{
			yyerror("In parallel move field: Illegal destination register specified.");
			dst_reg = 0;
		}
		move.sflag = 1;
		move.w0 = 0x40c000 | 0x3400 | ((dst_reg & 0x18) << 17) | ((dst_reg & 0x7) << 16);
		move.w1 = val;
	}
	return move;
}


bcode GenParExpRegShort(int val, uint dst_reg)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
	} else
	{
		PipeLineNewDstAguReg(dst_reg);

		if (val != (val & 0xff))
		{
			yyerror("Warning: In parallel move field: Immediate data excess 8bit number -> truncated");
		}
		dst_reg = ddddd_2_ddddd(dst_reg);
		if (dst_reg == -1)
		{
			yyerror("In parallel move field: Illegal destination register specified.");
			dst_reg = 0;
		}
		move.w0 = 0x200000 | ((val & 0xff) << 8) | (dst_reg << 16);
	}
	return move;
}


bcode GenParRegReg(uint src_reg, uint dst_reg)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
	} else
	{
		PipeLineNewSrcAguReg(src_reg);
		PipeLineNewDstAguReg(dst_reg);

		src_reg = ddddd_2_ddddd(src_reg);
		if (src_reg == -1)
		{
			yyerror("In parallel move field: Illegal source register specified.");
			src_reg = 0;
		}
		dst_reg = ddddd_2_ddddd(dst_reg);
		if (dst_reg == -1)
		{
			yyerror("In parallel move field: Illegal destination register specified.");
			dst_reg = 0;
		}
		move.w0 = 0x200000 | (src_reg << 13) | (dst_reg << 8);
	}
	return move;
}


bcode GenParUpdate(uint update_op)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
	} else
	{
		move.w0 = 0x204000 | (update_op << 8);
	}
	return move;
}


bcode GenMemReg(const uint *opcodes, bcode *ea, uint dst_reg, int is_dst)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
		move.sflag = ea->sflag;
		if (ea->sflag == 2)				/* indicate extension word usage */
			move.sflag = 0;
	} else
	{
		PipeLineNewSrcEA(ea);
		PipeLineNewDstAguReg(dst_reg);

		dst_reg = ddddd_2_ddddd(dst_reg);
		if (dst_reg == -1)
		{
			yyerror("In parallel move field: Illegal %s register specified.", is_dst ? "destination" : "source");
			dst_reg = 0;
		}
		if (ea->sflag == 2)
		{
			if (ea->w1 != (ea->w1 & 0x3f))
			{
				yyerror("In parallel move field: Value out of range");
			}
			move.sflag = 0;
			move.w0 = opcodes[0] | ((dst_reg & 0x18) << 17) | ((dst_reg & 0x7) << 16) | ((ea->w1 & 0x3f) << 8);
		} else
		{
			move.sflag = ea->sflag;
			move.w0 = opcodes[1] | ((dst_reg & 0x18) << 17) | ((dst_reg & 0x7) << 16) | (ea->w0 << 8);
			move.w1 = ea->w1;
		}
	}
	return move;
}


bcode GenParExpRegRegReg(int val, uint dst_reg1, uint src_reg2, uint dst_reg2)
{
	bcode ea;

	ea.sflag = 1;
	ea.w0 = 0x34;
	ea.w1 = val;

	PipeLineNewDstAguReg(dst_reg1);
	PipeLineNewSrcAguReg(src_reg2);
	PipeLineNewDstAguReg(dst_reg2);

	return GenParEaRegRegReg(&ea, dst_reg1, src_reg2, dst_reg2);
}


bcode GenParEaRegRegReg(bcode *ea, uint dst_reg1, uint src_reg2, uint dst_reg2)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
		move.sflag = ea->sflag;
#if 0
		if (ea->sflag == 2)
			ea->sflag = 1;

		if (ea->w0 == 0x30 && ea->w0 == 0x34)
			move.sflag = 1;
		else
			move.sflag = 0;
#endif
	} else
	{
		PipeLineNewSrcEA(ea);

		src_reg2 = ddddd_2_d(src_reg2);
		if (src_reg2 == -1)
		{
			yyerror("In Y field: Illegal register specified: A,B are only allowed.");
			src_reg2 = 0;
		}
		dst_reg2 = ddddd_2_yff(dst_reg2);
		if (dst_reg2 != 0 && dst_reg2 != 1)
		{
			yyerror("In Y field: Illegal register: Y0,Y1 only allowed.");
		}
		dst_reg1 = ddddd_2_ff(dst_reg1);
		if (dst_reg1 == -1)
		{
			yyerror("In X field: Illegal register specified: X0,X1,A,B only allowed");
			dst_reg1 = 0;
		}
		move.sflag = ea->sflag;
		move.w0 = 0x108000 | (ea->w0 << 8) | (dst_reg2 << 16) | (src_reg2 << 17) | (dst_reg1 << 18);
		move.w1 = ea->w1;
	}
	return move;
}


bcode GenParRegEaRegReg(uint src_reg1, bcode *ea, uint src_reg2, uint dst_reg2)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
		move.sflag = ea->sflag;
		if (ea->sflag == 2)
			ea->sflag = 1;
	} else
	{
		PipeLineNewDstEA(ea);

		if (ea->w0 == 0x34)				/* cancel short addressing attempt */
		{
			ea->w0 = 0x30;
			ea->sflag = 1;
		}
		if (src_reg1 != dst_reg2 && src_reg2 != 0x4)
		{
			src_reg2 = ddddd_2_d(src_reg2);
			if (src_reg2 == -1)
			{
				yyerror("In Y field; Illegal source register; A,B are allowed");
				src_reg2 = 0;
			}
			dst_reg2 = ddddd_2_yff(dst_reg2);
			if (dst_reg2 == -1)
			{
				yyerror("In Y field; Illegal destination register; Y0,Y1 only allowed.");
			}
			src_reg1 = ddddd_2_ff(src_reg1);
			if (src_reg1 == -1)
			{
				yyerror("Illegal register specified; X0,X1,A,B only allowed");
				src_reg1 = 0;
			}
			move.sflag = ea->sflag;
			move.w0 = 0x100000 | (ea->w0 << 8) | (dst_reg2 << 16) | (src_reg2 << 17) | (src_reg1 << 18);
			move.w1 = ea->w1;
		} else
		{
			if ((src_reg1 = ddddd_2_d(src_reg1)) < 0)
			{
				src_reg1 = 0;
				yyerror("In X field; A or B registers are only allowed");
			}
			if ((dst_reg2 = ddddd_2_d(dst_reg2)) < 0)
			{
				dst_reg2 = 0;
				yyerror("In Y field; A or B registers are only allowed");
			}
			src_reg2 = ddddd_2_ff(src_reg2);
			if (src_reg2 != 0)
			{
				yyerror("Only X0 allowed");
				move.sflag = 0;
				move.w0 = 0;
				move.w1 = 0;
			} else
			{
				move.sflag = ea->sflag;
				move.w0 = 0x080000 | (src_reg1 << 16) | (ea->w0 << 8);
				move.w1 = ea->w1;
			}
		}
	}
	return move;
}


bcode GenParRegRegExpReg(uint src_reg1, uint dst_reg1, int val, uint dst_reg2)
{
	bcode ea;

	ea.sflag = 1;
	ea.w0 = 0x34;
	ea.w1 = val;
	return GenParRegRegEaReg(src_reg1, dst_reg1, &ea, dst_reg2);
}

bcode GenParRegRegEaReg(uint src_reg1, uint dst_reg1, bcode *ea, uint dst_reg2)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
		move.sflag = ea->sflag;
		if (ea->sflag == 2)
			move.sflag = 1;
	} else
	{
		PipeLineNewSrcEA(ea);

		src_reg1 = ddddd_2_d(src_reg1);
		if (src_reg1 == -1)
		{
			src_reg1 = 0;
			yyerror("In X field; Only A or B registers are allowed");
		}
		dst_reg1 = ddddd_2_ff(dst_reg1);
		if (dst_reg1 != 0 && dst_reg1 != 1)
		{
			yyerror("In X field: Illegal register: X0,X1 only allowed.");
			dst_reg1 = 0;
		}
		dst_reg2 = ddddd_2_yff(dst_reg2);
		if (dst_reg2 == -1)
		{
			yyerror("In Y field: Illegal register: Y0,Y1,A,B only allowed.");
		}
		move.sflag = ea->sflag;
		move.w0 = 0x10c000 | (ea->w0 << 8) | (dst_reg2 << 16) | (dst_reg1 << 18) | (src_reg1 << 19);
		move.w1 = ea->w1;
	}
	return move;
}


bcode GenParRegRegRegEa(uint src_reg1, uint dst_reg1, uint dst_reg2, bcode *ea)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
		move.sflag = ea->sflag;
		if (ea->sflag == 2)
			move.sflag = 1;
	} else
	{
		PipeLineNewDstEA(ea);

		if (dst_reg1 != dst_reg2 && src_reg1 != 0x6)
		{
			if ((src_reg1 = ddddd_2_d(src_reg1)) < 0)
			{
				src_reg1 = 0;
				yyerror("In Y field: A or B registers are allowed");
			}
			dst_reg1 = ddddd_2_ff(dst_reg1);

			if (dst_reg1 != 0 && dst_reg1 != 1)
			{
				yyerror("In X field: Illegal register: X0,X1 only allowed.");
			}

			dst_reg2 = ddddd_2_yff(dst_reg2);
			move.sflag = ea->sflag;
			move.w0 = 0x104000 | (ea->w0 << 8) | (dst_reg2 << 16) | (dst_reg1 << 18) | (src_reg1 << 19);
			move.w1 = ea->w1;
		} else
		{
			if ((dst_reg2 = ddddd_2_d(dst_reg2)) < 0)
			{
				dst_reg2 = 0;
				yyerror("A or B registers are allowed");
			}

			src_reg1 = ddddd_2_yff(src_reg1);

			if ((dst_reg1 = ddddd_2_d(dst_reg1)) < 0)
			{
				dst_reg1 = 0;
				yyerror("A or B registers are allowed");
			}

			if (src_reg1 != 0)
			{
				yyerror("Only Y0 allowed");
				move.sflag = 0;
				move.w0 = 0;
				move.w1 = 0;
			} else
			{
				move.sflag = ea->sflag;
				move.w0 = 0x088000 | (dst_reg1 << 16) | (ea->w0 << 8);
				move.w1 = ea->w1;
			}
		}
	}
	return move;
}


bcode GenLMemReg(const uint *opcodes, bcode *ea, uint dst_reg)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
		move.sflag = ea->sflag;
		if (ea->sflag == 2)
			move.sflag = 0;
	} else
	{
		PipeLineNewSrcEA(ea);

		if ((dst_reg = ddddd_2_LLL(dst_reg)) < 0)
		{
			yyerror("Wrong register in long parallel move (use: a10,b10,x,y,ab,ba)");
			dst_reg = 0;
		}
		if (ea->sflag == 2)
		{
			if (ea->w1 != (ea->w1 & 0x3f))
			{
				yyerror("In L field: Value out of range");
			}
			move.sflag = 0;
			move.w0 = opcodes[0] | ((dst_reg & 0x4) << 17) | ((dst_reg & 0x3) << 16) | ((ea->w1 & 0x3f) << 8);
		} else
		{
			move.sflag = ea->sflag;
			move.w0 = opcodes[1] | (ea->w0 << 8) | ((dst_reg & 0x4) << 17) | ((dst_reg & 0x3) << 16);
			move.w1 = ea->w1;
		}
	}
	return move;
}


bcode GenParXRegYReg(uint opcode, bcode *src_ea1, uint dst_reg1, bcode *src_ea2, uint dst_reg2)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
	} else
	{
		PipeLineNewDstEA(src_ea1);
		PipeLineNewDstEA(src_ea2);

		src_ea1->w0 = ea_2_MMRRR(src_ea1->w0);
		if (src_ea1->w0 == -1)
		{
			yyerror("In X field: Illegal addressing mode specified.");
			src_ea1->sflag = 0;
			src_ea1->w0 = 0;
		}
		src_ea2->w0 = ea_2_mmrr(src_ea2->w0, src_ea1->w0);
		if (src_ea2->w0 == -2)
		{
			yyerror("Same AGU register bank in parallel move specified.");
			src_ea2->sflag = 0;
			src_ea2->w0 = 0;
		}
		if (src_ea2->w0 == -1)
		{
			yyerror("In Y field; Illegal addressing mode specified.");
			src_ea2->sflag = 0;
			src_ea2->w0 = 0;
		}
#if 0
		if (dst_reg1 == dst_reg2)
		{
			yyerror("In parallel move: Same destination registers specified.");
		}
#endif
		dst_reg1 = ddddd_2_ff(dst_reg1);
		if (dst_reg1 == -1)
		{
			yyerror("In x field: Illegal register specified: X0,X1,A,B only allowed.");
			dst_reg1 = 0;
		}
		dst_reg2 = ddddd_2_yff(dst_reg2);
		if (dst_reg2 == -1)
		{
			yyerror("In Y field: Illegal register specified: Y0,Y1,A,B only allowed.");
			dst_reg2 = 0;
		}
		move.w0 =
			opcode | ((src_ea2->w0 & 0xc) << 18) | (dst_reg1 << 18) | (dst_reg2 << 16) | ((src_ea2->w0 & 0x3) << 13) |
			(src_ea1->w0 << 8);
	}
	return move;
}


bcode GenParEmpty(void)
{
	bcode move;

	move.sflag = 0;
	move.w0 = 0;
	move.w1 = 0;
	if (g_passNum == 0)
	{
	} else
	{
		move.w0 = 0x200000;
	}
	return move;
}
