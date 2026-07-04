/*

Project:    asm56k
Author:     M.Buras (sqward)

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm_types.h>
#include <Value.h>
#include <SymbolTable.h>
#include <CodeUtils.h>
#include <Parser.h>
#include <TokenStream.h>
#include <StringBuffer.h>
#include <export.h>

/* includes stack */

#define MAX_INCLUDES_NESTED 32

const char *inc_names[MAX_INCLUDES_NESTED];
const char *g_CurrentFile;
FILE *inc_handles[MAX_INCLUDES_NESTED];
void *inc_buffers[MAX_INCLUDES_NESTED];
int inc_lines[MAX_INCLUDES_NESTED];
int g_incStackDeepth = -1;

TokenVal *g_tokens;

void *buffer;							/* top level buffer... TODO: it should be local to fetch_tokens */

int if_stack_l;
char if_stack[MAX_CONDITION_NESTS];

/* streams stack */

int g_streamsStrackIndex;
StreamStackEntry streamsStack[MACRO_NEST_DEPTH];

TokenVal *g_pParamsPool;
TokenVal **g_pParamsArrayPool;
int g_MacroNumInstances;
TokenVal *params_pointers[MACRO_PARAMS_POINTER_BUFFER];
TokenVal macros_params[MACRO_PARAMS_TOKEN_BUFFER];

static int skipping;



void InitTokenStream(const char *file_name)
{
#if 0
	buffer = (void *) asm_create_buffer(input, LEX_BUFFER);
	asm_switch_to_buffer(buffer);
	inc_names[0] = file_name;
	inc_buffers[0] = buffer;
#endif
	g_CurrentFile = file_name;
}


TokenVal *CopyToken(int token, TokenVal *pToken)
{
	pToken->token = token;
	pToken->data.val = yylval;
	return pToken++;
}


void PushStream(TokenVal *pMacro, const char *pFileName, int curline, int params_count, int instancesNumber)
{
	g_streamsStrackIndex++;

	if (MACRO_NEST_DEPTH <= g_streamsStrackIndex)
	{
		yyerror("Macros nested too deep!");
		asm_abort();
	}

	g_incStackDeepth = g_streamsStrackIndex;
	inc_names[g_streamsStrackIndex] = pFileName;

	streamsStack[g_streamsStrackIndex].params_array = g_pParamsArrayPool;
	streamsStack[g_streamsStrackIndex].pParamsPool = g_pParamsPool;

	streamsStack[g_streamsStrackIndex].params_count = params_count;
	streamsStack[g_streamsStrackIndex].macro_ptr = pMacro;
	streamsStack[g_streamsStrackIndex].line_num = curline;
	streamsStack[g_streamsStrackIndex].instancesNumber = instancesNumber;

	debugprint("PushStream: depth = %d, currentline = %d\n", g_streamsStrackIndex, curline);
}

int PopStream(void)
{
	g_currentLine = streamsStack[g_streamsStrackIndex].line_num;
	g_errorLine = g_currentLine;

	g_pParamsArrayPool = streamsStack[g_streamsStrackIndex].params_array;
	g_pParamsPool = streamsStack[g_streamsStrackIndex].pParamsPool;

	debugprint("PopStream: depth = %d, currentline = %d\n", g_streamsStrackIndex, g_currentLine);

	--g_streamsStrackIndex;
	g_incStackDeepth = g_streamsStrackIndex;

	return g_streamsStrackIndex;
}

int TopPosStream(void)
{
	return g_streamsStrackIndex;
}

void ResetStream(void)
{
	g_pParamsPool = &macros_params[0];
	g_pParamsArrayPool = &params_pointers[0];
	g_streamsStrackIndex = -1;
}

TokenVal *GetCurrentStreamPos(void)
{
	return streamsStack[g_streamsStrackIndex].macro_ptr;
}

int GetToken(TokenVal **pTokenValue)
{
	TokenVal *pToken = streamsStack[g_streamsStrackIndex].macro_ptr;
	int token = pToken->token;

	if (pTokenValue != NULL)
	{
		*pTokenValue = pToken;
	}

	if (token == TOKEN_NEW_PTR)
	{
		streamsStack[g_streamsStrackIndex].macro_ptr = (TokenVal *) pToken->data.pNextBlock;
		debugprint("macro defragmentation in line: %d\nnew mc_tmp = %p\n", g_currentLine, GetCurrentStreamPos());
		token = GetToken(pTokenValue);
	} else
	{
		streamsStack[g_streamsStrackIndex].macro_ptr++;
		yylval = pToken->data.val;
	}

	if (token == EOL)
	{
		g_currentLine = pToken->data.val.integer;
		g_errorLine = g_currentLine - 1;
	}

	return token;
}

