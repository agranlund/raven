/*

Project:    asm56k
Author:     M.Buras (sqward)

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>

#ifdef _MSC_VER
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <math.h>
#include <string.h>
#include <asm_types.h>
#include "export.h"
#include <Value.h>
#include <SymbolTable.h>
#include <CodeUtils.h>
#include <Parser.h>
#include <TokenStream.h>
#include <MacroProxy.h>
#include <OutputLod.h>
#include <OutputP56.h>
#include <OutputEmbededAsm.h>
#include <OutputEmbededC.h>
#include <PipeLineRestriction.h>
#include <StringBuffer.h>
#include <getopt.h>

extern int yyparse (void);

int g_currentLine = 1;
int g_errorLine = 1;
int g_passNum;
int g_errorCount;
int g_warnCount;
int g_LocalSerial = 0;
jmp_buf critical_error;

static char *lod_output_name = NULL;
static char *p56_output_name = NULL;
static char *embed_asm_output_name = NULL;
static char *embed_c_output_name = NULL;

static char *input_name = NULL;
int g_dsp_cpu = 56001;
int g_output_symbols;
int g_write_zero_sections;

static char const program[] = "asm56k";
static char const version[] = "1.02";


void mtest(void *pMem, const char *File, int Line)
{
	if (pMem == NULL)
	{
		yyerror("Out of memory. Program aborted.\n");
		asm_abort();
	}
}


void debugprint(const char *pFmt, ...)
{
#ifdef DEBUG
	va_list arglist;

	va_start(arglist, pFmt);
	vfprintf(stderr, pFmt, arglist);
	va_end(arglist);
#endif
}


static void yymessage(const char *lineformat, const char *format, va_list arglist)
{
	int i;

	if (g_incStackDeepth > 0)
	{
		for (i = 0; i < g_incStackDeepth; i++)
		{
			fprintf(stderr, "%s:%d: in file included from here.\n", inc_names[i], inc_lines[i]);
		}
	}

	fprintf(stderr, lineformat, inc_names[g_incStackDeepth], g_errorLine);
	vfprintf(stderr, format, arglist);
	fputc('\n', stderr);
}


void yywarning(const char *s, ...)
{
	va_list arglist;

	va_start(arglist, s);
	yymessage("%s:%d: Warning: ", s, arglist);
	va_end(arglist);
	g_warnCount++;
}


void yyerror(const char *s, ...)
{
	va_list arglist;

	va_start(arglist, s);
	yymessage("%s:%d: ", s, arglist);
	va_end(arglist);
	g_errorCount++;
}


void asm_abort(void)
{
	fprintf(stderr, "Terminating execution.\n");
	longjmp(critical_error, 1);
}


static void InitParserPass1(void)
{
	g_LocalSerial = 0;
	in_section = FALSE;
	g_passNum = 0;						/* pass 1 */
	pc = 0;
	g_currentLine = 1;
	g_errorLine = 1;
	num_chunks = 0;
	num_chunks2 = 0;
	if_stack_l = 0;
	ResetStream();
	PipeLineReset();
	PushStream(g_tokens, inc_names[0], 1, -1, 0);
}


static void InitParserPass2(void)
{
	g_LocalSerial = 0;
	in_section = FALSE;
	g_passNum = 1;						/* pass 2 */
	pc = 0;
	if_stack_l = 0;
	g_currentLine = 1;
	g_errorLine = 1;
	ResetStream();
	PipeLineReset();
	PushStream(g_tokens, inc_names[0], 1, -1, 0);
}




static int asm56k(void)
{
	int num_tokens = 0;

	if (input_name == NULL)
	{
		fprintf(stderr, "No input file given.\n");
		return EXIT_FAILURE;
	}

	if (lod_output_name == NULL &&
		p56_output_name == NULL &&
		embed_asm_output_name == NULL &&
		embed_c_output_name == NULL)
	{
		fprintf(stderr, "No output file given.\n");
		return EXIT_FAILURE;
	}

	AddIncDir("./");

	if (PushNewMainFile(input_name) == FALSE)
	{
		fprintf(stderr, "File not found: '%s'\n", input_name);
		return EXIT_FAILURE;
	}

	if (setjmp(critical_error) == 0)
	{
		InitTokenStream(input_name);
		InitMacroProxy();

		num_tokens = PrefetchTokens();

		debugprint("PASS1\n");
		InitParserPass1();
		debugprint("%d tokens fetched\n", num_tokens);

		if (yyparse() == 0 && g_errorCount == 0)
		{
			/*  pass 2 */
			debugprint("PASS2\n");
			close_vchunk();
			InitMacroProxy();
			InitParserPass2();
			yyparse();
			close_chunk();
			verify_code();
		}
#ifdef DEBUG
		debugprint("Listing symbols:");
		ListSymbolTable();
		debugprint("end of symbols...");
#endif

		if (g_errorCount)
		{
			printf("Finished with %d error(s).\n", g_errorCount);
		} else
		{
			if (NULL != lod_output_name)
			{
				SaveFileLod(lod_output_name, input_name);
			}

			if (NULL != p56_output_name)
			{
				SaveFileP56(p56_output_name);
			}

			if (NULL != embed_asm_output_name)
			{
				SaveFileEmbeded(embed_asm_output_name);
			}

			if (NULL != embed_c_output_name)
			{
				SaveFileEmbededC(embed_c_output_name);
			}
		}
	}

	return g_errorCount != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}


