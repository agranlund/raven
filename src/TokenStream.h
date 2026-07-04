/*

Project:    asm56k
Author:     M.Buras (sqward)

*/
#ifndef _TOKENSTREAM_H_
#define _TOKENSTREAM_H_

#define MAX_CONDITION_NESTS 1024

#define	MACRO_NEST_DEPTH 64
#define	MACRO_PARAMS_TOKEN_BUFFER 2048
#define MACRO_PARAMS_POINTER_BUFFER 32*64

#define LEX_BUFFER 0x8000			/* this is size of the flex buffer (for each included file) */


struct TokenVal;

typedef struct  
{
	int token;
	union
	{
		void *pNextBlock;
		YYSTYPE val;
	}data;
} TokenVal;

typedef struct{
	TokenVal 	*macro_ptr;
	int			instancesNumber;
	int			line_num;
	int 	    menager_mode;
	int 	    params_count;
	TokenVal**	params_array;
	TokenVal*	pParamsPool;
}StreamStackEntry;


extern void*	buffer;

extern	int		if_stack_l;
extern	char	if_stack[];

/* includes stack: */
extern const char*	inc_names[];
extern const char*	g_CurrentFile;
extern FILE*		inc_handles[];
extern void*		inc_buffers[];
extern int			inc_lines[];
extern int			g_incStackDeepth;

extern TokenVal *g_tokens;
extern TokenVal *wglobal_tokens;

extern int 			g_streamsStrackIndex;
extern StreamStackEntry	streamsStack[];

extern TokenVal *g_pParamsPool;
extern TokenVal **g_pParamsArrayPool;
extern int			g_MacroNumInstances;
extern TokenVal*	params_pointers[];
extern TokenVal		macros_params[];


void		InitTokenStream(const char *file_name);
void		PushStream(TokenVal *pMacro, const char *pFileName, int curline, int params_count, int instancesNumber);
int			PopStream(void);
int			TopPosStream(void);
void		ResetStream(void);
TokenVal*	GetCurrentStreamPos(void);
int			GetToken(TokenVal **pTokenValue);
int			SkipToken(void);
TokenVal*	CopyToken(int token,TokenVal *pToken);
int			PrefetchTokens(void);
void    	Skip_line(void);
int     	SkipConditional(void);
int     	PopFile(void);
int         PushNewFile(const char *pFileName);
bool		PushNewMainFile(const char *pFileName);
int    		IncludeFile(void);

void		AddIncDir(const char *pDir);
bool		GetIncDir(const char **pDir, uint *pDirNum);

/* interfacing with lex */

#define YYEOF 0

struct yy_buffer_state;

struct yy_buffer_state *asm_create_buffer(FILE *, int);
void asm_delete_buffer(struct yy_buffer_state *);
void asm_flush_buffer(struct yy_buffer_state *);
void asm_switch_to_buffer(struct yy_buffer_state *);

void Unput(int c);
int Input(void);


#endif /* _TOKENSTREAM_H_ */
