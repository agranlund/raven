/**
 * $Id: cdbind.h,v 1.3 1993/11/14 14:43:52 AGK Rel $
 *
 * Interface to MetaDOS CD I/O
 *
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _CDBIND_H
#define _CDBIND_H

#include <osbind.h>

#define	CDread_aud(a,b,c,d)	__lxspls(58,a,b,c,d)
#define	CDstart_aud(a,b,c)	__lxssc(59,a,b,c)
#define	CDstop_aud(a)	__lxs(60,a)
#define	CDset_songtime(a,b,c,d)	__lxssss(61,a,b,c,d)
#define	CDget_toc(a,b,c)	__lxssp(62,a,b,c)
#define	CDdisc_info(a,b)	__lxsp(63,a,b)
#endif
