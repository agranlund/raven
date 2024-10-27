/*      EXT.H

        Extended library definitions

        Copyright (C) Borland International 1990
        All Rights Reserved.
*/


#if !defined( __EXT_H__ )
#define __EXT_H__

#ifdef _SYS_STAT_H
# error do not include both <ext.h> and <sys/stat.h>
#endif

#ifndef __TIME_H__
#include <time.h>
#endif

extern char __text[];
extern char __data[];
extern char __bss[];

#define FA_UPDATE       0x00
#define FA_RDONLY       0x01
#define FA_HIDDEN       0x02
#define FA_SYSTEM       0x04
#define FA_LABEL        0x08
#ifndef FA_DIREC
#define FA_DIREC        0x10
#endif
#ifndef FA_ARCH
#define FA_ARCH         0x20
#endif

#define MAXPATH   119
#define MAXDRIVE  3
#define MAXDIR    102
#define MAXFILE   9
#define MAXEXT    5

#define S_IFCHR   0x2000
#define S_IFBLK   0x6000
#define S_IFDIR   0x4000
#define S_IFREG   0x8000
#define S_IFLNK   0xe000
#define S_IFIFO   0xa000
#define S_IFSOCK  0x1000
#define S_IFMEM   0xc000

#define S_IEXEC   0x0040
#define S_IREAD   0x0100
#define S_IWRITE  0x0080

#define S_IFMT 0170000

#define	S_IRUSR	00400	/* Read by owner.  */
#define	S_IWUSR	00200	/* Write by owner.  */
#define	S_IXUSR	00100	/* Execute by owner.  */
#define	S_IRWXU	(S_IRUSR|S_IWUSR|S_IXUSR)

#define	S_IRGRP	(S_IRUSR >> 3)	/* Read by group.  */
#define	S_IWGRP	(S_IWUSR >> 3)	/* Write by group.  */
#define	S_IXGRP	(S_IXUSR >> 3)	/* Execute by group.  */
#define	S_IRWXG	(S_IRWXU >> 3)

#define	S_IROTH	(S_IRGRP >> 3)	/* Read by others.  */
#define	S_IWOTH	(S_IWGRP >> 3)	/* Write by others.  */
#define	S_IXOTH	(S_IXGRP >> 3)	/* Execute by others.  */
#define	S_IRWXO	(S_IRWXG >> 3)

#define	S_ISUID 04000	/* Set user ID on execution.  */
#define	S_ISGID	02000	/* Set group ID on execution.  */
#define S_ISVTX	01000
#define S_ISTXT	S_ISVTX

#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
#define S_ISMEM(m)	(((m) & S_IFMT) == S_IFMEM)

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

/* moved here from stdlib.h: */
#define random( x ) (rand() % (x))

struct ffblk
{
    char ff_reserved[21];               /* Reserved by TOS */
    char ff_attrib;                     /* Attribute found */
    int  ff_ftime;                      /* File time */
    int  ff_fdate;                      /* File date */
    long ff_fsize;                      /* File size */
    char ff_name[13];                   /* File name found */
};

struct date
{
    int    da_year;                     /* Current year */
    char   da_day;                      /* Day of the month */
    char   da_mon;                      /* Month ( 1 = Jan ) */
};

struct time
{
    unsigned char   ti_min;             /* Minutes */
    unsigned char   ti_hour;            /* Hours */
    unsigned char   ti_hund;            /* Hundredths of seconds */
    unsigned char   ti_sec;             /* Seconds */
};

struct ftime
{
    unsigned ft_hour:   5;
    unsigned ft_min:    6;
    unsigned ft_tsec:   5;
    unsigned ft_year:   7;
    unsigned ft_month:  4;
    unsigned ft_day:    5;
};

#ifndef __NO_REDIRECT
#define stat __purec_stat
#define fstat __purec_fstat
#define lstat __purec_lstat
#endif

struct stat
{
    int    st_dev;
    int    st_ino;
    int    st_mode;
    int    st_nlink;
    int    st_uid;
    int    st_gid;
    int    st_rdev;
    size_t st_size;
	struct {
		union {
			unsigned long  tv_sec; /* actually time&date in DOSTIME format */
			struct {
				unsigned short time;
				unsigned short date;
			} d;
		} u;
	} st_atim;
#define st_atime	st_atim.u.tv_sec
	struct {
		union {
			unsigned long  tv_sec; /* actually time&date in DOSTIME format */
			struct {
				unsigned short time;
				unsigned short date;
			} d;
		} u;
	} st_mtim;
#define st_mtime	st_mtim.u.tv_sec
	struct {
		union {
			unsigned long  tv_sec; /* actually time&date in DOSTIME format */
			struct {
				unsigned short time;
				unsigned short date;
			} d;
		} u;
	} st_ctim;
#define st_ctime	st_ctim.u.tv_sec
};

struct dfree
{
        unsigned df_avail;
        unsigned df_total;
        unsigned df_bsec;
        unsigned df_sclus;
};


int         getcurdir( int drive, char *path );
/* getcwd() also declared in unistd.h */
#ifndef __NO_REDIRECT
#undef getcwd
#define getcwd __purec_getcwd
#endif
char        *getcwd( char *buffer, int bufflen );
int         getdisk( void );
void        getdfree( unsigned char drive, struct dfree *dtable );
int         setdisk( int drive );

int         findfirst( const char *filename, struct ffblk *ffblk, int attrib );
int         findnext( struct ffblk *ffblk );

/* POSIX-getdate() also defined in <time.h> */
#ifndef __NO_REDIRECT
#undef getdate
#define getdate __purec_getdate
#endif

void        getdate( struct date *dateRec );
void        gettime( struct time *timeRec );
void        setdate( struct date *dateRec );
void        settime( struct time *timeRec );
int         getftime( int handle, struct ftime *ftimep );
int         setftime( int handle, struct ftime *ftimep );

struct tm   *ftimtotm(const struct ftime *f);
time_t		ftimtosec(const struct ftime *f );

void        delay( unsigned milliseconds );
/* sleep() also declared in unistd.h */
unsigned int sleep( unsigned seconds );

int         kbhit( void );
int         getch( void );
int         getche( void );
int         putch( int c);
/* chdir() also declared in unistd.h */
int         chdir( const char *filename );

int         fstat( int handle, struct stat *statbuf );
int         stat( const char *path, struct stat *buff );

/* isatty() also declared in unistd.h */
int         isatty( int handle );
long        filelength( int handle );
long        fsize( const char *name );

size_t      coreleft( void );

#endif /* __EXT_H__ */


/***********************************************************************/
