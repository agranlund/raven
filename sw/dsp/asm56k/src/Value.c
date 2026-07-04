/*

Project:    asm56k
Author:     M.Buras (sqward)

*/

#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm_types.h>
#include <export.h>
#include <Value.h>
#include <CodeUtils.h>
#include <SymbolTable.h>
#include <Parser.h>
#include <TokenStream.h>
#include <MacroProxy.h>
#include <StringBuffer.h>
#include <assert.h>

/* TODO: introduce overflow checking! */

Value Val_CastToInt(Value val)
{
	Value ret;

	if (val.m_type == kInt)
	{
		return val;
	} else if (val.m_type == kFloat || val.m_type == kFract)
	{
		if (val.m_value.m_float > 0x7fffff || val.m_value.m_float < -0x7fffff)
			yywarning("integer overflow when casting");
		ret.m_value.m_int = (int) val.m_value.m_float;
	}

	ret.m_type = kInt;

	return ret;
}


/* TODO: introduce overflow checking! */

Value Val_CastToFloat(Value val)
{
	Value ret;

	if (val.m_type == kFloat)
	{
		return val;
	} else if (val.m_type == kInt)
	{
		ret.m_value.m_float = (double) val.m_value.m_int;
	} else if (val.m_type == kFract)
	{
		ret.m_value = val.m_value;
	}

	ret.m_type = kFloat;

	return ret;
}

/* TODO: introduce overflow checking! */

Value Val_CastToFract(Value val)
{
	Value ret;

	if (val.m_type == kFract)
	{
		return val;
	} else if (val.m_type == kInt)
	{
		ret.m_value.m_float = (double) val.m_value.m_int;
	} else if (val.m_type == kFloat)
	{
		ret.m_value = val.m_value;
	}

	ret.m_type = kFract;

	return ret;
}


int Val_GetAsInt(Value val)
{
	if (val.m_type != kInt && val.m_type != kUnresolved)
	{
		yyerror("Integer value expected, not %d.", val.m_type);
		return 0;
	}
	return Val_CastToInt(val).m_value.m_int;
}


bool Val_CheckResolved(Value val2)
{
	if (val2.m_type == kUnresolved)
	{
		return TRUE;
	}
	return FALSE;
}

bool Val_CheckResolved2(Value val1, Value val2)
{
	if (val2.m_type == kUnresolved || val1.m_type == kUnresolved)
	{
		return TRUE;
	}
	return FALSE;
}

/* I think this might be flawed ... */

u32 Val_GetAsFract24(Value val)
{
	if (val.m_type == kFract || val.m_type == kFloat)
	{
		double raw = val.m_value.m_float;
		u32 m_int = (u32) round(raw * 0x800000);

		if (raw > 1.0f || raw < -1.0f)
		{
			yywarning("Overflow when casting to 24bit fractional.");
		}

		return m_int;
	} else if (val.m_type == kInt)
	{
		return val.m_value.m_int;
	}

	return 0;
}


u64 Val_GetAsFract48(Value val)
{
	if (val.m_type == kFract || val.m_type == kFloat)
	{
		double raw = val.m_value.m_float;

		if (raw > 1.0f || raw < -1.0f)
		{
			yywarning("Overflow when casting to 24bit fractional.");
		}

		return (u64) round(raw * 0x80000000000000LL);	/* ?? */
	} else if (val.m_type == kInt)
	{
		return val.m_value.m_int;
	}

	return 0;
}


Value Val_Add(Value val1, Value val2)
{
	Value ret;

	if (Val_CheckResolved2(val1, val2))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (val1.m_type != val2.m_type)
	{
		yyerror("Can't add incompatible types values.");
	}

	if (val1.m_type == kFract || val1.m_type == kFloat)
	{
		ret.m_value.m_float = val1.m_value.m_float + val2.m_value.m_float;
	} else
	{
		ret.m_value.m_int = val1.m_value.m_int + val2.m_value.m_int;
	}

	return ret;
}


