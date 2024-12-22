/**
 * $Id: ext.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Turbo C compatability definitions
 *
 * (c) Copyright 1990, 1993 HiSoft
**/

#ifndef _EXT_H
#define _EXT_H
#include <time.h>
#include <dos.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _SSIZE_T
#define _SSIZE_T
typedef signed long ssize_t;
#endif

#define	FA_UPDATE	0x00
#define	FA_RDONLY	FA_READONLY
#define	FA_LABEL	FA_VOLUME
#define	FA_DIREC	FA_SUBDIR
#define	FA_ARCH	FA_ARCHIVE

#define MAXPATH	FMSIZE
#define MAXDRIVE	3
#define MAXDIR	FMSIZE
#define MAXFILE	(FNSIZE-FESIZE-1)
#define MAXEXT	(FESIZE+2)

struct ffblk
{
	char ff_reserved[21];
	char ff_attrib;
	short ff_ftime;
	short ff_fdate;
	long ff_fsize;
	char ff_name[14];
};

int findfirst(const char *,struct ffblk *,int);
int findnext(struct ffblk *);
#define findfirst(a,b,c)	dfind((struct FILEINFO *)(b),(a),(c))
#define findnext(a)	dnext((struct FILEINFO *)(a))

struct date
{
	short da_year;
	char da_day;
	char da_mon;
};

void getdate(struct date *);
void setdate(struct date *);

struct time
{
	unsigned char ti_min;
	unsigned char ti_hour;
	unsigned char ti_hund;
	unsigned char ti_sec;
};

void gettime(struct time *);
void settime(struct time *);

struct ftime
{
	unsigned ft_hour:5;
	unsigned ft_min:6;
	unsigned ft_tsec:5;
	unsigned ft_year:7;
	unsigned ft_month:4;
	unsigned ft_day:5;
};

long __regargs _CXCERR(long);		/* always __regargs for efficiency */

int getftime(int,struct ftime *);
int setftime(int,struct ftime *);
#define getftime(a,b)	((int)_CXCERR(Fdatime((short *)(b),(a),0)))
#define setftime(a,b)	((int)_CXCERR(Fdatime((short *)(b),(a),1)))

struct tm *ftimtotm(struct ftime *);

struct dfree
{
	unsigned long df_avail;
	unsigned long df_total;
	unsigned long df_bsec;
	unsigned long df_sclus;
};

void getdfree(int,struct dfree *);
#define	getdfree(a,b)	getdfs((a),(struct DISKINFO *)(b))
int getdisk(void);
#define	getdisk	getdsk
int setdisk(int);
#define	setdisk	chgdsk
int getcurdir(int,char *);
#define	getcurdir	getcd

size_t coreleft(void);
#define coreleft()	((size_t)Malloc(-1))

void delay(size_t);	/* milliseconds */
void sleep(size_t);	/* seconds */

int chdir(const char *);
int isatty(int);
ssize_t filelength(int);
char *getcwd(char *,int);
#endif
