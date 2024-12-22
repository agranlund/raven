/**
 * $Id: osbind.h,v 1.7 1993/11/14 14:43:52 AGK Rel $
 *
 * Interface to GEMDOS/BIOS/XBIOS
 *
 * Copyright (c) 1989, 1992, 1993 HiSoft
**/

#ifndef _OSBIND_H
#define _OSBIND_H

/* GEMDOS bindings */
long *__ngv(int);
long __lgc(int,const char *);
long __lgcc(int,const char *,const char *);
long __lgcs(int,const char *,int);
long __lgcss(int,const char *,int,int);
long __lgds(int,char *,int);
long __lgdss(int,char *,int,int);
long __lgl(int,long);
long __lglss(int,long,int,int);
long __lgn(int,long *);
long __lgns(int,long *,int);
long __lgp(int,void *);
long __lgpl(int,void *,long);
long __lgrss(int,short *,int,int);
long __lgs(int,int);
long __lgscc(int,int,const char *,const char *);
long __lgsccc(int,int,const char *,const char *,const char *);
long __lgscp(int,int,const char *,void *);
long __lgsdc(int,int,char *,const char *);
long __lgsl(int,int,long);
long __lgsld(int,int,long,char *);
long __lgsll(int,int,long,long);
long __lgslp(int,int,long,void *);
long __lgslq(int,int,long,const void *);
long __lgsls(int,int,long,int);
long __lgsn(int,int,long *);
long __lgspl(int,int,void *,long);
long __lgsqp(int,int,const void *,void *);
long __lgss(int,int,int);
long __lgssl(int,int,int,long);
long __lgssn(int,int,int,long *);
long __lgssll(int,int,int,long,long);
long __lgsss(int,int,int,int);
long __lgu(int,unsigned int);
long __lgv(int);
long __sgunnn(int,unsigned,long *,long *,long *);
short __sgr(int,short *);
short __sgs(int,int);
short __sgsl(int,int,long);
short __sgsls(int,int,long,int);
short __sgss(int,int,int);
short __sgv(int);
unsigned short __ugv(int);
void (*__stdargs __ggsg(int,int,void (*__stdargs)(long)))(long);
void *__pgl(int,long);
void *__pgls(int,long,int);
void *__pgp(int,void *);
void *__pgs(int,int);
void *__pgv(int);
void __vgc(int,const char *);
void __vgls(int,long,int);
void __vgp(int,void *);
void __vgs(int,int);
void __vgv(int);

#define _GEMDOS	{register d2,a2; "4e41";}