int SkipToken(void)
{
	TokenVal *pToken = streamsStack[g_streamsStrackIndex].macro_ptr;
	int token = pToken->token;

	skipping = TRUE;
	if (token == TOKEN_NEW_PTR)
	{
		streamsStack[g_streamsStrackIndex].macro_ptr = (TokenVal *) pToken->data.pNextBlock;
		debugprint("macro defragmentation in line: %d\nnew mc_tmp = %p\n", g_currentLine, GetCurrentStreamPos());
		token = SkipToken();
	} else
	{
		streamsStack[g_streamsStrackIndex].macro_ptr++;
	}

	if (token == EOL)
	{
		g_currentLine = pToken->data.val.integer;
		g_errorLine = g_currentLine - 1;
	}

	skipping = FALSE;
	return token;
}

/*
 * Fetch whole source file as a tokens
 */

/*
 * We read the tokens and save them for
 * future use. This improves speed since
 * on the second phase we'er reading form
 * table, not from flex...
 */

#define TOKENS_BLOCK 	4096

int PrefetchTokens(void)
{
	int token;
	int token_count = 0;
	TokenVal *store_tokens;
	TokenVal *tokens_bound;

	g_tokens = store_tokens = (TokenVal *) malloc(TOKENS_BLOCK * sizeof(TokenVal));
	MTEST(store_tokens);
	tokens_bound = store_tokens;
	tokens_bound += (TOKENS_BLOCK - 2);

	do
	{
		token = asmlex();
		token_count++;

		if (store_tokens == tokens_bound)
		{
			TokenVal *store_tokens2 = (TokenVal *) malloc(TOKENS_BLOCK * sizeof(TokenVal));

			MTEST(store_tokens2);
			store_tokens->token = TOKEN_NEW_PTR;	/* "switch to next memory block" */
			store_tokens->data.pNextBlock = store_tokens2;
			tokens_bound = store_tokens2 + (TOKENS_BLOCK - 2);
			store_tokens = store_tokens2;
			debugprint("tokens fragmentation in line: %d\n", g_currentLine);
		}
		CopyToken(token, store_tokens);
		store_tokens++;
	}
	while (token != OP_END);

	return token_count;
}


static void CreateNewLexBuffer(FILE *pFile, const char *pFileName)
{
	g_incStackDeepth++;
	inc_handles[g_incStackDeepth] = pFile;
	inc_buffers[g_incStackDeepth] = (void *) asm_create_buffer(inc_handles[g_incStackDeepth], LEX_BUFFER);
	inc_names[g_incStackDeepth] = StringBufferInsert(pFileName);

	g_currentLine = 1;
	g_errorLine = 1;

	asm_switch_to_buffer(inc_buffers[g_incStackDeepth]);

	/*
	 * Insert source file name into the tokenized code
	 * This tells us which source line in which source
	 * is invalid (this is for error reporting)
	 */

	yylval.text.len = 0;				/* fake string */
	yylval.text.ptr = StringBufferInsert(pFileName);

	debugprint("Include_file(%s);\n", pFileName);
}


int PushNewFile(const char *pFileName)
{
	char name_buf[512];
	FILE *pFile;
	const char *pDir;
	uint dirnum = 0;

	inc_lines[g_incStackDeepth] = g_currentLine;

	pFile = fopen(pFileName, "r");
	if (pFile == NULL)
	{
		while (GetIncDir(&pDir, &dirnum))
		{
			strcpy(name_buf, pDir);
			if (name_buf[strlen(name_buf) - 1] != '\\' && name_buf[strlen(name_buf) - 1] != '/')
			{
				strncat(name_buf, "/", sizeof(name_buf) - strlen(name_buf));
			}
			strncat(name_buf, pFileName, sizeof(name_buf) - strlen(name_buf));
	
			pFile = fopen(name_buf, "r");
	
			if (pFile != NULL)
			{
				pFileName = name_buf;
				break;
			}
		}
	}

	if (pFile == NULL)
	{
		return -1;
	}

	CreateNewLexBuffer(pFile, pFileName);

	return 0;
}


