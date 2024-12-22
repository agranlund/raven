/* LINTLIBRARY */
#include <stdio.h>

#ifdef NOVOID
#define void int
#endif

#ifdef ATARI
#define MAXINT 32767	/* Constant folding on old compiler is broken */
#else
#define	MAXINT	((int) ((unsigned) -1 >> 1))
#endif

/*VARARGS1*/

char *_sprintf(s, control, args)
	char *s;
	char *control;
	char *args;
{
	/*
		typedef struct _iobuf {
			char *_ptr;
			int _cnt;
			char *_base;
			int _flag;
			int _fd;
			int _bufsize;
		} FILE;
	*/

	static FILE dummy = {
		0,
		MAXINT,
		0,
		_WRITE | _BIGBUF,
		-1,
		MAXINT
	};

	dummy._ptr = dummy._base = s;
	dummy._cnt	= MAXINT;
	(void) _fprintf(&dummy, control, args);
	*dummy._ptr = 0;

	return s;
}

char *sprintf(s, args)
char *s;
char *args;
{
	return _sprintf(s, args, &args+1);
}