#pragma inline __vgc((short),)	_GEMDOS
#pragma inline __vgls((short),,(short))	_GEMDOS
#pragma inline __vgp((short),)	_GEMDOS
#pragma inline __vgs((short),(short))	_GEMDOS
#pragma inline __vgv((short))	_GEMDOS
#pragma inline d0=__ggsg((short),(short),)	_GEMDOS
#pragma inline d0=__lgc((short),)	_GEMDOS
#pragma inline d0=__lgcc((short),,)	_GEMDOS
#pragma inline d0=__lgcs((short),,(short))	_GEMDOS
#pragma inline d0=__lgcss((short),,(short),(short))	_GEMDOS
#pragma inline d0=__lgds((short),,(short))	_GEMDOS
#pragma inline d0=__lgdss((short),,(short),(short))	_GEMDOS
#pragma inline d0=__lgl((short),)	_GEMDOS
#pragma inline d0=__lglss((short),,(short),(short))	_GEMDOS
#pragma inline d0=__lgn((short),)	_GEMDOS
#pragma inline d0=__lgns((short),,(short))	_GEMDOS
#pragma inline d0=__lgp((short),)	_GEMDOS
#pragma inline d0=__lgpl((short),,)	_GEMDOS
#pragma inline d0=__lgrss((short),,(short),(short))	_GEMDOS
#pragma inline d0=__lgs((short),(short))	_GEMDOS
#pragma inline d0=__lgscc((short),(short),,)	_GEMDOS
#pragma inline d0=__lgsccc((short),(short),,,)	_GEMDOS
#pragma inline d0=__lgscp((short),(short),,)	_GEMDOS
#pragma inline d0=__lgsdc((short),(short),,)	_GEMDOS
#pragma inline d0=__lgsl((short),(short),)	_GEMDOS
#pragma inline d0=__lgsld((short),(short),,)	_GEMDOS
#pragma inline d0=__lgsll((short),(short),,)	_GEMDOS
#pragma inline d0=__lgslp((short),(short),,)	_GEMDOS
#pragma inline d0=__lgslq((short),(short),,)	_GEMDOS
#pragma inline d0=__lgsls((short),(short),,(short))	_GEMDOS
#pragma inline d0=__lgsn((short),(short),)	_GEMDOS
#pragma inline d0=__lgspl((short),(short),,)	_GEMDOS
#pragma inline d0=__lgsqp((short),(short),,)	_GEMDOS
#pragma inline d0=__lgss((short),(short),(short))	_GEMDOS
#pragma inline d0=__lgssl((short),(short),(short),)	_GEMDOS
#pragma inline d0=__lgssn((short),(short),(short),)	_GEMDOS
#pragma inline d0=__lgssll((short),(short),(short),,)	_GEMDOS
#pragma inline d0=__lgsss((short),(short),(short),(short))	_GEMDOS
#pragma inline d0=__lgu((short),(short))	_GEMDOS
#pragma inline d0=__lgv((short))	_GEMDOS
#pragma inline d0=__ngv((short))	_GEMDOS
#pragma inline d0=__pgl((short),)	_GEMDOS
#pragma inline d0=__pgls((short),,(short))	_GEMDOS
#pragma inline d0=__pgp((short),)	_GEMDOS
#pragma inline d0=__pgs((short),(short))	_GEMDOS
#pragma inline d0=__pgv((short))	_GEMDOS
#pragma inline d0=__sgr((short),)	_GEMDOS
#pragma inline d0=__sgs((short),(short))	_GEMDOS
#pragma inline d0=__sgsl((short),(short),)	_GEMDOS
#pragma inline d0=__sgsls((short),(short),,(short))	_GEMDOS
#pragma inline d0=__sgss((short),(short),(short))	_GEMDOS
#pragma inline d0=__sgunnn((short),(short),,,)	_GEMDOS
#pragma inline d0=__sgv((short))	_GEMDOS
#pragma inline d0=__ugv((short))	_GEMDOS