static void DefineSymbol(char *v)
{
	char *pSymbol;
	char *pValue;
	Value val;
	stext symstr;

	pSymbol = v;
	pValue = strchr(pSymbol, '=');
	if (pValue)
	{
		*pValue++ = 0;
	}
	symstr.len = strlen(pSymbol);
	symstr.ptr = pSymbol;
	AddSym(&symstr, 1);
	if (pValue)
		val = Val_CreateInt(atoi(pValue));
	else
		val = Val_CreateInt(1);
	SymSet(pSymbol, val);
}


static void AddIncludePath(const char *v)
{
	AddIncDir(v);
	debugprint("adding incdir: %s\n", v);
}


enum opt {
	OPT_FLAG_SET = 0,
	OPT_HELP = 'h',
	OPT_VERSION = 'V',
	
	OPT_EMBED_ASM = 'e',
	OPT_EMBED_C = 'k',
	OPT_OUTPUT = 'o',
	OPT_P56 = 'p',
	OPT_SYMBOLS = 's',
	OPT_ZEROES = 'z',
	OPT_DEFINE = 'D',
	OPT_INCLUDE = 'I',
	OPT_CPU = 'c'
};

static struct option const long_options[] = {
	{ "lod-file", required_argument, NULL, OPT_OUTPUT },
	{ "output", required_argument, NULL, OPT_OUTPUT },
	{ "p56-file", required_argument, NULL, OPT_P56 },
	{ "embed-asm-file", required_argument, NULL, OPT_EMBED_ASM },
	{ "embed-c-file", required_argument, NULL, OPT_EMBED_C },
	{ "symbols", no_argument, NULL, OPT_SYMBOLS },
	{ "write-zero", no_argument, NULL, OPT_ZEROES },
	{ "zero", no_argument, NULL, OPT_ZEROES },
	{ "define", required_argument, NULL, OPT_DEFINE },
	{ "include", required_argument, NULL, OPT_INCLUDE },
	{ "cpu", required_argument, NULL, OPT_CPU },
	{ "help", no_argument, NULL, OPT_HELP },
	{ "version", no_argument, NULL, OPT_VERSION },
	{ NULL, no_argument, NULL, 0 },
};


static void usage(FILE *fp, int status)
{
	fprintf(fp, "usage: %s [options] <input-file>\n", program);
	fprintf(fp, "options:\n");
	fprintf(fp, "  -o, --output <file>          LOD output file.\n");
	fprintf(fp, "  -p, --p56-file <file>        P56 output file.\n");
	fprintf(fp, "  -e, --embed-asm-file <file>  Output devpac/vasm file.\n");
	fprintf(fp, "  -k, --embed-c-file <file>    Output C file.\n");
	fprintf(fp, "  -s, --symbols                Output symbols (in LOD).\n");
	fprintf(fp, "  -z, --write-zero             Output section even if it contains only zeros.\n");
	fprintf(fp, "  -D, --define <name[=value]>  Define a symbol.\n");
	fprintf(fp, "  -I, --include <dir>          Add include path.\n");
	fprintf(fp, "  -c, --cpu <type>             Sets CPU type.\n");
	
	exit(status);
}


int main(int argc, char *argv[])
{
	int ret = EXIT_SUCCESS;
	int c;
	
	StringBufferInit(0x8000);
	InitSymbolTable();

	while ((c = getopt_long(argc, argv, "c:e:k:o:p:szD:I:Vh", long_options, NULL)) != -1)
	{
		switch ((enum opt) c)
		{
		case OPT_OUTPUT:
			free(lod_output_name);
			lod_output_name = strdup(optarg);
			break;

		case OPT_P56:
			free(p56_output_name);
			p56_output_name = strdup(optarg);
			break;

		case OPT_EMBED_ASM:
			free(embed_asm_output_name);
			embed_asm_output_name = strdup(optarg);
			break;

		case OPT_EMBED_C:
			free(embed_c_output_name);
			embed_c_output_name = strdup(optarg);
			break;

		case OPT_SYMBOLS:
			g_output_symbols = 1;
			break;
		
		case OPT_ZEROES:
			g_write_zero_sections = 1;
			break;
		
		case OPT_DEFINE:
			DefineSymbol(optarg);
			break;
		
		case OPT_INCLUDE:
			AddIncludePath(optarg);
			break;
		
		case OPT_CPU:
			g_dsp_cpu = (int)strtol(optarg, NULL, 10);
			break;
		
		case OPT_VERSION:
			fprintf(stdout, "%s version %s\n", program, version);
			return EXIT_SUCCESS;
		
		case OPT_HELP:
			usage(stdout, EXIT_SUCCESS);
			break;
		
		case OPT_FLAG_SET:
			break;
		
		default:
			return EXIT_FAILURE;
		}
	}

	if (optind < argc)
		input_name = argv[optind];
	
	ret = asm56k();

	return ret;
}
