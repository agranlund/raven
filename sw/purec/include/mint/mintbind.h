#ifndef _MINT_MINTBIND_H
#define _MINT_MINTBIND_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

#ifndef _MINT_OSBIND_H
# include <mint/osbind.h>
#endif

__BEGIN_DECLS

#if !defined(__XATTR) && !defined(__KERNEL__) && !defined(__KERNEL_MODULE__)
#define __XATTR
typedef struct xattr
{
	unsigned short st_mode;
	long           st_ino;	/* must be 32 bits */
	unsigned short st_dev;	/* must be 16 bits */
	unsigned short st_rdev;	/* not supported by the kernel */
	unsigned short st_nlink;
	unsigned short st_uid;	/* must be 16 bits */
	unsigned short st_gid;	/* must be 16 bits */
	long           st_size;
	long           st_blksize;
	long           st_blocks;
	struct {
		union {
			unsigned long  tv_sec; /* actually time&date in DOSTIME format */
			struct {
				unsigned short time;
				unsigned short date;
			} d;
		} u;
	} st_mtim;
#define st_mtime   st_mtim.u.tv_sec
	struct {
		union {
			unsigned long  tv_sec; /* actually time&date in DOSTIME format */
			struct {
				unsigned short time;
				unsigned short date;
			} d;
		} u;
	} st_atim;
#define st_atime   st_atim.u.tv_sec
	struct {
		union {
			unsigned long  tv_sec; /* actually time&date in DOSTIME format */
			struct {
				unsigned short time;
				unsigned short date;
			} d;
		} u;
	} st_ctim;
#define st_ctime   st_ctim.u.tv_sec
	short          st_attr;
	short res1;		/* reserved for future kernel use */
	long res2[2];
} XATTR;
#define FSTAT		(('F'<< 8) | 0)
#endif

#ifndef __MSG
#define __MSG
typedef struct
{
        long     msg1;
        long     msg2;
        short    pid;
} MSG;
#endif

/* The requests for Dpathconf() */
#define DP_IOPEN	0	/* internal limit on # of open files */
#define DP_MAXLINKS	1	/* max number of hard links to a file */
#define DP_PATHMAX	2	/* max path name length */
#define DP_NAMEMAX	3	/* max length of an individual file name */
#define DP_ATOMIC	4	/* # of bytes that can be written atomically */
#define DP_TRUNC	5	/* file name truncation behavior */
#	define	DP_NOTRUNC	0	/* long filenames give an error */
#	define	DP_AUTOTRUNC	1	/* long filenames truncated */
#	define	DP_DOSTRUNC	2	/* DOS truncation rules in effect */
#define DP_CASE		6
#	define	DP_CASESENS	0	/* case sensitive */
#	define	DP_CASECONV	1	/* case always converted */
#	define	DP_CASEINSENS	2	/* case insensitive, preserved */
#define DP_MODEATTR		7
#	define	DP_ATTRBITS	0x000000ffL	/* mask for valid TOS attribs */
#	define	DP_MODEBITS	0x000fff00L	/* mask for valid Unix file modes */
#	define	DP_FILETYPS	0xfff00000L	/* mask for valid file types */
#	define	DP_FT_DIR	0x00100000L	/* directories (always if . is there) */
#	define	DP_FT_CHR	0x00200000L	/* character special files */
#	define	DP_FT_BLK	0x00400000L	/* block special files, currently unused */
#	define	DP_FT_REG	0x00800000L	/* regular files */
#	define	DP_FT_LNK	0x01000000L	/* symbolic links */
#	define	DP_FT_SOCK	0x02000000L	/* sockets, currently unused */
#	define	DP_FT_FIFO	0x04000000L	/* pipes */
#	define	DP_FT_MEM	0x08000000L	/* shared memory or proc files */
#ifndef DP_XATTRFIELDS
#define DP_XATTRFIELDS 8		/* information about supported extended attributes */
#  define   DP_INDEX    (0x0001)    /* index field unique for every file on the fs */
#  define   DP_DEV      (0x0002)    /* device field valid */
#  define   DP_RDEV     (0x0004)    /* rdev field valid (and not identical to dev) */
#  define   DP_NLINK    (0x0008)    /* number of links valid */
#  define   DP_UID      (0x0010)    /* user id valid */
#  define   DP_GID      (0x0020)    /* group id valid */
#  define   DP_BLKSIZE  (0x0040)    /* block size valid */
#  define   DP_SIZE     (0x0080)    /* size field valid (and meaningful!) */
#  define   DP_NBLOCKS  (0x0100)    /* number of blocks valid */
#  define   DP_ATIME    (0x0200)    /* file system has last access time */
#  define   DP_CTIME    (0x0400)    /* file system has last status change time */
#  define   DP_MTIME    (0x0800)    /* file system has last modification time */
#endif
#define DP_VOLNAMEMAX 9         /* maximum length of a volume name (0 if volume names not supported) */
#define DP_MAXREQ	(-1) /* Dpathconf(-1) */			/* highest legal request */

