/**
 * $Id: string.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * String and memory operations interface file
 *
 * (c) Copyright 1989, 1993 HiSoft
**/
 
#ifndef _STRING_H
#define _STRING_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef	_SIZE_T
#define	_SIZE_T
typedef unsigned long int size_t;
#endif

extern char _SLASH;		/* path separator character */

/*
 *
 * External definitions for string services
 *
 */
char *strcat(char *, const char *);
char *strchr(const char *, int);
int strcmp(const char *, const char *);
char *strcpy(char *, const char *);
size_t strcspn(const char *, const char *);
size_t strspn(const char *, const char *);
size_t strlen(const char *);
char *strncat(char *, const char *, size_t);
int strncmp(const char *, const char *, size_t);
char *strncpy(char *, const char *, size_t);
char *strpbrk(const char *, const char *);
char *strrchr(const char *, int);
char *strstr(const char *, const char *);
char *strtok(char *, const char *);
char *strerror(int);
int strcoll(const char *, const char *);
size_t strxfrm(char *, const char *, size_t);

size_t __builtin_strlen(const char *);
char *__builtin_strcpy(char *, const char *);
int __builtin_strcmp(const char *, const char *);

#define strlen(x)	__builtin_strlen(x)
#define strcpy(x,y)	__builtin_strcpy(x,y)
#define strcmp(x,y)	__builtin_strcmp(x,y)

#ifndef _ANSI
size_t stcarg(const char *, const char *);
size_t stccpy(char *to, const char *from, size_t length);
char *stpcpy(char *, const char *);
char *strdup(const char *);
void strins(char *, const char *);
char *strnset(char *, int, size_t);
char *strrev(char *);
size_t stcis(const char *, const char *);
size_t stcisn(const char *, const char *);
size_t stcpm(const char *, const char *, char **);
size_t stcpma(const char *, const char *);
char *stpblk(const char *);
char *stpbrk(const char *, const char *);
char *stpchr(const char *, int);
char *stpsym(const char *, char *, size_t);
char *stpchrn(const char *, int);
char *stptok(const char *, char *, size_t, const char *);
long strbpl(char **, size_t, const char *);

int stcgfe(char *, const char *);
int stcgfn(char *, const char *);
int stcd_i(const char *, int *);
int stcd_l(const char *, long *);
int stch_i(const char *, int *);
int stch_l(const char *, long *);
int stco_i(const char *, int *);
int stco_l(const char *, long *);
int stci_d(char *, int);
int stci_h(char *, int);
int stci_o(char *, int);
int stcl_d(char *, long);
int stcl_h(char *, long);
int stcl_o(char *, long);
int stcsma(char *, char *);
int stcu_d(char *, unsigned);
int stcul_d(char *, unsigned long);

size_t stclen(const char *);
#define stclen(a) strlen(a)

char *stpdate(char *, int, const char *);
char *stptime(char *, int, const char *);

int strmid(const char *, char *, size_t, size_t);
char *strlwr(char *);
void strmfe(char *, const char *, const char *);
void strmfn(char *, const char *, const char *, const char *, const char *);
void strmfp(char *, const char *, const char *);
int stcgfp(char *, const char *);

int strnicmp(const char *, const char *, size_t);
int stricmp(const char *, const char *);

char *strset(char *, int);
void strsfn(const char *, char *, char *, char *, char *);
char *strupr(char *);
int stspfp(char *, int *);
void strsrt(char *[], size_t);

#define strcmpi stricmp		/* For Microsoft compatibility */

char *index(const char *, int);
char *rindex(const char *, int);
void swab(const void *,void *,size_t);
#endif

/*
 *
 * External definitions for memory block services
 *
 */
void *memchr(const void *, int, size_t);
int memcmp(const void *, const void *, size_t);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);

void *__builtin_memset(void *, int, size_t);
int __builtin_memcmp(const void *, const void *, size_t);
void *__builtin_memcpy(void *, const void *, size_t);

#define memset(x,y,z)	__builtin_memset(x,y,z)
#define memcmp(x,y,z)	__builtin_memcmp(x,y,z)
#define memcpy(x,y,z)	__builtin_memcpy(x,y,z)

#ifndef _ANSI
void bzero(void *, size_t);
void bcopy(const void *, void *, size_t);
int bcmp(const void *, const void *, size_t);

#define bzero(x,y)	__builtin_memset(x,0,y)
#define bcopy(x,y,z)	__builtin_memcpy(y,x,z)
#define bcmp(x,y,z)	__builtin_memcmp(y,x,z)

void *memccpy(void *, const void *, int, size_t);
void *memswp(void *, void *, size_t);
void *memrep(void *, const void *, size_t, size_t);

void setmem(void *, unsigned, int);
void movmem(const void *, void *, unsigned);
void repmem(const void *, void *, unsigned, unsigned);
void swmem(void *, void *, unsigned);
#endif

#ifndef NULL
#define NULL	__NULL
#endif
#endif
