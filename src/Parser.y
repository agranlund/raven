%{
/*

Project:    asm56k
Author:     M.Buras (sqward)

*/

#include <stdio.h>
#include <asm_types.h>
#include <export.h>
#include <Value.h>
#include <ErrorMessages.h>
#include <math.h>
#include <CodeUtils.h>
#include <SymbolTable.h>
#include <Parser.h>
#include <OutputLod.h>
#include <ConvertFields.h>
#include <TokenStream.h>
#include <MacroProxy.h>
#include <PipeLineRestriction.h>

#include <codegen/GenMisc.h>
#include <codegen/GenParMoves.h>
#include <codegen/GenArith.h>
#include <codegen/GenBitOps.h>


%}

%union{
    Value val;
    u64 integer;
    const int *pattern;
    int reg;
    double fp;
    hs* label;
    stext text;
    int condition;
    bcode opcode;
    raddr rel_addr;
};


%start input

/*
 * non terminals
 */

%type <opcode> _opcode ea imm_ea par_move

%type <val> exp
%type <label> label
%type <rel_addr> rel_target

%type <integer> ddddd exp_int MMRRR X_or_Y QQQQ plus_minus QQQ SH condition dc_params mem_space

%left '&' ',' '|' '^' LSL LSR 
%left '+' '-' '~' '!'
%left '*' '/'

/*
 * terminals
 */

%token <integer> NUM_INTEGER
%token <fp> NUM_FRACT NUM_FLOAT
%token <integer> COMMENT XMEM YMEM LMEM PMEM EOS EOL
%token <integer> A10 AAAA B10 BBBB AABB BBAA XXXX YYYY SR MR CCR OMR SP SSH SSL LA LC EOM COM _EP VBA SC SZ
%token <reg> AREG BREG MREG NREG RREG XREG YREG PLUS_N MINUS_N
%token <integer> OPENBRACE CLOSEBRACE LSL LSR

%token <pattern> OP_ADD_SUB OP_CMP OP_AND_EOR_OR OP_ASX OP_LSX OP_BIT_XXX OP_R_BIT_XXX
%token <pattern>  OP_J_BIT_XXX OP_DEC OP_EXTRACT OP_EXTRACTU OP_INC OP_INSERT
%token <condition> OP_BCC OP_BRKCC OP_BSCC OP_DEBUGCC OP_IFCC OP_IFCCU OP_JCC OP_JSCC OP_TCC OP_TRAPCC
%token <integer> OP_ONE_PAR_INSN OP_NONE_PAR_INSN OP_ADC_SBC OP_ADDx_SUBx OP_ABS OP_ADC OP_ADD OP_AND OP_ANDI_ORI
%token <integer> OP_ASL OP_ASR OP_BCHG OP_BCLR OP_BRA OP_BRCLR OP_BRSET OP_BSCLR OP_BSET OP_BSR OP_BSSET OP_BTST
%token <integer> OP_CLB OP_CLR OP_CMPM OP_CMPU OP_DEBUG OP_DIV OP_DMAC OP_DO OP_DOR OP_ENDDO OP_EOR
%token <integer> OP_ILLEGAL OP_JCLR OP_JMP OP_JSCLR OP_JSET OP_JSR OP_JSSET OP_LRA
%token <integer> OP_LSL OP_LSR OP_LUA OP_MAC OP_MACI OP_MACxx OP_MACR OP_MACRI OP_MAX OP_MAXM OP_MERGE OP_MOVE
%token <integer> OP_MOVEC OP_MOVEM OP_MOVEP OP_MPY OP_MPYxx OP_MPYI OP_MPYR OP_MPYRI OP_NEG OP_NOP OP_NORM OP_NORMF
%token <integer> OP_NOT OP_OR OP_PFLUSH OP_PFLUSHUN OP_PFREE OP_PLOCK_PUNLOCK OP_PLOCKR_PUNLOCKR OP_REP OP_RESET
%token <integer> OP_RND OP_ROL OP_ROR OP_RTI OP_RTS OP_SBC OP_STOP OP_SUB OP_SWI OP_TFR OP_TRAP
%token <integer> OP_TST OP_VSL OP_WAIT OP_EQU OP_INCLUDE OP_SET OP_MACRO OP_ENDM OP_END OP_ORG OP_ORG2 OP_PAGE
%token <integer> OP_DC OP_GROUPING OP_GROUPINGEND TAG1 OP_IF OP_IFDEF OP_IFNDEF OP_ENDC OP_ELSE MACRO_PARAM MACRO_SYM
%token <integer> TYPE_FRACT TYPE_FLOAT TYPE_INT OP_FOREVER OP_DS OP_DSM OP_REPT OP_ERROR OP_MSG OP_ALIGN
%token <text> OP_MACROCALL OP_STRING OP_STRING2 SYM SYM2                 