#if defined(__PUREC__) || defined(__AHCC__) || defined(__TURBOC__) || (defined(__GNUC__) && !defined(__mc68000__))

#include <mint/slb.h>

/* we supply a library of bindings for TurboC / PureC */

#ifndef __mint_sighandler_t_defined
#define __mint_sighandler_t_defined 1
#ifdef __NO_CDECL
typedef void *__mint_sighandler_t;
#else
typedef void _CDECL (*__mint_sighandler_t) (long signum);
#endif
#endif

#if defined(__AHCC__)
	#define __GEMDOS(b) cdecl __syscall__( 1,b)
	#define __BIOS(b)   cdecl __syscall__(13,b)
	#define __XBIOS(b)  cdecl __syscall__(14,b)
#else
	#define __GEMDOS(b)
	#define __BIOS(b)
	#define __XBIOS(b)
#endif

short __GEMDOS(0xff)	Syield(void);
long __GEMDOS(0x100)	Fpipe(short *ptr);
short __GEMDOS(0x101)	Ffchown(short f, short uid, short gid);
short __GEMDOS(0x102)	Ffchmod(short f, short mode);
short __GEMDOS(0x103)	Fsync(short f);
#undef Fcntl
long __GEMDOS(0x104)	Fcntl(short f, long arg, short cmd);
#define Fcntl(f, arg, cmd) Fcntl(f, (long)(arg), cmd)
long __GEMDOS(0x105)	Finstat(short f);
long __GEMDOS(0x106)	Foutstat(short f);
long __GEMDOS(0x107)	Fgetchar(short f, short mode);
long __GEMDOS(0x108)	Fputchar(short f, long c, short mode);
long __GEMDOS(0x109)	Pwait(void);
short __GEMDOS(0x10a)	Pnice(short delta);
short __GEMDOS(0x10b)	Pgetpid(void);
short __GEMDOS(0x10c)	Pgetppid(void);
short __GEMDOS(0x10d)	Pgetpgrp(void);
short __GEMDOS(0x10e)	Psetpgrp(short pid, short newgrp);
short __GEMDOS(0x10f)	Pgetuid(void);
short __GEMDOS(0x110)	Psetuid(short id);
short __GEMDOS(0x111)	Pkill(short pid, short sig);
__mint_sighandler_t __GEMDOS(0x112) Psignal(short sig, __mint_sighandler_t handler);
long __GEMDOS(0x113)	Pvfork(void);
short __GEMDOS(0x114)	Pgetgid(void);
short __GEMDOS(0x115)	Psetgid(short id);
long __GEMDOS(0x116)	Psigblock(unsigned long mask);
long __GEMDOS(0x117)	Psigsetmask(unsigned long mask);
long __GEMDOS(0x118)	Pusrval(long arg);
short __GEMDOS(0x119)	Pdomain(short newdom);
long __GEMDOS(0x11a)	Psigreturn(void);
long __GEMDOS(0x11b)	Pfork(void);
long __GEMDOS(0x11c)	Pwait3(short flag, long *rusage);
short __GEMDOS(0x11d)	Fselect(unsigned short timeout, long *rfds, long *wfds, long *xfds);
long __GEMDOS(0x11e)	Prusage(long r[8]);
long __GEMDOS(0x11f)	Psetlimit(short lim, long value);
long __GEMDOS(0x120)	Talarm(long secs);
long __GEMDOS(0x121)	Pause(void);
long __GEMDOS(0x122)	Sysconf(short n);
long __GEMDOS(0x123)	Psigpending(void);
long __GEMDOS(0x124)	Dpathconf(const char *name, short n);
long __GEMDOS(0x125)	Pmsg(short mode, long mbox, void *msg);
long __GEMDOS(0x126)	Fmidipipe(short pid, short in, short out);
short __GEMDOS(0x127)	Prenice(short pid, short delta);
long __GEMDOS(0x128)	Dopendir(const char *name, short flag);
long __GEMDOS(0x129)	Dreaddir(short buflen, long dir, char *buf);
#define Dreaddir(buflen, dir, buf) Dreaddir(buflen, (long)(dir), buf)
long __GEMDOS(0x12a)	Drewinddir(long dir);
#define Drewinddir(dir) Drewinddir((long)(dir))
long __GEMDOS(0x12b)	Dclosedir(long dir);
#define Dclosedir(dir) Dclosedir((long)(dir))
long __GEMDOS(0x12c)	Fxattr(short flag, const char *name, XATTR *buf);
long __GEMDOS(0x12d)	Flink(const char *oldname, const char *newname);
long __GEMDOS(0x12e)	Fsymlink(const char *oldname, const char *newname);
long __GEMDOS(0x12f)	Freadlink(short siz, char *buf, const char *name);
long __GEMDOS(0x130)	Dcntl(short cmd, const char *name, long arg);
long __GEMDOS(0x131)	Fchown(const char *name, short uid, short gid);
long __GEMDOS(0x132)	Fchmod(const char *name, short mode);
unsigned short __GEMDOS(0x133)	Pumask(unsigned short mask);
long __GEMDOS(0x134)	Psemaphore(short mode, long id, long timeout);
short __GEMDOS(0x135)	Dlock(short mode, short drive);
long __GEMDOS(0x136)	Psigpause(unsigned long mask);
long __GEMDOS(0x137)	Psigaction(short sig, long act, long oact);
#define Psigaction(sig, act, oact) Psigaction(sig, (long)(act), (long)(oact))
short __GEMDOS(0x138)	Pgeteuid(void);
short __GEMDOS(0x139)	Pgetegid(void);
long __GEMDOS(0x13a)	Pwaitpid(short pid, short flag, long *rusage);
long __GEMDOS(0x13b)	Dgetcwd(char *path, short drv, short size);
long __GEMDOS(0x13c)	Salert(const char *msg);
unsigned long __GEMDOS(0x13d)	Tmalarm(unsigned long millisecs);
long __GEMDOS(0x13e)	Psigintr(short vec, short sig);
long __GEMDOS(0x13f)	Suptime(unsigned long *cur_uptime, unsigned long loadave[3]);
short __GEMDOS(0x140)	Ptrace(short request, short pid, void *addr, long data);
long __GEMDOS(0x141)	Mvalidate(short pid, void *addr, long size, long *flags);
long __GEMDOS(0x142)	Dxreaddir(short len, long handle, char *buf, XATTR *attr, long *xret);
long __GEMDOS(0x143)	Pseteuid(short id);
long __GEMDOS(0x144)	Psetegid(short id);
long __GEMDOS(0x145)	Pgetauid(void);
long __GEMDOS(0x146)	Psetauid(short id);
long __GEMDOS(0x147)	Pgetgroups(short gidsetlen, unsigned short gidset[]);
long __GEMDOS(0x148)	Psetgroups(short ngroups, unsigned short gidset[]);
long __GEMDOS(0x149)	Tsetitimer(short which, long *interval, long *value, long *ointerval, long *ovalue);
long __GEMDOS(0x14a)	Scookie(short action, void *yummy);
long __GEMDOS(0x14a)	Dchroot(const char *path);
/* Fstat64 uses mint internal struct stat, which is now the same as mintlibs struct stat */
#ifdef _SYS_STAT_H
long __GEMDOS(0x14b)	Fstat64(int flag, const char *name, struct stat *st);
#else
long __GEMDOS(0x14b)	Fstat64(int flag, const char *name, void /* struct stat */ *st);
#endif
long __GEMDOS(0x14c)	Fseek64(long high, unsigned long low, short handle, short how, long *newpos);
long __GEMDOS(0x14d)	Dsetkey(long major, unsigned long minor, const char *key, short cipher);
long __GEMDOS(0x14e)	Psetreuid(short rid, short eid);
long __GEMDOS(0x14f)	Psetregid(short rid, short eid);
long __GEMDOS(0x150)	Sync(void);
long __GEMDOS(0x151)	Shutdown(long restart);
long __GEMDOS(0x152)	Dreadlabel(const char *path, char *label, short maxlen);
long __GEMDOS(0x153)	Dwritelabel(const char *path, const char *label);
long __GEMDOS(0x154)	Ssystem(short mode, long arg1, long arg2);
#if defined(_SYS_TIME_H) && defined(__USE_BSD) /* otherwise struct timezone not declared */
long __GEMDOS(0x155)	Tgettimeofday(struct timeval *tv, struct __mint_timezone *tz);
long __GEMDOS(0x156)	Tsettimeofday(const struct timeval *tv, const struct __mint_timezone *tz);
long __GEMDOS(0x157)	Tadjtime(const struct timeval *delta, struct timeval *olddelta);
#endif
long __GEMDOS(0x158)	Pgetpriority(short which, short who);
long __GEMDOS(0x159)	Psetpriority(short which, short who, short pri);
long __GEMDOS(0x15a)	Fpoll(void *fds, long nfds, unsigned long timeout);
long __GEMDOS(0x15b)	Fwritev(short fd, void /* struct iovec */ *iov, long niov);
long __GEMDOS(0x15c)	Freadv(short fd, void /* struct iovec */ *iov, long niov);
long __GEMDOS(0x15d)	Ffstat64(short fd, void /* struct stat */ *st);
long __GEMDOS(0x15e)	Psysctl(long *name, long namelen, void *old, unsigned long *oldlen, const void *_new, unsigned long newlen);
long __GEMDOS(0x15f)	Semulation(short which, short op, long a1, long a2, long a3, long a4, long a5, long a6, long a7);
#define Pemulation Semulation
#ifdef _SYS_SOCKET_H
long __GEMDOS(0x160)	Fsocket(long domain, long type, long protocol);
long __GEMDOS(0x161)	Fsocketpair(long domain, long type, long protocol, short fds[2]);
long __GEMDOS(0x162)	Faccept(short fd, struct sockaddr *name, unsigned long *anamelen);
long __GEMDOS(0x163)	Fconnect(short fd, const struct sockaddr *name, unsigned long anamelen);
long __GEMDOS(0x164)	Fbind(short fd, const struct sockaddr *name, unsigned long namelen);
long __GEMDOS(0x165)	Flisten(short fd, long backlog);
long __GEMDOS(0x166)	Frecvmsg(short fd, struct msghdr *msg, long flags);
long __GEMDOS(0x167)	Fsendmsg(short fd, const struct msghdr *msg, long flags);
long __GEMDOS(0x168)	Frecvfrom(short fd, void *buf, unsigned long len, long flags, struct sockaddr *from, unsigned long *fromlenaddr);
long __GEMDOS(0x169)	Fsendto(short fd, const void *buf, unsigned long len, long flags, const struct sockaddr *to, unsigned long tolen);
long __GEMDOS(0x16a)	Fsetsockopt(short fd, long level, long name, const void *val, unsigned long valsize);
long __GEMDOS(0x16b)	Fgetsockopt(short fd, long level, long name, void *val, unsigned long *avalsize);
long __GEMDOS(0x16c)	Fgetpeername(short fd, struct sockaddr *asa, unsigned long *alen);
long __GEMDOS(0x16d)	Fgetsockname(short fd, struct sockaddr *asa, unsigned long *alen);
long __GEMDOS(0x16e)	Fshutdown(short fd, long how);
#endif
#if defined(_SYS_SHM_H)
long __GEMDOS(0x170)	Pshmget(long key, long size, long shmflg);
long __GEMDOS(0x171)	Pshmctl(long shmid, long cmd, struct shmid_ds *buf);
long __GEMDOS(0x172)	Pshmat(long shmid, const void *shmaddr, long shmflg);
long __GEMDOS(0x173)	Pshmdt(const void *shmaddr);
long __GEMDOS(0x174)	Psemget(long key, long nsems, long semflg);
long __GEMDOS(0x175)	Psemctl(long semid, long semnum, long cmd, void /* union semun */ *arg);
long __GEMDOS(0x176)	Psemop(long semid, struct sembuf *sops, long nsops);
long __GEMDOS(0x177)	Psemconfig(long flag);
long __GEMDOS(0x178)	Pmsgget(long key, long msgflg);
long __GEMDOS(0x179)	Pmsgctl(long msqid, long cmd, struct msqid_ds *buf);
long __GEMDOS(0x17a)	Pmsgsnd(long msqid, const void *msgp, long msgsz, long msgflg);
long __GEMDOS(0x17b)	Pmsgrcv(long msqid, void *msgp, long msgsz, long msgtyp, long msgflg);
#endif
long __GEMDOS(0x17d)	Maccess(void *addr, long size, short mode);
long __GEMDOS(0x180)	Fchown16(const char *name, short uid, short gid, short follow);
long __GEMDOS(0x181)	Fchdir(short fd);
long __GEMDOS(0x182)	Ffdopendir(short fd);
long __GEMDOS(0x183)	Fdirfd(long handle);

