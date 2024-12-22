/**
 * $Id: stdio.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * ANSI I/O interface file
 *
 * (c) Copyright 1989, 1992, 1993 HiSoft
**/
 
#ifndef _STDIO_H
#define _STDIO_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef	_SIZE_T
#define	_SIZE_T
typedef __size_t size_t;
#endif

long __regargs _CXCERR(long);		/* always __regargs for efficiency */

typedef unsigned long fpos_t;

#define _BUFSIZ	512					/* standard buffer size */
#define BUFSIZ	_BUFSIZ				/* standard buffer size */
#define OPEN_MAX	32				/* maximum number of files */
#define _NFILE	OPEN_MAX			/* maximum number of files */
#define FOPEN_MAX	OPEN_MAX		/* maximum number of files */
#define FILENAME_MAX	128			/* FMSIZE */
#define L_tmpnam	FILENAME_MAX	/* maximum tmpnam filename length */
#define TMP_MAX	17576				/* Guaranteed unique temp names */

/**
*
* Definitions associated with _iobuf._flag
*
**/

#define _IOFBF	0		/* fully buffered (for setvbuf) */
#define _IOREAD	1		/* read flag */
#define _IOWRT	2		/* write flag */
#define _IONBF	4		/* non-buffered flag */
#define _IOMYBUF	8	/* private buffer flag */
#define _IOEOF	16		/* end-of-file flag */
#define _IOERR	32		/* error flag */
#define _IOLBF	64		/* line-buffered flag */
#define _IORW	128		/* read-write (update) flag */
#define _IOAPP	0x4000	/* append flag */
#define _IOXLAT	0x8000	/* translation flag */

#ifndef EOF
#define EOF	__EOF
#endif

#ifndef SEEK_SET
#define SEEK_SET	__SEEK_SET
#endif
#ifndef SEEK_CUR
#define SEEK_CUR	__SEEK_CUR
#endif
#ifndef SEEK_END
#define SEEK_END	__SEEK_END
#endif

typedef struct _iobuf {
	struct _iobuf *_next;
	unsigned char *_ptr;	/* current buffer pointer */
	int _rcnt;				/* current byte count for reading */
	int _wcnt;				/* current byte count for writing */
	unsigned char *_base;	/* base address of I/O buffer */
	int _size;				/* size of buffer */
	int _flag;				/* control flags */
	int _file;				/* file descriptor */
	unsigned char _cbuff;	/* single char buffer */
} FILE;

extern struct _iobuf _iob[];

#define stdin (&_iob[0])	/* standard input file pointer */
#define stdout (&_iob[1])	/* standard output file pointer */
#define stderr (&_iob[2])	/* standard error file pointer */
#define	stdaux (&_iob[3])	/* standard auxiliary file pointer */
#define	stdprt (&_iob[4])	/* standard printer file pointer */

/*
 * Prototypes for ANSI standard functions.
 */
#undef rename
int rename(const char *,const char *);
#ifndef __NO_STDIO_DEFINES
long _lgscc(int,int,const char *,const char *);
#pragma inline d0=_lgscc((short),(short),,)	{register d2,a2; "4e41";}
#define rename(x,y)	_CXCERR(_lgscc(0x56,0,x,y))
#endif

#undef remove
int remove(const char *);
#ifndef __NO_STDIO_DEFINES
long _lgc(int,const char *);
#pragma inline d0=_lgc((short),)	{register d2,a2; "4e41";}
#define remove(x)	_CXCERR(_lgc(0x41,x))
#endif

FILE *tmpfile(void);
char *tmpnam(char *s);

int fclose(FILE *);
int fflush(FILE *);
FILE *fopen(const char *, const char *);
FILE *freopen(const char *, const char *, FILE *);
void setbuf(FILE *, char *);
int setvbuf(FILE *, char *, int, size_t);

int fprintf(FILE *, const char *, ...);
int fscanf(FILE *, const char *, ...);
int printf(const char *, ...);
int lprintf(const char *, ...);
int scanf(const char *, ...);
int sprintf(char *, const char *, ...);
int sscanf(const char *, const char *, ...);
int vfprintf(FILE *, const char *, char *);
int vprintf(const char *, char *);
int vsprintf(char *, const char *, char *);

int __builtin_printf(const char *, ...);
#ifndef __NO_STDIO_DEFINES
#define printf __builtin_printf
#endif

int fgetc(FILE *);
char *fgets(char *, int, FILE *);
int fputc(int, FILE *);
int fputs(const char *, FILE *);
int getc(FILE *);
#ifndef __NO_STDIO_DEFINES
#define getc(p) (--(p)->_rcnt>=0 ? *(p)->_ptr++ : fgetc(p))
#endif
int getchar(void);
#ifndef __NO_STDIO_DEFINES
#define getchar() getc(stdin)
#endif
char *gets(char *);
int putc(int, FILE *);
#ifndef __NO_STDIO_DEFINES
#define putc(c,p) (--(p)->_wcnt>=0 ? *(p)->_ptr++=(c) : fputc(c,p))
#endif
int putchar(int);
#ifndef __NO_STDIO_DEFINES
#define putchar(c) putc(c,stdout)
#endif
int puts(const char *);
int ungetc(int, FILE *);

size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int fgetpos(FILE *, fpos_t *);
int fseek(FILE *, long int, int);
int fsetpos(FILE *, const fpos_t *);
long int ftell(FILE *);
void rewind(FILE *);
#ifndef __NO_STDIO_DEFINES
#define rewind(p) fseek(p, 0L, 0)
#endif

void clearerr(FILE *);
#ifndef __NO_STDIO_DEFINES
#define clearerr(p) ((p)->_flag &= ~(_IOERR | _IOEOF))
#endif
int feof(FILE *);
#ifndef __NO_STDIO_DEFINES
#define feof(p) (((p)->_flag & _IOEOF) != 0)
#endif
int ferror(FILE *);
#ifndef __NO_STDIO_DEFINES
#define ferror(p) (((p)->_flag & _IOERR) != 0)
#endif
void perror(const char *);

/*
 * Prototypes for Non-ANSI functions.
 */
int fcloseall(void);
FILE *fdopen(int, const char *);
int fgetchar(void);
int fileno(FILE *);
int flushall(void);
void fmode(FILE *, int);
int __stdargs _writes(char *);
int _tinyprintf(char *, ...);
int fputchar(int);
int setnbf(FILE *);
int __flushbuff(int, FILE *);
FILE *fopene(const char *, const char *,char *);

FILE *popen(const char *,const char *);
int	pclose(FILE *);

extern int sys_nerr;
extern const char *sys_errlist[];

#undef unlink
int unlink(const char *);
#ifndef __NO_STDIO_DEFINES
#define unlink remove
#endif

char *mktemp(char *s);
short fputw(int,FILE *);
long fputl(long,FILE *);
short fgetw(FILE *);
long fgetl(FILE *);

#ifndef __NO_STDIO_DEFINES
#define	clrerr	clearerr
#define	fileno(p) ((p)->_file)
#define	_flsbf	__flushbuff
#define	_getbf	__getbuff
#endif

int _getbf(FILE *);

extern int __fmask;
extern int __fmode;
extern int _bufsiz;		/* default file buffer size */

#ifndef NULL
#define NULL	__NULL
#endif
#endif