/* This stuff is not used directly in parsing */
%token TOKEN_NO_VAL TOKEN_INT_VAL TOKEN_STRING TOKEN_MACRO_PARAM TOKEN_NEW_PTR TOKEN_ENTER_FILE TOKEN_LEAVE_FILE

%%
input   :	/* empty */
        |	input line
        ;

line    :	EOL                                         {	g_errorLine = g_currentLine;	}
        |	label OP_END                                {   YYACCEPT;               		}
        |	OP_END                                      {   YYACCEPT;               		}
        |	label OP_MACROCALL                          {   MacroCall($2.ptr);      		}
        |	OP_MACROCALL                                {   MacroCall($1.ptr);      		}
        |	label EOL                                   {   g_errorLine = g_currentLine;	}
        |	_opcode EOL                                 {	g_errorLine = g_currentLine;	}
        |	_opcode OP_END                              {   YYACCEPT;               		}
        |	label _opcode EOL							{	g_errorLine = g_currentLine;	}
        |	label _opcode OP_END						{	YYACCEPT;						}
		|	label OP_EQU exp OP_END 					{	SymSet($1->pString,$3); 		}
		|	label OP_EQU exp EOL						{	SymSet($1->pString,$3); g_errorLine = g_currentLine; }
		|	label OP_MACRO OP_END						{	MacroRecord($1->pString);		}
		|	label OP_MACRO EOL							{	MacroRecord($1->pString); g_errorLine = g_currentLine; }

		|	label OP_DSM exp							{	GenDSM( $1, $3 );				}
		|	OP_DSM	exp 								{	GenDSM( NULL, $2 ); 			}
		|	OP_SET SYM ',' exp							{	SymSet($2.ptr,$4);				}

		|	OP_MACRO OP_END 							{	MacroError();					}
		|	OP_MACRO EOL								{	MacroError(); g_errorLine = g_currentLine; }
		|	OP_ERROR OP_STRING							{	
															if (g_passNum != 0) 
																yyerror($2.ptr);
														}
		|	OP_MSG OP_STRING							{	
															if (g_passNum != 0)
																yywarning($2.ptr);
														}
		|	error EOL									{	g_errorLine = g_currentLine; }
		;

label	:	SYM ':' 									{	$$ = AddLabel( &$1 );		  }
		|	SYM2 ':'									{	$$ = AddLabel( &$1 );		  }
		|	SYM2										{	$$ = AddLabel( &$1 );		  }
		;


condition
		:  '='														{	 $$=EQUAL;				}
		|	'<' 													{	 $$=SMALLER;			}
		|	'>' 													{	 $$=BIGGER; 			}
		|	'<' '=' 												{	 $$=SMALLER_OR_EQUAL;	}
		|	'>' '=' 												{	 $$=BIGGER_OR_EQUAL;	}
		|	'!' '=' 												{	 $$=NOT_EQUAL;			}
		;


dc_params
		:	exp 													{ GenDc($1);					}
		|	OP_STRING												{ InsertString($1.ptr,$1.len);	}
		|	dc_params ',' dc_params
		;

mem_space
		:	XMEM													{	$$=X_MEM;					}
		|	YMEM													{	$$=Y_MEM;					}
		|	PMEM													{	$$=P_MEM;					}
		|	LMEM													{	$$=L_MEM;					}
		;