long __GEMDOS(0x15)		Srealloc(long size);


/* KAOS 1.2 */
long __GEMDOS(0x33)		Sconfig(short mode, long value);
#ifdef __AHCC__
#define Fshrink(a)      Fwrite(a, 0L, (void *) -1L)
#define Mgrow(p,s) Mshrink(p,s)
#define Mblavail(block) Mshrink(block, -1l)
#else
long Fshrink(short handle);
long Mgrow(void *block, long newsize);
long Mblavail(void *block);
#endif

/* MagiC 5.20 Share Library Support */
long __GEMDOS(0x16)		Slbopen(const char *name, const char *path, long min_ver, SLB_HANDLE *slb, SLB_EXEC *slbexec);
long __GEMDOS(0x17)		Slbclose(SLB_HANDLE slb);

#undef __GEMDOS
#undef __BIOS
#undef __XBIOS

#endif /* __PUREC__ */



#if defined(__GNUC__) && defined(__mc68000__)

#ifndef __mint_sighandler_t_defined
#define __mint_sighandler_t_defined 1
#ifdef __NO_CDECL
typedef void *__mint_sighandler_t;
#else
typedef void _CDECL (*__mint_sighandler_t) (long signum);
#endif
#endif

/* see compiler.h for __extension__ and AND_MEMORY */

