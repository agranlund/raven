%option noyywrap
%{
/*
 * Copyright (C) 1990-1992 Quinn C. Jensen
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  The author makes no representations
 * about the suitability of this software for any purpose.  It is
 * provided "as is" without express or implied warranty.
 *
 */
 
 /* Modified by Sqward 2001 and 2010*/

#include <stdio.h>
#include <stdlib.h>
#include <asm_types.h>
#include <export.h>
#include <Value.h>
#include <CodeUtils.h>
#include <SymbolTable.h>
#include <Parser.h>
#include <StringBuffer.h>
#include <TokenStream.h>
#include <codegen/GenArith.h>
#include <codegen/GenMisc.h>
#include <codegen/GenBitOps.h>

#define COND(x,y) yylval.condition = x ;BEGIN(symbol);  return y;
#define INTEG(x,y) yylval.integer = x ;BEGIN(symbol);  return y;
#define PATT(x,y) yylval.pattern = x ;BEGIN(symbol);  return y;
#define NOVAL(y) BEGIN(symbol);  return y;
#define ADD_STRING(x) BEGIN(x); yylval.text.ptr = StringBufferInsert(asmtext); yylval.text.len=asmleng;
#define ADD_STRING2() yylval.text.ptr = StringBufferInsert(asmtext); yylval.text.len=asmleng;

static void ParseString(void);
static int atobin(const char *string);

#ifdef _MSC_VER
    #define strtoll( a,b,c ) _strtol_l(a, b, c, _get_current_locale());
#endif


%}

%s macro
%s symbol
%s macro_param

%a	10000
%o	10000
%e	2000
%k	2000
%p	3000

A	[Aa]
B	[Bb]
C	[Cc]
D	[Dd]
E	[Ee]
F	[Ff]
G	[Gg]
H	[Hh]
I	[Ii]
J	[Jj]
K	[Kk]
L	[Ll]
M	[Mm]
N	[Nn]
O	[Oo]
P	[Pp]
Q	[Qq]
R	[Rr]
S	[Ss]
T	[Tt]
U	[Uu]
V	[Vv]
W	[Ww]
X	[Xx]
Y	[Yy]
Z	[Zz]

ALLCHAR2 .
ALLCHAR {ALLCHAR2}*
WHITE	[ \t]+
ALCHR	[_A-Za-z]
ALPHA	{ALCHR}+
NUM	[0-9]+
FRAC ("-"|"")(({NUM}"."{NUM})|("."{NUM})|({NUM}"."{NUM}{E}("+"|"-"){NUM})|("."{NUM}{E}("+"|"-"){NUM}))
ALNUM	{ALCHR}[_A-Za-z0-9]*
HEXNUM	[0-9A-Fa-f]+
BINNUM [01]+
BW	(^|{WHITE})

%%

 /* -------------------------------------------------------------------------------------------------------------------------- */

BEGIN(macro)

 /* -------------------------------------------------------------------------------------------------------------------------- */

{F}{R}{A}{C}{T}							{	return TYPE_FRACT;																}
{F}{L}{O}{A}{T}							{	return TYPE_FLOAT;																}	
{I}{N}{T}								{	return TYPE_INT;																}

