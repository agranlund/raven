/*
 * _iob.c - define standard I/O control control blocks
 *
 * $Log: _iob.c,v $
 * Revision 1.1  1991/06/16  09:41:02  AGK
 * Initial revision
 *
 * Copyright (c) 1991 HiSoft
 */

#include <stdio.h>

struct _iobuf _iob[5] = { {&_iob[1]}, {&_iob[2]}, {&_iob[3]}, {&_iob[4]} };
