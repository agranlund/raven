/*

Project:    asm56k
Author:     M.Buras (sqward)


*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm_types.h>
#include <CodeUtils.h>
#include <SymbolTable.h>
#include <Parser.h>
#include <TokenStream.h>
#include <SymbolTable.h>
#include <MacroProxy.h>
#include <export.h>
#include <ErrorMessages.h>
#include <StringBuffer.h>
#include "export.h"

/*
This is the replacement of the orginal
yylex function.
Such a strange hack is performed to allow
macros menagment, conditional assembly and
such stuff, which is not handled by the
bison parser. So all the input that needs
some additional processing is serviced here

the function has two main "states":
PURE_PARSER and REPLAY_MACRO
in the PURE_PARSER mode yylex() just
calls the flex scanner and passes the
result to the bison parser
in the REPLAY_MACRO state yylex() feeds
bison parser with prevoiusly "recorded"
tokens thus inserting the macro into the
program.
*/

/* Here's the explanation how macros are handled:
A small routine is inserted between flex and bison
for intercepting macros and storing them for future
use. This interception routine is controlled from bison
grammar. The  following rule causes recording of all
the code after the 'macro' keyword until 'endm' is
encountered.*/


void InitMacroProxy(void)
{
	ResetStream();
	g_MacroNumInstances = 0;
}


int yylex(void)
{
	TokenVal *pTokenValue;
	int token = GetToken(&pTokenValue);

	switch (token)
	{
	case TOKEN_ENTER_FILE:
		inc_lines[g_incStackDeepth] = g_currentLine;
		g_currentLine = 1;
		g_errorLine = 1;
		g_incStackDeepth++;
		inc_names[g_incStackDeepth] = yylval.text.ptr;
		g_CurrentFile = inc_names[g_incStackDeepth];

		debugprint("leaving: %s in line: %d, entering: %s.\n", inc_names[g_incStackDeepth - 1],
				   inc_lines[g_incStackDeepth - 1], inc_names[g_incStackDeepth]);

		token = yylex();
		break;
	case TOKEN_LEAVE_FILE:
		g_incStackDeepth--;
		g_CurrentFile = inc_names[g_incStackDeepth];
		g_currentLine = inc_lines[g_incStackDeepth];
		g_errorLine = g_currentLine;
		debugprint("re-entering: %s in line: %d.\n", inc_names[g_incStackDeepth], g_currentLine);
		token = yylex();
		break;
	case OP_END:
		if (TopPosStream() > 0)
		{
			yyerror("Unexpected end of file (in macro).");
			asm_abort();
		}
		break;

	case OP_ENDM:
		debugprint("end of macro\n");

		if (PopStream() == -1)
		{
			yyerror("Illegal use of ENDM directive. Maybe you've missed a \"macro\" keyword? ");
			asm_abort();
		}

		token = yylex();
		break;

	case SYM:
	case SYM2:
	case OP_STRING:
		if (yylval.text.ptr[0] == '@' || yylval.text.ptr[0] == '.' || yylval.text.ptr[0] == '_')
		{
			char temp_name[128];
			int stamp = 0;

			if (TopPosStream() == 0)
			{						/* local label */
				stamp = g_LocalSerial;
			} else
			{						/* macro local label */
				stamp = streamsStack[g_streamsStrackIndex].instancesNumber;
			}

			snprintf(temp_name, sizeof(temp_name), "%s(%i)", yylval.text.ptr, stamp);

			if (TopPosStream() == 0)
			{
				if (g_passNum == 0)
				{					/* don't need to add twice */
					yylval.text.ptr = pTokenValue->data.val.text.ptr = StringBufferInsert(temp_name);
					yylval.text.len = pTokenValue->data.val.text.len = strlen(temp_name);
				}
			} else
			{
				yylval.text.ptr = StringBufferInsert(temp_name);
				yylval.text.len = strlen(temp_name);
			}
		}
		break;

	case MACRO_PARAM:
		{
			int param_index = yylval.integer;

			if (TopPosStream() == 0)
			{
				yyerror("Macro argument referenced outside of macro.");
				Skip_line();
			} else
			{
				debugprint("Macro param %d\n", param_index);

				if (param_index > streamsStack[g_streamsStrackIndex].params_count)
				{
					yyerror("Invalid argument referred.");
					asm_abort();
				}

				PushStream(streamsStack[g_streamsStrackIndex].params_array[param_index - 1],
						   inc_names[g_incStackDeepth], g_currentLine, -1, 0);
			}
			token = yylex();
		}
		break;
	}

	return token;
}