_opcode 
		:	OP_DS	exp 											{	GenDS( $2 );							}
		|	OP_ALIGN exp											{	GenAlign( $2 ); 						}
		|	OP_IFCC exp 											{	EvalIfOneArg( $1, $2  );				}
		|	OP_IF	exp condition exp								{	EvalCondition($3,$2,$4);				}
		|	OP_IF	exp 											{	EvalIfOneArg(0x2,$2);					}
		|	OP_IFDEF SYM											{	EvalDefined($2.ptr,FALSE);				}
		|	OP_IFNDEF SYM											{	EvalDefined($2.ptr,TRUE);				}
		|	OP_IF	exp error exp									{	GenIfError();							}
		|	OP_ELSE 												{	GenElse();								}
		|	OP_ENDC 												{	if_stack_l--;							}
		|	OP_ORG	mem_space exp_int								{	PipeLineReset(); GenOrg($2, $3);		}
		|	OP_DC	dc_params										{											}
		|	code													{	CheckCodeInLMem(); PipeLineNewInst();	}
		;

code	
		:	OP_ONE_PAR_INSN ddddd par_move							{	GenOneParamParMove($1,$2,&$3);			}
		|	OP_AND_EOR_OR ddddd ',' ddddd par_move					{	GenAndEorOr($1[0],$2,$4,&$5);			}
		|	OP_AND_EOR_OR '#' exp_int	',' ddddd					{	GenAddSubEorOrLong( $1[2], $3, $5 );	}	/* 56301 extension */
		|	OP_AND_EOR_OR '#' '<' exp_int ',' ddddd 				{	GenAddSubEorOrShort( $1[1], $4, $6 );	}	/* 56301 extension */
		|	OP_ADC_SBC ddddd ',' ddddd par_move 					{	GenAdcSbc($1,$2,$4,&$5);				}
		|	OP_ADD_SUB ddddd ',' ddddd par_move 					{	GenAddSub($1[0],$2,$4,&$5); 			}
		|	OP_CMP ddddd ',' ddddd par_move 						{	GenCmp($1[0],$2,$4,&$5);				}
		|	OP_ADD_SUB '#' exp_int	',' ddddd						{	GenAddSubEorOrLong( $1[2], $3, $5 );	}	/* 56301 extension */
		|	OP_ADD_SUB '#' '<' exp_int ',' ddddd					{	GenAddSubEorOrShort( $1[1], $4, $6 );	}	/* 56301 extension */
		|	OP_ADDx_SUBx ddddd ',' ddddd par_move					{	GenAddxSubx( $1,$2, $4, &$5 );			}	/* 56301 extension	*/
		|	OP_ANDI_ORI '#' exp_int ',' ddddd						{	GenAndiOri($1,$3,$5);					}	/* 56301 extension	*/
		|	OP_ASX ddddd par_move									{	GenOneParamParMove($1[0],$2,&$3);		}
		|	OP_ASX '#' exp_int ',' ddddd ',' ddddd					{	GenAsxImmediate($1[1],$3,$5,$7);		}	/* 56301 extension	*/
		|	OP_ASX ddddd ',' ddddd ',' ddddd						{	GenAsxReg($1[2],$2,$4,$6);				}	/* 56301 extension	*/
		|	OP_BCC rel_target										{	GenBccRelTarger($1,&$2);				}	/* 56301 extension	*/
		|	OP_BIT_XXX '#' exp_int ',' X_or_Y ea					{	GenBitOp($1,$3,$5,&$6); 				}
		|	OP_BIT_XXX '#' exp_int ',' ddddd						{	GenBitOpReg($1[5],$3,$5);				}
		|	OP_BRA rel_target										{	GenBraRelTarger(&$2);					}	/* 56301 extension	*/
		|	OP_R_BIT_XXX '#' exp_int ',' X_or_Y ea ',' rel_target	{	GenJccBitRel( $1, $3, $5, &$6,&$8); 	}
		|	OP_R_BIT_XXX '#' exp_int ',' ddddd ',' rel_target		{	GenJccBitRelReg( $1[5], $3, $5, &$7);	}	/* 56301 extension? */
		|	OP_BRKCC												{	GenBrkCC($1);							}	/* 56301 extension	*/
		|	OP_BSCC rel_target										{	GenBscc($1, &$2);						}	/* 56301 extension	*/
		|	OP_BSR rel_target										{	GenBsr(&$2);							}	/* 56301 extension	*/
		|	OP_CLB ddddd ',' ddddd									{	GenClb($2,$4);							}	/* 56301 extension	*/
		|	OP_CMPM ddddd ',' ddddd par_move						{	GenCmpm($2, $4,&$5);					}
		|	OP_CMPU ddddd ',' ddddd 								{	GenCmpu($2, $4);						}	/* 56301 extension	*/
		|	OP_DEBUG												{	GenDebug(0x200,0);						}	/* 56301 extension	*/
		|	OP_DEBUGCC												{	GenDebug(0x300,$1); 					}	/* 56301 extension	*/
		|	OP_DEC ddddd											{	GenIncDec($1[1],$2);					}	/* 56301 extension	*/
		|	OP_DIV ddddd ',' ddddd									{	GenDiv($2,$4);							}
		|	OP_DMAC plus_minus QQQQ ',' ddddd						{	GenDMac($1,$2,$3,$5);					}	/* 56301 extension	*/
		|	OP_DO X_or_Y ea ',' rel_target							{	GenDo1($2,&$3,&$5); 					}
		|	OP_DO '#' exp_int ',' rel_target						{	GenDo2($3,&$5); 						}
		|	OP_DO ddddd ',' rel_target								{	GenDo3($2,&$4); 						}
		|	OP_DO OP_FOREVER ',' rel_target 						{	GenDoForever(&$4);						}	/* 56301 extension	*/
		|	OP_DOR X_or_Y ea ',' rel_target 						{	GenDor1($2,&$3,&$5);					}	/* 56301 extension	*/
		|	OP_DOR '#' exp_int ',' rel_target						{	GenDo2($3,&$5); 						}	/* 56301 extension	*/
		|	OP_DOR ddddd ',' rel_target 							{	GenDor3($2,&$4);						}	/* 56301 extension	*/
		|	OP_DOR OP_FOREVER ',' rel_target						{	GenDorForever(&$4); 					}	/* 56301 extension	*/
		|	OP_ENDDO												{	GenEnddo(); 							}
		|	OP_EXTRACT ddddd ',' ddddd ',' ddddd					{	GenInsExt1($1[0],$2,$4,$6); 			}	/* 56301 extension	*/
		|	OP_EXTRACT '#' exp_int ',' ddddd ',' ddddd				{	GenInsExt2($1[1],$3,$5,$7); 			}	/* 56301 extension	*/
		|	OP_EXTRACTU ddddd ',' ddddd ',' ddddd					{	GenInsExt1($1[2],$2,$4,$6); 			}	/* 56301 extension	*/
		|	OP_EXTRACTU '#' exp_int ',' ddddd ',' ddddd 			{	GenInsExt2($1[3],$3,$5,$7); 			}	/* 56301 extension	*/
		|	OP_INSERT ddddd ',' ddddd ',' ddddd 					{	GenInsExt1($1[4],$2,$4,$6); 			}	/* 56301 extension	*/
		|	OP_INSERT '#' exp_int ',' ddddd ',' ddddd				{	GenInsExt2($1[5],$3,$5,$7); 			}	/* 56301 extension	*/
		|	OP_ILLEGAL												{	GenIllegal();							}
		|	OP_INC ddddd											{	GenIncDec($1[0],$2);					}	/* 56301 extension	*/
		|	OP_J_BIT_XXX '#' exp_int ',' X_or_Y ea ',' rel_target	{	GenJccBitAbs( $1, $3, $5, &$6,&$8); 	}
		|	OP_J_BIT_XXX '#' exp_int ',' ddddd ',' rel_target		{	GenJccBitAbsReg( $1[5], $3, $5, &$7);	}
		|	OP_JCC ea												{	GenJmpJsrJsccJcc(jcc_pattern,$1, &$2);	}
		|	OP_JMP ea												{	GenJmpJsrJsccJcc(jmp_pattern,0, &$2);	}
		|	OP_JSCC ea												{	GenJmpJsrJsccJcc(jscc_pattern,$1, &$2); }
		|	OP_JSR ea												{	GenJmpJsrJsccJcc(jsr_pattern,0, &$2);	}
		|	OP_LRA rel_target ',' ddddd 							{	GenLra(&$2,$4); 						}	/* 56301 extension	*/
		|	OP_LSX ddddd par_move									{	GenOneParamParMove($1[0],$2,&$3);		}
		|	OP_LSX '#' exp_int ',' ddddd							{	GenLsxImmediate($1,$3,$5);				}	/* 56301 extension	*/
		|	OP_LSX ddddd ',' ddddd									{	GenLsxReg($1,$2,$4);					}	/* 56301 extension	*/
		|	OP_LUA MMRRR ',' ddddd									{	GenLua1($2,$4); 						}
		|	OP_LUA '(' RREG '+' exp_int ')' ',' ddddd				{	GenLua2($3,$5,$8);						}	/* 56301 extension	*/
		|	OP_LUA '(' RREG '-' exp_int ')' ',' ddddd				{	GenLua2($3,-$5,$8); 					}	/* 56301 extension	*/
		|	OP_MAC plus_minus QQQ ',' ddddd par_move				{	GenMul1(mac_pattern,$2,$3,$5,&$6);		}
		|	OP_MAC plus_minus ddddd TAG1 exp_int ',' ddddd			{	GenMul2(mac_pattern,$2,$3,$5,$7);		}	/* 56301 extension	*/
		|	OP_MACI plus_minus '#' exp_int ',' ddddd ',' ddddd		{	GenMuli(mac_pattern,$2,$4,$6,$8);		}	/* 56301 extension	*/
		|	OP_MACxx plus_minus QQQQ ',' ddddd						{	GenMulxx(mac_pattern,$1,$2,$3,$5);		}	/* 56301 extension	*/
		|	OP_MPY plus_minus QQQ ',' ddddd par_move				{	GenMul1(mpy_pattern,$2,$3,$5,&$6);		}
		|	OP_MPY plus_minus ddddd TAG1 exp_int ',' ddddd			{	GenMul2(mpy_pattern,$2,$3,$5,$7);		}	/* 56301 extension	*/
		|	OP_MPYI plus_minus '#' exp_int ',' ddddd ',' ddddd		{	GenMuli(mpy_pattern,$2,$4,$6,$8);		}	/* 56301 extension	*/
		|	OP_MPYxx plus_minus QQQQ ',' ddddd						{	GenMulxx(mpy_pattern,$1,$2,$3,$5);		}	/* 56301 extension	*/
		|	OP_MACR plus_minus QQQ ',' ddddd par_move				{	GenMul1(macr_pattern,$2,$3,$5,&$6); 	}
		|	OP_MACR plus_minus ddddd TAG1 exp_int ',' ddddd 		{	GenMul2(macr_pattern,$2,$3,$5,$7);		}	/* 56301 extension	*/
		|	OP_MACRI plus_minus '#' exp_int ',' ddddd ',' ddddd 	{	GenMuli(macr_pattern,$2,$4,$6,$8);		}	/* 56301 extension	*/		  
		|	OP_MPYR plus_minus QQQ ',' ddddd par_move				{	GenMul1(mpyr_pattern,$2,$3,$5,&$6); 	}
		|	OP_MPYR plus_minus ddddd TAG1 exp_int ',' ddddd 		{	GenMul2(mpyr_pattern,$2,$3,$5,$7);		}	/* 56301 extension	*/
		|	OP_MPYRI plus_minus '#' exp_int ',' ddddd ',' ddddd 	{	GenMuli(mpyr_pattern,$2,$4,$6,$8);		}	/* 56301 extension	*/
		|	OP_MAX ddddd ',' ddddd par_move 						{	GenMax(max_pattern[0],$2,$4,&$5);		}	/* 56301 extension	*/
		|	OP_MAXM ddddd ',' ddddd par_move						{	GenMax(max_pattern[1],$2,$4,&$5);		}	/* 56301 extension	*/
		|	OP_MERGE ddddd ',' ddddd								{	GenMerge($2,$4);						}	/* 56301 extension	*/
		|	OP_MOVE par_move										{	GenMove(&$2);							}
		|	OP_MOVEC X_or_Y ea ',' ddddd							{	GenMovec1(movec_pattern1,0,$2,&$3,$5);	}
		|	OP_MOVEC ddddd ',' X_or_Y ea							{	GenMovec1(movec_pattern2,1,$4,&$5,$2);	}
		|	OP_MOVEC ddddd ',' ddddd								{	GenMovec2($2,$4);						}
		|	OP_MOVEC '#' SH exp_int ',' ddddd						{	GenMovec3($3,$4,$6);					}
		|	OP_MOVEM ddddd ',' PMEM ea								{	GenMovem(movem_pattern1,0,&$5,$2);		}
		|	OP_MOVEM PMEM ea ',' ddddd								{	GenMovem(movem_pattern2,1,&$3,$5);		}
		
		|	OP_MOVEP  X_or_Y ea ',' X_or_Y ea						{	GenMovep($2,&$3,$5,&$6);				}

		|	OP_MOVEP  PMEM ea ',' X_or_Y ea 						{	GenMovep2(1,&$3,$5,&$6);				}
		|	OP_MOVEP  X_or_Y ea ',' PMEM ea 						{	GenMovep2(0,&$6,$2,&$3);				}
		|	OP_MOVEP  '#' SH exp_int ',' X_or_Y ea					{	GenMovep3($4,$6,&$7);					}
		|	OP_MOVEP  X_or_Y ea ',' ddddd							{	GenMovep4(0,$2,&$3,$5); 				}
		|	OP_MOVEP  ddddd ',' X_or_Y ea							{	GenMovep4(1,$4,&$5,$2); 				}
		|	OP_NONE_PAR_INSN										{	GenNoArgOpcode($1); 					}
		|	OP_NORM ddddd ',' ddddd 								{	GenNorm($2,$4); 						}
		|	OP_NORMF	ddddd ',' ddddd 							{	GenNorm($2,$4); 						}	/* 56301 extension	*/
		|	OP_PLOCK_PUNLOCK ea 									{	GenPlockPunloc($1,&$2); 				}	/* 56301 extension	*/
		|	OP_PLOCKR_PUNLOCKR rel_target							{	GenPlockrPunlockr($1,&$2);				}	/* 56301 extension	*/
		|	OP_REP X_or_Y ea										{	GenRep1($2,&$3);						}
		|	OP_REP '#' SH exp_int									{	GenRep2($4);							}
		|	OP_REP ddddd											{	GenRep3($2);							}
		|	OP_TCC ddddd ',' ddddd									{	GenTcc1($1,$2,$4);						}
		|	OP_TCC ddddd ',' ddddd ddddd ',' ddddd					{	GenTcc2($1,$2,$4,$5,$7);				}				 
		|	OP_TFR ddddd ',' ddddd par_move 						{	GenTfr($2,$4,&$5);						}
		|	OP_TRAPCC												{	GenTrapcc($1);							}
		|	OP_VSL ddddd TAG1 exp_int ',' LMEM	ea					{	GenVsl($2,$4,&$7);						}
		|	OP_MOVEM error											{	
																		if (g_passNum != 0) yyerror("Illegal movem operation.");
																	}
		;

