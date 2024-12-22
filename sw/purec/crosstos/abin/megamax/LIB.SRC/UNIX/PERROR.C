
#include <stdio.h>
#include "../string.h"

#ifndef	CURERRMSG
#define	CURERRMSG	((-errno)>sys_nerr?sys_errlist[sys_nerr-1]:\
					 (sys_errlist[-errno]?sys_errlist[-errno]:sys_unused))
#endif

char *sys_unused = perror_1;
#define	UNUSED	0

int sys_nerr = 68;
char *sys_errlist[] = {
	perror_2,			/* 0 */
	perror_3,			/* 1 */
	perror_4,			/* 2 */
	perror_5,			/* 3 */
	perror_6,			/* 4 */
	perror_7,			/* 5 */
	perror_8,			/* 6 */
	perror_9,			/* 7 */
	perror_10,			/* 8 */
	perror_11,			/* 9 */
	perror_12,			/* 10 */
	perror_13,			/* 11 */
	perror_14,			/* 12 */
	perror_15,			/* 13 */
	perror_16,			/* 14 */
	perror_17,			/* 15 */
	perror_18,			/* 16 */
	perror_19,			/* 17 */
	UNUSED,				/* 18 */
	UNUSED,				/* 19 */
	UNUSED,				/* 20 */
	UNUSED,				/* 21 */
	UNUSED,				/* 22 */
	UNUSED,				/* 23 */
	UNUSED,				/* 24 */
	UNUSED,				/* 25 */
	UNUSED,				/* 26 */
	UNUSED,				/* 27 */
	UNUSED,				/* 28 */
	UNUSED,				/* 29 */
	UNUSED,				/* 30 */
	UNUSED,				/* 31 */
	perror_20,          /* 32 */
	perror_21,			/* 33 */
	perror_22,			/* 34 */
	perror_23,		    /* 35 */
	perror_24,			/* 36 */
	UNUSED,				/* 37 */
	UNUSED,				/* 38 */
	perror_25,			/* 39 */
	perror_26,			/* 40 */
	UNUSED,				/* 41 */
	UNUSED,				/* 42 */
	UNUSED,				/* 43 */
	UNUSED,				/* 44 */
	UNUSED,				/* 45 */
	perror_27,			/* 46 */
	UNUSED,				/* 47 */
	UNUSED,				/* 48 */
	perror_28,			/* 49 */
	UNUSED,				/* 50 */
	UNUSED,				/* 51 */
	UNUSED,				/* 52 */
	UNUSED,				/* 53 */
	UNUSED,				/* 54 */
	UNUSED,				/* 55 */
	UNUSED,				/* 56 */
	UNUSED,				/* 57 */
	UNUSED,				/* 58 */
	UNUSED,				/* 59 */
	UNUSED,				/* 60 */
	UNUSED,				/* 61 */
	UNUSED,				/* 62 */
	UNUSED,				/* 63 */
	perror_29,			/* 64 */
	perror_30,			/* 65 */
	perror_31,			/* 66 */
	perror_32};			/* 67 */
	
fperror(s, fd)
char *s;
int fd;
{
	if (s && *s) {
		write(fd, s, strlen(s));
		write(fd, ": ", 2);
	}

	s = CURERRMSG;
	write(fd, s, strlen(s));
	write(fd, "\n", 1);
}

perror(s)
char *s;
{
	fperror(s, STDERR);
}