bool PushNewMainFile(const char *pFileName)
{
	FILE *pFile;

	pFile = fopen(pFileName, "r");
	if (pFile != NULL)
	{
		CreateNewLexBuffer(pFile, pFileName);
		return TRUE;
	}
	return FALSE;
}


/*
 * Include another source file
 */
int IncludeFile(void)
{
	int i;
	char inc_temp_string[512];
	char *sptr = inc_temp_string;
	int len_str = 0;

	if (g_incStackDeepth == MAX_INCLUDES_NESTED)
	{
		yyerror("Includes nested too deep.");
		asm_abort();
		return FALSE;
	}

	if (skipping)
		return TRUE;

	for (;;)
	{
		i = Input();

		if (i == YYEOF || i == '\r' || i == '\n')
		{
			Unput(i);
			yyerror("Invalid include path.");
			return FALSE;
		}

		if (i != ' ' && i != '\t')
		{
			break;
		}
	}

	for (;;)
	{
		if (YYEOF == i)
		{
			yyerror("Unexpected end of file.");
			return FALSE;
		} else if ('\r' == i || '\n' == i || ' ' == i || '\t' == i)
		{
			*sptr++ = 0;
			len_str++;
			Unput(i);
			break;
		} else
		{
			*sptr++ = i;
			len_str++;
		}

		if (len_str == sizeof(inc_temp_string) - 1)
		{
			yyerror("File path too long.");
			*sptr++ = 0;
			break;
		}

		i = Input();
	}

	/* remove quotation marks if used */

	if (inc_temp_string[0] == '\"')
	{
		memmove(inc_temp_string, inc_temp_string + 1, strlen(inc_temp_string + 1) + 1);
		if (strrchr(inc_temp_string, '\"') != NULL)
		{
			*strrchr(inc_temp_string, '\"') = 0;
		}
	}

	if (PushNewFile(inc_temp_string))
	{
		yyerror("File not found: '%s'.", inc_temp_string);
		return FALSE;
	}
	return TRUE;
}


/*
 * leave included file
 */
int PopFile(void)
{
	asm_delete_buffer(inc_buffers[g_incStackDeepth]);
	fclose(inc_handles[g_incStackDeepth]);

	if (g_incStackDeepth > 0)
	{
		g_incStackDeepth--;
		g_currentLine = inc_lines[g_incStackDeepth];
		g_errorLine = g_currentLine;

		asm_switch_to_buffer(inc_buffers[g_incStackDeepth]);
		return FALSE;
	}

	return TRUE;
}


/*
 * skip a block of code
 */
int SkipConditional(void)
{
	int token;
	int nest = 0;

	for (;;)
	{
		token = SkipToken();

		switch (token)
		{
		case OP_IF:
		case OP_IFCC:
			nest++;
			if (nest > 1024)
			{
				yyerror("Conditional assembly nested too deep.");
				asm_abort();
			}
			break;
		case OP_ENDC:
			if (nest == 0)
			{
				return token;
			}
			nest--;
			break;
		case OP_ELSE:
			if (nest == 0)
			{
				return token;
			}
			break;
		case OP_END:
			yyerror("Unexpected end of file (in conditional).");
			asm_abort();
			break;
		}

		if (nest < 0)
		{
			yyerror("Illegal use of conditional assembling.");
			asm_abort();
		}
	}
}


void Skip_line(void)
{
	int token;

	do
	{
		token = SkipToken();
	} while (token != EOL && token != OP_END);
}

/* include dirs */

#define			MAX_INC_DIRS_LIST 32

static const char *g_IncDirs[MAX_INC_DIRS_LIST];
static int g_IncDirsNum = 0;

void AddIncDir(const char *pDir)
{
	if (g_IncDirsNum >= MAX_INC_DIRS_LIST)
	{
		yyerror("Too many include directories... MAX_INC_DIRS = %d\n", MAX_INC_DIRS_LIST);
	} else
	{
		g_IncDirs[g_IncDirsNum] = strdup(pDir);
		g_IncDirsNum++;
	}
}

bool GetIncDir(const char **pDir, uint *pDirNum)
{
	uint dirnum = *pDirNum;

	(*pDirNum)++;
	*pDir = NULL;
	if (dirnum < g_IncDirsNum)
	{
		*pDir = g_IncDirs[dirnum];
		return TRUE;
	}
	return FALSE;
}