#define trap_1_wllw(n, a, b, c)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long _a = (long)(a);						\
	long  _b = (long) (b);						\
	short  _c = (short) (c);					\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(12),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c)     /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wwlw(n, a, b, c)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	long  _b = (long) (b);						\
	short  _c = (short) (c);					\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(10),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c)     /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wwww(n, a, b, c)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	short  _b = (short)(b);						\
	short  _c = (short)(c);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"addql	#8,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c)     /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wwwl(n, a, b, c)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	short  _b = (short)(b);						\
	long  _c = (long)(c);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(10),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c)     /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wwl(n, a, b)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	long  _b = (long) (b);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"addql	#8,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b)		/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wlllw(n, a, b, c, d)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long _a = (long) (a);						\
	long _b = (long) (b);						\
	long _c = (long) (c);						\
	short _d = (short) (d);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(16),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c), "r"(_d) /* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wllwwl(n,a,b,c,d,e) \
__extension__ \
({ \
	register long __retvalue __asm__("d0"); \
	long _a = (long)(a); \
	long _b = (long)(b); \
	short _c = (short)(c); \
	short _d = (short)(d); \
	long _e = (long)(e); \
	 \
	__asm__ volatile \
	( \
		"movl	%6,%%sp@-\n\t" \
		"movw	%5,%%sp@-\n\t" \
		"movw	%4,%%sp@-\n\t" \
		"movl	%3,%%sp@-\n\t" \
		"movl	%2,%%sp@-\n\t" \
		"movw	%1,%%sp@-\n\t" \
		"trap	#1\n\t" \
		"lea	%%sp@(18),%%sp" \
	: "=r"(__retvalue) /* outputs */ \
	: "g"(n), "r"(_a), "r"(_b), "r"(_c), "r"(_d), "r"(_e) /* inputs  */ \
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc" /* clobbered regs */ \
	  AND_MEMORY \
	); \
	__retvalue; \
})

