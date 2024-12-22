#ifndef DL_STDIO
#define DL_STDIO

#define _BUFSIZE 512
#define BUFSIZ	_BUFSIZE		/* Unix compatable */
#define _NFILE 20

typedef struct _iobuf {
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _fd;
    int _bufsize;	/* buffer size for this file */
} FILE;
extern FILE _iob[_NFILE];

#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])
#define STDIN 	0
#define STDOUT 	1
#define STDERR 	STDOUT
#define STDAUX	2
#define STDPRT	3

#define _READ	 01
#define _WRITE	 02
#define _APPEND  04
#define _UNBUF	 010
#define _BIGBUF  020
#define _EOF	 040
#define _ERR	 0100
#define _DIRTY	 0200	/* buffer was changed */
#define _LINBUF  0400
#define	_IFLUSH	 01000	/* (ONLY STDIN) Flush stdout when filling */
#define _RDWR    02000

#define NULL 0L    /* must be long since it can be passed as a parameter */
#define EOF (-1)

#define getc(p) (--(p)->_cnt >= 0 ? *(p)->_ptr++ & 0377 : _fillbuf(p))
#define getchar() getc(stdin)
#define putc(x,p) (--(p)->_cnt >= 0 ? (*(p)->_ptr++ = (x)) & 0377 : \
    _flushbuf((x),p))
#define putchar(x) putc(x,stdout)
#define feof(p) ((p)->_flag&_EOF)
#define ferror(p) ((p)->_flag&_ERR)
#define clearerr(p) ((p)->_flag &= ~(_ERR | _EOF))
#define fileno(p) ((p)->_fd)
#define rand() (int)(_seed = _seed * 6907 + 130253)
#define srand(x) _seed = x;

extern FILE *fopen(), *fdopen(), *freopen();
extern long ftell();
extern char *gets();
extern char *fgets();
extern long _seed;

/* 
 * These are not normally part of stdio.h, but are included here to help
 * reduce errors made by beginning programmers. 
 */
extern char *sprintf(), *malloc(), *lmalloc(), *calloc(), *lcalloc();
extern char *alloca(), *realloc(), *lrealloc();
extern long labs(), lseek();

extern int errno;	/* defined in exit.c */

typedef long jmp_buf[14];

#endif /* DL_STDIO */