#define Pterm0()		__vgv(0x0)
#define Cconin()		__lgv(0x1)
#define Cconout(a)		__vgs(0x2,a)
#define Cauxin()		__sgv(0x3)
#define Cauxout(a)		__vgs(0x4,a)
#define Cprnout(a)		__lgs(0x5,a)
#define Crawio(a)		__lgs(0x6,a)
#define Crawcin()		__lgv(0x7)
#define Cnecin()		__lgv(0x8)
#define Cconws(a)		__vgc(0x9,a)
#define Cconrs(a)		__vgp(0x0a,a)
#define Cconis()		__sgv(0x0b)
#define Dsetdrv(a)		__lgs(0x0e,a)
#define Cconos()		__sgv(0x10)
#define Cprnos()		__sgv(0x11)
#define Cauxis()		__sgv(0x12)
#define Cauxos()		__sgv(0x13)
#define Maddalt(a,b)	__lgpl(0x14,a,b)
#define Srealloc(a)		__pgl(0x15,a)
#define Dgetdrv()		__sgv(0x19)
#define Fsetdta(a)		__vgp(0x1a,a)
#define Super(a)		__pgp(0x20,a)
#define Tgetdate()		__ugv(0x2a)
#define Tsetdate(a)		__lgu(0x2b,a)
#define Tgettime()		__ugv(0x2c)
#define Tsettime(a)		__lgu(0x2d,a)
#define Fgetdta()		__pgv(0x2f)
#define Sversion()		__ugv(0x30)
#define Ptermres(a,b)	__vgls(0x31,a,b)
#define Dfree(a,b)		__lgns(0x36,a,b)
#define Dcreate(a)		__lgc(0x39,a)
#define Ddelete(a)		__lgc(0x3a,a)
#define Dsetpath(a)		__lgc(0x3b,a)
#define Fcreate(a,b)	__lgcs(0x3c,a,b)
#define Fopen(a,b)		__lgcs(0x3d,a,b)
#define Fclose(a)		__lgs(0x3e,a)
#define Fread(a,b,c)	__lgslp(0x3f,a,b,c)
#define Fwrite(a,b,c)	__lgslq(0x40,a,b,c)
#define Fdelete(a)		__lgc(0x41,a)
#define Fseek(a,b,c)	__lglss(0x42,a,b,c)
#define Fattrib(a,b,c)	__lgcss(0x43,a,b,c)
#define Mxalloc(a,b)	__pgls(0x44,a,b)
#define Fdup(a)			__lgs(0x45,a)
#define Fforce(a,b)		__lgss(0x46,a,b)
#define Dgetpath(a,b)	__lgds(0x47,a,b)
#define Malloc(a)		__pgl(0x48,a)
#define Mfree(a)		__lgp(0x49,a)
#define Mshrink(a,b)	__lgspl(0x4a,0,a,b)
#define Pexec(a,b,c,d)	__lgsccc(0x4b,a,b,c,d)
#define Pterm(a)		__vgs(0x4c,a)
#define Fsfirst(a,b)	__lgcs(0x4e,a,b)
#define Fsnext()		__lgv(0x4f)
#define Frename(a,b,c)	__lgscc(0x56,a,b,c)
#define Fdatime(a,b,c)	__lgrss(0x57,a,b,c)
#define Flock(a,b,c,d)	__lgssll(0x5c,a,b,c,d)

/* BIOS bindings */
long *__nbv(int);
long __lbs(int,int);
long __lbspsss(int,int,void *,int,int,int);
long __lbspsssl(int,int,void *,int,int,int,long);
long __lbss(int,int,int);
long __lbv(int);
unsigned long __mbv(int);
void (*__fbsf(int,int,void (*)()))();
void *__pbs(int,int);
void __vbp(int,void *);

#define _BIOS	{register d2,a2; "4e4d";}

#pragma inline __vbp((short),)	_BIOS
#pragma inline d0=__fbsf((short),(short),)	_BIOS
#pragma inline d0=__lbs((short),(short))	_BIOS
#pragma inline d0=__lbspsss((short),(short),,(short),(short),(short))	_BIOS
#pragma inline d0=__lbspsssl((short),(short),,(short),(short),(short),)	_BIOS
#pragma inline d0=__lbss((short),(short),(short))	_BIOS
#pragma inline d0=__lbv((short))	_BIOS
#pragma inline d0=__mbv((short))	_BIOS
#pragma inline d0=__nbv((short))	_BIOS
#pragma inline d0=__pbs((short),(short))	_BIOS

#define Getmpb(a)			__vbp(0,a)
#define Bconstat(a)			__lbs(1,a)
#define Bconin(a)			__lbs(2,a)
#define Bconout(a,b)		__lbss(3,a,b)
#define Rwabs(a,b,c,d,e)	__lbspsss(4,a,b,c,d,e)
#define Lrwabs(a,b,c,d,e)	__lbspsssl(4,a,b,c,-1,d,e)
#define Setexc(a,b)			__fbsf(5,a,b)
#define Tickcal()			__lbv(6)
#define Getbpb(a)			__pbs(7,a)
#define Bcostat(a)			__lbs(8,a)
#define Mediach(a)			__lbs(9,a)
#define Drvmap()			__mbv(10)
#define Kbshift(a)			__lbs(11,a)

