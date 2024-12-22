/**
 * $Id: time.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * ANSI/POSIX.1 Date and Time definitions
 *
 * (c) Copyright 1993 HiSoft
**/

#ifndef _TIME_H
#define _TIME_H

#ifndef __TYPES_H
#include <__types.h>
#endif

#ifndef NULL
#define NULL	__NULL
#endif

#define CLOCKS_PER_SEC	(200)

#ifndef	_CLOCK_T
#define _CLOCK_T
typedef	__clock_t clock_t;
#endif

#ifndef	_TIME_T
#define _TIME_T
typedef	__time_t time_t;
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef __size_t size_t;
#endif

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

clock_t (clock)(void);
double (difftime)(time_t, time_t);
time_t (mktime)(struct tm *);
time_t (time)(time_t *);

char *(asctime)(const struct tm *);
char *(ctime)(const time_t *);
struct tm *(gmtime)(const time_t *);
struct tm *(localtime)(const time_t *);
size_t (strftime)(char *, size_t, const char *, const struct tm *);

extern char *__tzname[2];
extern int __daylight;
extern long int __timezone;
void (__tzset)(void);

#if !defined(_ANSI) \
	|| defined(_POSIX_SOURCE)
#define	CLK_TCK	CLOCKS_PER_SEC
void (tzset)(void);
extern char *(tzname)[2];
#endif

#if !(defined(_ANSI) \
	|| defined(_POSIX_SOURCE)) \
	|| defined(_LATTICE)
void (getclk)(unsigned char *);
int (chgclk)(unsigned const char *);
void (utunpk)(long, char *);
long (utpack)(const char *);
#endif

#if !(defined(_ANSI) \
	|| defined(_SYSV))
int (stime)(const time_t *);
#endif
#endif
