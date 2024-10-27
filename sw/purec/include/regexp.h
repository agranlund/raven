/* The <regexp.h> header is used by the (V8-compatible) regexp(3) routines. */
/* This file is only maintained for the sake of those that don't want to
   use the GPL'd code for the POSIX or GNU compatible interface.  */
#ifndef _REGEXP_H
#define _REGEXP_H

#ifdef _REGEXP_LIBRARY_H
# error "<regexp.h> and <regex.h> can't both be included."
#endif

#define regexp __V8_regexp
#define regcomp __V8_regcomp
#define regsub __V8_regsub
/*#define regerror __V8_regerror */
#define regdump __V8_regdump
#define regexec __V8_regexec

#ifndef	_FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_TYPES_H
# include <sys/types.h>
#endif

__BEGIN_DECLS

#define CHARBITS 0377
#define NSUBEXP  10
typedef struct regexp {
	char *startp[NSUBEXP];
	char *endp[NSUBEXP];
	char regstart;		/* Internal use only. */
	char reganch;		/* Internal use only. */
	char *regmust;		/* Internal use only. */
	size_t regmlen;		/* Internal use only. */
	char program[1];	/* Unwarranted chumminess with compiler. */
} regexp;


extern regexp *regcomp (char *_exp);
extern int regexec (regexp *_prog, char *_string, int _bolflag);
extern void regsub (regexp *_prog, char *_source, char *_dest);
extern void regerror (char const *_message);
extern void regdump (regexp *r);

__END_DECLS

#endif /* _REGEXP_H */
