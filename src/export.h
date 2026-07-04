/*

Project:    asm56k
Author:     M.Buras (sqward)

*/

#ifndef _EXPORT_H_
#define _EXPORT_H_

#define TRUE 1
#define FALSE 0

extern int		g_dsp_cpu;
extern int	    g_passNum;
extern int      g_write_zero_sections;
extern void		asm_abort(void);

extern int 		g_currentLine;
extern int 		g_errorLine;
extern int      g_LocalSerial;
extern void		yyerror(const char *s, ... ) __attribute__((format(printf, 1, 2)));
extern void     yywarning(const char *s, ... ) __attribute__((format(printf, 1, 2)));
extern int		yylex(void);
extern int		asmlex(void);
extern void		mtest(void *vector, const char *File, int Line);
#define MTEST(v) mtest(v, __FILE__, __LINE__)
extern void		debugprint(const char *s, ...) __attribute__((format(printf, 1, 2)));

#ifdef _MSC_VER
	#define snprintf sprintf_s
	#include <malloc.h>
#endif

/*#define DEBUG */

#define TRUE 1
#define FALSE 0

#endif /* _EXPORT_H_ */
