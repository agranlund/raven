/*
 * This file belongs to FreeMiNT.  It's not in the original MiNT 1.12
 * distribution.  See the file Changes.MH for a detailed log of changes.
 */

/*
 * begin:	1998-06
 * last change: 1998-09-07
 * 
 * Author: Frank Naumann - <fnaumann@cs.uni-magdeburg.de>
 * 
 * please send suggestions, patches or bug reports to me or
 * the MiNT mailing list
 * 
 * 
 * Description: Constants for Dcntl() and Fcntl() calls.
 * 
 */

# ifndef _DCNTL_H
# define _DCNTL_H


#ifndef LLONG
# define LLONG
# ifdef __GNUC__
typedef long long			      llong;    /* 64bit int */
# else
typedef struct { long hi; unsigned long lo; } llong;	/* 64bit int */
# endif
#endif


# define F_DUPFD		0		/* handled by kernel */
# define F_GETFD		1		/* handled by kernel */
# define F_SETFD		2		/* handled by kernel */
/* flag: */
# define FD_CLOEXEC		1		/* close on exec flag */

# define F_GETFL		3		/* handled by kernel */
# define F_SETFL		4		/* handled by kernel */
# define F_GETLK		5
# define F_SETLK		6
# define F_SETLKW		7

#ifdef __KERNEL__
struct flock
{
	short	l_type;				/* type of lock */
# define F_RDLCK		0
# define F_WRLCK		1
# define F_UNLCK		3
	short	l_whence;			/* SEEK_SET, SEEK_CUR, SEEK_END */
	long	l_start;			/* start of locked region */
	long	l_len;				/* length of locked region */
	short	l_pid;				/* pid of locking process (F_GETLK only) */
};
#endif

# define F_GETOPENS		8		/* handled by kernel */

/* jr: structure for F_GETOPENS */
struct listopens
{
# define LO_FILEOPEN		1
# define LO_DIROPEN		2
# define LO_CURDIR		4
# define LO_CURROOT		8
	short	lo_pid;				/* input: first pid to check;
						 * output: who's using it? */
	short	lo_reason;			/* input: bitmask of interesting reasons;
						 * output: why EACCDN? */
	short	lo_flags;			/* file's open flags */
};


/*
 * Minix-FS
 */

# define MFS_VERIFY		0x100		/* minixfs/docs/syscall.doc */
# define  MFS_MAGIC		0x18970431	/* Magic number from MFS_VERIFY */
# define MFS_SYNC		0x101		/* minixfs/docs/syscall.doc */
# define MFS_CINVALID		0x102		/* minixfs/docs/syscall.doc */
# define MFS_FINVALID		0x103		/* minixfs/docs/syscall.doc */
# define MFS_INFO		0x104		/* minixfs/docs/syscall.doc */
# define MFS_USAGE		0x105		/* minixfs/minixfs.h */
# define MFS_IMODE		0x106		/* minixfs/docs/syscall.doc */
# define MFS_GTRANS		0x107		/* minixfs/docs/syscall.doc */
# define MFS_STRANS		0x108		/* minixfs/docs/syscall.doc */
# define MFS_PHYS		0x109		/* minixfs/minixfs.h */
# define MFS_IADDR		0x10a		/* minixfs/docs/syscall.doc */
# define MFS_UPDATE		0x10b		/* minixfs/docs/syscall.doc */
# define MFS_MOUNT		0x10c		/* minixfs/docs/syscall.doc */
# define MFS_UMOUNT		0x10d		/* minixfs/docs/syscall.doc */
# define MFS_LOPEN		0x10e		/* minixfs/docs/syscall.doc */
# define MFS_MKNOD		0x10f		/* minixfs/docs/syscall.doc */


/*
 * MagiC specific calls for filesystem installation (use group 'm'!)
 */

/*#define KER_DRVSTAT			0x0104 */	/* Kernel: Drive-Status (ab 9.9.95) */
/*#define KER_XFSNAME			0x0105 */	/* Kernel: XFS-Name (ab 15.6.96) */
/*# define KER_INSTXFS		0x0200*/	/* mgx_dos.txt */
/*# define KER_SETWBACK		0x0300*/	/* mgx_dos.txt */
/*# define DFS_GETINFO		0x1100*/	/* mgx_dos.txt */
/*# define DFS_INSTDFS		0x1200*/	/* mgx_dos.txt */


/*
 * CD-ROMs
 */