Value Val_Mul(Value val1, Value val2)
{
	Value ret;

	if (Val_CheckResolved2(val1, val2))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (val1.m_type == kFract || val1.m_type == kFloat)
	{
		val2 = Val_CastToFloat(val2);
		ret.m_value.m_float = val1.m_value.m_float * val2.m_value.m_float;
	} else if (val2.m_type == kFract || val2.m_type == kFloat)
	{
		val1 = Val_CastToFloat(val1);
		ret.m_value.m_float = val1.m_value.m_float * val2.m_value.m_float;
		ret.m_type = val1.m_type;
	} else
	{
		ret.m_value.m_int = val1.m_value.m_int * val2.m_value.m_int;
	}

	return ret;
}


Value Val_Div(Value val1, Value val2)
{
	Value ret;

	if (Val_CheckResolved2(val1, val2))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (val1.m_type == kFract || val1.m_type == kFloat)
	{
		val2 = Val_CastToFloat(val2);
		if (val2.m_value.m_float == 0)
		{
			if (g_passNum != 0)
			{
				yyerror("Division by zero.");
			}
			ret.m_value.m_float = 1;
		} else
		{
			ret.m_value.m_float = val1.m_value.m_float / val2.m_value.m_float;
		}
	} else if (val2.m_type == kFract || val2.m_type == kFloat)
	{
		val1 = Val_CastToFloat(val1);
		if (val2.m_value.m_float == 0)
		{
			if (g_passNum != 0)
			{
				yyerror("Division by zero.");
			}
			ret.m_value.m_float = 1;
		} else
		{
			ret.m_value.m_float = val1.m_value.m_float / val2.m_value.m_float;
		}
		ret.m_type = val1.m_type;
	} else
	{
		if (val2.m_value.m_int == 0)
		{
			if (g_passNum != 0)
			{
				yyerror("Division by zero.");
			}
			ret.m_value.m_int = 1;
		} else
		{
			ret.m_value.m_int = val1.m_value.m_int / val2.m_value.m_int;
		}
	}

	return ret;
}


#if 0 /* unused */
static Value Val_Mod(Value val1, Value val2)
{
	Value ret;

	ret.m_type = val1.m_type;

	if (Val_CheckResolved2(val1, val2))
	{
		return Val_CreateUnresolved();
	}

	if (val1.m_type == kFract || val1.m_type == kFloat || val2.m_type == kFract || val2.m_type == kFloat)
	{
		ret.m_value.m_float = 0;
		yyerror("Can't calculate modulo of float or fract type.");
	} else
	{
		if (val2.m_value.m_int == 0)
		{
			if (g_passNum != 0)
			{
				yyerror("Division by zero.");
			}
			ret.m_value.m_int = 1;
		} else
		{
			ret.m_value.m_int = val1.m_value.m_int % val2.m_value.m_int;
		}
	}

	return ret;
}
#endif


Value Val_Neg(Value val1)
{
	Value ret;

	if (Val_CheckResolved(val1))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (val1.m_type == kFract || val1.m_type == kFloat)
	{
		ret.m_value.m_float = -val1.m_value.m_float;
	} else
	{
		ret.m_value.m_int = -val1.m_value.m_int;
	}

	return ret;
}


Value Val_BinNot(Value val1)
{
	Value ret;

	if (Val_CheckResolved(val1))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (val1.m_type == kFract || val1.m_type == kFloat)
	{
		yyerror("Float not allowed for bitwise operations");
		ret.m_value.m_float = 0;
	} else
	{
		ret.m_value.m_int = ~val1.m_value.m_int;
	}

	return ret;
}


Value Val_Not(Value val1)
{
	Value ret;

	if (Val_CheckResolved(val1))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (val1.m_type == kFract || val1.m_type == kFloat)
	{
		ret.m_value.m_float = val1.m_value.m_float == 0.0;
	} else
	{
		ret.m_value.m_int = val1.m_value.m_int == 0;
	}

	return ret;
}