/* XBIOS bindings */
char **__exccc(int,const char *,const char *,const char *);
long *__nxv(int);
long __lxcp(int, const char *, void *);
long __lxg(int,long (*)(void));
long __lxlsps(int,long,int,void *,int);
long __lxn(int,long *);
long __lxp(int,void *);
long __lxs(int,int);
long __lxs(int,int);
long __lxsl(int,int,long);
long __lxsll(int,short,long,long);
long __lxsp(int,int,void *);
long __lxspls(int,int,void *,long,int);
long __lxspp(int,int,void *,void *);
long __lxsqls(int,int,void *,long,int);
long __lxss(int,int,int);
long __lxss(int,int,int);
long __lxssc(int,int,int,const char *);
long __lxssp(int,int,int,void *);
long __lxssss(int,int,int,int,int);
long __lxsssss(int,int,int,int,int,int);
long __lxsssss(int,int,int,int,int,int);
long __lxv(int);
short __sxcs(int, const char *, int);
short __sxcsp(int, const char *, int, void *);
short __sxll(int, long, long);
short __sxp(int,void *);
short __sxppss(int,void *,void *,int,int);
short __sxppsssss(int,void *,void *,int,int,int,int,int);
short __sxprsssssls(int,void *,short *,int,int,int,int,int,long,int);
short __sxqls(int, const void *, long, int);
short __sxs(int,int);
short __sxsp(int,int,void *);
short __sxss(int,int,int);
short __sxsssd(int,int,int,int,char *);
short __sxv(int);
unsigned char bxv(int);
unsigned long __mxssssss(int,int,int,int,int,int,int);
unsigned long __mxv(int);
void **__rxv(int);
void *__pxs(int,int);
void *__pxv(int);
void __vxc(int,const char *);
void __vxcldl(int, unsigned const char *, long, unsigned char *, long);
void __vxfg(int, void (*__stdargs)(long), long (*)(void));
void __vxllqp(int, long, long, const void *, void *);
void __vxm(int,unsigned long);
void __vxnn(int, long *, long *);
void __vxp(int,void *);
void __vxplln(int, void *, long, long, long *);
void __vxplss(int,void *,long,int,int);
void __vxpps(int,void *,void *,int);
void __vxqlln(int, const void *, long, long, long *);
void __vxqlpl(int, const void *, long, void *, long);
void __vxolnl(int, const long *, long, long *, long);
void __vxlls(int, long, long, int);
void __vxqls(int, const void *, long, int);
void __vxqpllln(int, const void *, void *, long, long, long, long *);
void __vxs(int,int);
void __vxsc(int,int,const char *);
void __vxsf(int,int,void (*)(void));
void __vxsl(int,int,long);
void __vxsp(int,int,void *);
void __vxssl(int,int,int,long);
void __vxspf(int,int,void *,void (*)());
void __vxssn(int, int, int, long *);
void __vxsso(int, int, int, const long *);
void __vxssr(int,int,int,short *);
void __vxsss(int, int, int, int);
void __vxsssf(int,int,int,int,void (*)());
void __vxtlrl(int, const short *, long, short *, long);
void __vxv(int);

#define _XBIOS	{register d2,a2; "4e4e";}