void Record_Macro(hs *temp)
{
	SymSetValueMacro(temp, T_MACRO, (void *) GetCurrentStreamPos(), (void *) inc_names[g_incStackDeepth],
					 g_currentLine);
	Skip_Macro();
}


void Skip_Macro(void)
{
	int token;

	do
	{
		token = SkipToken();

		if (token == OP_END)
		{
			yyerror("Unexpected end of file (in macro).");
			asm_abort();
		}
	}
	while (token != OP_ENDM);
}


/*
 * Replay_Macro() inserts previously defined macro
 */
void Replay_Macro(hs *name)
{
	int token;
	int params_count = 0;
	int commas = 0;

	TokenVal **pParamArray = g_pParamsArrayPool;
	TokenVal *pStoreParam = g_pParamsPool;
	TokenVal *pFirst = pStoreParam;

	*pParamArray++ = pStoreParam;

	while (1)
	{
		token = yylex();

		if (pParamArray >= &params_pointers[MACRO_PARAMS_POINTER_BUFFER])
		{
			yyerror("Too heavy usage of macros! You can increase MACRO_PARAMS_POINTER_BUFFER if you really need ...");
			asm_abort();
		}

		if (pStoreParam >= &macros_params[MACRO_PARAMS_TOKEN_BUFFER])
		{
			yyerror("Too heavy usage of macros! You can increase MACRO_PARAMS_TOKEN_BUFFER if you really need ...");
			asm_abort();
		}

		if (',' == token)
		{
			if (pFirst == pStoreParam)
			{
				yyerror("Void macro parameter.");
			} else
			{
				commas++;
				params_count++;
			}

			pStoreParam->token = OP_ENDM;
			pStoreParam++;
			*pParamArray++ = pStoreParam;
		} else if (OP_END == token || EOL == token)
		{
			if (pFirst == pStoreParam && commas > 0)
			{
				yyerror("Void macro parameter.");
			} else
			{
				params_count++;
			}

			pStoreParam->token = OP_ENDM;
			pStoreParam++;
			break;
		} else
		{
			CopyToken(token, pStoreParam);
			pStoreParam++;
		}
	};

	debugprint("params_count = %d\n", params_count);

	g_MacroNumInstances++;

	PushStream((TokenVal *) name->m_data1, (char *) name->m_data2, g_currentLine, params_count, g_MacroNumInstances);

	g_CurrentFile = name->m_data2;
	g_currentLine = name->m_data3;
	g_errorLine = g_currentLine;

	g_pParamsArrayPool = pParamArray;
	g_pParamsPool = pStoreParam;

	debugprint("macro ptr %p, nr of params: 0x%X\n", streamsStack[g_streamsStrackIndex].macro_ptr, params_count);
}


void MacroCall(const char *pString)
{
	hs *temp;

	temp = FindSymbol(pString);

	if (temp == 0)
	{
		if (g_passNum != 0)
		{
			yyerror("Undefined instruction/macro.");
		}
		Skip_line();
	} else
	{
		if (temp->type != T_MACRO)
		{
			if (g_passNum != 0)
			{
				yyerror("label isn't a macro.");
			}
			Skip_line();
		} else
		{
			Replay_Macro(temp);
		}
	}
}


void MacroRecord(const char *pString)
{
	if (g_passNum == 0)
	{
		hs *temp = FindSymbol(pString);

		if (!temp)
		{
			yyerror("Macro redefined.");
		}
		Record_Macro(temp);
	} else
	{
		Skip_Macro();
	}
}


void MacroError(void)
{
	if (g_passNum == 0)
	{
		yyerror("Macro must have a name.");
		Skip_Macro();
	} else
	{
		Skip_Macro();
	}
}


void ResetLocalLabel(const char *pString)
{
	if (TopPosStream() == 0 && (pString[0] != '.' && pString[0] != '_'))
	{
		g_LocalSerial++;
	}
}
