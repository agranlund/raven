/*

Project:    asm56k
Author:     M.Buras (sqward)


*/

#include "ConvertFields.h"
#include "export.h"

static const char *const g_regNames[] = {

	"x0", "x1",							/* 0, 1 */
	"y0", "y1",							/* 2, 3 */
	"a0", "a1", "a2",					/* 4,5,6 */
	"b0", "b1", "b2",					/* 7,8,9 */
	"A",								/* 10 */
	"B",								/* 11 */

	"R0",								/* 12 */
	"R1",								/* 13 */
	"R2",								/* 14 */
	"R3",								/* 15 */
	"R4",								/* 16 */
	"R5",								/* 17 */
	"R6",								/* 18 */
	"R7",								/* 19 */

	"N0",								/* 20 */
	"N1",								/* 21 */
	"N2",								/* 22 */
	"N3",								/* 23 */
	"N4",								/* 24 */
	"N5",								/* 25 */
	"N6",								/* 26 */
	"N7",								/* 27 */

	"A10",								/* 28 */
	"B10",								/* 29 */
	"X",								/* 30 */
	"Y",								/* 31 */
	"AB",								/* 32 */
	"BA",								/* 33 */

	"MR",								/* 34 */
	"CCR",								/* 35 */
	"COM",								/* 36 */
	"EOM",								/* 37 */

	"M0",								/* 38 */
	"M1",								/* 39 */
	"M2",								/* 40 */
	"M3",								/* 41 */
	"M4",								/* 42 */
	"M5",								/* 43 */
	"M6",								/* 44 */
	"M7",								/* 45 */

	"EP",								/* 46 */
	"VBA",								/* 47 */
	"SC",								/* 48 */
	"SZ",								/* 49 */
	"SR",								/* 50 */
	"OMR",								/* 51 */
	"SP",								/* 52 */
	"SSH",								/* 53 */
	"SSL",								/* 54 */
	"LA",								/* 55 */
	"LC",								/* 56 */
};

static signed char const _ddddd_2_ddddd[64] = {
	0x04, 0x05, 0x06, 0x07, 0x08, 0x0c, 0x0a, 0x09, 0x0d, 0x0b, 0x0e, 0x0f,	/* alu registers mapping */
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,	/* agu Rx registers mapping */
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,	/* agu Nx registers mapping */
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,
};