#define trap_1_wlll(n, a, b, c)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long _a = (long)(a);						\
	long _b = (long)(b);						\
	long _c = (long)(c);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(14),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c)	/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
									\
	__retvalue;							\
})

#define trap_1_wwllll(n, a, b, c, d, e)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	long  _b = (long) (b);						\
	long  _c = (long) (c);						\
	long  _d = (long) (d);						\
	long  _e = (long) (e);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%6,%%sp@-\n\t"					\
		"movl	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(20),%%sp "					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c), "r"(_d), "r"(_e) /* inputs  */ \
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wllll(n, a, b, c, d)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long _a = (long)(a);						\
	long _b = (long)(b);						\
	long _c = (long)(c);						\
	long _d = (long)(d);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(18),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c),				\
	  "r"(_d)				/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
									\
	__retvalue;							\
})

#define trap_1_wwlllll(n, a, b, c, d, e, f)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	long _b = (long)(b);						\
	long _c = (long)(c);						\
	long _d = (long)(d);						\
	long _e = (long)(e);						\
	long _f = (long)(f);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%7,%%sp@-\n\t"					\
		"movl	%6,%%sp@-\n\t"					\
		"movl	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(24),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c),				\
	  "r"(_d), "r"(_e), "r"(_f)		/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
									\
	__retvalue;							\
})

#define trap_1_wlllll(n, a, b, c, d, e)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long _a = (long)(a);						\
	long _b = (long)(b);						\
	long _c = (long)(c);						\
	long _d = (long)(d);						\
	long _e = (long)(e);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%6,%%sp@-\n\t"					\
		"movl	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(22),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c),				\
	  "r"(_d), "r"(_e)			/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
									\
	__retvalue;							\
})

#define trap_1_wllllll(n, a, b, c, d, e, f)				\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long _a = (long)(a);						\
	long _b = (long)(b);						\
	long _c = (long)(c);						\
	long _d = (long)(d);						\
	long _e = (long)(e);						\
	long _f = (long)(f);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%7,%%sp@-\n\t"					\
		"movl	%6,%%sp@-\n\t"					\
		"movl	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(26),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c),				\
	  "r"(_d), "r"(_e), "r"(_f)		/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
									\
	__retvalue;							\
})


#define Srealloc(newsize)					\
		trap_1_wl(0x15, (long)(newsize))
#define Slbopen(name, path, min_ver, sl, fn)			\
		trap_1_wlllll(0x16, (long)(name), (long)(path), (long)(min_ver), (long)(sl), (long)(fn))
#define Slbclose(sl)						\
		trap_1_wl(0x17, (long)(sl))
#define	Syield()						\
		trap_1_w(0xff)
#define Fpipe(ptr)						\
		trap_1_wl(0x100, (long)(ptr))
#define Ffchown(f, uid, gid)					\
		trap_1_wwww(0x101, (short)(f), (short)(uid), (short)(gid))
#define Ffchmod(f, mode)					\
		trap_1_www(0x102, (short)(f), (short)(mode))
#define Fsync(f)						\
		trap_1_ww(0x103, (short)(f))
#define Fcntl(f, arg, cmd)					\
		trap_1_wwlw(0x104, (short)(f), (long)(arg), (short)(cmd))
#define Finstat(f)						\
		trap_1_ww(0x105, (short)(f))
#define Foutstat(f)						\
		trap_1_ww(0x106, (short)(f))
#define Fgetchar(f, mode)					\
		trap_1_www(0x107, (short)(f), (short)(mode))
#define Fputchar(f, ch, mode)					\
		trap_1_wwlw(0x108, (short)(f), (long)(ch), (short)(mode))

#define Pwait()							\
		trap_1_w(0x109)
#define Pnice(delta)						\
		trap_1_ww(0x10a, (short)(delta))
#define Pgetpid()						\
		(short)trap_1_w(0x10b)
#define Pgetppid()						\
		(short)trap_1_w(0x10c)
#define Pgetpgrp()						\
		(short)trap_1_w(0x10d)
#define Psetpgrp(pid, grp)					\
		(short)trap_1_www(0x10e, (short)(pid), (short)(grp))