#pragma inline __vxc((short),)	_XBIOS
#pragma inline __vxcldl((short),,,,)	_XBIOS
#pragma inline __vxfg((short),,)	_XBIOS
#pragma inline __vxllqp((short),,,,)	_XBIOS
#pragma inline __vxm((short),)	_XBIOS
#pragma inline __vxnn((short),,)	_XBIOS
#pragma inline __vxp((short),)	_XBIOS
#pragma inline __vxplln((short),,,,)	_XBIOS
#pragma inline __vxplss((short),,,(short),(short))	_XBIOS
#pragma inline __vxpps((short),,,(short))	_XBIOS
#pragma inline __vxqlln((short),,,,)	_XBIOS
#pragma inline __vxqlpl((short),,,,)	_XBIOS
#pragma inline __vxolnl((short),,,,)	_XBIOS
#pragma inline __vxlls((short),,,(short))	_XBIOS
#pragma inline __vxqls((short),,,(short))	_XBIOS
#pragma inline __vxqpllln((short),,,,,,)	_XBIOS
#pragma inline __vxs((short),(short))	_XBIOS
#pragma inline __vxsc((short),(short),)	_XBIOS
#pragma inline __vxsf((short),(short),)	_XBIOS
#pragma inline __vxsl((short),(short),)	_XBIOS
#pragma inline __vxsp((short),(short),)	_XBIOS
#pragma inline __vxssl((short),(short),(short),)	_XBIOS
#pragma inline __vxspf((short),(short),,)	_XBIOS
#pragma inline __vxssn((short),(short),(short),)	_XBIOS
#pragma inline __vxsso((short),(short),(short),)	_XBIOS
#pragma inline __vxssr((short),(short),(short),)	_XBIOS
#pragma inline __vxsss((short),(short),(short))	_XBIOS
#pragma inline __vxsssf((short),(short),(short),(short),)	_XBIOS
#pragma inline __vxtlrl((short),,,,)	_XBIOS
#pragma inline __vxv((short))	_XBIOS
#pragma inline d0=__bxv((short))	_XBIOS
#pragma inline d0=__exccc((short),,,)	_XBIOS
#pragma inline d0=__lxcp((short),,)	_XBIOS
#pragma inline d0=__lxg((short),)	_XBIOS
#pragma inline d0=__lxlsps((short),,(short),,(short))	_XBIOS
#pragma inline d0=__lxn((short),)	_XBIOS
#pragma inline d0=__lxp((short),)	_XBIOS
#pragma inline d0=__lxs((short),(short))	_XBIOS
#pragma inline d0=__lxsl((short),(short),)	_XBIOS
#pragma inline d0=__lxsll((short),(short),,)	_XBIOS
#pragma inline d0=__lxsp((short),(short),)	_XBIOS
#pragma inline d0=__lxspls((short),(short),,,(short))	_XBIOS
#pragma inline d0=__lxspp((short),(short),,)	_XBIOS
#pragma inline d0=__lxsqls((short),(short),,,(short))	_XBIOS
#pragma inline d0=__lxss((short),(short),(short))	_XBIOS
#pragma inline d0=__lxss((short),(short),(short))	_XBIOS
#pragma inline d0=__lxssc((short),(short),(short),)	_XBIOS
#pragma inline d0=__lxssp((short),(short),(short),)	_XBIOS
#pragma inline d0=__lxssss((short),(short),(short),(short),(short))	_XBIOS
#pragma inline d0=__lxsssss((short),(short),(short),(short),(short),(short))	_XBIOS
#pragma inline d0=__lxsssss((short),(short),(short),(short),(short),(short))	_XBIOS
#pragma inline d0=__lxv((short))	_XBIOS
#pragma inline d0=__mxssssss((short),(short),(short),(short),(short),(short),(short))	_XBIOS
#pragma inline d0=__mxv((short))	_XBIOS
#pragma inline d0=__nxv((short))	_XBIOS
#pragma inline d0=__pxs((short),(short))	_XBIOS
#pragma inline d0=__pxv((short))	_XBIOS
#pragma inline d0=__rxv((short))	_XBIOS
#pragma inline d0=__sxcs((short),,(short))	_XBIOS
#pragma inline d0=__sxcsp((short),,(short),)	_XBIOS
#pragma inline d0=__sxll((short),,)	_XBIOS
#pragma inline d0=__sxp((short),)	_XBIOS
#pragma inline d0=__sxppss((short),,,(short),(short))	_XBIOS
#pragma inline d0=__sxppsssss((short),,,(short),(short),(short),(short),(short))	_XBIOS
#pragma inline d0=__sxprsssssls((short),,,(short),(short),(short),(short),(short),,(short))	_XBIOS
#pragma inline d0=__sxqls((short),,,(short))	_XBIOS
#pragma inline d0=__sxs((short),(short))	_XBIOS
#pragma inline d0=__sxsp((short),(short),)	_XBIOS
#pragma inline d0=__sxss((short),(short),(short))	_XBIOS
#pragma inline d0=__sxsssd((short),(short),(short),(short),)	_XBIOS
#pragma inline d0=__sxv((short))	_XBIOS