# define CDROMREADOFFSET	(('C'<< 8) | 0)		/* cdromio.h */
# define CDROMPAUSE		(('C'<< 8) | 1)		/* cdromio.h */
# define CDROMRESUME		(('C'<< 8) | 2)		/* cdromio.h */
# define CDROMPLAYMSF		(('C'<< 8) | 3)		/* cdromio.h */
# define CDROMPLAYTRKIND	(('C'<< 8) | 4)		/* cdromio.h */
# define CDROMREADTOCHDR	(('C'<< 8) | 5)		/* cdromio.h */
# define CDROMREADTOCENTRY	(('C'<< 8) | 6)		/* cdromio.h */
# define CDROMSTOP		(('C'<< 8) | 7)		/* cdromio.h */
# define CDROMSTART		(('C'<< 8) | 8)		/* cdromio.h */
# define CDROMEJECT		(('C'<< 8) | 9)		/* cdromio.h */
# define CDROMVOLCTRL		(('C'<< 8) | 10)	/* cdromio.h */
# define CDROMSUBCHNL		(('C'<< 8) | 11)	/* cdromio.h */
# define CDROMREADMODE2		(('C'<< 8) | 12)	/* cdromio.h */
# define CDROMREADMODE1		(('C'<< 8) | 13)	/* cdromio.h */
# define CDROMPREVENTREMOVAL	(('C'<< 8) | 14)	/* cdromio.h */
# define CDROMALLOWTREMOVAL	(('C'<< 8) | 15)	/* cdromio.h */
# define CDROMAUDIOCTRL		(('C'<< 8) | 16)	/* cdromio.h */
# define CDROMREADDA		(('C'<< 8) | 17)	/* cdromio.h */

# define CDROMGETMCN		(('C'<< 8) | 19)	/* cdromio.h */
# define CDROMGETTISRC		(('C'<< 8) | 20)	/* cdromio.h */


/*
 * Big-DOS
 */

# define DL_SECSIZ	(('D'<< 8) | 0)		/* max. bpb->recsiz */
# define DL_MINFAT	(('D'<< 8) | 1)		/* min. Anzahl FATs */
# define DL_MAXFAT	(('D'<< 8) | 2)		/* max. Anzahl FATs */
# define DL_MINSPC	(('D'<< 8) | 3)		/* min. bpb->clsiz */
# define DL_MAXSPC	(('D'<< 8) | 4)		/* max. bpb->clsiz */
# define DL_CLUSTS	(('D'<< 8) | 5)		/* max. bpb->numcl FAT16 */
# define DL_MAXSEC	(('D'<< 8) | 6)		/* max. number of sectors */
# define DL_DRIVES	(('D'<< 8) | 7)		/* max. number of drives */
# define DL_CLSIZB	(('D'<< 8) | 8)		/* max. bpb->clsizb */
# define DL_RDLEN	(('D'<< 8) | 9)		/* max. (bpb->rdlen * bpb->recsiz / 32) */
# define DL_FSIZ	(('D'<< 8) | 10)	/* max. bpb->fsiz */
# define DL_FATREC	(('D'<< 8) | 11)	/* max. bpb->fatrec */
# define DL_CLUSTS12	(('D'<< 8) | 12)	/* max. bpb->numcl FAT12 */
# define DL_CLUSTS32	(('D'<< 8) | 13)	/* max. bpb->l_numcl FAT32 */
# define DL_BFLAGS	(('D'<< 8) | 14)	/* supported bits in bpb->bflags */
# define DL_FILESYS	(('D'<< 8) | 15)	/* cookie of the filesystem */
# define DL_VERSION	(('D'<< 8) | 16)	/* version of the filesystem */
# define DL_CHAR	(('D'<< 8) | 17)	/* pointer to a string with legal chars */
# define DL_SETCHAR	(('D'<< 8) | 18)	/* ask or set valid names */


/*
 * file
 */

# define FSTAT		(('F'<< 8) | 0)		/* handled by kernel */
# define FIONREAD	(('F'<< 8) | 1)
# define FIONWRITE	(('F'<< 8) | 2)
# define FUTIME		(('F'<< 8) | 3)

/* structure for D/Fcntl(FUTIME,...) */
struct mutimbuf
{
    unsigned short actime, acdate;		/* GEMDOS format */
    unsigned short modtime, moddate;
};

# define FTRUNCATE	(('F'<< 8) | 4)
# define FIOEXCEPT	(('F'<< 8) | 5)
# define FSTAT64	(('F'<< 8) | 6)
# define FUTIME_UTC (('F'<< 8) | 7)
# define FIONBIO	(('F'<< 8) | 8)		/* just mintlib emulation */
# define FIBMAP		(('F'<< 8) | 10)