#define Pgetuid()						\
		(short)trap_1_w(0x10f)
#define Psetuid(id)						\
		(short)trap_1_ww(0x110, (short)(id))
#define Pkill(pid, sig)						\
		(short)trap_1_www(0x111, (short)(pid), (short)(sig))
#define Psignal(sig, handler)					\
		(__mint_sighandler_t)trap_1_wwl(0x112, (short)(sig), (long)(handler))
#define Pvfork()						\
		trap_1_w(0x113)
#define Pgetgid()						\
		(short)trap_1_w(0x114)
#define Psetgid(id)						\
		(short)trap_1_ww(0x115, (short)(id))
#define Psigblock(mask)						\
		trap_1_wl(0x116, (unsigned long)(mask))
#define Psigsetmask(mask)					\
		trap_1_wl(0x117, (unsigned long)(mask))
#define Pusrval(arg)						\
		trap_1_wl(0x118, (long)(arg))
#define Pdomain(arg)						\
		(short)trap_1_ww(0x119, (short)(arg))
#define Psigreturn()						\
		(void)trap_1_w(0x11a)
#define Pfork()							\
		trap_1_w(0x11b)
#define Pwait3(flag, rusage)					\
		trap_1_wwl(0x11c, (short)(flag), (long)(rusage))
#define Fselect(time, rfd, wfd, xfd)				\
		(short)trap_1_wwlll(0x11d, (unsigned short)(time), (long)(rfd), \
				(long)(wfd), (long)(xfd))
#define Prusage(rsp)						\
		trap_1_wl(0x11e, (long)(rsp))
#define Psetlimit(i, val)					\
		trap_1_wwl(0x11f, (short)(i), (long)(val))

#define Talarm(sec)						\
		trap_1_wl(0x120, (long)(sec))
#define Pause()							\
		trap_1_w(0x121)
#define Sysconf(n)						\
		trap_1_ww(0x122, (short)(n))
#define Psigpending()						\
		trap_1_w(0x123)
#define Dpathconf(name, which)					\
		trap_1_wlw(0x124, (long)(name), (short)(which))

#define Pmsg(mode, mbox, msg)					\
		trap_1_wwll(0x125, (short)(mode), (long)(mbox), (long)(msg))
#define Fmidipipe(pid, in, out)					\
		trap_1_wwww(0x126, (short)(pid), (short)(in),(short)(out))
#define Prenice(pid, delta)					\
		(short)trap_1_www(0x127, (short)(pid), (short)(delta))
#define Dopendir(name, flag)					\
		trap_1_wlw(0x128, (long)(name), (short)(flag))
#define Dreaddir(len, handle, buf)				\
		trap_1_wwll(0x129, (short)(len), (long)(handle), (long)(buf))
#define Drewinddir(handle)					\
		trap_1_wl(0x12a, (long)(handle))
#define Dclosedir(handle)					\
		trap_1_wl(0x12b, (long)(handle))
#define Fxattr(flag, name, buf)					\
		trap_1_wwll(0x12c, (short)(flag), (long)(name), (long)(buf))
#define Flink(old, new)						\
		trap_1_wll(0x12d, (long)(old), (long)(new))
#define Fsymlink(old, new)					\
		trap_1_wll(0x12e, (long)(old), (long)(new))
#define Freadlink(siz, buf, linknm)				\
		trap_1_wwll(0x12f, (short)(siz), (long)(buf), (long)(linknm))
#define Dcntl(cmd, name, arg)					\
		trap_1_wwll(0x130, (short)(cmd), (long)(name), (long)(arg))
#define Fchown(name, uid, gid)					\
		trap_1_wlww(0x131, (long)(name), (short)(uid), (short)(gid))
#define Fchmod(name, mode)					\
		trap_1_wlw(0x132, (long)(name), (short)(mode))
#define Pumask(mask)						\
		(unsigned short)trap_1_ww(0x133, (unsigned short)(mask))
#define Psemaphore(mode, id, tmout)				\
		trap_1_wwll(0x134, (short)(mode), (long)(id), (long)(tmout))
#define Dlock(mode, drive)					\
		(short)trap_1_www(0x135, (short)(mode), (short)(drive))
#define Psigpause(mask)						\
		trap_1_wl(0x136, (unsigned long)(mask))
#define Psigaction(sig, act, oact)					\
		trap_1_wwll(0x137, (short)(sig), (long)(act), (long)(oact))
#define Pgeteuid()						\
		(short)trap_1_w(0x138)
#define Pgetegid()						\
		(short)trap_1_w(0x139)
#define Pwaitpid(pid,flag, rusage)				\
		trap_1_wwwl(0x13a, (short)(pid), (short)(flag), (long)(rusage))
#define Dgetcwd(path, drv, size)				\
		trap_1_wlww(0x13b, (long)(path), (short)(drv), (short)(size))
#define Salert(msg)						\
		trap_1_wl(0x13c, (long)(msg))
/* The following are not yet official... */
#define Tmalarm(ms)						\
		trap_1_wl(0x13d, (long)(ms))
