/**
 * $Id: fctype.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Define function forms of "is" and "to" operations (obsolete)
 *
 * (c) Copyright 1990, 1993 HiSoft
**/

#ifndef _FCTYPE_H
#define _FCTYPE_H

#undef isalnum
#undef isalpha
#undef iscntrl
#undef isdigit
#undef isgraph
#undef islower
#undef isprint
#undef ispunct
#undef isspace
#undef isupper
#undef isxdigit
#undef tolower
#undef toupper

int isalnum(int);
int isalpha(int);
int iscntrl(int);
int isdigit(int);
int isgraph(int);
int islower(int);
int isprint(int);
int ispunct(int);
int isspace(int);
int isupper(int);
int isxdigit(int);

int tolower(int);
int toupper(int);


#ifndef _ANSI
/*
 * Extensions to the ANSI standard.
 */

#undef isascii
#undef iscsym
#undef iscsymf
#undef toascii

int isascii(int);
int iscsym(int);
int iscsymf(int);
int toascii(int);
#endif
#endif