# define FMACOPENRES	(('F'<< 8) | 72)	/* macmint/macfile.h */
# define FMACGETTYCR	(('F'<< 8) | 73)	/* macmint/macfile.h */
# define FMACSETTYCR	(('F'<< 8) | 74)	/* macmint/macfile.h */
# define FMAGICMAC	(('F'<< 8) | 75)	/* (reserved for MagiCMac) */


/*
 * shared memory
 */

# define SHMGETBLK	(('M'<< 8) | 0)
# define SHMSETBLK	(('M'<< 8) | 1)


/*
 * processes
 */

# define PPROCADDR	(('P'<< 8) | 1)
# define PBASEADDR	(('P'<< 8) | 2)
# define PCTXTSIZE	(('P'<< 8) | 3)
# define PSETFLAGS	(('P'<< 8) | 4)
# define PGETFLAGS	(('P'<< 8) | 5)
# define PTRACESFLAGS	(('P'<< 8) | 6)
# define PTRACEGFLAGS	(('P'<< 8) | 7)

/* flags: */
# define P_ENABLE	(1 << 0)		/* enable tracing */
# ifdef NOTYETDEFINED
# define P_DOS		(1 << 1)		/* trace DOS calls - unimplemented */
# define P_BIOS		(1 << 2)		/* trace BIOS calls - unimplemented */
# define P_XBIOS	(1 << 3)		/* trace XBIOS calls - unimplemented */
# endif

# define PTRACEGO	(('P'<< 8) | 8)		/* these 4 must be together */
# define PTRACEFLOW	(('P'<< 8) | 9)
# define PTRACESTEP	(('P'<< 8) | 10)
# define PTRACE11	(('P'<< 8) | 11)
# define PLOADINFO	(('P'<< 8) | 12)
# define PFSTAT		(('P'<< 8) | 13)

/* D/Fcntl(PLOADINFO,...) */
struct ploadinfo
{
	/* passed */
	short fnamelen;	/* length of fname including EOS */
	
	/* returned */
	char *cmdlin; /* 128 byte */
	char *fname;
};


/*
 * Socket ioctls: these require MiNT-Net 3.0 (or later)
 */

# define SIOCGLOWAT	(('S' << 8) | 1)
# define SIOCSLOWAT	(('S' << 8) | 2)
# define SIOCGHIWAT	(('S' << 8) | 3)
# define SIOCSHIWAT	(('S' << 8) | 4)
# define SIOCSPGRP	(('S' << 8) | 5)
# define SIOCGPGRP	(('S' << 8) | 6)
# define SIOCATMARK	(('S' << 8) | 7)

# define SIOCGIFCONF	(('S' << 8) | 12)	/* get iface list */
# define SIOCGIFFLAGS	(('S' << 8) | 13)	/* get flags */
# define SIOCSIFFLAGS	(('S' << 8) | 14)	/* set flags */
# define SIOCGIFADDR	(('S' << 8) | 15)	/* get iface address */
# define SIOCSIFADDR	(('S' << 8) | 16)	/* set iface address */
# define SIOCGIFDSTADDR	(('S' << 8) | 17)	/* get iface remote address */
# define SIOCSIFDSTADDR	(('S' << 8) | 18)	/* set iface remotw address */
# define SIOCGIFBRDADDR	(('S' << 8) | 19)	/* get iface ibroadcast address */
# define SIOCSIFBRDADDR	(('S' << 8) | 20)	/* set iface broadcast address */
# define SIOCGIFNETMASK	(('S' << 8) | 21)	/* get iface network mask */
# define SIOCSIFNETMASK	(('S' << 8) | 22)	/* set iface network mask */
# define SIOCGIFMETRIC	(('S' << 8) | 23)	/* get metric */
# define SIOCSIFMETRIC	(('S' << 8) | 24)	/* set metric */

# define SIOCGIFMTU	(('S' << 8) | 27)	/* get MTU size */
# define SIOCSIFMTU	(('S' << 8) | 28)	/* set MTU size */

# define SIOCADDRT	(('S' << 8) | 30)	/* add routing table entry */
# define SIOCDELRT	(('S' << 8) | 31)	/* delete routing table entry */

# define SIOCDARP	(('S' << 8) | 40)	/* delete ARP table entry */
# define SIOCGARP	(('S' << 8) | 41)	/* get ARP table entry */
# define SIOCSARP	(('S' << 8) | 42)	/* set ARP table entry */


