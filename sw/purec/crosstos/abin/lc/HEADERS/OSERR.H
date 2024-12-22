/**
 * $Id: oserr.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * TOS specific error code information
 *
 * (c) Copyright 1991, 1993 HiSoft
**/

#ifndef _OSERR_H
#define _OSERR_H

#define E_OK	0
#define ERROR	1
#define EDRVNR	2
#define EUNCMD	3
#define E_CRC	4
#define EBADRQ	5
#define E_SEEK	6
#define EMEDIA	7
#define ESECNF	8
#define EPAPER	9
#define EWRITF	10
#define EREADF	11
#define EWRPRO	13
#define E_CHNG	14
#define EUNDEV	15
#define EBADSF	16
#define EOTHER	17
#define EDISC	18
#define EDEVNRS	19
#define EHARD	20

#define EINVFN	32
#define EFILNF	33
#define EPTHNF	34
#define ENHNDL	35
#define EACCDN	36
#define EIHNDL	37
#define ENSMEM	39
#define EIMBA	40
#define EDRIVE	46
#define ENSAME	48
#define ENMFIL	49
#define ELOCKED	58
#define ENSLOCK	59
#define E_RANGE	64
#define EINTRN	65
#define EPLFMT	66
#define EGSBF	67

extern long volatile _OSERR;
extern int os_nerr;
extern const char *os_errlist[];
#endif
