/**
 * $Id: dos.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Interface between compiler and OS
 *
 * (c) Copyright 1989, 1993 HiSoft
**/

#ifndef _DOS_H
#define _DOS_H
#include <osbind.h>
#include <conio.h>
#include <basepage.h>

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long int size_t;
#endif

/*
 * OS specific definitions
 */
extern short _tos;		/* tos version number */
extern enum {USA,FRG,FRA,GBR,SPA,ITA,SWE,SWF,SWG,TUR,FIN,NOR,DEN,SAU,HOL,CZE,HUN} _country;

/*
 * The following symbols define the sizes of file names and node names.
 */
#define FNSIZE 13	/* maximum file node size */
#define FMSIZE 128	/* maximum file name size */
#define FESIZE 4	/* maximum file extension size */

/*
 * This structure contains disk size information returned by the getdfs
 * function.
 */
struct DISKINFO
{
	unsigned long free;	/* number of free clusters */
	unsigned long cpd;	/* clusters per drive */
	unsigned long bps;	/* bytes per sector */
	unsigned long spc;	/* sectors per cluster */
};

/*
 * The following structure is used by the dfind and dnext functions to
 * hold file information.
 */
struct FILEINFO
{
	char resv[21];		/* reserved */        
	char attr;			/* actual file attribute */
	long time;			/* file time  and date */
	long size;			/* file size in bytes */
	char name[14];		/* file name */
};

/*
 * Structure used by Getbpb()
 */
typedef struct
{
	short recsiz;
	short clsiz;
	short clsizb;
	short rdlen;
	short fsiz;
	short fatrec;
	short datrec;
	short numcl;
	short bflags;
} BPB;

/*
 * Structure used by Initmous()
 */
typedef struct param
{
	char topmode;
	char buttons;
	char xparam;
	char yparam;
	short xmax;
	short ymax;
	short xinitial;
	short yinitial;
} MOUSE;

/*
 * Structure used by Iorec()
 */
typedef struct iorec
{
	char *ibuf;
	short ibufsiz;
	short ibufhd;
	short ibuftl;
	short ibuflow;
	short ibufhi;
} IOREC;

/*
 * Structure used by Kbdvbase()
 */
typedef struct kbdvecs
{
	void (*__asm midivec)(register __d0 unsigned char);
	void (*vkbderr)(void);
	void (*vmiderr)(void);
	void (*__asm statvec)(register __a0 void *);
	void (*__asm mousevec)(register __a0 void *);
	void (*__asm clockvec)(register __a0 void *);
	void (*__asm joyvec)(register __a0 void *);
	void (*midisys)(void);
	void (*ikbdsys)(void);
	char kstate;
} KBDVBASE;

/*
 * Structure used by Keytbl()
 */
typedef struct keytab
{
	char *unshift;
	char *shift;
	char *capslock;
} KEYTAB;

/*
 * Structure used by Prtblk()
 */
typedef struct prtarg
{
	char *scrptr;
	unsigned short offset;
	unsigned short width;
	unsigned short height;
	unsigned short left;
	unsigned short right;
	unsigned short srcres;
	unsigned short dstres;
	unsigned short *colpal;
	unsigned short type;
	unsigned short port;
	char *masks;
} PBDEF;

/*
 * Structures used by Bconmap()
 */
typedef struct maptab 
{
	long (*Bconstat)(int);
	long (*Bconin)(short);
	long (*Bcostat)(int);
	long (*Bconout)(int,int);
	unsigned long (*Rsconf)(int,int,int,int,int,int);
	IOREC *iorec;
} MAPTAB;

typedef struct bconmap
{
	MAPTAB *maptab;
	short maptabsize;
} BCONMAP;

/*
 * ROM configuration structure (0x4f2)
 */
typedef struct _osheader
{
	unsigned short os_entry;	/* $00 BRA to reset handler */
	unsigned short os_version;	/* $02 TOS version number */
	void (*reseth)(void);		/* $04 -> reset handler */
	struct _osheader *os_beg;	/* $08 -> base of OS */
	void *os_end;				/* $0c -> end BIOS/GEMDOS/VDI ram usage */
	void *os_rsv1;				/* $10 << unused,reserved >> */
	long *os_magic;				/* $14 -> GEM memory usage parm. block */
	long os_date;				/* $18 Date of system build ($DDMMYYYY) */
	unsigned short os_conf;		/* $1c OS configuration bits */
	unsigned short os_dosdate;	/* $1e DOS-format date of system build */
/* The next three fields are only available in TOS versions 1.2 and greater */
	void **p_root;				/* $20 -> base of OS pool */
	unsigned char *pkbshift;	/* $24 -> keyboard shift state variable */
	BASEPAGE **p_run;			/* $28 -> GEMDOS PID of current process */
	void *p_rsv2;				/* $2c << unused, reserved >> */
} OSHEADER;

extern size_t _MSTEP;
extern long _MNEED;

/*
 * Ctrl-C checking master switch variable
 */

extern int __chkabort;		/* defaults to 0, i.e. OFF */

/*
 * I/O services
 */

/* hidden by #defines */
long _dclose(int);
long _dcreat(const char *, int);
long _dopen(const char *, int);
int getcd(int, char *);
int getfa(const char *);
int chgfa(const char *, int);
int getdsk(void);

long __regargs _CXCERR(long);		/* always __regargs for efficiency */
long __regargs _CXCERZ(long);		/* always __regargs for efficiency */

long _dcreatx(const char *, int);
long _dread(int, void *, long);
long _dwrite(int, const void *, long);
long _dseek(int, long, int);
int _disatty(int);
int _ddup(int);
int _ddup2(int, int);

unsigned long chgdsk(int);
void chgdta(struct FILEINFO *);
struct FILEINFO *getdta(void);

int dfind(struct FILEINFO *, const char *, int);
int dnext(struct FILEINFO *);
int getdfs(int,struct DISKINFO *);
long getft(int);
int chgft(int, long);

/*
 * Attributes for Fcreate()/Fsfirst()
 */
#define FA_READONLY	0x01
#define FA_HIDDEN	0x02
#define FA_SYSTEM	0x04
#define FA_VOLUME	0x08
#define FA_SUBDIR	0x10
#define FA_ARCHIVE	0x20

/*
 * Fopen() modes
 */
#define FO_READ	0
#define FO_WRITE	1
#define FO_RW	2

/*
 * Fseek() modes
 */
#define FSEEK_SET	0
#define FSEEK_CUR	1
#define FSEEK_END	2

/*
 *
 * Miscellaneous external definitions
 *
 */
long ftpack(const char *);
void ftunpk(long, char *);
int getpf(char *,const char *);
int getpfe(char *,const char *);
void __stdargs _stub(void);
void __stdargs _xcovf(void);
void getclk(unsigned char *);
int chgclk(unsigned const char *);
int onbreak(int (*)());
void onerror(int);
int poserr(const char *);

extern long volatile _oserr;
extern long volatile _OSERR;
extern unsigned long int _STACK;

#define geta4 __builtin_geta4
extern void geta4(void);

#define getreg __builtin_getreg
extern long getreg(int);

#define putreg __builtin_putreg
extern void putreg(int, long);

#define __emit __builtin_emit
extern void __emit(int);

#define REG_D0 0
#define REG_D1 1
#define REG_D2 2
#define REG_D3 3
#define REG_D4 4
#define REG_D5 5
#define REG_D6 6
#define REG_D7 7
#define REG_A0 8
#define REG_A1 9
#define REG_A2 10
#define REG_A3 11
#define REG_A4 12
#define REG_A5 13
#define REG_A6 14
#define REG_A7 15

#endif
