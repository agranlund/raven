/*

Project:    asm56k
Author:     M.Buras (sqward)


*/
#ifndef _MACROPROXY_H_
#define _MACROPROXY_H_

#define PURE_PARSER 0
#define REPLAY_MACRO 1

void 	InitMacroProxy(void);
void 	Replay_Macro(hs *name);
void 	Record_Macro(hs *temp);
void 	Skip_Macro(void);

void 	MacroCall(const char *pString);
void 	MacroRecord(const char *pString);
void 	MacroError(void);

void    ResetLocalLabel(const char *pString);

#endif /* _MACROPROXY_H_ */