SH		:	/* empty */ 											{	$$=1;							}
		|	'<' 													{	$$=0;							}
		;

rel_target
		:	exp 													{$$ = GenRelAddrLong(Val_GetAsInt($1)); }
		|	'>' exp 												{$$ = GenRelAddrLong(Val_GetAsInt($2)); }
		|	'<' exp 												{$$ = GenRelAddrShort(Val_GetAsInt($2));}
		|	RREG													{		$$ = GenRelAddrReg($1); 		}
		;

plus_minus	
		:	/* empty */ 											{	$$=0;							}
		|	'+' 													{	$$=0;							}
		|	'-' 													{	$$=1;							}
		;

QQQ 	
		:	XREG ',' XREG											{ $$ = GetQQQ( $1, $3, FALSE  );		}
		|	XREG ',' YREG											{ $$ = GetQQQ( $1, $3 + 2, FALSE  );	}
		|	YREG ',' XREG											{ $$ = GetQQQ( $1 + 2, $3, FALSE  );	}
		|	YREG ',' YREG											{ $$ = GetQQQ( $1 + 2, $3 + 2, FALSE  );}
		;

QQQQ
		:	XREG ',' XREG											{ $$ = GetQQQQXregXreg( $1, $3);	}
		|	XREG ',' YREG											{ $$ = GetQQQQXregYreg( $1, $3);	}
		|	YREG ',' XREG											{ $$ = GetQQQQYregXreg( $1, $3);	}
		|	YREG ',' YREG											{ $$ = GetQQQQYregYreg( $1, $3);	}
		;

