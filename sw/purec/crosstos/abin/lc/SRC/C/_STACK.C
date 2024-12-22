/*
 * _stack.c - set stack size for program
 *
 * Started 15/9/89 Alex G. Kiernan
 *
 * $Log: _stack.c,v $
 * Revision 1.1  1991/06/17  12:48:08  AGK
 * Initial revision
 *
 * Copyright (c) 1989 HiSoft
 */

#include <stdlib.h>

unsigned long int _STACK = 4096;		/* 4K default size */