{X}:									{	return(XMEM);																	}
{Y}:									{	return(YMEM);																	}
{L}:									{	return(LMEM);																	}
{P}:									{	return(PMEM);																	}
{A}10									{	return(A10);																	}
{A}										{	return(AAAA);																	}
{B}10									{	return(B10);																	}
{B}										{	return(BBBB);																	}
{A}{B}									{	return(AABB);																	}
{B}{A}									{	return(BBAA);																	}
{C}{O}{M}								{	return(COM);																	}
{E}{O}{M}								{	return(EOM);																	}
{E}{P}									{	return _EP;																		}
{X}										{	return(XXXX);																	}
{Y}										{	return(YYYY);																	}
{S}{R}									{	return(SR);																		}
{M}{R}									{	return(MR);																		}
{C}{C}{R}								{	return(CCR);																	}
{O}{M}{R}								{	return(OMR);																	}
{S}{P}									{	return(SP);																		}
{S}{S}{H}								{	return(SSH);																	}
{S}{S}{L}								{	return(SSL);																	}
{L}{A}									{	return(LA);																		}
{L}{C}									{	return(LC);																		}
{A}[0-2]								{	yylval.reg = atoi(&yytext[1]); return(AREG);									}
{B}[0-2]								{	yylval.reg = atoi(&yytext[1]); return(BREG);									}
{M}[0-7]								{	yylval.reg = atoi(&yytext[1]); return(MREG);									}
{N}[0-7]								{	yylval.reg = atoi(&yytext[1]); return(NREG);									}
{R}[0-7]								{	yylval.reg = atoi(&yytext[1]); return(RREG);									}
{V}{B}{A}								{	return VBA;																		}
{S}{C}									{	return SC;																		}
{S}{Z}									{	return SZ;																		}
{X}[01]									{	yylval.reg = atoi(&yytext[1]); return(XREG);									}
{Y}[01]									{	yylval.reg = atoi(&yytext[1]); return(YREG);									}
"+"{WHITE}*{N}[0-7]						{	yylval.reg = atoi(&yytext[2]); return(PLUS_N);									}
"-"{WHITE}*{N}[0-7]						{	yylval.reg = atoi(&yytext[2]); return(MINUS_N);									}
"#"										{	return '#';																		}
"+"										{	return '+';																		}
"-"										{	return '-';																		}
"*"										{	return '*';																		}
"/"										{	return '/';																		}
","										{	return ',';																		}
"("										{	return '(';																		}
")"										{	return ')';																		}
":"										{	return ':';																		}
">"										{	return '>';																		}
"<"										{	return '<';																		}
"|"										{	return '|';																		}
"="										{	return '=';																		}
"!"										{	return '!';																		}
"~"										{	return '~';																		}
"&"										{	return '&';																		}
"<<"									{	return LSL;																		}
">>"									{	return LSR;																		}
<symbol>","{WHITE}*"#"					{	return TAG1;																	}
"{"										{	return OPENBRACE;																}
"}"										{	return CLOSEBRACE;																}
"\\"{NUM}								{	yylval.integer = atoi(&yytext[1]); return MACRO_PARAM;							}
"$"{HEXNUM}								{	yylval.integer = strtoll(&yytext[1], 0, 16 ); return NUM_INTEGER;				}
"0"{X}{HEXNUM}							{	yylval.integer = strtoll(&yytext[2], 0, 16 ); return NUM_INTEGER;				}
"%"{BINNUM}								{	yylval.integer = atobin(&yytext[1]); return NUM_INTEGER;						}
{BINNUM}{B}								{	yytext[strlen(yytext)-1]=0;yylval.integer = atobin(yytext);return NUM_INTEGER;  }
{FRAC}									{	yylval.fp = (double) atof(yytext); return NUM_FRACT;							}
{FRAC}{F}								{	yylval.fp = (double) atof(yytext); return NUM_FLOAT;							}
{NUM}									{	yylval.integer = strtoll(yytext, 0, 10 );  return NUM_INTEGER ;					}

 /* -------------------------------------------------------------------------------------------------------------------------- */

{A}{B}{S}               				{	INTEG(0x26,OP_ONE_PAR_INSN);		}
{A}{D}{C}               				{	INTEG(0x21,OP_ADC_SBC);				}
{A}{D}{D}               				{	PATT(add_patterns,OP_ADD_SUB);		}
{A}{D}{D}{L}            				{	INTEG(0x12,OP_ADDx_SUBx);			}
{A}{D}{D}{R}            				{	INTEG(0x2,OP_ADDx_SUBx);			}
{A}{N}{D}               				{	PATT(and_patterns,OP_AND_EOR_OR);	}
{A}{N}{D}{I}            				{	INTEG(0xb8,OP_ANDI_ORI);			}
{A}{S}{L}               				{	PATT(asl_patterns,OP_ASX);			}
{A}{S}{R}               				{	PATT(asr_patterns,OP_ASX);			}

{B}{C}{C}               				{	COND(0x0,OP_BCC);					}
{B}{H}{S}               				{	COND(0x0,OP_BCC);					}
{B}{G}{E}               				{	COND(0x1,OP_BCC);					}
{B}{N}{E}               				{	COND(0x2,OP_BCC);					}
{B}{P}{L}               				{	COND(0x3,OP_BCC);					}
{B}{N}{N}               				{	COND(0x4,OP_BCC);					}
{B}{E}{C}               				{	COND(0x5,OP_BCC);					}
{B}{L}{C}               				{	COND(0x6,OP_BCC);					}
{B}{G}{T}               				{	COND(0x7,OP_BCC);					}
{B}{C}{S}               				{	COND(0x8,OP_BCC);					}
{B}{L}{O}               				{	COND(0x8,OP_BCC);					}
{B}{L}{T}               				{	COND(0x9,OP_BCC);					}
{B}{E}{Q}               				{	COND(0xA,OP_BCC);					}
{B}{M}{I}               				{	COND(0xB,OP_BCC);					}
{B}{N}{R}               				{	COND(0xC,OP_BCC);					}
{B}{E}{S}               				{	COND(0xD,OP_BCC);					}
{B}{L}{S}               				{	COND(0xE,OP_BCC);					}