X_or_Y	:	XMEM													{			$$=0;				}
		|	YMEM													{			$$=1;				}
		;

ddddd	:	XREG													{			$$=$1;				}
		|	YREG													{			$$=2+$1;			}
		|	AREG													{			$$=4+$1;			}
		|	BREG													{			$$=7+$1;			}
		|	AAAA													{			$$=10;				}
		|	BBBB													{			$$=11;				}
		|	RREG													{			$$=12+$1;			}
		|	NREG													{			$$=20+$1;			}
		|	A10 													{			$$=28;				}
		|	B10 													{			$$=29;				}
		|	XXXX													{			$$=30;				}
		|	YYYY													{			$$=31;				}
		|	AABB													{			$$=32;				}
		|	BBAA													{			$$=33;				}
		|	MR														{			$$=34;				}
		|	CCR 													{			$$=35;				}
		|	COM 													{			$$=36;				}
		|	EOM 													{			$$=37;				}
		|	MREG													{			$$=38+$1;			}
		|	_EP 													{			$$=46;				}
		|	VBA 													{			$$=47;				}
		|	SC														{			$$=48;				}
		|	SZ														{			$$=49;				}
		|	SR														{			$$=50;				}
		|	OMR 													{			$$=51;				}
		|	SP														{			$$=52;				}
		|	SSH 													{			$$=53;				}
		|	SSL 													{			$$=54;				}
		|	LA														{			$$=55;				}
		|	LC														{			$$=56;				}
		;

