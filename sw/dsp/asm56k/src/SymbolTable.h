/*

Project:    asm56k
Author:     M.Buras (sqward)


*/

#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#define MAX_SYM_LEN 32
#define MAX_STR_BUF_SIZE 8192
#define MAX_DESC 256
#define MAX_CODE_SIZE 8192*4
#define MAX_MACRO_SIZE 8192*4
#define MAX_MACRO_STR_SIZE 4096

#define HASH_SIZE 512

#define T_PTR	0
#define T_MACRO 1
#define T_VALUE 2
#define T_UNINIT 3

typedef struct
{
    const char *ptr;
    int len;
}stext;

typedef struct
{
	const char	*pString;
	void		*pNext;
	int			type;
	int			mem_space;
	int			chunk_num;
	int			len;

	Value		m_val;

	void*		m_data1;
	void*		m_data2;
	int			m_data3;

}
hs;

typedef struct
{
	hs*			pHead;
	hs*			pTail;
}
hashEntry;

extern hashEntry hash_tab[HASH_SIZE];

void InitSymbolTable(void);
hs*	 AddSymbol(const char *string, int len, int forceCopy);
hs*	 FindSymbol(const char *pString);
void SymSetValue(hs *slot, int meaning, Value val);
void SymSetValueMacro(hs *slot, int meaning, void *val1, void *val2, int val3);
void SymbolSetMemSpace(hs *slot, int mem_space);
void ListSymbolTable(void);

void SymSet(const char *pSymbol, Value val);
hs *AddSym(const stext *pSymName, int forceCopy);
Value GetSym(const char *pString);
hs *AddLabel(const stext *pSymName);

#endif /* _SYMBOLTABLE_H_ */
