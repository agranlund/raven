/**
 * $Id: conio.h,v 1.2 1993/11/14 14:43:52 AGK Rel $
 *
 * Console I/O declarations
 *
 * (c) Copyright 1989, 1993 HiSoft
**/

#ifndef _CONIO_H
#define _CONIO_H

int cget(void);
int cgetc(void);
char *cgets(char *);
int cputc(int);
int cputs(const char *);
int cprintf(const char *, ...);
int cscanf(const char *, ...);
int getch(void);
int getche(void);
int kbhit(void);
int iskbhit(void);
int putch(int);
int ungetch(int);
#endif