/*								 Parallel moves syntax */

par_move:
			OP_IFCC 									{	$$=GenParIFcc(0x20200,$1);								}	/* 56301 extension	*/
		|	OP_IFCCU									{	$$=GenParIFcc(0x20300,$1);								}	/* 56301 extension	*/
		|	'#' exp_int ',' ddddd						{	$$=GenParExpReg( $2, $4 );								}	/* uses extension word */
		|	'#' '<' exp_int ',' ddddd					{	$$=GenParExpRegShort( $3, $5 ); 						}	/* Doesn't use extension word */	
		|	ddddd ',' ddddd 							{	$$=GenParRegReg($1,$3); 								}
		|	MMRRR										{	$$=GenParUpdate($1);									}	/* U: address register update */
		|	XMEM ea ',' ddddd							{	$$=GenMemReg(xmem_reg_pattern1,&$2, $4, 1);				}	/* X: parallel moves */
		|	ddddd ',' XMEM ea							{	$$=GenMemReg(xmem_reg_pattern2,&$4, $1, 0);				}	/* X: parallel moves */
		|	'#' exp_int ',' ddddd ddddd ',' ddddd		{	$$=GenParExpRegRegReg($2,$4,$5,$7); 					}	/*buggy*/		/* X:R parallel moves */
		|	XMEM ea ',' ddddd ddddd ',' ddddd			{	$$=GenParEaRegRegReg(&$2,$4,$5,$7); 					}	/*buggy*/	
		|	ddddd ',' XMEM ea ddddd ',' ddddd			{	$$=GenParRegEaRegReg($1,&$4,$5,$7); 					}	/* X:R: (class II move encapsuled) */
		|	YMEM ea ',' ddddd							{	$$=GenMemReg(ymem_reg_pattern1,&$2, $4, 1);				}
		|	ddddd ',' YMEM ea							{	$$=GenMemReg(ymem_reg_pattern2,&$4, $1, 0);				}
		|	ddddd ',' ddddd '#' SH exp_int ',' ddddd	{	$$=GenParRegRegExpReg($1,$3,$6,$8); 					}	/*untested*/
		|	ddddd ',' ddddd YMEM ea ',' ddddd			{	$$=GenParRegRegEaReg($1,$3,&$5,$7); 					}	/*untested*/
		|	ddddd ',' ddddd ddddd ',' YMEM ea			{	$$=GenParRegRegRegEa( $1,$3,$4,&$7);					}
		|	LMEM ea ',' ddddd							{	$$=GenLMemReg(lmem_reg_pattern1,&$2, $4);				}
		|	ddddd ',' LMEM ea							{	$$=GenLMemReg(lmem_reg_pattern2,&$4, $1);				}
		|	XMEM ea ',' ddddd YMEM ea ',' ddddd 		{	$$=GenParXRegYReg(XRegYReg_pattern[0],&$2,$4,&$6,$8);	}	/* X:Y: parallel moves */
		|	XMEM ea ',' ddddd ddddd ',' YMEM ea 		{	$$=GenParXRegYReg(XRegYReg_pattern[1],&$2,$4,&$8,$5);	}	/* X:Y: parallel moves */
		|	ddddd ',' XMEM ea YMEM ea ',' ddddd 		{	$$=GenParXRegYReg(XRegYReg_pattern[2],&$4,$1,&$6,$8);	}	/* X:Y: parallel moves */
		|	ddddd ',' XMEM ea ddddd ',' YMEM ea 		{	$$=GenParXRegYReg(XRegYReg_pattern[3],&$4,$1,&$8,$5);	}	/* X:Y: parallel moves */
		|	/*empty*/									{	$$=GenParEmpty();										}	/* no parallel move */
		;