{B}{R}{A}								{	NOVAL(OP_BRA);						}
{B}{R}{C}{L}{R}							{	PATT(brclr_patterns,OP_R_BIT_XXX);	}

{B}{R}{K}{C}{C}							{	COND(0x0, OP_BRKCC);				}
{B}{R}{K}{H}{S}							{	COND(0x0, OP_BRKCC);				}
{B}{R}{K}{G}{E}							{	COND(0x1, OP_BRKCC);				}
{B}{R}{K}{N}{E}							{	COND(0x2, OP_BRKCC);				}
{B}{R}{K}{P}{L}							{	COND(0x3, OP_BRKCC);				}
{B}{R}{K}{N}{N}							{	COND(0x4, OP_BRKCC);				}
{B}{R}{K}{E}{C}							{	COND(0x5, OP_BRKCC);				}
{B}{R}{K}{L}{C}							{	COND(0x6, OP_BRKCC);				}
{B}{R}{K}{G}{T}							{	COND(0x7, OP_BRKCC);				}
{B}{R}{K}{C}{S}							{	COND(0x8, OP_BRKCC);				}
{B}{R}{K}{L}{O}							{	COND(0x8, OP_BRKCC);				}
{B}{R}{K}{L}{T}							{	COND(0x9, OP_BRKCC);				}
{B}{R}{K}{E}{Q}							{	COND(0xA, OP_BRKCC);				}
{B}{R}{K}{M}{I}							{	COND(0xB, OP_BRKCC);				}
{B}{R}{K}{N}{R}							{	COND(0xC, OP_BRKCC);				}
{B}{R}{K}{E}{S}							{	COND(0xD, OP_BRKCC);				}
{B}{R}{K}{L}{S}							{	COND(0xE, OP_BRKCC);				}

{B}{R}{S}{E}{T}							{	PATT(brset_patterns,OP_R_BIT_XXX);	}

{B}{S}{C}{C}            				{	COND(0x0,OP_BSCC);					}
{B}{S}{H}{S}            				{	COND(0x0,OP_BSCC);					}
{B}{S}{G}{E}            				{	COND(0x1,OP_BSCC);					}
{B}{S}{N}{E}            				{	COND(0x2,OP_BSCC);					}
{B}{S}{P}{L}            				{	COND(0x3,OP_BSCC);					}
{B}{S}{N}{N}            				{	COND(0x4,OP_BSCC);					}
{B}{S}{E}{C}            				{	COND(0x5,OP_BSCC);					}
{B}{S}{L}{C}            				{	COND(0x6,OP_BSCC);					}
{B}{S}{G}{T}            				{	COND(0x7,OP_BSCC);					}
{B}{S}{C}{S}            				{	COND(0x8,OP_BSCC);					}
{B}{S}{L}{O}            				{	COND(0x8,OP_BSCC);					}
{B}{S}{L}{T}            				{	COND(0x9,OP_BSCC);					}
{B}{S}{E}{Q}            				{	COND(0xA,OP_BSCC);					}
{B}{S}{M}{I}            				{	COND(0xB,OP_BSCC);					}
{B}{S}{N}{R}            				{	COND(0xC,OP_BSCC);					}
{B}{S}{E}{S}            				{	COND(0xD,OP_BSCC);					}
{B}{S}{L}{S}            				{	COND(0xE,OP_BSCC);					}

{B}{S}{C}{L}{R}         				{	PATT(bsclr_patterns,OP_R_BIT_XXX);	}
{B}{S}{R}								{	NOVAL(OP_BSR);			}
{B}{S}{S}{E}{T}         				{	PATT(bsset_patterns,OP_R_BIT_XXX);	}

{B}{C}{L}{R}            				{	PATT(bclr_patterns,OP_BIT_XXX);		}
{B}{S}{E}{T}            				{	PATT(bset_patterns,OP_BIT_XXX);		}
{B}{C}{H}{G}            				{	PATT(bchg_patterns,OP_BIT_XXX);		}
{B}{T}{S}{T}            				{	PATT(btst_patterns,OP_BIT_XXX);		}

{C}{L}{B}								{	NOVAL(OP_CLB);						}

{C}{L}{R}               				{	INTEG(0x13,OP_ONE_PAR_INSN);		}
{C}{M}{P}               				{	PATT(cmp_patterns,OP_CMP);		    }
{C}{M}{P}{M}            				{	NOVAL(OP_CMPM);						}
{C}{M}{P}{U}							{	NOVAL(OP_CMPU);						}

{D}{C}                  				{	NOVAL(OP_DC);						}
{D}{S}                  				{	NOVAL(OP_DS);						}
{D}{S}{M}                  				{	NOVAL(OP_DSM);						}
{A}{L}{I}{G}{N}                         {	NOVAL(OP_ALIGN);					}

