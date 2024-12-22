/**
 * $Id: mintbind.h,v 1.7 1993/11/14 14:43:52 AGK Rel $
 *
 * Interface to MiNT
 *
 * Copyright (c) 1992, 1993 HiSoft
**/

#ifndef _MINTBIND_H
#define _MINTBIND_H

#include <osbind.h>

#define Syield()	__vgv(0xff)
#define Fpipe(a)	__sgr(0x100,a)
#define Fcntl(a,b,c)	__lgsls(0x104,a,b,c)
#define Finstat(a)	__lgs(0x105,a)
#define Foutstat(a)	__lgs(0x106,a)
#define Fgetchar(a,b)	__lgss(0x107,a,b)
#define Fputchar(a,b,c)	__lgsls(0x108,a,b,c)
#define Pwait()	__lgv(0x109)
#define Pnice(a)	__sgs(0x10a,a)
#define Pgetpid()	__sgv(0x10b)
#define Pgetppid()	__sgv(0x10c)
#define Pgetpgrp()	__sgv(0x10d)
#define Psetpgrp(a,b)	__sgss(0x010e,a,b)
#define Pgetuid()	__sgv(0x10f)
#define Psetuid(a)	__sgs(0x110,a)
#define Pkill(a,b)	__sgss(0x111,a,b)
#define Psignal(a,b)	__ggsg(0x112,a,b)
#define Pvfork()	__sgv(0x113)
#define Pgetgid()	__sgv(0x114)
#define Psetgid(a)	__sgs(0x115,a)
#define Psigblock(a)	__lgl(0x116,a)
#define Psigsetmask(a)	__lgl(0x117,a)
#define Pusrval(a)	__lgl(0x118,a)
#define Pdomain(a)	__sgs(0x119,a)
#define Psigreturn()	__lgv(0x11a)
#define Pfork()	__lgv(0x11b)
#define Pwait3(a,b)	__lgsn(0x11c,a,b)
#define Fselect(a,b,c,d)	__sgunnn(0x11d,a,b,c,d)
#define Prusage(a)	__lgn(0x11e,a)
#define Psetlimit(a,b)	__lgsl(0x11f,a,b)
#define Talarm(a)	__lgl(0x120,a)
#define Pause()	__vgv(0x121)
#define Sysconf(a)	__lgs(0x122,a)
#define Psigpending()	__lgv(0x123)
#define Dpathconf(a,b)	__lgcs(0x124,a,b)
#define Pmsg(a,b,c)	__lgslp(0x125,a,b,c)
#define Fmidipipe(a,b,c)	__lgsss(0x126,a,b,c)
#define Prenice(a,b)	__sgss(0x127,a,b)
#define Dopendir(a,b)	__lgcs(0x128,a,b)
#define Dreaddir(a,b,c)	__lgslp(0x129,a,b,c)
#define Drewinddir(a)	__lgl(0x12a,a)
#define Dclosedir(a)	__lgl(0x12b,a)
#define Fxattr(a,b,c)	__lgscp(0x12c,a,b,c)
#define Flink(a,b)	__lgcc(0x12d,a,b)
#define Fsymlink(a,b)	__lgcc(0x12e,a,b)
#define Freadlink(a,b,c)	__lgsdc(0x12f,a,b,c)
#define Dcntl(a,b,c)	__lgscp(0x130,a,b,c)
#define Fchown(a,b,c)	__lgcss(0x131,a,b,c)
#define Fchmod(a,b)	__lgcs(0x132,a,b)
#define Pumask(a)	__lgu(0x133,a)
#define Psemaphore(a,b,c)	__lgsll(0x134,a,b,c)
#define Dlock(a,b)	__lgss(0x135,a,b)
#define Psigpause(a)	__lgl(0x136, a)
#define Psigaction(a,b,c)	__lgsqp(0x137,a,b,c)
#define Pgeteuid()	__sgv(0x138)
#define Pgetegid()	__sgv(0x139)
#define Pwaitpid(a,b,c)	__lgssn(0x13a,a,b,c)
#define Dgetcwd(a,b,c)	__lgdss(0x13b,a,b,c)
#define Salert(a)	__lgc(0x13c,a)

#define Dos_tab()	__ngv(-1)
#define Bios_tab()	__nbv(-1)
#define Xbios_tab()	__nxv(-1)
#endif
