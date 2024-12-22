/**
 * $Id: stdlib.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * General utilities and defines
 *
 * (c) Copyright 1989, 1993 HiSoft
**/
 
#ifndef _STDLIB_H
#define	_STDLIB_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long int size_t;
#endif

#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned char wchar_t;
#endif

#define MB_CUR_MAX __mb_cur_max
extern char __mb_cur_max;

typedef struct
{
	int quot;
	int rem;
} div_t;

typedef struct
{
	long int quot;
	long int rem;
} ldiv_t;

void *malloc(size_t);
void *calloc(size_t,size_t);
void *realloc(void *, size_t);
void free(void *);

void *getml(size_t);
int rlsml(void *, size_t);
size_t sizmem(void);
size_t chkml(void);

void *getmem(unsigned);
int rlsmem(void *, unsigned);

void *alloca(size_t);

extern size_t _stkdelta;	/* stack/data area separation chicken factor */

void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
void dqsort(double *,size_t);
void fqsort(float *,size_t);
void lqsort(long *,size_t);
void sqsort(short *,size_t);
void tqsort(char **,size_t);

int mblen(const char *,size_t);
size_t mbstowcs(wchar_t *, const char *, size_t);
int mbtowc(wchar_t *, const char *, size_t);
size_t wcstombs(char *, const wchar_t *, size_t);
int wctomb(char *, wchar_t);

void exit(int);
void abort(void);
int atoi(const char *);
double atof(const char *);
long int atol(const char *);
char *getenv(const char *);

void _exit(int);
void _XCEXIT(int);
char *argopt(int, const char *[], const char *, int *, char *);

void *bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));

void *lsearch(const void *, void *, size_t *, size_t, int (*)(const void *, const void *));
void *lfind(const void *, const void *, const size_t *, size_t, int (*)(const void *, const void *));

int system(const char *);
size_t _hash(const char *);

#undef abs
int abs(int);
int __builtin_abs(int);
#define abs(i) __builtin_abs(i)

#undef iabs
int iabs(int);
int __builtin_abs(int);
#define iabs(i) __builtin_abs(i)

#undef labs
long labs(long);
long __builtin_labs(long);
#define labs(i) __builtin_labs(i)

#ifndef min
int min(int,int);
int __builtin_min(int,int);
#define min(a,b)	__builtin_min(a,b)
#endif

#ifndef max
int max(int,int);
int __builtin_max(int,int);
#define max(a,b)	__builtin_max(a,b)
#endif

char *ecvt(double, int, int *, int *);
char *fcvt(double, int, int *, int *);
char *gcvt(double, int, char *);
long getfnl(const char *, char *, size_t, int);
int onexit(int(*)(int));
int putenv(char *);

int rand(void);
int rmvenv(const char *);
void srand(unsigned int);
double strtod(const char *, char **);
long int strtol(const char *, char **, int);
unsigned long int strtoul(const char *,char **,int);
long int utpack(const char *);
void utunpk(long int, char *);

int atexit(void (* )(void));
div_t div(int, int);
ldiv_t ldiv(long int, long int);

unsigned long _lrotl(unsigned long,int);
#pragma inline d0=_lrotl(D0,D1) {"e3b8";}

unsigned short _rotl(unsigned int,int);
#pragma inline d0=_rotl(D0,D1) {"e378";}

unsigned long _lrotr(unsigned long,int);
#pragma inline d0=_lrotr(D0,D1) {"e2b8";}

unsigned short _rotr(unsigned int,int);
#pragma inline d0=_rotr(D0,D1) {"e278";}

extern char *optarg;
extern int optopt;
extern int optind;
extern int opterr;
int getopt(int, char *const *, const char *);

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 3

#define RAND_MAX 32767

#ifndef NULL
#define NULL	__NULL
#endif
#endif
