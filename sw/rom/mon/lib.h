/*
 * Mini C library
 */
#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef signed char     sint8;
typedef signed short    sint16;
typedef signed int      sint32;

extern size_t strlen(const char *s);
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
extern size_t hexdump(const uint8 *addr, uint32 address, size_t length, char width);
extern void fmt(const char *format, ...);
extern void _fmt(int (*emit)(int c), const char *format, va_list ap);
extern int scan(const char *buf, const char *format, ...);

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