/*
 * terminals
 */

# define TIOCGETP	(('T'<< 8) | 0)
# define TIOCSETN	(('T'<< 8) | 1)
# define TIOCGETC	(('T'<< 8) | 2)
# define TIOCSETC	(('T'<< 8) | 3)
# define TIOCGLTC	(('T'<< 8) | 4)
# define TIOCSLTC	(('T'<< 8) | 5)
# define TIOCGPGRP	(('T'<< 8) | 6)
# define TIOCSPGRP	(('T'<< 8) | 7)
# define TIOCFLUSH	(('T'<< 8) | 8)
# define TIOCSTOP	(('T'<< 8) | 9)
# define TIOCSTART	(('T'<< 8) | 10)
# define TIOCGWINSZ	(('T'<< 8) | 11)
# define TIOCSWINSZ	(('T'<< 8) | 12)
# define TIOCGXKEY	(('T'<< 8) | 13)
# define TIOCSXKEY	(('T'<< 8) | 14)
# define TIOCIBAUD	(('T'<< 8) | 18)
# define TIOCOBAUD	(('T'<< 8) | 19)
# define TIOCCBRK	(('T'<< 8) | 20)
# define TIOCSBRK	(('T'<< 8) | 21)
# define TIOCGFLAGS	(('T'<< 8) | 22)
# define TIOCSFLAGS	(('T'<< 8) | 23)
# define TIOCOUTQ	(('T'<< 8) | 24)
# define TIOCSETP	(('T'<< 8) | 25)
# define TIOCHPCL	(('T'<< 8) | 26)
# define TIOCCAR	(('T'<< 8) | 27)
# define TIOCNCAR	(('T'<< 8) | 28)
# define TIOCWONLINE	(('T'<< 8) | 29)
# define TIOCSFLAGSB	(('T'<< 8) | 30)
# define TIOCGSTATE	(('T'<< 8) | 31)
# define TIOCSSTATEB	(('T'<< 8) | 32)
# define TIOCGVMIN	(('T'<< 8) | 33)
# define TIOCSVMIN	(('T'<< 8) | 34)
# define TIOCGHUPCL	(('T'<< 8) | 98)	/* mdm0 ioctls */
# define TIOCSHUPCL	(('T'<< 8) | 99)
# define TIOCGSOFTCAR	(('T'<< 8) | 100)
# define TIOCSSOFTCAR	(('T'<< 8) | 101)

# define TIOCBUFFER	(('T'<< 8) | 128)	/* HSMODA/hsm_doku/sersoftst.txt */
# define TIOCCTLMAP	(('T'<< 8) | 129)
# define TIOCCTLGET	(('T'<< 8) | 130)
# define TIOCCTLSET	(('T'<< 8) | 131)
# define TIOCCTLSFAST	(('T'<< 8) | 132)	/* HSMODA/hsm_doku/sersoftst.txt */
# define TIOCCTLSSLOW	(('T'<< 8) | 133)	/* HSMODA/hsm_doku/sersoftst.txt */
# define TIONOTSEND	(('T'<< 8) | 134)
# define TIOCERROR	(('T'<< 8) | 135)

# define TIOCLBIS	(('T'<< 8) | 246)	/* faked by the MiNT library */
# define TIOCLBIC	(('T'<< 8) | 247)
# define TIOCMGET	(('T'<< 8) | 248)

/* bits in longword fetched by TIOCMGET: */
# define TIOCM_LE	0001 /* not supported */
# define TIOCM_DTR	0002
# define TIOCM_RTS	0004
# define TIOCM_ST	0010 /* not supported */
# define TIOCM_SR	0020 /* not supported */
# define TIOCM_CTS	0040
# define TIOCM_CAR	0100
# define TIOCM_CD	TIOCM_CAR
# define TIOCM_RNG	0200
# define TIOCM_RI	TIOCM_RNG
# define TIOCM_DSR	0400 /* not supported */

# define TIOCCDTR	(('T'<< 8) | 249)
# define TIOCSDTR	(('T'<< 8) | 250)
# define TIOCNOTTY	(('T'<< 8) | 251)
# define TIOCGETD	(('T'<< 8) | 252)  /* not yet implemented in MiNT */
# define TIOCSETD	(('T'<< 8) | 253)
# define TIOCLGET	(('T'<< 8) | 254)
# define TIOCLSET	(('T'<< 8) | 255)

# define NTTYDISC	1


/*
 * (V)FAT filesystem extensions (MiNT/MagiC)
 */
    
