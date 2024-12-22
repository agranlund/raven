#ifndef __WARNINGS_H__
#define __WARNINGS_H__

typedef enum {
	WARN_ANY,			/* internal warnings */
/* warnings used by Pure-C: */
	WARN_DUP,			/* "Redefinition of 'XXXXXX' is not identical." */
	WARN_RET,			/* "Both return and return of a value used." */
	WARN_STR,			/* "'XXXXXX' not part of structure." */
	WARN_STU,			/* "Undefined structure 'XXXXXX'." */
	WARN_SUS,			/* "Suspicious pointer conversion." */
	WARN_VOI,			/* "Void functions may not return a value." */
	WARN_ZST,			/* "Zero length structure." */
	WARN_AUS,			/* "'XXXXXX' is assigned a value which is never used." */
	WARN_DEF,			/* "Possible use of 'XXXXXX' before definition." */
	WARN_EFF,			/* "Code has no effect." */
	WARN_PAR,			/* "Parameter 'XXXXXX' is never used." */
	WARN_PIA,			/* "Possible incorrect assignment." */
	WARN_RCH,			/* "Unreachable code." */
	WARN_RVL,			/* "Function should return a value." */
	WARN_AMB,			/* "Ambiguous operators need parentheses." */
	WARN_AMP,			/* "Superfluous & with function or array." */
	WARN_MLT,			/* "Hexadecimal constant too large." */
	WARN_NOD,			/* "Implicit declaration of function 'XXXXXX'." */
	WARN_PRO,			/* "Call to function 'XXXXXX' with no prototype." */
	WARN_STV,			/* "Structure passed by value." */
	WARN_USE,			/* 'XXXXXX' declared but never used." */
	WARN_APT,			/* "Non portable pointer conversion." */
	WARN_CLN,			/* "Constant is long." */
	WARN_CPT,			/* "Non portable pointer comparision." */
	WARN_RNG,			/* "Constant out of range in comparison." */
	WARN_SIG,			/* "Conversion may lose significant digits." */
	WARN_UCP,			/* "Mixing pointers to signed and unsigned char." */
	WARN_RPT,			/* "Non portable pointer assignment." */
	WARN_ASM,			/* "Unknown assembler instruction" */
	WARN_STK,			/* */
	WARN_PC_LAST = WARN_STK,
/* AHCC specific warnings: */
	WARN_CON,			/* "uninitialized const object" */
	WARN_GOT,			/* "'goto' used */
	WARN_ASC,			/* "ascii constant too wide" */
	WARN_MUL,			/* "multi-character character constant" */
	WARN_PRC,			/* "current C does not support local procedures" */
	WARN_NST,			/* "/ * within comment" */
	WARN_CPP,			/* "C++ style comments are not allowed in ISO C90" */
	WARN_DEC,			/* "type defaults to 'int' in declaration of '%s' object" */
	WARN_CST,			/* "Cast pointer constant to real" */
	WARN_ASS,			/* "'%s' is a assembler directive" */
	WARN_PMA,			/* "unknown pragma" */
	WARN_PCC,			/* several */
	WARN_AHCC_LAST = WARN_PCC,
/* Assembler warnings: */
	WARN_LBL,			/* "'%%N' needs constant expression or register name" */
	WARN_DIF,			/* "can not diff %%N with advance ref" */
	WARN_2ND,			/* "Assembler needs second phase" */
	WARN_XNL,			/* "Xn defaults to .l for Coldfire" */
	WARN_MAX
} warning_category;

typedef struct {
	warning_category category;
	char short_switch[4];
	int level;
	const char *long_switch; /* should follow gnu warning names; NYI */
	const char *msg;
} warning;

extern warning const warnings[];

#endif /* __WARNINGS_H__*/