#define Initmous(a,b,c)				__vxspf(0,a,b,c)
#define Ssbrk(a)					__lxs(1,a)
#define Physbase()					__pxv(2)
#define Logbase()					__pxv(3)
#define Getrez()					__sxv(4)
#define Setscreen(a,b,c)			__vxpps(5,a,b,c)
#define Setpallete(a)				__vxp(6,a)
#define Setpalette(a)				__vxp(6,a)
#define Setcolor(a,b)				__sxss(7,a,b)
#define Floprd(a,b,c,d,e,f,g)		__sxppsssss(8,a,b,c,d,e,f,g)
#define Flopwr(a,b,c,d,e,f,g)		__sxppsssss(9,a,b,c,d,e,f,g)
#define Flopfmt(a,b,c,d,e,f,g,h,i)	__sxprsssssls(10,a,b,c,d,e,f,g,h,i)
#define Dbmsg(a,b,c)				__vxssl(11,a,b,c)
#define Midiws(a,b)					__vxsc(12,a,b)
#define Mfpint(a,b)					__vxsf(13,a,b)
#define Iorec(a)					__pxs(14,a)
#define Rsconf(a,b,c,d,e,f)			__mxssssss(15,a,b,c,d,e,f)
#define Keytbl(a,b,c)				__exccc(16,a,b,c)
#define Random()					__lxv(17)
#define Protobt(a,b,c,d)			__vxplss(18,a,b,c,d)
#define Flopver(a,b,c,d,e,f,g)		__sxppsssss(19,a,b,c,d,e,f,g)
#define Scrdmp()					__vxv(20)
#define Cursconf(a,b)				__sxss(21,a,b)
#define Settime(a)					__vxm(22,a)
#define Gettime()					__mxv(23)
#define Bioskeys()					__vxv(24)
#define Ikbdws(a,b)					__vxsc(25,a,b)
#define Jdisint(a)					__vxs(26,a)
#define Jenabint(a)					__vxs(27,a)
#define Giaccess(a,b)				__sxss(28,a,b)
#define Offgibit(a)					__vxs(29,a)
#define Ongibit(a)					__vxs(30,a)
#define Xbtimer(a,b,c,d)			__vxsssf(31,a,b,c,d)
#define Dosound(a)					__vxc(32,a)
#define Setprt(a)					__sxs(33,a)
#define Kbdvbase()					__rxv(34)
#define Kbrate(a,b)					__sxss(35,a,b)
#define Prtblk(a)					__sxp(36,a)
#define Vsync()						__vxv(37)
#define Supexec(a)					__lxg(38,a)
#define Puntaes()					__vxv(39)
#define Floprate(a,b)				__sxss(41,a,b)
#define DMAread(a,b,c,d)			__lxlsps(42,a,b,c,d)
#define DMAwrite(a,b,c,d)			__lxlsps(43,a,b,c,d)
#define Bconmap(a)					__lxs(44,a)
#define NVMaccess(a,b,c,d)			__sxsssd(46,a,b,c,d)

#define Metainit(a)	__vxp(48,a)
#define Minit(a)	__vxp(48,a)
#define	Mopen(a,b)	__sxsp(49,a,b)
#define	Mclose(a)	__sxs(50,a)
#define	Mread(a,b,c,d)	__lxspls(51,a,b,c,d)
#define	Mwrite(a,b,c,d)	__lxsqls(51,a,b,c,d)
#define	Mseek(a,b)	__lxsl(53,a,b)
#define	Mstatus(a,b)	__lxsp(54,a,b)

#define Blitmode(a)					__sxs(64,a)
#define EsetShift(a)				__sxs(80,a)
#define EgetShift()					__sxv(81)
#define EsetBank(a)					__sxs(82,a)
#define EsetColor(a,b)				__sxss(83,a,b)
#define EsetPalette(a,b,c)			__vxssr(84,a,b,c)
#define EgetPalette(a,b,c)			__vxssr(85,a,b,c)
#define EsetGray(a)					__sxs(86,a)
#define EsetSmear(a)				__sxs(87,a)

