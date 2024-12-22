
#include "debug.h"

/*
	Debugger Command Table
*/
_cmdlist _cmds[] = {
	"?",			_help,
	"altfile",		_setaltfile,
	"break",		_breakpt,
	"dump",			_mdump,
	"eval",			_eval,
	"exit",    		_doexit,
	"find",			_find,
	"g",			_null,
	"go",			_null,
	"help",			_help,
	"list",			_list,
	"exec",			_execcmd,
	"mprint",		_mprint,
	"mset",			_mset,
	"print",		_print,
	"quit",			_doexit,
	"screenoff",	_setaltscreen,
	"set",			_set,
	"show",			_find,
	"sniffer",		_setsniffer,
	"trace",		_trace,
	"type",			_settype,
	"unbreak",		_nobreak,
	"untrace",		_notrace,
	"where",		_where,
	"watch",		_watch,
	"unwatch",		_unwatch,
	"",				_null
};


_typelist _types[] = {
	{ "char",		1,	_pchar   },
	{ "uchar",		1,	_puchar  },
	{ "byte",		1,	_pbyte   },
	{ "ubyte",		1,	_pubyte  },
	{ "short",		2,	_pint	 },
	{ "ushort",		2,	_pword	 },
	{ "int",		2,	_pint    },
	{ "unsigned",	2,	_pword   },
	{ "enum",		2,	_pword	 },
	{ "long",		4,	_plong   },
	{ "ulong",		4,	_pulong  },
	{ "ptr",		4,	_pulong  },
	{ "array",		4,	_pulong	 },
	{ "string",		4,	_pstring },
	{ "float",		0,	_pfloat  },
	{ "double",		0,	_pdouble },
	{ "struct",		0,	_pulong	 },
	{ "union",		0,	_pulong	 }
};

int _maxcmds   = sizeof(_cmds)  / sizeof(_cmdlist);
int _maxtypes  = sizeof(_types) / sizeof(_typelist);

/*
	User Functions
*/