Value Val_And(Value val1, Value val2)
{
	Value ret;

	if (Val_CheckResolved2(val1, val2))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (val1.m_type == kFract || val1.m_type == kFloat || val2.m_type == kFract || val2.m_type == kFloat)
	{
		ret.m_value.m_float = 0;
		yyerror("Float not allowed for bitwise operations");
	} else
	{
		ret.m_value.m_int = val1.m_value.m_int & val2.m_value.m_int;
	}

	return ret;
}


Value Val_Xor(Value val1, Value val2)
{
	Value ret;

	if (Val_CheckResolved2(val1, val2))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (val1.m_type == kFract || val1.m_type == kFloat || val2.m_type == kFract || val2.m_type == kFloat)
	{
		ret.m_value.m_float = 0;
		yyerror("Float not allowed for bitwise operations");
	} else
	{
		ret.m_value.m_int = val1.m_value.m_int ^ val2.m_value.m_int;
	}

	return ret;
}


Value Val_Or(Value val1, Value val2)
{
	Value ret;

	if (Val_CheckResolved2(val1, val2))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (val1.m_type == kFract || val1.m_type == kFloat || val2.m_type == kFract || val2.m_type == kFloat)
	{
		ret.m_value.m_float = 0;
		yyerror("Float not allowed for bitwise operations");
	} else
	{
		ret.m_value.m_int = val1.m_value.m_int | val2.m_value.m_int;
	}

	return ret;
}


Value Val_Lsr(Value val1, Value val2)
{
	Value ret;

	if (Val_CheckResolved2(val1, val2))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (kInt != val2.m_type)
	{
		yyerror("Can't shift by float.");
	}

	if (val1.m_type == kFract || val1.m_type == kFloat || val2.m_type == kFract || val2.m_type == kFloat)
	{
		ret.m_value.m_float = 0;
		yyerror("Float not allowed for bit-wise shift");
	} else
	{
		ret.m_value.m_int = val1.m_value.m_int >> val2.m_value.m_int;
	}

	return ret;
}


Value Val_Lsl(Value val1, Value val2)
{
	Value ret;

	if (Val_CheckResolved2(val1, val2))
	{
		return Val_CreateUnresolved();
	}

	ret.m_type = val1.m_type;

	if (kInt != val2.m_type)
	{
		yyerror("Can't shift by float.");
	}

	if (val1.m_type == kFract || val1.m_type == kFloat || val2.m_type == kFract || val2.m_type == kFloat)
	{
		ret.m_value.m_float = 0;
		yyerror("Float not allowed for bit-wise shift");
	} else
	{
		ret.m_value.m_int = val1.m_value.m_int << val2.m_value.m_int;
	}

	return ret;
}


Value Val_CreateUnresolved(void)
{
	Value ret;

	ret.m_type = kUnresolved;
	return ret;
}

Value Val_CreateFloat(double val)
{
	Value ret;

	ret.m_type = kFloat;
	ret.m_value.m_float = val;
	return ret;
}

Value Val_CreateInt(s64 val)
{
	Value ret;

	ret.m_type = kInt;
	ret.m_value.m_int = val;
	return ret;
}

Value Val_CreateFract(double val)
{
	Value ret;

	ret.m_type = kFract;
	ret.m_value.m_float = val;
	return ret;
}


bool Val_Eq(Value val1, Value val2)
{
	if (val1.m_type == kInt)
	{
		int rhs = Val_GetAsInt(Val_CastToInt(val2));

		if (val1.m_value.m_int == rhs)
		{
			return TRUE;
		}
	} else if (val1.m_type == kFract)
	{
		double rhs = Val_CastToFract(val2).m_value.m_float;

		yywarning("Comparing to fractionals for equality is a bit silly...");
		if (val1.m_value.m_float == rhs)
		{
			return TRUE;
		}
	} else if (val1.m_type == kFloat)
	{
		double rhs = Val_CastToFloat(val2).m_value.m_float;

		yywarning("Comparing to floats for equality is a bit silly...");
		if (val1.m_value.m_float == rhs)
		{
			return TRUE;
		}
	}
	return FALSE;
}