#define Psigintr(vec, sig)					\
		trap_1_www(0x13e, (short)(vec), (short)(sig))
#define Suptime(uptime, avenrun)				\
		trap_1_wll(0x13f, (long)(uptime), (long)(avenrun))
#define Ptrace(request, pid, addr, data)		\
		trap_1_wwwll(0x140, (short)(request), (short)(pid), \
			      (long)(addr), (long)(data))
#define Mvalidate(pid,addr,size,flags)				\
		trap_1_wwlll(0x141, (short)(pid), (long)(addr), (long)(size), (long)(flags))
#define Dxreaddir(len, handle, buf, xattr, xret)		\
		trap_1_wwllll(0x142, (short)(len), (long)(handle), \
			      (long)(buf), (long)(xattr), (long)(xret))
#define Pseteuid(id)						\
		(short)trap_1_ww(0x143, (short)(id))
#define Psetegid(id)						\
		(short)trap_1_ww(0x144, (short)(id))
#define Pgetauid()						\
		(short)trap_1_w(0x145)
#define Psetauid(id)						\
		(short)trap_1_ww(0x146, (short)(id))
#define Pgetgroups(gidsetlen, gidset)				\
		trap_1_wwl(0x147, (short)(gidsetlen), (long)(gidset))
#define Psetgroups(gidsetlen, gidset)				\
		trap_1_wwl(0x148, (short)(gidsetlen), (long)(gidset))
#define Tsetitimer(which, interval, value, ointerval, ovalue)	\
		trap_1_wwllll(0x149, (short)(which), (long)(interval), \
			      (long)(value), (long)(ointerval), (long)(ovalue))
#define Dchroot(dir)						\
		trap_1_wl(0x14a, (long)(dir))
#define Fstat64(flag, name, stat)					\
		trap_1_wwll(0x14b, (short)(flag), (long)(name), (long)(stat))
#define Fseek64(high, low, fh, how, newpos) \
		trap_1_wllwwl(0x14c, (long)(high), (long)(low), (short)(fh), \
		(short)(how), (long)(newpos))
#define Dsetkey(major, minor, key, cipher)				\
		trap_1_wlllw(0x14d, (long)(major), (long)(minor), (long)(key), \
		(short)(cipher))
#define Psetreuid(rid, eid)   \
		(short)trap_1_www(0x14e, (short)(rid), (short)(eid))
#define Psetregid(rid, eid)   \
		(short)trap_1_www(0x14f, (short)(rid), (short)(eid))
#define Sync()   \
		trap_1_w(0x150)
#define Shutdown(restart)  \
		trap_1_wl(0x151, (long)(restart))
#define Dreadlabel(path, label, maxlen)  \
		trap_1_wllw(0x152, (long)(path), (long)(label), (short)(maxlen))
#define Dwritelabel(path, label)  \
		trap_1_wll(0x153, (long)(path), (long)(label))
#define Ssystem(mode, arg1, arg2) \
		trap_1_wwll(0x154, (short)(mode), (long)(arg1), (long)(arg2))
#define Tgettimeofday(tvp, tzp) \
		trap_1_wll(0x155, (long)(tvp), (long)(tzp))
#define Tsettimeofday(tvp, tzp) \
		trap_1_wll(0x156, (long)(tvp), (long)(tzp))
#define Tadjtime(delta, olddelta) \
		trap_1_wll(0x157, (long)(delta), (long)(olddelta))
#define Pgetpriority(which, who) \
		trap_1_www(0x158, (short)(which), (short)(who))
#define Psetpriority(which, who, prio) \
		trap_1_wwww(0x159, (short)(which), (short)(who), (short)(prio))
#define Fpoll(fds, nfds, timeout) \
		trap_1_wlll(0x15a,(long)(fds),(long)(nfds),(long)(timeout))
#define Fwritev(fh, iovp, iovcnt) \
		trap_1_wwll(0x15b,(short)(fh),(long)(iovp),(long)(iovcnt))
#define Freadv(fh, iovp, iovcnt) \
		trap_1_wwll(0x15c,(short)(fh),(long)(iovp),(long)(iovcnt))
#define Ffstat64(fh, stat) \
		trap_1_wwl(0x15d,(short)(fh),(long)(stat))
#define Psysctl(name, namelen, old, oldlenp, new, newlen) \
		trap_1_wllllll(0x15e,(long)(name),(long)(namelen),(long)(old),(long)(oldlenp),(long)(new),(long)(newlen))
#define Pemulation(which, op, a1, a2, a3, a4, a5, a6, a7) \
		trap_1_wwwlllllll(0x15f,(short)(which),(short)(op),(long)(a1),(long)(a2),(long)(a3),(long)(a4),(long)(a5),(long)(a6),(long)(a7))
#define Fsocket(domain, type, protocol) \
		trap_1_wlll(0x160,(long)(domain),(long)(type),(long)(protocol))
#define Fsocketpair(domain, type, protocol, rsv) \
		trap_1_wllll(0x161,(long)(domain),(long)(type),(long)(protocol),(long)(rsv))