# define VFAT_CNFDFLN	(('V'<< 8) | 0)		/* MiNT/MagiC */
# define VFAT_CNFLN	(('V'<< 8) | 1)		/* MiNT/MagiC */
# define V_CNTR_SLNK	(('V'<< 8) | 2)		/* MiNT, works on FAT and VFAT */
# define V_CNTR_RES3	(('V'<< 8) | 3)		/* reserved, not used at the moment */
# define V_CNTR_MODE	(('V'<< 8) | 4)		/* MiNT, works on FAT */

/* name mode flags: */
# define VFAT_GEMDOS	0
# define VFAT_ISO		1
# define VFAT_MSDOS		2

# define V_CNTR_FAT32	(('V'<< 8) | 10)	/* MiNT, FAT32 control */

struct control_FAT32
{
	unsigned short	mode;	/* 0 = fill out; 1 = set values */
	unsigned short	mirr;	/* FAT mirroring: 0 = enabled, otherwise active FAT */
	unsigned short	fats;	/* number of FATs */
	unsigned short	info;	/* status of additional info sector */
# define FAT32_info_exist	0x1
# define FAT32_info_active	0x2
# define FAT32_info_reset	0x4	/* in set mode: clear info sector */
};

# define V_CNTR_WP	(('V'<< 8) | 100)	/* MiNT, write protect control */
# define V_CNTR_WB	(('V'<< 8) | 101)	/* MiNT, write back control */


/*
 * Ext2 filesystem extensions
 */

#ifndef _IOR
# define _IOR(type,nr,size) (((type) << 8) | (nr))
#endif
#ifndef _IOW
# define _IOW(type,nr,size) (((type) << 8) | (nr))
#endif

# define EXT2_IOC_GETFLAGS			_IOR('f', 1, long)
# define EXT2_IOC_SETFLAGS			_IOW('f', 2, long)
# define EXT2_IOC_GETVERSION_NEW	_IOR('f', 3, long)
# define EXT2_IOC_SETVERSION_NEW	_IOW('f', 4, long)
# define EXT2_IOC_GROUP_EXTEND		_IOW('f', 7, unsigned long)
# define EXT2_IOC_GROUP_ADD			_IOW('f', 8, struct ext2_new_group_input)
/* watch out: different from linux code, size size of structure not encoded */
# define EXT4_IOC_GROUP_ADD			_IOW('f', 6, struct ext4_new_group_input)

# define EXT2_IOC_GETVERSION	_IOR('v', 1, long)
# define EXT2_IOC_SETVERSION	_IOW('v', 2, long)


/*
 * cursor control
 */

# define TCURSOFF	(('c'<< 8) | 0)
# define TCURSON	(('c'<< 8) | 1)
# define TCURSBLINK	(('c'<< 8) | 2)
# define TCURSSTEADY	(('c'<< 8) | 3)
# define TCURSSRATE	(('c'<< 8) | 4)
# define TCURSGRATE	(('c'<< 8) | 5)
# define TCURSSDELAY	(('c'<< 8) | 6)		/* undocumented! */
# define TCURSGDELAY	(('c'<< 8) | 7)		/* undocumented! */


/*
 * MagiC opcodes (all group 'm' opcodes are reserved for MagiC)
 */

# define MX_KER_GETINFO		(('m'<< 8) | 0)		/* mgx_dos.txt */
# define MX_KER_DOSLIMITS	(('m'<< 8) | 1)		/* mgx_dos.txt */
# define MX_KER_INSTXFS		(('m'<< 8) | 2)		/* mgx_dos.txt */
# define MX_KER_DRVSTAT		(('m'<< 8) | 4)		/* mgx_dos.txt */
# define MX_KER_XFSNAME		(('m'<< 8) | 5)		/* mgx_dos.txt */
# define MX_DEV_INSTALL 	(('m'<< 8) | 0x20)	/* mgx_dos.txt */
# define MX_DEV_INSTALL2 	(('m'<< 8) | 0x21)	/* mgx_dos.txt */
# define MX_DFS_GETINFO		(('m'<< 8) | 0x40)	/* mgx_dos.txt */
# define MX_DFS_INSTDFS		(('m'<< 8) | 0x41)	/* mgx_dos.txt */


/*
 * Printers (whole range reserved)
 */

/*# define PNVDI		(('p'<< 8) | *)*/	/* NVDI programmer's manual */


/*
 * MagiC specific calls for device driver installation (use group 'm'!)
 */

