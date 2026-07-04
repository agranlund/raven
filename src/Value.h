/*

Project:    asm56k
Author:     M.Buras (sqward)

*/
#ifndef _VALUE_H_
#define _VALUE_H_

#define EQUAL 0
#define SMALLER 1
#define BIGGER 2
#define SMALLER_OR_EQUAL 3
#define BIGGER_OR_EQUAL 4
#define NOT_EQUAL 5

extern long asci2frac(char* string, int mode);

enum ValueType 
{
    kUnresolved = 0,
	kInt,
	kFract,
	kFloat
};

typedef struct 
{
	enum ValueType  m_type;

	union 
    {
		u64     m_int;
		double  m_float;
	} m_value;

} Value;

Value Val_CastToInt(Value val);
Value Val_CastToFloat(Value val);
Value Val_CastToFract(Value val);

int	Val_GetAsInt(Value val);

u32	Val_GetAsFract24(Value val);
u64	Val_GetAsFract48(Value val);

Value Val_Add(Value val1, Value val2);
Value Val_Sub(Value val1, Value val2);
Value Val_Mul(Value val1, Value val2);
Value Val_Div(Value val1, Value val2);
Value Val_Neg(Value val1);
Value Val_Not(Value val1);
Value Val_BinNot(Value val1);
Value Val_And(Value val1, Value val2);
Value Val_Xor(Value val1, Value val2);
Value Val_Or(Value val1, Value val2);

Value Val_Lsr(Value val1, Value val2);
Value Val_Lsl(Value val1, Value val2);

bool Val_Eq(Value val1, Value val2);
bool Val_Ls(Value val1, Value val2);
bool Val_Gt(Value val1, Value val2);
bool Val_Le(Value val1, Value val2);
bool Val_Gte(Value val1, Value val2);

bool Val_CheckResolved(Value val2);
bool Val_CheckResolved2(Value val1, Value val2);

Value Val_CreateUnresolved(void);
Value Val_CreateFloat(double val);
Value Val_CreateInt(s64 val);
Value Val_CreateFract(double val);

void EvalCondition(uint condition, Value val1, Value val2);
void EvalDefined(const char* pStr, bool invert);
void GenIfError(void);
void GenElse(void);
void EvalIfOneArg(uint cond, Value val);

int StrToInt(const char* pString);

#endif /* _VALUE_H_ */