#define VsetScreen(a,b,c,d)	__sxppss(5,a,b,c,d)
#define Vsetmode(a)	__sxs(88,a)
#define Montype()	__sxv(89)
#define VgetMonitor()	__sxv(89)
#define VsetSync(a)	__vxs(90,a)
#define VgetSize(a)	__lxs(91,a)
#define VsetRGB(a,b,c)	__vxsso(93,a,b,c)
#define VgetRGB(a,b,c)	__vxssn(94,a,b,c)

#define	Dsp_DoBlock(a,b,c,d)	__vxqlpl(96,a,b,c,d)
#define	Dsp_BlkHandShake(a,b,c,d)	__vxqlpl(97,a,b,c,d)
#define	Dsp_BlkUnpacked(a,b,c,d)	__vxolnl(98,a,b,c,d)
#define	Dsp_InStream(a,b,c,d)	__vxqlln(99,a,b,c,d)
#define	Dsp_OutStream(a,b,c,d)	__vxqlln(100,a,b,c,d)
#define	Dsp_IOStream(a,b,c,d,e,f)	__vxqpllln(101,a,b,c,d,e,f)
#define	Dsp_RemoveInterrupts(a)	__vxs(102,a)
#define	Dsp_GetWordSize()	__sxv(103)
#define	Dsp_Lock()	__sxv(104)
#define	Dsp_Unlock()	__vxv(105)
#define	Dsp_Available(a,b)	__vxnn(106,a,b)
#define	Dsp_Reserve(a,b)	__sxll(107,a,b)
#define	Dsp_LoadProg(a,b,c)	__sxcsp(108,a,b,c)
#define	Dsp_ExecProg(a,b,c)	__vxqls(109,a,b,c)
#define	Dsp_ExecBoot(a,b,c)	__vxqls(110,a,b,c)
#define	Dsp_LodToBinary(a,b)	__lxcp(111,a,b)
#define	Dsp_TriggerHC(a)	__vxs(112,a)
#define	Dsp_RequestUniqueAbility()	__sxv(113)
#define	Dsp_GetProgAbility()	__sxv(114)
#define	Dsp_FlushSubroutines()	__vxv(115)
#define	Dsp_LoadSubroutine(a,b,c)	__sxqls(116,a,b,c)
#define	Dsp_InqSubrAbility(a)	__sxs(117,a)
#define	Dsp_RunSubroutine(a)	__sxs(118,a)
#define	Dsp_Hf0(a)	__sxs(119,a)
#define	Dsp_Hf1(a)	__sxs(120,a)
#define	Dsp_Hf2()	__sxv(121)
#define	Dsp_Hf3()	__sxv(122)
#define	Dsp_BlkWords(a,b,c,d)	__vxtlrl(123,a,b,c,d)
#define	Dsp_BlkBytes(a,b,c,d)	__vxcldl(124,a,b,c,d)
#define	Dsp_HStat()	__bxv(125)
#define	Dsp_SetVectors(a,b)	__vxfg(126,a,b)
#define	Dsp_MultBlocks(a,b,c,d)	__vxllqp(127,a,b,c,d)

#define	Locksnd()	__lxv(128)
#define	Unlocksnd()	__lxv(129)
#define	Soundcmd(a,b)	__lxss(130,a,b)
#define	Setbuffer(a,b,c)	__lxspp(131,a,b,c)
#define	Setmode(a)	__lxs(132,a)
#define	Settrack(a,b)	__lxss(133,a,b)
#define	Setmontrack(a)	__lxs(134,a)
#define	Setinterrupt(a,b)	__lxss(135,a,b)
#define	Buffoper(a)	__lxs(136,a)
#define	Dsptristate(a,b)	__lxss(137,a,b)
#define	Gpio(a,b)	__lxss(138,a,b)
#define	Devconnect(a,b,c,d,e)	__lxsssss(139,a,b,c,d,e)
#define	Sndstatus(a)	__lxs(140,a)
#define	Buffptr(a)	__lxn(141,a)

#define VsetMask(a,b,c)	__vxlls(150,a,b,c)

int _mediach(int);		/* force GEMDOS media change */
#endif
