/**
 * $Id: assert.h,v 1.4 1993/11/14 14:42:42 AGK Rel $
 *
 * ANSI macros and prototypes for assertion validation
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#undef assert

#ifndef NDEBUG
#define _STR(x)	_VAL(x)
#define _VAL(x)	#x
void __assert(const char *);
#define assert(x)	((x)?(void)0:(__assert)(__FILE__ ":" _STR(__LINE__) "(" #x ")"))
#else
#define assert(x)	((void)0)
#endif
