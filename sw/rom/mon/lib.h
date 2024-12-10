/*
 * Mini C library
 */
#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef uint8_t
typedef unsigned char       uint8_t;
#endif
#ifndef uint16_t
typedef unsigned short      uint16_t;
#endif
#ifndef uint32_t
typedef unsigned int        uint32_t;
#endif
#ifndef uint64_t
typedef unsigned long long  uint64_t;
#endif
#ifndef int8_t
typedef signed char         int8_t;
#endif
#ifndef int16_t
typedef signed short        int16_t;
#endif
#ifndef int32_t
typedef signed int          int32_t;
#endif
#ifndef int64_t
typedef signed long long    int64_t;
#endif
#ifndef TRUE
#define TRUE                1
#endif
#ifndef FALSE
#define FALSE               0
#endif
#ifndef NULL
#define NULL                0
#endif
#ifndef true
#define true                1
#endif
#ifndef false
#define false               0
#endif
#ifndef null
#define null                0
#endif
#ifndef nullptr
#define nullptr             0
#endif

typedef long jmp_buf[6 + 1 + 6 + 8 * 3]; /* 6 data regs, retaddr, 6 addr regs, 8 fpu regs */

extern int setjmp(jmp_buf buf);
extern void longjmp(jmp_buf, int);
extern int sigsetjmp(jmp_buf buf, int mask);

extern size_t strlen(const char *s);
extern char* strchr(const char *s, char c);
extern char* strcpy(char *to, const char *from);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
extern void *memset(void *b, int c, size_t len);
extern void *memcpy(void *restrict dst, const void *restrict src, size_t len);
extern int memcmp(const void *s1, const void *s2, size_t n);
extern int putchar(int c);
extern int getc();
extern int puts(const char *str);
extern char *gets(char *restrict str, int size);
extern size_t hexdump(const uint8_t *addr, uint32_t address, size_t length, char width);
extern void fmt(const char *format, ...);
extern void _fmt(int (*emit)(int c), const char *format, va_list ap);
extern int scan(const char *buf, const char *format, ...);
extern void printf(const char* format, ...);

static inline int toupper(int c)
{
    return ((c >= 'a') && (c <= 'z')) ? c + ('A' - 'a') : c;
}

static inline int isspace(int c)
{
    return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\v') || (c == '\r');
}

static inline int isprint(int c)
{
	return (c >= ' ') && (c <= '~');
}

static inline int isdigit(int c)
{
	return (c >= '0') && (c <= '9');
}

static inline int isxdigit(int c)
{
	return isdigit(c) || (((c | 0x20) >= 'a') && ((c | 0x20) <= 'z'));
}

static inline int xdigit(int c)
{
	return isdigit(c) ? c - '0' : isxdigit(c) ? (c | 0x20) - 'a' + 10: -1;
}