{D}{E}{B}{U}{G}{C}{C}   				{	COND(0x0,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{H}{S}   				{	COND(0x0,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{G}{E}   				{	COND(0x1,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{N}{E}   				{	COND(0x2,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{P}{L}   				{	COND(0x3,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{N}{N}   				{	COND(0x4,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{E}{C}   				{	COND(0x5,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{L}{C}   				{	COND(0x6,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{G}{T}   				{	COND(0x7,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{C}{S}   				{	COND(0x8,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{L}{O}   				{	COND(0x8,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{L}{T}   				{	COND(0x9,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{E}{Q}   				{	COND(0xA,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{M}{I}   				{	COND(0xB,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{N}{R}   				{	COND(0xC,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{E}{S}   				{	COND(0xD,OP_DEBUGCC);				}
{D}{E}{B}{U}{G}{L}{S}   				{	COND(0xE,OP_DEBUGCC);				}

{D}{E}{B}{U}{G}							{	NOVAL(OP_DEBUG);					}
{D}{E}{C}								{	PATT(incdec_patterns,OP_DEC);		}
{D}{I}{V}               				{	NOVAL(OP_DIV);						}
{D}{M}{A}{C}{S}{S}						{	INTEG(0,OP_DMAC);					}
{D}{M}{A}{C}{S}{U}						{	INTEG(2,OP_DMAC);					}
{D}{M}{A}{C}{U}{U}						{	INTEG(3,OP_DMAC);					}

{D}{O}{R}								{	NOVAL(OP_DOR);						}
{D}{O}									{	NOVAL(OP_DO);						}
{E}{L}{S}{E}							{	NOVAL(OP_ELSE);						}
{E}{N}{D}{D}{O}							{	NOVAL(OP_ENDDO);					}

{E}{N}{D}{M}							{	NOVAL(OP_ENDM);						}
{E}{N}{D}{C}							{	NOVAL(OP_ENDC);						}
{E}{N}{D}{I}{F}							{	NOVAL(OP_ENDC);						}
{E}{N}{D}								{	BEGIN(symbol);
											if( PopFile() )
											{
												return(OP_END);
											}
											else
											{
						 						return TOKEN_LEAVE_FILE;
											}
										}
{E}{O}{R}               				{	PATT(eor_patterns,OP_AND_EOR_OR);	}
{E}{Q}{U}               				{	NOVAL(OP_EQU);						}
{E}{X}{T}{R}{A}{C}{T}{U}				{	PATT(extract_patterns,OP_EXTRACTU);	}
{E}{X}{T}{R}{A}{C}{T}					{	PATT(extract_patterns,OP_EXTRACT);	}

{E}{R}{R}{O}{R}                         {   NOVAL(OP_ERROR);                    }
{F}{A}{I}{L}                            {   NOVAL(OP_ERROR);                    }
{M}{S}{G}                               {   NOVAL(OP_MSG);                      }

{F}{O}{R}{E}{V}{E}{R}					{	NOVAL(OP_FOREVER);					}

{I}{F}{C}{C}"."{U}      				{	COND(0x0,OP_IFCCU);					}
{I}{F}{H}{S}"."{U}      				{	COND(0x0,OP_IFCCU);					}
{I}{F}{G}{E}"."{U}      				{	COND(0x1,OP_IFCCU);					}
{I}{F}{N}{E}"."{U}      				{	COND(0x2,OP_IFCCU);					}
{I}{F}{P}{L}"."{U}      				{	COND(0x3,OP_IFCCU);					}
{I}{F}{N}{N}"."{U}      				{	COND(0x4,OP_IFCCU);					}
{I}{F}{E}{C}"."{U}      				{	COND(0x5,OP_IFCCU);					}
{I}{F}{L}{C}"."{U}      				{	COND(0x6,OP_IFCCU);					}
{I}{F}{G}{T}"."{U}      				{	COND(0x7,OP_IFCCU);					}
{I}{F}{C}{S}"."{U}      				{	COND(0x8,OP_IFCCU);					}
{I}{F}{L}{O}"."{U}      				{	COND(0x8,OP_IFCCU);					}
{I}{F}{L}{T}"."{U}      				{	COND(0x9,OP_IFCCU);					}
{I}{F}{E}{Q}"."{U}      				{	COND(0xA,OP_IFCCU);					}
{I}{F}{M}{I}"."{U}      				{	COND(0xB,OP_IFCCU);					}
{I}{F}{N}{R}"."{U}      				{	COND(0xC,OP_IFCCU);					}
{I}{F}{E}{S}"."{U}      				{	COND(0xD,OP_IFCCU);					}
{I}{F}{L}{S}"."{U}      				{	COND(0xE,OP_IFCCU);					}

{I}{F}{C}{C}            				{	COND(0x0,OP_IFCC);					}
{I}{F}{H}{S}            				{	COND(0x0,OP_IFCC);					}
{I}{F}{G}{E}            				{	COND(0x1,OP_IFCC);					}
{I}{F}{N}{E}            				{	COND(0x2,OP_IFCC);					} 
{I}{F}{P}{L}            				{	COND(0x3,OP_IFCC);					}
{I}{F}{N}{N}            				{	COND(0x4,OP_IFCC);					}
{I}{F}{E}{C}            				{	COND(0x5,OP_IFCC);					}
{I}{F}{L}{C}            				{	COND(0x6,OP_IFCC);					}
{I}{F}{G}{T}            				{	COND(0x7,OP_IFCC);					}
{I}{F}{C}{S}            				{	COND(0x8,OP_IFCC);					}
{I}{F}{L}{O}            				{	COND(0x8,OP_IFCC);					}
{I}{F}{L}{T}            				{	COND(0x9,OP_IFCC);					}
{I}{F}{E}{Q}            				{	COND(0xA,OP_IFCC);					}
{I}{F}{M}{I}            				{	COND(0xB,OP_IFCC);					}
{I}{F}{N}{R}            				{	COND(0xC,OP_IFCC);					}
{I}{F}{E}{S}            				{	COND(0xD,OP_IFCC);					}
{I}{F}{L}{S}            				{	COND(0xE,OP_IFCC);					}
{I}{F}{L}{E}            				{	COND(0xF,OP_IFCC);					}


{I}{F}									{	NOVAL(OP_IF);						}
{I}{F}{D}{E}{F}							{	NOVAL(OP_IFDEF);					}
{I}{F}{N}{D}{E}{F}						{	NOVAL(OP_IFNDEF);					}

{I}{L}{L}{E}{G}{A}{L}					{	NOVAL(OP_ILLEGAL);					}
{I}{N}{C}{L}{U}{D}{E} 					{
											if ( IncludeFile() )
											{
												NOVAL(TOKEN_ENTER_FILE);
											}
										}
{I}{N}{C}								{	PATT(incdec_patterns,OP_INC);		}
{I}{N}{S}{E}{R}{T}						{	PATT(extract_patterns,OP_INSERT);	}

{J}{C}{C}               				{	COND(0x0,OP_JCC);					}
{J}{H}{S}               				{	COND(0x0,OP_JCC);					}
{J}{G}{E}               				{	COND(0x1,OP_JCC);					}
{J}{N}{E}               				{	COND(0x2,OP_JCC);					}
{J}{P}{L}               				{	COND(0x3,OP_JCC);					}
{J}{N}{N}               				{	COND(0x4,OP_JCC);					}
{J}{E}{C}               				{	COND(0x5,OP_JCC);					}
{J}{L}{C}               				{	COND(0x6,OP_JCC);					}
{J}{G}{T}               				{	COND(0x7,OP_JCC);					}
{J}{C}{S}               				{	COND(0x8,OP_JCC);					}
{J}{L}{O}               				{	COND(0x8,OP_JCC);					}
{J}{L}{T}               				{	COND(0x9,OP_JCC);					}
{J}{E}{Q}               				{	COND(0xA,OP_JCC);					}
{J}{M}{I}               				{	COND(0xB,OP_JCC);					}
{J}{N}{R}               				{	COND(0xC,OP_JCC);					}
{J}{E}{S}               				{	COND(0xD,OP_JCC);					}
{J}{L}{S}               				{	COND(0xE,OP_JCC);					}
{J}{L}{E}               				{	COND(0xF,OP_JCC);					}

{J}{M}{P}               				{	NOVAL(OP_JMP);						}

{J}{C}{L}{R}            				{	PATT(jclr_patterns,OP_J_BIT_XXX);	}
{J}{S}{E}{T}            				{	PATT(jset_patterns,OP_J_BIT_XXX);	}

{J}{S}{C}{C}            				{	COND(0x0,OP_JSCC);					}
{J}{S}{H}{S}            				{	COND(0x0,OP_JSCC);					}
{J}{S}{G}{E}            				{	COND(0x1,OP_JSCC);					}
{J}{S}{N}{E}            				{	COND(0x2,OP_JSCC);					}
{J}{S}{P}{L}            				{	COND(0x3,OP_JSCC);					}
{J}{S}{N}{N}            				{	COND(0x4,OP_JSCC);					}
{J}{S}{E}{C}            				{	COND(0x5,OP_JSCC);					}
{J}{S}{L}{C}            				{	COND(0x6,OP_JSCC);					}
{J}{S}{G}{T}            				{	COND(0x7,OP_JSCC);					}
{J}{S}{C}{S}            				{	COND(0x8,OP_JSCC);					}
{J}{S}{L}{O}            				{	COND(0x8,OP_JSCC);					}
{J}{S}{L}{T}            				{	COND(0x9,OP_JSCC);					}
{J}{S}{E}{Q}            				{	COND(0xA,OP_JSCC);					}
{J}{S}{M}{I}            				{	COND(0xB,OP_JSCC);					}
{J}{S}{N}{R}            				{	COND(0xC,OP_JSCC);					}
{J}{S}{E}{S}            				{	COND(0xD,OP_JSCC);					}
{J}{S}{L}{S}            				{	COND(0xE,OP_JSCC);					}
{J}{S}{L}{E}            				{	COND(0xF,OP_JSCC);					}

{J}{S}{R}               				{	NOVAL(OP_JSR);						}
{J}{S}{C}{L}{R}         				{	PATT(jsclr_patterns,OP_J_BIT_XXX);	}
{J}{S}{S}{E}{T}         				{	PATT(jsset_patterns,OP_J_BIT_XXX);	}

{L}{R}{A}								{	NOVAL(OP_LRA);						}

{L}{S}{L}               				{	PATT(lsl_patterns,OP_LSX);			}
{L}{S}{R}               				{	PATT(lsr_patterns,OP_LSX);			}

{L}{U}{A}               				{	NOVAL(OP_LUA);						}
 /*{L}{E}{A}               				{	NOVAL(OP_LEA);						}*/

{M}{A}{C}{S}{U}         				{	INTEG(0,OP_MACxx);					}
{M}{A}{C}{U}{U}         				{	INTEG(1,OP_MACxx);					}
{M}{A}{C}{R}{O}							{	NOVAL(OP_MACRO);					}

{M}{A}{C}{I}            				{	NOVAL(OP_MACI);						}
{M}{A}{C}               				{	NOVAL(OP_MAC);						}
{M}{A}{C}{R}            				{	NOVAL(OP_MACR);						}
{M}{A}{C}{R}{I}         				{	NOVAL(OP_MACRI);					}
{M}{P}{Y}               				{	NOVAL(OP_MPY);						}

{M}{P}{Y}{S}{U}         				{	INTEG(0,OP_MPYxx);					}
{M}{P}{Y}{U}{U}         				{	INTEG(1,OP_MPYxx);					}

{M}{P}{Y}{R}            				{	NOVAL(OP_MPYR);						}

{M}{A}{X}{M}							{	NOVAL(OP_MAXM);						}
{M}{A}{X}								{	NOVAL(OP_MAX);						}
{M}{E}{R}{G}{E}							{	NOVAL(OP_MERGE);					}

{M}{O}{V}{E}            				{	NOVAL(OP_MOVE);						}
{M}{O}{V}{E}{C}         				{	NOVAL(OP_MOVEC);					}
{M}{O}{V}{E}{M}         				{	NOVAL(OP_MOVEM);					}
{M}{O}{V}{E}{P}         				{	NOVAL(OP_MOVEP);					}

{N}{E}{G}               				{	INTEG(0x36,OP_ONE_PAR_INSN);		}
{N}{O}{P}               				{	INTEG(0,OP_NONE_PAR_INSN);			}
{N}{O}{R}{M}            				{	BEGIN(symbol); return(OP_NORM);		}
{N}{O}{T}               				{	INTEG(0x17,OP_ONE_PAR_INSN);		}

{O}{R}                  				{	PATT(or_patterns,OP_AND_EOR_OR);	}
{O}{R}{I}               				{	INTEG(0xf8,OP_ANDI_ORI);			}

{P}{F}{L}{U}{S}{H}						{	INTEG(0x3,OP_NONE_PAR_INSN);		}
{P}{F}{L}{U}{S}{H}{U}{N}				{	INTEG(0x1,OP_NONE_PAR_INSN);		}
{P}{F}{R}{E}{E}							{	INTEG(0x2,OP_NONE_PAR_INSN);		}

{P}{L}{O}{C}{K}							{	INTEG(0xbc081,OP_PLOCK_PUNLOCK);	}
{P}{L}{O}{C}{K}{R}						{	INTEG(0xf,OP_PLOCKR_PUNLOCKR);		}
{P}{U}{N}{L}{O}{C}{K}					{	INTEG(0xac081,OP_PLOCK_PUNLOCK);	}
{P}{U}{N}{L}{O}{C}{K}{R}				{	INTEG(0xe,OP_PLOCKR_PUNLOCKR);		}

{R}{N}{D}               				{	INTEG(0x11,OP_ONE_PAR_INSN);		}
{R}{O}{L}               				{	INTEG(0x37,OP_ONE_PAR_INSN);		}
{R}{O}{R}               				{	INTEG(0x27,OP_ONE_PAR_INSN);		}

{R}{E}{P}               				{	BEGIN(symbol); return(OP_REP);		}
{R}{E}{P}{E}{A}{T}               		{	BEGIN(symbol); return(OP_REPT);		}

{R}{E}{S}{E}{T}         				{	INTEG(0x84,OP_NONE_PAR_INSN);		}
{R}{T}{I}               				{	INTEG(0x4,OP_NONE_PAR_INSN);		}
{R}{T}{S}               				{	INTEG(0xc,OP_NONE_PAR_INSN);		}

{S}{B}{C}               				{	INTEG(0x25,OP_ADC_SBC);				}
{S}{E}{T}               				{	BEGIN(symbol); return(OP_SET);		}
{S}{T}{O}{P}            				{	INTEG(0x87,OP_NONE_PAR_INSN);		}

{S}{U}{B}               				{	PATT(sub_patterns,OP_ADD_SUB);		}
{S}{U}{B}{L}            				{	INTEG(0x16,OP_ADDx_SUBx);			}
{S}{U}{B}{R}            				{	INTEG(0x6,OP_ADDx_SUBx);			}

{S}{W}{I}               				{	INTEG(0xc,OP_NONE_PAR_INSN);		}

{T}{C}{C}               				{	COND(0x0,OP_TCC);					}
{T}{H}{S}               				{	COND(0x0,OP_TCC);					}
{T}{G}{E}               				{	COND(0x1,OP_TCC);					}
{T}{N}{E}               				{	COND(0x2,OP_TCC);					}
{T}{P}{L}               				{	COND(0x3,OP_TCC);					}
{T}{N}{N}               				{	COND(0x4,OP_TCC);					}
{T}{E}{C}               				{	COND(0x5,OP_TCC);					}
{T}{L}{C}               				{	COND(0x6,OP_TCC);					}
{T}{G}{T}               				{	COND(0x7,OP_TCC);					}
{T}{C}{S}               				{	COND(0x8,OP_TCC);					}
{T}{L}{O}               				{	COND(0x8,OP_TCC);					}
{T}{L}{T}               				{	COND(0x9,OP_TCC);					}
{T}{E}{Q}               				{	COND(0xA,OP_TCC);					}
{T}{M}{I}               				{	COND(0xB,OP_TCC);					}
{T}{N}{R}               				{	COND(0xC,OP_TCC);					}
{T}{E}{S}               				{	COND(0xD,OP_TCC);					}
{T}{L}{S}               				{	COND(0xE,OP_TCC);					}
{T}{L}{E}               				{	COND(0xF,OP_TCC);					}

{T}{F}{R}               				{	NOVAL(OP_TFR);						}
{T}{R}{A}{P}							{	INTEG(0x6,OP_NONE_PAR_INSN);		}

{T}{R}{A}{P}{C}{C}      				{	COND(0x0,OP_TRAPCC);				}
{T}{R}{A}{P}{H}{S}      				{	COND(0x0,OP_TRAPCC);				}
{T}{R}{A}{P}{G}{E}      				{	COND(0x1,OP_TRAPCC);				}
{T}{R}{A}{P}{N}{E}      				{	COND(0x2,OP_TRAPCC);				}
{T}{R}{A}{P}{P}{L}      				{	COND(0x3,OP_TRAPCC);				}
{T}{R}{A}{P}{N}{N}      				{	COND(0x4,OP_TRAPCC);				}
{T}{R}{A}{P}{E}{C}      				{	COND(0x5,OP_TRAPCC);				}
{T}{R}{A}{P}{L}{C}      				{	COND(0x6,OP_TRAPCC);				}
{T}{R}{A}{P}{G}{T}      				{	COND(0x7,OP_TRAPCC);				}
{T}{R}{A}{P}{C}{S}      				{	COND(0x8,OP_TRAPCC);				}
{T}{R}{A}{P}{L}{O}	    				{	COND(0x8,OP_TRAPCC);				}
{T}{R}{A}{P}{L}{T}      				{	COND(0x9,OP_TRAPCC);				}
{T}{R}{A}{P}{E}{Q}      				{	COND(0xA,OP_TRAPCC);				}
{T}{R}{A}{P}{M}{I}      				{	COND(0xB,OP_TRAPCC);				}
{T}{R}{A}{P}{N}{R}      				{	COND(0xC,OP_TRAPCC);				}
{T}{R}{A}{P}{E}{S}      				{	COND(0xD,OP_TRAPCC);				}
{T}{R}{A}{P}{L}{S}      				{	COND(0xE,OP_TRAPCC);				}
{T}{R}{A}{P}{L}{E}      				{	COND(0xF,OP_TRAPCC);				}

{T}{S}{T}               				{	INTEG(0x03,OP_ONE_PAR_INSN);		}
{V}{S}{L}								{	NOVAL(OP_VSL);						}
{W}{A}{I}{T}            				{	INTEG(0x86,OP_NONE_PAR_INSN);		}
({O}{R}{G})             				{	NOVAL(OP_ORG);						}
{P}{A}{G}{E}            				{	NOVAL(OP_PAGE);						}
{G}{R}{P}			    				{	NOVAL(OP_GROUPING);					}
{G}{R}{P}{E}{N}{D}	    				{	NOVAL(OP_GROUPINGEND);				}

 /* -------------------------------------------------------------------------------------------------------------------------- */

"\""									{ ParseString(  ) ; return OP_STRING ;  }
"\'"						            { ParseString(  ) ; return OP_STRING2 ; }

 /* -------------------------------------------------------------------------------------------------------------------------- */

{WHITE}	;
{ALNUM}/":"					            {	ADD_STRING(macro); return SYM2;         }
^"@"{ALNUM}			                    {	ADD_STRING(macro); return SYM2;         }
^"."{ALNUM}			                    {	ADD_STRING(macro); return SYM2;         }
^"_"{ALNUM}			                    {	ADD_STRING(macro); return SYM2;         }
^{ALNUM}			                    {	ADD_STRING(macro); return SYM2;         }

<macro>{ALNUM}			                { ADD_STRING(macro_param);return OP_MACROCALL; }

<macro_param>{ALNUM}	                {	ADD_STRING2(); return SYM;             }
"@"{ALNUM}		                        {	ADD_STRING2(); return SYM;             }
"."{ALNUM}		                        {	ADD_STRING2(); return SYM;             }
"_"{ALNUM}		                        {	ADD_STRING2(); return SYM;             }
{ALNUM}			                        {	ADD_STRING2(); return SYM;             }

 /* -------------------------------------------------------------------------------------------------------- */

";"{ALLCHAR}			{ }
"//"{ALLCHAR}			{ }
"/*"            		{
                            int nestLevel = 0;
                            int first   = 0;

                            while ( 1 )
                            {
                                int sym = 0;
                                int second = input();

                                if ( second == YYEOF )
                                {
                                    yyerror("Unexpected end of file in C-style comment.");
								    break;
                                }

								
                                if ( second == '\n' )
                                {
									g_currentLine++;
								}

                                sym = (first << 8) | second;
								(void) sym;

                                if ( first == '/' && second == '*' )
                                {
                                    nestLevel ++;
                                }
                                else if ( first == '*' && second == '/' )
                                {
                                    if ( nestLevel == 0 )
                                    {
                                        break;
                                    }
                                    --nestLevel ;
                                }
                                first = second;
                            }
                            yylval.integer = g_currentLine ;
                            BEGIN(macro);  
                            return EOL;	
                        }


 /* -------------------------------------------------------------------------------------------------------- */

"\n"					{	
                            g_currentLine++;  
                            yylval.integer = g_currentLine ;
                            BEGIN(macro);  
                            return EOL;	
                        }

<<EOF>>					{
							if( PopFile() )
							{
								return OP_END;
							}
							else
							{
								return TOKEN_LEAVE_FILE;
							}
						 }

%%
 /* -------------------------------------------------------------------------------------------------------- */
/* make input() visible outside lexer. */
int Input(void) { return input(); }
void Unput(int c) { unput(c); }

static int atobin(const char *string)
{
	int temp = 0;
	int cnt = 24;

	do
	{
		temp <<= 1;
		temp |= (*string++) & 1;
		cnt--;
		if (cnt <= 0)
		{
			yyerror("Binary constant out of range.");
			return temp;
		}

	} while (*string != 0);

	return temp;
}


static void ParseString(void)
{
	char string[512];
	char *sptr = string;
	char c;
	char quote = asmtext[0];
	int len_str = 0;

	for (;;)
	{
		c = input();

		if (c == quote)
		{
			*sptr = '\0';
			break;
		} else if (c == YYEOF)
		{
			yyerror("String spans the input file.");
			break;
		} else if (c == '\n')
		{
			yyerror("Unterminated quotation.");
			g_currentLine++;
			break;
		} else
		{
			*sptr++ = c;
			len_str++;
		}

		if (len_str >= (sizeof(string) - 1))
		{
			yyerror("String too long.");
			*sptr = '\0';
			break;
		}
	}

	yylval.text.ptr = StringBufferInsert(string);
	yylval.text.len = len_str;
}