MMRRR	:	'(' RREG ')' MINUS_N						{	$$ = GenEA1( 0,$2, $4 ).w0; 			}
		|	'(' RREG ')' PLUS_N 						{	$$ = GenEA1( 0x8,$2, $4 ).w0;			}
		|	'(' RREG ')' '-'							{	$$ = GenEA2( 0x10, $2 ).w0; 			}
		|	'(' RREG ')' '+'							{	$$ = GenEA2( 0x18, $2 ).w0; 			}
		;

imm_ea	:  exp_int										{	$$ = GenImmLong( 0x30, $1 );		}
/*		  |  '>' exp_int									{	$$ = GenImmLong( 0x30, $2 );		} */
		|	LSL exp_int 								{	$$ = GenImmShortIO( 0x30, $2 ); 	}	
		|	'<' exp_int 								{	$$ = GenImmShortAbs( 0x30, $2 );	}	
		;

ea		:	'(' RREG ')' MINUS_N						{	$$ = GenEA1( 0x00, $2, $4 );	}
		|	'(' RREG ')' PLUS_N 						{	$$ = GenEA1( 0x08, $2, $4 );	}
		|	'(' RREG ')' '-'							{	$$ = GenEA2( 0x10, $2 );		}
		|	'(' RREG ')' '+'							{	$$ = GenEA2( 0x18, $2 );		}
		|	'(' RREG ')'								{	$$ = GenEA2( 0x20, $2 );		}
		|	'(' RREG PLUS_N ')' 						{	$$ = GenEA1( 0x28, $2, $3 );	}
		|	'-' '(' RREG ')'							{	$$ = GenEA2( 0x38, $3);			}
		|	imm_ea										{	$$ = $1;						}
		;