#define Faccept(fh, name, namelen) \
		trap_1_wwll(0x162,(short)(fh),(long)(name),(long)(namelen))
#define Fconnect(fh, name, namelen) \
		trap_1_wwll(0x163,(short)(fh),(long)(name),(long)(namelen))
#define Fbind(fh, name, namelen) \
		trap_1_wwll(0x164,(short)(fh),(long)(name),(long)(namelen))
#define Flisten(fh, backlog) \
		trap_1_wwl(0x165,(short)(fh),(long)(backlog))
#define Frecvmsg(fh, msg, flags) \
		trap_1_wwll(0x166,(short)(fh),(long)(msg),(long)(flags))
#define Fsendmsg(fh, msg, flags) \
		trap_1_wwll(0x167,(short)(fh),(long)(msg),(long)(flags))
#define Frecvfrom(fh, buf, len, flags, from, fromlen) \
		trap_1_wwlllll(0x168,(short)(fh),(long)(buf),(long)(len),(long)(flags),(long)(from),(long)(fromlen))
#define Fsendto(fh, buf, len, flags, to, tolen) \
		trap_1_wwlllll(0x169,(short)(fh),(long)(buf),(long)(len),(long)(flags),(long)(to),(long)(tolen))
#define Fsetsockopt(fh, level, name, val, valsize) \
		trap_1_wwllll(0x16a,(short)(fh),(long)(level),(long)(name),(long)(val),(long)(valsize))
#define Fgetsockopt(fh, level, name, val, avalsize) \
		trap_1_wwllll(0x16b,(short)(fh),(long)(level),(long)(name),(long)(val),(long)(avalsize))
#define Fgetpeername(fh, addr, addrlen) \
		trap_1_wwll(0x16c,(short)(fh),(long)(addr),(long)(addrlen))
#define Fgetsockname(fh, addr, addrlen) \
		trap_1_wwll(0x16d,(short)(fh),(long)(addr),(long)(addrlen))
#define Fshutdown(fh, how) \
		trap_1_wwl(0x16e,(short)(fh),(long)(how))
/* 0x16f */
#define Pshmget(key, size, shmflg) \
		trap_1_wlll(0x170,(long)(key),(long)(size),(long)(shmflg))
#define Pshmctl(shmid, cmd, buf) \
		trap_1_wlll(0x171,(long)(shmid),(long)(cmd),(long)(buf))
#define Pshmat(shmid, shmaddr, shmflg) \
		trap_1_wlll(0x172,(long)(shmid),(long)(shmaddr),(long)(shmflg))
#define Pshmdt(shmaddr) \
		trap_1_wl(0x173,(long)(shmaddr))
#define Psemget(key, nsems, semflg) \
		trap_1_wlll(0x174,(long)(key),(long)(nsems),(long)(semflg))
#define Psemctl(semid, semnum, cmd, arg) \
		trap_1_wllll(0x175,(long)(semid),(long)(semnum),(long)(cmd),(long)(arg))
#define Psemop(semid, sops, nsops) \
		trap_1_wlll(0x176,(long)(semid),(long)(sops),(long)(nsops))
#define Psemconfig(flag) \
		trap_1_wl(0x177,(long)(flag))
#define Pmsgget(key, msgflg) \
		trap_1_wll(0x178,(long)(key),(long)(msgflg))
#define Pmsgctl(msqid, cmd, buf) \
		trap_1_wlll(0x179,(long)(msqid),(long)(cmd),(long)(buf))
#define Pmsgsnd(msqid, msgp, msgsz, msgflg) \
		trap_1_wllll(0x17a,(long)(msqid),(long)(msgp),(long)(msgsz),(long)(msgflg))
#define Pmsgrcv(msqid, msgp, msgsz, msgtyp, msgflg) \
		trap_1_wlllll(0x17b,(long)(msqid),(long)(msgp),(long)(msgsz),(long)(msgtyp),(long)(msgflg))
/* 0x17c */
#define Maccess(addr,size,mode) \
		trap_1_wllw(0x17d, (long)(addr), (long)(size), (short)(mode))
/* 0x17e */
/* 0x17f */
#define Fchown16(name, uid, gid, follow_links) \
		trap_1_wlwww(0x180, (long)(name), (short)(uid), (short)(gid), (short)follow_links)
#define Fchdir(fh) \
		trap_1_ww(0x181, (short)(fh))
#define Ffdopendir(fh) \
		trap_1_ww(0x182, (short)(fh))
#define Fdirfd(handle) \
		trap_1_wl(0x183, (long)(handle))

#define Sconfig(mode, value) \
		trap_1_wwl(0x33, (short)(mode), (long)(value))
#define Mgrow(block, newsize) \
		trap_1_wwll(0x4a, (short)0, (long)(block), (long)(newsize))
#define Mblavail(block) \
		trap_1_wwll(0x4a, (short)0, (long)(block), (long)(-1))
#define Fshrink(a)      Fwrite(a, 0L, (void *) -1L)         /* KAOS 1.2 */

#endif /* __GNUC__ */

__END_DECLS

#endif /* _MINT_MINTBIND_H */