static signed char const _ddddd_2_d[64] = {
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, 0x00, 0x01,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_ff[64] = {
	0x00, 0x01,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, 0x02, 0x03,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_LLL[64] = {
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, 0x04, 0x05,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,	  -1,   -1,   -1,   -1,
	0x00, 0x01, 0x02, 0x03, 0x06, 0x07,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_sss[64] = {
	0x04, 0x06, 0x05, 0x07,   -1, 0x02,   -1,   -1, 0x03,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_EE[64] = {
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,	  -1,   -1,
	0x00, 0x01, 0x02, 0x03,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,	  -1,   -1,   -1, 0x02,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_JJ[64] = {
	0x00, 0x02, 0x01, 0x03,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_DDDDDD[64] = {
	0x04, 0x05, 0x06, 0x07, 0x08, 0x0c, 0x0a, 0x09,	0x0d, 0x0b, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13,	0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b,	0x1c, 0x1d, 0x1e, 0x1f,
	  -1,   -1,   -1,   -1,	  -1,   -1,
	  -1,   -1,   -1,   -1,
	0x20, 0x21,	0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x2a, 0x30,	0x31, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_ggg[64] = {
	0x04, 0x06, 0x05, 0x07,   -1, 0x00,   -1,   -1,	0x00,   -1, 0x00, 0x00,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_dddd[64] = {
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,	/* alu registers mapping */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,	/* agu Rx registers mapping */
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,	/* agu Nx registers mapping */
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_QQ[64] = {
	0x01, 0x03, 0x02, 0x00,   -1,   -1,   -1,   -1,	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_qq[64] = {
	0x00, 0x02, 0x01, 0x03,   -1,   -1,   -1,   -1,	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

static signed char const _ddddd_2_RRR[64] = {
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,	  -1,   -1,   -1,   -1,
	0x00, 0x01, 0x02, 0x03,	0x04, 0x05, 0x06, 0x07,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};


int ddddd_2_ff(unsigned int code)
{
	return _ddddd_2_ff[code];
}

int ddddd_2_ddddd(unsigned int code)
{
	return _ddddd_2_ddddd[code];
}

static signed char const _ddddd_2_yff[64] = {
	  -1,   -1, 0x00, 0x01,   -1,   -1,   -1,   -1,	  -1,   -1, 0x02, 0x03,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

int ddddd_2_yff(unsigned int code)
{
	return _ddddd_2_yff[code];
}

int ddddd_2_LLL(unsigned int code)
{
	return _ddddd_2_LLL[code];
}

int ddddd_2_sss(unsigned int code)
{
	return _ddddd_2_sss[code];
}

int ddddd_2_d(unsigned int code)
{
	return _ddddd_2_d[code];
}

int ddddd_2_d_src(unsigned int code)
{
	int reg = _ddddd_2_d[code];

	if (reg == -1)
	{
		yyerror("In operands field: Illegal source register specified: A or B only allowed.");
		reg = 0;
	}
	return reg;
}

int ddddd_2_d_dst(unsigned int code)
{
	int reg = _ddddd_2_d[code];

	if (reg == -1)
	{
		yyerror("In operands field: Illegal destination register specified: A or B only allowed.");
		reg = 0;
	}
	return reg;
}

static signed char const _ddddd_2_JJJ[64] = {
	0x08, 0x0C, 0x0A, 0x0E,   -1,   -1,   -1,   -1,	  -1,   -1, 0x02, 0x02,
	  -1,   -1,   -1,   -1,	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,	  -1,   -1,   -1,   -1,
	  -1,   -1, 0x04, 0x06,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

int ddddd_2_JJJ(unsigned int code)
{
	return _ddddd_2_JJJ[code];
}

/* used by TFR */
static signed char const _ddddd_2_JJJ2[64] = {
	0x08 >> 1, 0x0C >> 1, 0x0A >> 1, 0x0E >> 1,   -1,   -1,   -1,   -1,	  -1,   -1, 0x00, 0x00,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

int ddddd_2_JJJ2(unsigned int code)
{
	return _ddddd_2_JJJ2[code];
}

int ddddd_2_EE(unsigned int code)
{
	return _ddddd_2_EE[code];
}

int ddddd_2_JJ(unsigned int code)
{
	return _ddddd_2_JJ[code];
}

int ddddd_2_JJ_src(unsigned int code)
{
	int reg = _ddddd_2_JJ[code];

	if (reg == -1)
	{
		yyerror("In operands field: Illegal source register: X0,Y0,X1 or Y1 are only allowed.");
		reg = 0;
	}
	return reg;
}


int ddddd_2_DDDDDD(unsigned int code)
{
	return _ddddd_2_DDDDDD[code];
}

int ddddd_2_ggg(unsigned int code)
{
	return _ddddd_2_ggg[code];
}

int ddddd_2_dddd(unsigned int code)
{
	return _ddddd_2_dddd[code];
}

int ddddd_2_QQ(unsigned int code)
{
	return _ddddd_2_QQ[code];
}

int ddddd_2_qq(unsigned int code)
{
	return _ddddd_2_qq[code];
}

static signed char const _ddddd_2_ddddd2[64] = {
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,	  -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
	  -1,   -1,   -1,   -1,	  -1,   -1,
	  -1,   -1,   -1,   -1,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	  -1,   -1,	  -1,   -1, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,	0x1f,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

int ddddd_2_ddddd2(unsigned int code)
{
	return _ddddd_2_ddddd2[code];
}

static signed char const _ddddd_2_eeeeee[64] = {
	0x04, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0c, 0x09,	0x0b, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13,	0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b,	0x1c, 0x1d, 0x1e, 0x1f,
	  -1,   -1,   -1,   -1,	  -1,   -1,
	  -1,   -1,   -1,   -1,
	0x20, 0x21,	0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	  -1,   -1,   -1,   -1, 0x39, 0x3a,	0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	  -1,   -1,   -1,   -1,   -1,   -1,   -1
};

int ddddd_2_eeeeee(unsigned int code)
{
	return _ddddd_2_eeeeee[code];
}

int ddddd_2_RRR(unsigned int code)
{
	return _ddddd_2_RRR[code];
}

int ea_2_MMRRR(unsigned int code)
{
	switch ((code & 0x38) >> 3)
	{
	case 1:
		return code;
	case 2:
		return code;
	case 3:
		return code;
	case 4:
		return code & 0x7;
	}

	return -1;
}

int ea_2_mmrr(unsigned int code, unsigned int code2)
{
	int reg2;

	if ((code & 0x4) == (code2 & 0x4))
	{
		return -2;
	}

	reg2 = code & 0x3;

	switch ((code & 0x38) >> 3)
	{
	case 1:
		return (1 << 2) | reg2;
	case 2:
		return (2 << 2) | reg2;
	case 3:
		return (3 << 2) | reg2;
	case 4:
		return (0 << 2) | reg2;
	}

	return -1;
}

int exp_2_ssss(int value)
{
	switch (value)
	{									/* MB: why we start with 0x2 ?? confirm this with manual and fix anyways! */
	case 0x2:
		return 22;
	case 0x2 << 1:
		return 21;
	case 0x2 << 2:
		return 20;
	case 0x2 << 3:
		return 19;
	case 0x2 << 4:
		return 18;
	case 0x2 << 5:
		return 17;
	case 0x2 << 6:
		return 16;
	case 0x2 << 7:
		return 15;
	case 0x2 << 8:
		return 14;
	case 0x2 << 9:
		return 13;
	case 0x2 << 10:
		return 12;
	case 0x2 << 11:
		return 11;
	case 0x2 << 12:
		return 10;
	case 0x2 << 13:
		return 9;
	case 0x2 << 14:
		return 8;
	case 0x2 << 15:
		return 7;
	case 0x2 << 16:
		return 6;
	case 0x2 << 17:
		return 5;
	case 0x2 << 18:
		return 4;
	case 0x2 << 19:
		return 3;
	case 0x2 << 20:
		return 2;
	case 0x2 << 21:
		return 1;
	}
	return -1;
}


int GetQQQ(int first, int second, int dont_iterate)
{
	if (first == 0 && second == 00)
		return 0;
	else if (first == 1 && second == 00)
		return 2;
	else if (first == 0 && second == 03)
		return 4;
	else if (first == 1 && second == 2)
		return 6;
	else if (first == 2 && second == 00)
		return 5;
	else if (first == 3 && second == 1)
		return 7;
	else if (first == 2 && second == 2)
		return 1;
	else if (first == 3 && second == 2)
		return 3;
	else if (!dont_iterate)
		return GetQQQ(second, first, 1);
	else
		return -1;
}


int GetQQQQXregXreg(int first, int second)
{
	switch ((first << 1) | second)
	{
	case 0:
		return 0x0;
	case 1:
		return 0xa;
	case 2:
		return 0x2;
	case 3:
		return 0x8;
	default:
		return -1;
	}
}


int GetQQQQXregYreg(int first, int second)
{
	switch ((first << 1) | second)
	{
	case 0:
		return 0xd;
	case 1:
		return 0x4;
	case 2:
		return 0x6;
	case 3:
		return 0xf;
	default:
		return -1;
	}
}


int GetQQQQYregXreg(int first, int second)
{
	switch ((first << 1) | second)
	{
	case 0:
		return 0x5;
	case 1:
		return 0xe;
	case 2:
		return 0xc;
	case 3:
		return 0x7;
	default:
		return -1;
	}
}


int GetQQQQYregYreg(int first, int second)
{
	switch ((first << 1) | second)
	{
	case 0:
		return 0x1;
	case 1:
		return 0xb;
	case 2:
		return 0x3;
	case 3:
		return 0x9;
	default:
		return -1;
	}
}


bool isAguReg(uint reg)
{
	if ((reg >= 12 && reg < 28) || (reg >= 38 && reg < 46))
	{
		return TRUE;
	}
	return FALSE;
}


const char *getRegName(uint reg)
{
	return reg < (sizeof(g_regNames) / sizeof(g_regNames[0])) ? g_regNames[reg] : "???";
}