bool Val_Ls(Value val1, Value val2)
{
	if (val1.m_type == kInt)
	{
		int rhs = Val_GetAsInt(Val_CastToInt(val2));

		if (val1.m_value.m_int < rhs)
		{
			return TRUE;
		}
	} else if (val1.m_type == kFract)
	{
		double rhs = Val_CastToFract(val2).m_value.m_float;

		if (val1.m_value.m_float < rhs)
		{
			return TRUE;
		}
	} else if (val1.m_type == kFloat)
	{
		double rhs = Val_CastToFloat(val2).m_value.m_float;

		yywarning("Comparing to floats for equality is a bit silly...");
		if (val1.m_value.m_float < rhs)
		{
			return TRUE;
		}
	}
	return FALSE;
}


void EvalCondition(uint condition, Value val1, Value val2)
{
	int answer = FALSE;

	if (Val_CheckResolved2(val1, val2))
	{
		yyerror("Forward referenced symbols not allowed in conditionals.");
		return;
	}

	switch (condition)
	{
	case EQUAL:
		answer = Val_Eq(val1, val2);
		break;
	case NOT_EQUAL:
		answer = !Val_Eq(val1, val2);
		break;
	case SMALLER:
		answer = Val_Ls(val1, val2);
		break;
	case BIGGER:
		answer = Val_Ls(val2, val1);
		break;
	case SMALLER_OR_EQUAL:
		answer = Val_Ls(val1, val2) || Val_Eq(val1, val2);
		break;
	case BIGGER_OR_EQUAL:
		answer = Val_Ls(val2, val1) || Val_Eq(val1, val2);
		break;
	}

	if_stack_l++;

	if (!answer)
	{
		if (SkipConditional() == OP_ENDC)
		{
			if_stack_l--;
		}
	}
}


void EvalDefined(const char *pStr, bool invert)
{
	if_stack_l++;

	if ((NULL == FindSymbol(pStr)) ^ invert)
	{
		if (SkipConditional() == OP_ENDC)
		{
			if_stack_l--;
		}
	}
}

void GenIfError(void)
{
	if (g_passNum == 0)
	{
		yyerror("Illegal condition.");
	}

	if (SkipConditional() == OP_ELSE)
		SkipConditional();
}

void GenElse(void)
{
	SkipConditional();
	if_stack_l--;
}

int StrToInt(const char *pString)
{
	int len = strlen(pString);
	int i, val = 0;

	if (len > 3)
	{
		yywarning("Literal constant too long: \"%s\".", pString);
		len = 3;
	}

	for (i = 0; i != len; i++)
	{
		val = (val << 8) | pString[i];
	}

	return val;
}


void EvalIfOneArg(uint cond, Value val1)
{
	int answer = FALSE;
	int val = 0;

	if (Val_CheckResolved(val1))
	{
		yyerror("Forward referenced symbols not allowed in conditionals.");
		return;
	}

	val = Val_GetAsInt(val1);

	switch (cond)
	{
	case 0x1:
		answer = val >= 0 ? 1 : 0;		/* GE */
		break;
	case 0x2:
		answer = val != 0 ? 1 : 0;
		break;
	case 0x3:
		answer = val < 0 ? 0 : 1;		/* PL */
		break;
	case 0x7:
		answer = val > 0 ? 1 : 0;		/* GT */
		break;
	case 0x9:
		answer = val < 0 ? 1 : 0;		/* LT */
		break;
	case 0xa:
		answer = val == 0 ? 1 : 0;
		break;
	case 0xf:
		answer = val <= 0 ? 1 : 0;
		break;
	default:
		yyerror("Illegal IFxx opcode.");
		return;
	}

	if_stack_l++;

	if (!answer)
	{
		if (SkipConditional() == OP_ENDC)
		{
			if_stack_l--;
		}
	}
}