exp_int :	exp 										{	$$=Val_GetAsFract24($1);		}
		|  '>' exp										{	$$=Val_GetAsInt($2);			}
		;

exp 	:	NUM_FRACT									{	$$ = Val_CreateFract($1);				}
		|	NUM_FLOAT									{	$$ = Val_CreateFloat($1);				}
		|	NUM_INTEGER 								{	$$ = Val_CreateInt($1); 				}
		|	SYM 										{	$$ = GetSym($1.ptr);					}
		|	OP_STRING2									{	$$ = Val_CreateInt(StrToInt($1.ptr));	}
		|	'*' 										{	$$ = Val_CreateInt(GetCurrentPC()); 	}
		|	exp '&' exp 								{	$$ = Val_And($1,$3);					}
		|	exp '|' exp 								{	$$ = Val_Or($1,$3); 					}
		|	exp '^' exp 								{	$$ = Val_Xor($1,$3);					}
		|	exp LSL exp 								{	$$ = Val_Lsl($1,$3);					}
		|	exp LSR exp 								{	$$ = Val_Lsr($1,$3);					}
		|	exp '+' exp 								{	$$ = Val_Add($1,$3);					}
		|	exp '-' exp 								{	$$ = Val_Add($1,Val_Neg($3));			}
		|	exp '*' exp 								{	$$ = Val_Mul($1,$3);					}
		|	exp '/' exp 								{	$$ = Val_Div($1,$3);					}
		|  '-' exp										{	$$ = Val_Neg($2);						}
		|  '!' exp										{	$$ = Val_Not($2);						}
		|  '~' exp										{	$$ = Val_BinNot($2);					}
		|	'(' exp ')' 								{	$$ = $2;								}
		|	TYPE_FRACT	'(' exp ')' 					{	$$ = Val_CastToFract($3);				}
		|	TYPE_FLOAT	'(' exp ')' 					{	$$ = Val_CastToFloat($3);				}
		|	TYPE_INT	'(' exp ')' 					{	$$ = Val_CastToInt($3); 				}
		|	TYPE_INT	'(' OP_STRING ')'				{	$$ = Val_CreateInt(StrToInt($3.ptr));	}
		;

%%