/*# define PROC_CREATE		0xcc00*/		/* mgx_dos.txt */
/*# define DEV_M_INSTALL	0xcd00*/		/* mgx_dos.txt */


/*
 * device driver installation
 */

# define DEV_NEWTTY	0xde00
# define DEV_NEWBIOS	0xde01
# define DEV_INSTALL	0xde02
# define DEV_INSTALL2	0xde03


/*
 * filesystem installation
 */

# define FS_INSTALL	0xf001		/* let the kernel know about the file system */
# define FS_MOUNT	0xf002		/* make a new directory for a file system */
# define FS_UNMOUNT	0xf003		/* remove a directory for a file system */
# define FS_UNINSTALL	0xf004		/* remove a file system from the list */

/*
 * filesystem information
 */

# define FS_INFO	0xf100		/* xfs fill out the following struct */

struct fs_info
{
	char	name [32];	/* name of the xfs (same as MX_KER_XFSNAME but more space) */
	long	version;	/* upper word: major version; lower word: minor version */
	long	type;		/* upper word: major type; lower word: minor type */
	char	type_asc[32];	/* human readable version of type */
};

/* values of fs_info.type */
# define _MAJOR_OLDTOS  ( 0L << 16)
# define _MAJOR_FAT     ( 1L << 16)
# define _MAJOR_VFAT    ( 2L << 16)
# define _MAJOR_MINIX   ( 3L << 16)
# define _MAJOR_RAMFS   ( 4L << 16)
# define _MAJOR_EXT2    ( 5L << 16)
# define _MAJOR_HSIERRA ( 6L << 16)
# define _MAJOR_ISO9660 ( 7L << 16)
# define _MAJOR_JOLIET  ( 8L << 16)
# define _MAJOR_HFS     ( 9L << 16)
# define _MAJOR_CDRAW   (10L << 16)
# define _MAJOR_STONX   (11L << 16)
# define _MAJOR_NFS     (12L << 16)
# define _MAJOR_HOSTFS  (15L << 16)

# define _MINOR_FAT12	0
# define _MINOR_FAT16	1
# define _MINOR_FAT32	2

# define FS_OLDTOS	(_MAJOR_OLDTOS)			/* default/unknown */
# define FS_FAT12	(_MAJOR_FAT  | _MINOR_FAT12)	/* MiNT 1.15 */
# define FS_FAT16	(_MAJOR_FAT  | _MINOR_FAT16)	/* MiNT 1.15 */
# define FS_FAT32	(_MAJOR_FAT  | _MINOR_FAT32)	/* MiNT 1.15 */
# define FS_VFAT12	(_MAJOR_VFAT | _MINOR_FAT12)	/* MiNT 1.15 */
# define FS_VFAT16	(_MAJOR_VFAT | _MINOR_FAT16)	/* MiNT 1.15 */
# define FS_VFAT32	(_MAJOR_VFAT | _MINOR_FAT32)	/* MiNT 1.15 */
# define FS_MINIX	(_MAJOR_MINIX)			/* MinixFS */
# define FS_RAMFS	(_MAJOR_RAMFS)			/* fnramFS */
# define FS_EXT2	(_MAJOR_EXT2)			/* Ext2-FS */
# define FS_HSIERRA	(_MAJOR_HSIERRA)		/* Spin 0.35 */
# define FS_ISO9660	(_MAJOR_ISO9660)		/* Spin 0.35 */
# define FS_JOLIET	(_MAJOR_JOLIET)			/* Spin 0.35 */
# define FS_HFS		(_MAJOR_HFS)			/* Spin 0.35 */
# define FS_CDRAW	(_MAJOR_CDRAW)			/* Spin 0.35 */
# define FS_STONX	(_MAJOR_STONX)			/* STonXfs4MiNT */
# define FS_NFS2	(_MAJOR_NFS)			/* nfs 0.55 */
# define FS_HOSTFS  (_MAJOR_HOSTFS)         /* aranym/stonx hostfs */

# define FS_USAGE	0xf101		/* xfs fill out the following struct */

struct fs_usage
{
	unsigned long blocksize;	/* 32bit: size in bytes of a block */
	llong	blocks;		/* 64bit: number of blocks */
	llong	free_blocks;	/* 64bit: number of free blocks */
	llong	inodes;		/* 64bit: number of inodes or FS_UNLIMITED */
	llong	free_inodes;	/* 64bit: number of free inodes or FS_UNLIMITED */
# define FS_UNLIMITED	-1
};

# endif /* _DCNTL_H */
