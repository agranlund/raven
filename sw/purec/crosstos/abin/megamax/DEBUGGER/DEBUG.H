 
/*

					The Megamax Pseudo Debugger

						       by

						  Megamax, Inc.
rpt
*/

#ifndef __DEBUG__
#define __DEBUG__

/*
	Type section
*/
typedef unsigned char byte;
typedef int (*funcptr)();

typedef struct {
	char *lowtpa;
	char *hightpa;
	char *codebase;
	long  codelen;
	char *database;
	long  datalen;
	char *bssbase;
	long  bsslen;
} basepage;

typedef struct {
	char  *name;		/* name of function or variable		*/
	char  *addr;		/* address of function or variable	*/
	char  *format;		/* pointer to printf control string */
	int  (*userf)();	/* control function for print		*/
	struct {
		unsigned breakpt	: 2;	/* Is it a break point?  function only	*/
		unsigned trace		: 1;	/* Trace the? variables only			*/
		unsigned var		: 1;	/* variable or function?				*/
		unsigned Static		: 1;	/* is var a static?						*/
		unsigned sys		: 1;	/* is var a system ref					*/
		unsigned type		: 8;	/* type information could be useful		*/
	} flags;
} _varlist;

typedef struct _watchtype {
	char *addr;			/* Address to watched */
	long size;			/* Number of bytes to watch */
	long chksum;		/* Last checksum value */
} _watchtype;

typedef struct _cmdlist {
	char *name;
	int (*func)();
} _cmdlist;

typedef struct _typelist {
	char  *name;
	int    size;
	int  (*func)();
} _typelist;


/*
	Command & function list.
*/
extern _cmdlist _cmds[];
extern int 		_maxcmds;

/*				
	Type list data structure
*/
extern _typelist _types[];
extern int		 _maxtypes;


/*
	External Functions
*/
extern char		*_getcmd();
extern char		*_getparm();
extern long      _getconst();
extern char     *_getfname();


/*
	External Debugger User Functions
*/
extern _list(), _find(), _trace(), _breakpt(), _execcmd();
extern _null(), _doexit(), _help(), _nobreak(), _notrace(), _mdump(), _print();
extern _settype(), _print(), _mprint(), _set(), _mset(), _eval();
extern _setaltscreen(), _setaltfile(), _setsniffer(), _where(), _watch();
extern _unwatch();


/*
	External Debugger Print Functions
*/
extern _pbyte(), _pubyte(), _pchar(), _puchar(), _pint(), _pword();
extern _plong(), _pulong(), _pfloat(), _pdouble();
extern _pstring();

/*
	Types list
*/
#define CHAR		0
#define UCHAR		1
#define BYTE		2
#define UBYTE		3
#define SHORT		4
#define USHORT		5
#define INT			6
#define UNSIGNED	7
#define ENUM		8
#define LONG		9
#define ULONG		10
#define PTR			11
#define ARRAY		12
#define STRING		13
#define FLOAT		14
#define DOUBLE		15

/*
	checkptr flags
*/
#define NOTEST_ODD	0
#define TEST_ODD	1

#endif __DEBUG__
