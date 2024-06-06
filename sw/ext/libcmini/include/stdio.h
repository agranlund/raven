/*
 * stdio.h
 *
 *  Created on: 29.05.2013
 *      Author: mfro
 */

#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdlib.h>
#include <stdarg.h>
#include <mint/osbind.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/* The mode of I/O, as given in the MODE argument to fopen, etc.  */
typedef struct
{
  unsigned int __read:1;    /* Open for reading.  */
  unsigned int __write:1;   /* Open for writing.  */
  unsigned int __append:1;  /* Open for appending.  */
  unsigned int __binary:1;  /* Opened binary.  */
  unsigned int __create:1;  /* Create the file.  */
  unsigned int __exclusive:1;   /* Error if it already exists.  */
  unsigned int __truncate:1;    /* Truncate the file on opening.  */
} __io_mode;

typedef struct __stdio_file FILE;
struct __stdio_file
{
    long __magic;
#define	_IOMAGIC (0xfedabeecL)	/* Magic number to fill `__magic'.  */

    void *__cookie;			/* Magic cookie. Holds GEMDOS handle */
	int __pushback;
    FILE *__next;     		/* Next FILE in the linked list.  */
	__io_mode __mode;     /* File access mode.  */
	unsigned int __eof:1;
	unsigned int __error:1;
};

extern FILE *stdout;
extern FILE *stdin;
extern FILE *stderr;

#define _IONBF	2

#ifndef NULL
#define NULL        ((void *)0)
#endif

#define OPEN_MAX        32
#define FOPEN_MAX       32
#define FILENAME_MAX    128
#define PATH_MAX        128
#define BUFSIZ          1024
#define EOF             (-1)

/*
 * access modes for open()
 */
#define O_RDONLY        0x00    /* open read-only */
#define O_WRONLY        0x01    /* open write-only (this doesn't work apparently on _all_ TOS versions */
#define O_RDWR          0x02    /* open for reading and writing */
#define O_APPEND        0x08    /* position file pointer to end of file before each write */
#define O_CREAT         0x20    /* create file if it doesn't exist */
#define O_TRUNC         0x40    /* truncate to zero size */
#define O_EXCL          0x80    /* ? */

#define SEEK_SET    0   /* Seek from beginning of file.  */
#define SEEK_CUR    1   /* Seek from current position.  */
#define SEEK_END    2   /* Seek from end of file.  */

extern int errno;

FILE *fopen(const char *path, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);
FILE *fdopen(int fd, const char *mode);
int fclose(FILE *fp);
int fcloseall(void);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fseek(FILE *fp, long offset, int origin);
long ftell(FILE *stream);
void rewind(FILE *stream);

int setvbuf(FILE *stream, char *buf, int mode, size_t size);

int fputs(const char *s, FILE *stream);
int puts(const char *s);
int fputc(int c, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
int puts(const char *s);

int fgetc(FILE *stream);
char* fgets(char *s, int n, FILE *stream);
int getc(FILE *stream);
/* No gets() anymore! */
#define getchar()  fgetc(stdin)
int ungetc(int c, FILE *stream);

int scanf(const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
int vscanf(const char *format, va_list list);
int vfscanf(FILE* fp, const char *format, va_list list);
int vsscanf(const char* str, const char *format, va_list list);

int fprintf(FILE *stream, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list ap);
int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list ap);
int snprintf(char *s, size_t size, const char *fmt, ...);
int vsnprintf(char *str, size_t size, const char *fmt, va_list va);
int sprintf(char *s, const char *format, ...);
int vsprintf(char *s, const char *format, va_list va);
int asprintf(char** strp, const char* format, ...);
int vasprintf(char** strp, const char* format, va_list ap);

int fflush(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void clearerr(FILE *stream);
int fileno(FILE *stream);

int open(const char *filename, int access, ...);
int close(int fd);
int unlink(const char *filename);

int remove(const char *filename);
int rename(const char *oldname, const char *newname);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* STDIO_H_ */
