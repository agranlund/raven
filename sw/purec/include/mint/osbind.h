/*
 * osbind.h	bindings for OS traps
 *
 *		++jrb bammi@cadence.com
 */

/*
 * majorly re-hacked for gcc-1.36 and probably beyond
 * all inlines changed to #defines, beacuse gcc is not
 * handling clobbered reggies correctly when -mshort.
 * We now use the Statement Exprs feature of GnuC
 *
 * 10/12/89
 *	changed all "g" constraints to "r" that will force
 *	all operands to be evaluated (or lea calculated)
 *	before the __asm__. This is necessary because
 *	if we had the (looser) "g" constraint, then sometimes
 *	we are in the situation where stuff is in the stack,
 *	and we are modifying the stack under Gcc (but eventually
 *	restoring it before the end of the __asm__), and it does
 *	not know about it (i believe there is no way to tell it
 *	this either, but you can hardly expect that). by forcing
 *	the stricter "r" constraint, we force the eval before using
 *	the val (or lea as the case may be) and we dont get into
 *	trouble.
 *	(thanks to ers for finding this problem!)
 *	[one side effect of this is that we may(depending on the
 *	  situation) actually end up with better code when the
 *	values are already in reggies, or that value is used
 *	later on (note that Gnu's reggie allocation notices the
 *	clobbered reggie, and does'nt put the value/or uses
 *	them from those reggies, nice huh!)
 *
 *  28/2/90
 *	another major re-hack:
 *	-- the basic reason: there was just no reliable
 *	way to get the definitions (inline or not does'nt matter) to
 *	fully evaluate the args before we changed the sp from under it.
 *	(if -fomit-frame-pointer is *not* used, then most of the time
 *	 we dont need to do this, as things will just reference off of
 *	 a6, but this is not true all of the time).
 *	my solution was to use local vars in the body of the statement
 *	exprs, and initialize them from the args of the statement expr block.
 *	to force the evaluation of the args before we change sp from
 *	under gcc's feet, we make the local vars volatile. we use a
 *	slight code optimization heuristic: if there are more than 4
 *	args, only then we make the local volatile, and relax
 *	the "r" constraint to "g". otherwise, we dont put the volatile
 *	and force the evaluation by putting the "r" constaint. this
 *	produces better code in most sitiations (when !__NO_INLINE__
 *	especially), as either the args are already in a register or
 *	there is good chance they will soon be reused, and in that
 *	case it will already be in a register.
 *      it may (the local vars, especially when no volatile)
 *	look like overhead, but in 99% of the situations gcc will just
 *	optimize that assignment right out. besides, this makes
 *	these defines totally safe (from re-evaluation of the macro args).
 *
 *	-- as suggested by andreas schwab (thanks!)
 *	 (schwab@ls5.informatik.uni-dortmund.de) all the retvalues are now
 *	 local register vals (see the extentions section in the info file)
 *	 this really worked out great as all the silly "movl d0,%0" at
 *	 the end of each def can now be removed, and the value of
 *	 retvalue ends up in the correct register. it avoids all the
 *	 silly "mov d0,[d|a]n" type sequences from being generated. a real win.
 *	 (note in the outputs "=r"(__retvalue) still has to be specified,
 *	 otherwise in certain situations you end up loosing the return
 *	 value in d0, as gcc sees no output, and correctly assumes that the
 *	 asm returns no value).
 *
 *	-- all the n's (the function #'s for the traps) are now given
 *	the more relaxed "g". This again results in better code, as
 *	it is always a constant, and gcc turns the movw %1,sp@- into
 *	a movw #n,sp@-. we could have given them a "i" constraint too,
 *	but "g" gives gcc more breathing room, and it does the right
 *	thing. note: the n's still need to have "r" constraints in the
 *	non-inline form (function form), as they are no longer constants
 *	in the function, but a normal arg on the stack frame, and hence
 *	we need to force evaluation before we change sp. (see osbind.c)
 *
 *	-- straps.cpp and traps.c are history. we dont need no stinking
 *	non-reentrant bindings (straps) or incorrect ones (traps.c :-)
 *
 * 03/15/92 ++jrb
 *	-- another re-hack needed for gcc-2.0: the optimization that we
 *      used earlier for traps with more than 4 args, making them volatile
 *	and using "g" constraints no longer works, because gcc has become
 *	so smart! we now remove the volatile, and give "r" constraints
 *	(just like traps with <= 4 args). that way the args are evaled
 *	before we change the stack under gcc, and at appropriate times
 *	put into reggies and pushed (or as in most cases, they are evaled
 *	straight into reggies and pushed -- and in even more common cases
 *	they are already in reggies, and they are just pushed). not doing
 *	this with -fomit-frame-pointer was causing the temps (from evaluing
 *	the args) to be created on the stack, but when we changed sp
 *	from under gccs feet, the offsets  to the temps ended up being wrong.
 *
 * 10/28/93 ++jrb
 *	relax the constraints on the inputs of trap_14_wwwwwww (only
 *	Rsconf maps to this)  to "g" from "r", as these many "r" 's
 *	give gcc 2.>3.X heartaches (understandably). note this is ok
 *	since these args will never be expressions, and we never
 *	have to constrain hard enough to force eval before we change
 *	sp from underneath gcc.
 *
 */

#ifndef _MINT_OSBIND_H
#define _MINT_OSBIND_H	1

#ifndef _FEATURES_H
# include <features.h>
#endif

#ifndef _MINT_OSTRUCT_H
# include <mint/ostruct.h>
#endif

__BEGIN_DECLS


#if defined(__PUREC__) || defined(__AHCC__) || defined(__TURBOC__) || (defined(__GNUC__) && !defined(__mc68000__))

/* Gemdos prototypes */

#if defined(__AHCC__)
	#define __GEMDOS(b) cdecl __syscall__( 1,b)
	#define __BIOS(b)   cdecl __syscall__(13,b)
	#define __XBIOS(b)  cdecl __syscall__(14,b)
#else
	#define __GEMDOS(b)
	#define __BIOS(b)
	#define __XBIOS(b)
#endif


void  __GEMDOS(0)	Pterm0( void );
long  __GEMDOS(1)	Cconin( void );
void  __GEMDOS(2)	Cconout( short c );
short __GEMDOS(3)	Cauxin( void );
void  __GEMDOS(4)	Cauxout( short c );
short __GEMDOS(5)	Cprnout( short c );
long  __GEMDOS(6)	Crawio( short w );
long  __GEMDOS(7)	Crawcin( void );
long  __GEMDOS(8)	Cnecin( void );
long  __GEMDOS(9)	Cconws( const char *buf );
long  __GEMDOS(10)	Cconrs( _CCONLINE *buf );
short __GEMDOS(11)	Cconis( void );
long  __GEMDOS(14)	Dsetdrv( short drv );
short __GEMDOS(16)	Cconos( void );
short __GEMDOS(17)	Cprnos( void );
short __GEMDOS(18)	Cauxis( void );
short __GEMDOS(19)	Cauxos( void );
short __GEMDOS(25)	Dgetdrv( void );
void  __GEMDOS(26)	Fsetdta( _DTA *buf );
long  __GEMDOS(32)	Super( void *stack );
unsigned short __GEMDOS(42)	Tgetdate( void );
long    __GEMDOS(43)	Tsetdate( unsigned short date );
unsigned short __GEMDOS(44)	Tgettime( void );
long    __GEMDOS(45)	Tsettime( unsigned short time );
_DTA    *__GEMDOS(47)	Fgetdta( void );
short   __GEMDOS(48)	Sversion( void );
void    __GEMDOS(49)	Ptermres( long keepcnt, short retcode );
short   __GEMDOS(54)	Dfree( _DISKINFO *buf, short driveno );
short   __GEMDOS(57)	Dcreate( const char *path );
short   __GEMDOS(58)	Ddelete( const char *path );
short   __GEMDOS(59)	Dsetpath( const char *path );
long    __GEMDOS(60)	Fcreate( const char *filename, short attr );
long    __GEMDOS(61)	Fopen( const char *filename, short mode );
short   __GEMDOS(62)	Fclose( short handle );
long    __GEMDOS(63)	Fread( short handle, long count, void *buf );
long    __GEMDOS(64)	Fwrite( short handle, long count, const void *buf );
short   __GEMDOS(65)	Fdelete( const char *filename );
long    __GEMDOS(66)	Fseek( long offset, short handle, short seekmode );
short   __GEMDOS(67)	Fattrib( const char *filename, short wflag, short attrib );
long    __GEMDOS(69)	Fdup( short handle );
long    __GEMDOS(70)	Fforce( short stdh, short nonstdh );
short   __GEMDOS(71)	Dgetpath( char *path, short driveno );
void   *__GEMDOS(72)	Malloc( long number );
short   __GEMDOS(73)	Mfree( void *block );
#if defined(__AHCC__)
/* need to explicitly pass the hidden first arg */
short   __GEMDOS(74)	_Mshrink( short zero, void *ptr, long size );
#define Mshrink(ptr, size) _Mshrink(0, ptr, size)
#else
short   __GEMDOS(74)	Mshrink( void *ptr, long size );
#endif
long    __GEMDOS(75)	Pexec( short mode, const char *ptr1, const void *ptr2, const void *ptr3 );
void    __GEMDOS(76)	Pterm( short retcode );
short   __GEMDOS(78)	Fsfirst( const char *filename, short attr );
short   __GEMDOS(79)	Fsnext( void );
short   __GEMDOS(86)	Frename( short zero, const char *oldname, const char *newname );
short   __GEMDOS(87)	Fdatime( _DOSTIME *timeptr, short handle, short rwflag );

/* TOS 030 GEMDOS extensions */

void    *__GEMDOS(68)	Mxalloc( long number, short mode );
long    __GEMDOS(20)	Maddalt( void *start, long size );

/* Network Gemdos Extension */

long	__GEMDOS(92)	Flock( short handle, short mode, long start, long length );
long    __GEMDOS(100)	F_lock( short handle, long count );
long    __GEMDOS(98)	Frlock( short handle, long start, long count );
long    __GEMDOS(96)	Nversion( void );
long    __GEMDOS(99)	Frunlock( short handle, long start );
long    __GEMDOS(101)	Funlock( short handle );
long    __GEMDOS(102)	Fflush( short handle );
long    __GEMDOS(123)	Unlock( const char *path );
long    __GEMDOS(124)	Lock( const char *path );

/* PamsNet Network */

short	__GEMDOS(127)	Nactive( void );
short	__GEMDOS(126)	Nnodeid( void );
short	__GEMDOS(125)	Nlogged( short nn );
long	__GEMDOS(124)	Nlock( const char *file );
long	__GEMDOS(123)	Nunlock( const char *file );
long	__GEMDOS(122)	Nlocked(void );
long	__GEMDOS(121)	Nprinter( short nn, short kopf, short dd );
void	__GEMDOS(120)	Nreset( void );
short	__GEMDOS(119)	Nrecord( short handle, short mm, long offset, long leng );
short	__GEMDOS(118)	Nmsg( short rw, char *buf, char *id, short node, short leng );
short	__GEMDOS(117)	Nremote( short nn );
void	__GEMDOS(116)	Ndisable( void );
short	__GEMDOS(115)	Nenable( void );

/* BIOS */

void    __BIOS( 0)	Getmpb( _MPB *ptr );
short   __BIOS( 1)	Bconstat( short dev );
long    __BIOS( 2)	Bconin( short dev );
long    __BIOS( 3)	Bconout( short dev, short c );
long    __BIOS( 4)	Rwabs( short rwflag, void *buf, short cnt, short recnr, short dev, ... /* long lrecno */ );
long    __BIOS( 4)	Lrwabs( short rwflag, void *buf, short cnt, short recnr, short dev, long lrecno );
void	(*__BIOS(5)	Setexc( short number, void (*exchdlr)(void) )) (void);
long    __BIOS( 6)	Tickcal( void );
_BPB    *__BIOS( 7)	Getbpb( short dev );
long    __BIOS( 8)	Bcostat( short dev );
long    __BIOS( 9)	Mediach( short dev );
long    __BIOS(10)	Drvmap( void );
long    __BIOS(11)	Kbshift( short mode );
#define Getshift() Kbshift(-1)

/* XBios */

void    __XBIOS(0)	Initmouse( short type, _PARAM *par, void (*mousevec)(void *) );
void    *__XBIOS(1)	Ssbrk( short count );
void    *__XBIOS(2)	Physbase( void );
void    *__XBIOS(3)	Logbase( void );
short   __XBIOS(4)	Getrez( void );
short   __XBIOS(5)	Setscreen( void *laddr, void *paddr, short rez);
void    __XBIOS(6)	Setpalette( void *pallptr );
short   __XBIOS(7)	Setcolor( short colornum, short color );
short   __XBIOS(8)	Floprd( void *buf, void *filler, short devno, short sectno,
               short trackno, short sideno, short count );
short   __XBIOS(9)	Flopwr( void *buf, void *filler, short devno, short sectno,
               short trackno, short sideno, short count );
short   __XBIOS(10)	Flopfmt( void *buf, void *filler, short devno, short spt, short trackno,
                short sideno, short shorterlv, long magic, short virgin );
void    __XBIOS(11)	Dbmsg(short rsrvd, short msg_num, long msg_arg);
void    __XBIOS(12)	Midiws( short cnt, const void *ptr );
void    __XBIOS(13)	Mfpint( short erno, void (*vector)(void) );
_IOREC   *__XBIOS(14)	Iorec( short dev );
long    __XBIOS(15)	Rsconf( short baud, short ctr, short ucr, short rsr, short tsr, short scr );
_KEYTAB  *__XBIOS(16)	Keytbl( void *unshift, void *shift, void *capslock );
long    __XBIOS(17)	Random( void );
void    __XBIOS(18)	Protobt( void *buf, long serialno, short disktype, short execflag );
short   __XBIOS(19)	Flopver( void *buf, void *filler, short devno, short sectno,
                short trackno, short sideno, short count );
void    __XBIOS(20)	Scrdmp( void );
short   __XBIOS(21)	Cursconf( short func, short rate );
void    __XBIOS(22)	Settime( unsigned long time );
unsigned long  __XBIOS(23)	Gettime( void );
void    __XBIOS(24)	Bioskeys( void );
void    __XBIOS(25)	Ikbdws( short count, const void *ptr );
void    __XBIOS(26)	Jdisint( short number );
void    __XBIOS(27)	Jenabint( short number );
char    __XBIOS(28)	Giaccess( char data, short regno );
void    __XBIOS(29)	Offgibit( short bitno );
void    __XBIOS(30)	Ongibit( short bitno );
void    __XBIOS(31)	Xbtimer( short timer, short control, short data, void (*vector)(void) );
void   *__XBIOS(32)	Dosound( void *buf );
short   __XBIOS(33)	Setprt( short config );
_KBDVECS *__XBIOS(34)	Kbdvbase( void );
short   __XBIOS(35)	Kbrate( short initial, short repeat );
void    __XBIOS(36)	Prtblk( _PBDEF *par );
void    __XBIOS(37)	Vsync( void );
long    __XBIOS(38)	Supexec( long (*func)(void) );
void    __XBIOS(39)	Puntaes( void );
short   __XBIOS(41)	Floprate( short devno, short newrate );
short   __XBIOS(64)	Blitmode( short mode );

/* TOS030 XBios */
short   __XBIOS(42)	DMAread( long sector, short count, void *buffer, short devno );
short   __XBIOS(43)	DMAwrite( long sector, short count, void *buffer, short devno );
long    __XBIOS(44)	Bconmap( short devno );
short   __XBIOS(46)	NVMaccess( short opcode, short start, short count, void *buffer );
short   __XBIOS(80)	EsetShift( short shftMode );
short   __XBIOS(81)	EgetShift( void );
short   __XBIOS(82)	EsetBank( short bankNum );
short   __XBIOS(83)	EsetColor( short colorNum, short color );
void    __XBIOS(84)	EsetPalette( short colorNum, short count, short *palettePtr );
void    __XBIOS(85)	EgetPalette( short colorNum, short count, short *palettePtr );
short   __XBIOS(86)	EsetGray( short swtch );
short   __XBIOS(87)	EsetSmear( short swtch );

/* ST-Book */

void	__XBIOS(47)	Waketime(unsigned short date, unsigned short time);

/* Milan/CT60 */
long __XBIOS(160)	CacheCtrl(short opcode, short param);
long __XBIOS(161)	WdgCtrl(short opcode);
long __XBIOS(162)	ExtRsConf(short command, short device, long param);

/*
 * fallback
 */
long gemdos	(short, ...);
long bios	(short, ...);
long xbios	(short, ...);
 
#undef __GEMDOS
#undef __BIOS
#undef __XBIOS

#endif /* __PUREC__ */

#if defined(__GNUC__) && defined(__mc68000__)


/*
 * GNU C (pseudo inline) Statement Exprs for traps
 *
 */

#define trap_1_w(n)							\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"addql	#2,%%sp\n\t"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n)				/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_ww(n, a)							\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"addql	#4,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a)			/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wl(n, a)							\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"addql	#6,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a)			/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wlw(n, a, b)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	short _b = (short)(b);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
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

#define trap_1_wwll(n, a, b, c)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	long  _b = (long) (b);						\
	long  _c = (long) (c);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
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

#define trap_1_wlww(n, a, b, c)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	short _b = (short)(b);						\
	short _c = (short)(c);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
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

#define trap_1_wlwww(n, a, b, c, d)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long _a = (long)(a);						\
	short _b = (short)(b);						\
	short _c = (short)(c);						\
	short _d = (short)(d);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%5,%%sp@-\n\t"					\
		"movw	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(12),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c), "r"(_d) /* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_1_www(n, a, b)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	short _b = (short)(b);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"addql	#6,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b)		/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wll(n, a, b)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	long  _b = (long) (b);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(10),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b)		/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_1_wwlll(n, a, b, c, d)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);			\
	long  _b = (long) (b);			\
	long  _c = (long) (c);			\
	long  _d = (long) (d);			\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(16),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c), "r"(_d) /* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_1_wwwll(n, a, b, c, d)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	short _b = (short)(b);						\
	long  _c = (long) (c);						\
	long  _d = (long) (d);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(14),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c), "r"(_d) /* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_1_wwllww(n, a, b, c, d, e)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	long  _b = (long) (b);						\
	long  _c = (long) (c);						\
	short _d = (short) (d);						\
	short _e = (short) (e);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%6,%%sp@-\n\t"					\
		"movw	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"lea	%%sp@(16),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c), "r"(_d), "r"(_e) /* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_13_wl(n, a)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#13\n\t"					\
		"addql	#6,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a)			/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_13_w(n)							\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#13\n\t"					\
		"addql	#2,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n)				/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_13_ww(n, a)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#13\n\t"					\
		"addql	#4,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a)			/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_13_www(n, a, b)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	short _b = (short)(b);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#13\n\t"					\
		"addql	#6,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b)		/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_13_wwlwww(n, a, b, c, d, e)				\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);			\
	long  _b = (long) (b);			\
	short _c = (short)(c);			\
	short _d = (short)(d);			\
	short _e = (short)(e);			\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%6,%%sp@-\n\t"					\
		"movw	%5,%%sp@-\n\t"					\
		"movw	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#13\n\t"					\
		"lea	%%sp@(14),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n),							\
	  "r"(_a), "r"(_b), "r"(_c), "r"(_d), "r"(_e) /* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_13_wwl(n, a, b)						\
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
		"trap	#13\n\t"					\
		"addql	#8,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b)		/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_wwl(n, a, b)						\
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
		"trap	#14\n\t"					\
		"addql	#8,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b)              /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_wwll(n, a, b, c)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	long  _b = (long) (b);						\
	long  _c = (long) (c);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(12),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c)     /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_ww(n, a)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"addql	#4,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a)			/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_w(n)							\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"addql	#2,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n)				/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_wllw(n, a, b, c)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	long  _b = (long) (b);						\
	short _c = (short)(c);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(12),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c)       /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_wl(n, a)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"addql	#6,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a)			/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_www(n, a, b)						\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	short _b = (short)(b);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"addql	#6,%%sp"						\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b)		/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_wllwwwww(n, a, b, c, d, e, f, g)			\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	long  _b = (long) (b);						\
	short _c = (short)(c);						\
	short _d = (short)(d);						\
	short _e = (short)(e);						\
	short _f = (short)(f);						\
	short _g = (short)(g);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%8,%%sp@-\n\t"					\
		"movw	%7,%%sp@-\n\t"					\
		"movw	%6,%%sp@-\n\t"					\
		"movw	%5,%%sp@-\n\t"					\
		"movw	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(20),%%sp "					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b),					\
	  "r"(_c), "r"(_d), "r"(_e), "r"(_f), "r"(_g) /* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_14_wllwwwwlw(n, a, b, c, d, e, f, g, h)			\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	long  _b = (long) (b);						\
	short _c = (short)(c);						\
	short _d = (short)(d);						\
	short _e = (short)(e);						\
	short _f = (short)(f);						\
	long  _g = (long) (g);						\
	short _h = (short)(h);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%9,%%sp@-\n\t"					\
		"movl	%8,%%sp@-\n\t"					\
		"movw	%7,%%sp@-\n\t"					\
		"movw	%6,%%sp@-\n\t"					\
		"movw	%5,%%sp@-\n\t"					\
		"movw	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(24),%%sp "					\
	: "=r"(__retvalue)			   /* outputs */	\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c),				\
	  "r"(_d), "r"(_e), "r"(_f), "r"(_g), "r"(_h) /* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_14_wllwwwwwlw(n, a, b, c, d, e, f, g, h, i)		\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	long  _b = (long) (b);						\
	short _c = (short)(c);						\
	short _d = (short)(d);						\
	short _e = (short)(e);						\
	short _f = (short)(f);						\
	short _g = (short)(g);						\
	long  _h = (long) (h);						\
	short _i = (short)(i);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%9,%%sp@-\n\t"					\
		"movl	%8,%%sp@-\n\t"					\
		"movw	%7,%%sp@-\n\t"					\
		"movw	%6,%%sp@-\n\t"					\
		"movw	%5,%%sp@-\n\t"					\
		"movw	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movl	%1,%%sp@-\n\t"					\
                "movw	%0,%%sp@-"					\
	:					      /* outputs */	\
	: "g"(n), "g"(_a), "g"(_b), "g"(_c), "g"(_d),			\
	  "g"(_e), "g"(_f), "g"(_g), "g"(_h), "g"(_i) /* inputs  */	\
	);								\
	    								\
	__asm__ volatile						\
	(								\
		"trap	#14\n\t"					\
		"lea	%%sp@(26),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: 					/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})


#define trap_14_wwwwwww(n, a, b, c, d, e, f)				\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	short _b = (short)(b);						\
	short _c = (short)(c);						\
	short _d = (short)(d);						\
	short _e = (short)(e);						\
	short _f = (short)(f);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%7,%%sp@-\n\t"					\
		"movw	%6,%%sp@-\n\t"					\
		"movw	%5,%%sp@-\n\t"					\
		"movw	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(14),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "g"(_a),						\
	  "g"(_b), "g"(_c), "g"(_d), "g"(_e), "g"(_f)	/* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_14_wlll(n, a, b, c)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	long  _b = (long) (b);						\
	long  _c = (long) (c);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(14),%%sp "					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c)     /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_wllww(n, a, b, c, d)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	long  _b = (long) (b);						\
	short _c = (short)(c);						\
	short _d = (short)(d);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%5,%%sp@-\n\t"					\
		"movw	%4,%%sp@-\n\t"					\
		"movl	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(14),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n),							\
	  "r"(_a), "r"(_b), "r"(_c), "r"(_d)    /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_14_wwwwl(n, a, b, c, d)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	short _b = (short)(b);						\
	short _c = (short)(c);						\
	long  _d = (long) (d);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%5,%%sp@-\n\t"					\
		"movw	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(12),%%sp "					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n),							\
	  "r"(_a), "r"(_b), "r"(_c), "r"(_d)        /* inputs  */	\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	);								\
	__retvalue;							\
})

#define trap_14_wwwl(n, a, b, c)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	short _a = (short)(a);						\
	short _b = (short)(b);						\
	long  _c = (long)(c);						\
	    								\
	__asm__ volatile						\
	(								\
		"movl	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movw	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(10),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n), "r"(_a), "r"(_b), "r"(_c)	/* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"    /* clobbered regs */	\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})

#define trap_14_wlwlw(n, a, b, c, d)					\
__extension__								\
({									\
	register long __retvalue __asm__("d0");				\
	long  _a = (long) (a);						\
	short _b = (short)(b);						\
	long  _c = (long) (c);						\
	short _d = (short)(d);						\
	    								\
	__asm__ volatile						\
	(								\
		"movw	%5,%%sp@-\n\t"					\
		"movl	%4,%%sp@-\n\t"					\
		"movw	%3,%%sp@-\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	%1,%%sp@-\n\t"					\
		"trap	#14\n\t"					\
		"lea	%%sp@(14),%%sp"					\
	: "=r"(__retvalue)			/* outputs */		\
	: "g"(n),							\
	  "r"(_a), "r"(_b), "r"(_c), "r"(_d)    /* inputs  */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc", "memory"			\
	  AND_MEMORY							\
	);								\
	__retvalue;							\
})


/* DEFINITIONS FOR OS FUNCTIONS */

/*
 *     GEMDOS  (trap1)
 */
#define	       Pterm0()					       	       \
       (void)trap_1_w((short)(0x00))
#define	       Cconin()						       \
       (long)trap_1_w((short)(0x01))
#define	       Cconout(c)					       \
       (void)trap_1_ww((short)(0x02),(short)(c))
#define	       Cauxin()						       \
       (short)trap_1_w((short)(0x03))
#define	       Cauxout(c)					       \
       (void)trap_1_ww((short)(0x04),(short)(c))
#define	       Cprnout(c)					       \
       (short)trap_1_ww((short)(0x05),(short)(c))
#define	       Crawio(data)					       \
       (long)trap_1_ww((short)(0x06),(short)(data))
#define	       Crawcin()					       \
       (long)trap_1_w((short)(0x07))
#define	       Cnecin()						       \
       (long)trap_1_w((short)(0x08))
#define	       Cconws(s)					       \
       (short)trap_1_wl((short)(0x09),(long)(s))
#define	       Cconrs(buf)					       \
       trap_1_wl((short)(0x0A),(long)(buf))
#define	       Cconis()						       \
       (short)trap_1_w((short)(0x0B))
#define	       Dsetdrv(d)					       \
       (long)trap_1_ww((short)(0x0E),(short)(d))
#define	       Cconos()						       \
       (short)trap_1_w((short)(0x10))
#define	       Cprnos()						       \
       (short)trap_1_w((short)(0x11))
#define	       Cauxis()						       \
       (short)trap_1_w((short)(0x12))
#define	       Cauxos()						       \
       (short)trap_1_w((short)(0x13))
#define	       Dgetdrv()					       \
       (short)trap_1_w((short)(0x19))
#define	       Fsetdta(dta)					       \
       (void)trap_1_wl((short)(0x1A),(long)(dta))

/*
 * The next binding is not quite right if used in another than the usual ways:
 *	1. Super(1L) from either user or supervisor mode
 *	2. ret = Super(0L) from user mode and after this Super(ret) from
 *	   supervisor mode
 * We get the following situations (usp, ssp relative to the start of Super):
 *	Parameter	Userstack	Superstack	Calling Mode	ret
 *	   1L		   usp		   ssp		    user	 0L
 *	   1L		   usp		   ssp		 supervisor	-1L
 *	   0L		  usp-6		   usp		    user	ssp
 *	   0L		   ssp		  ssp-6		 supervisor   ssp-6
 *	  ptr		  usp-6		  ptr+6		    user	ssp
 *	  ptr		  usp+6		   ptr		 supervisor	 sr
 * The usual C-bindings are safe only because the "unlk a6" is compensating
 * the errors when you invoke this function. In this binding the "unlk a6" at
 * the end of the calling function compensates the error made in sequence 2
 * above (the usp is 6 to low after the first call which is not corrected by
 * the second call).
 */
#define	       Super(ptr)					       \
       (long)trap_1_wl((short)(0x20),(long)(ptr))
	/* Tos 1.4: Super(1L) : rets -1L if in super mode, 0L otherwise */

/*
 * Safe binding to switch back from supervisor to user mode.
 * On TOS or EmuTOS, if the stack pointer has changed between Super(0)
 * and Super(oldssp), the resulting user stack pointer is wrong.
 * This bug does not occur with FreeMiNT.
 * So the safe way to return from supervisor to user mode is to backup
 * the stack pointer then restore it after the trap.
 * Sometimes, GCC optimizes the stack usage, so this matters.
 */
#define SuperToUser(ptr)						\
(void)__extension__							\
({									\
	register long __retvalue __asm__("d0");				\
	register long __sp_backup;					\
									\
	__asm__ volatile						\
	(								\
		"movl	%%sp,%1\n\t"					\
		"movl	%2,%%sp@-\n\t"					\
		"movw	#0x20,%%sp@-\n\t"					\
		"trap	#1\n\t"						\
		"movl	%1,%%sp\n\t"					\
	: "=r"(__retvalue), "=&r"(__sp_backup)	/* outputs */		\
	: "g"((long)(ptr)) 			/* inputs */		\
	: __CLOBBER_RETURN("d0") "d1", "d2", "a0", "a1", "a2", "cc"		\
	  AND_MEMORY							\
	);								\
})

#define	       Tgetdate()					       \
       (unsigned short)trap_1_w((short)(0x2A))
#define	       Tsetdate(date)					       \
       (long)trap_1_ww((short)(0x2B),(short)(date))
#define	       Tgettime()					       \
       (unsigned short)trap_1_w((short)(0x2C))
#define	       Tsettime(time)					       \
       (long)trap_1_ww((short)(0x2D),(short)(time))
#define	       Fgetdta()					       \
       (_DTA *)trap_1_w((short)(0x2F))
#define	       Sversion()					       \
       (short)trap_1_w((short)(0x30))
#define	       Ptermres(save,rv)				       \
       (void)trap_1_wlw((short)(0x31),(long)(save),(short)(rv))
#define	       Dfree(buf,d)					       \
       (long)trap_1_wlw((short)(0x36),(long)(buf),(short)(d))
#define	       Dcreate(path)					       \
       (short)trap_1_wl((short)(0x39),(long)(path))
#define	       Ddelete(path)					       \
       (long)trap_1_wl((short)(0x3A),(long)(path))
#define	       Dsetpath(path)					       \
       (long)trap_1_wl((short)(0x3B),(long)(path))
#define	       Fcreate(fn,mode)					       \
       (long)trap_1_wlw((short)(0x3C),(long)(fn),(short)(mode))
#define	       Fopen(fn,mode)					       \
       (long)trap_1_wlw((short)(0x3D),(long)(fn),(short)(mode))
#define	       Fclose(handle)					       \
       (long)trap_1_ww((short)(0x3E),(short)(handle))
#define	       Fread(handle,cnt,buf)				       \
       (long)trap_1_wwll((short)(0x3F),(short)(handle),	       \
			 (long)(cnt),(long)(buf))
#define	       Fwrite(handle,cnt,buf)				       \
       (long)trap_1_wwll((short)(0x40),(short)(handle),	       \
			 (long)(cnt),(long)(buf))
#define	       Fdelete(fn)					       \
       (long)trap_1_wl((short)(0x41),(long)(fn))
#define	       Fseek(where,handle,how)				       \
       (long)trap_1_wlww((short)(0x42),(long)(where),	       \
			 (short)(handle),(short)(how))
#define	       Fattrib(fn,rwflag,attr)				       \
       (short)trap_1_wlww((short)(0x43),(long)(fn),	       \
			  (short)(rwflag),(short)(attr))
#define	       Fdup(handle)					       \
       (long)trap_1_ww((short)(0x45),(short)(handle))
#define	       Fforce(Hstd,Hnew)				       \
       (long)trap_1_www((short)(0x46),(short)(Hstd),(short)(Hnew))
#define	       Dgetpath(buf,d)					       \
       (long)trap_1_wlw((short)(0x47),(long)(buf),(short)(d))
#define	       Malloc(size)					       \
       (void *)trap_1_wl((short)(0x48),(long)(size))
#define	       Mfree(ptr)					       \
       (long)trap_1_wl((short)(0x49),(long)(ptr))
#define	       Mshrink(ptr,size)				       \
       (long)trap_1_wwll((short)(0x4A),(short)(0),(long)(ptr),(long)(size))
#define	       Pexec(mode,prog,tail,env)		       \
       (long)trap_1_wwlll((short)(0x4B),(short)(mode),(long)(prog),   \
			   (long)(tail),(long)(env))
#define	       Pterm(rv)					       \
       (void)trap_1_ww((short)(0x4C),(short)(rv))
#define	       Fsfirst(filespec,attr)				       \
       (long)trap_1_wlw((short)(0x4E),(long)(filespec),(short)(attr))
#define	       Fsnext()						       \
       (long)trap_1_w((short)(0x4F))
#define	       Frename(zero,old,new)				       \
       (short)trap_1_wwll((short)(0x56),(short)(zero),	       \
			  (long)(old),(long)(new))
#define	       Fdatime(timeptr,handle,rwflag)			       \
       (long)trap_1_wlww((short)(0x57),(long)(timeptr),	       \
			 (short)(handle),(short)(rwflag))
#define	       Flock(handle,mode,start,length)			       \
       (long)trap_1_wwwll((short)(0x5C),(short)(handle),       \
			  (short)(mode),(long)(start),(long)(length))
#define Nversion() \
    	(long)trap_1_w(0x60)
#define 		Frlock(handle, start, count) \
		(long)trap_1_wwll(0x62, (short)(handle), (long)(start), \
				(long)(count))
#define			Frunlock(handle, start) \
		(long)trap_1_wwl(0x63, (short)(handle), (long)(start))
#define 		F_lock(handle, count) \
		(long)trap_1_wwl(0x64, (short)(handle), (long)(count))
#define			Funlock(handle) \
		(long)trap_1_ww(0x65, (short)(handle))
#define 		Fflush(handle) \
		(long)trap_1_ww(0x66, (short)(handle))
#define 		Unlock(path) \
		(long)trap_1_wl(0x7b, (long)(path))
#define 		Lock(path) \
		(long)trap_1_wl(0x7c, (long)(path))


/*
 *     BIOS    (trap13)
 */
#define Getmpb(ptr)					       \
       (void)trap_13_wl((short)(0x00),(long)(ptr))
#define	       Bconstat(dev)					       \
       (short)trap_13_ww((short)(0x01),(short)(dev))
#define	       Bconin(dev)					       \
       (long)trap_13_ww((short)(0x02),(short)(dev))
#define	       Bconout(dev,c)					       \
       (long)trap_13_www((short)(0x03),(short)(dev),(short)((c) & 0xFF))
/* since AHDI 3.1 there is a new call to Rwabs with one more parameter */
#define	       Rwabs(rwflag,buf,n,sector,d)			\
       (long)trap_13_wwlwww((short)(0x04),(short)(rwflag),(long)(buf), \
			     (short)(n),(short)(sector),(short)(d))
#define	       Setexc(vnum,vptr) 				      \
       (void (*) (void))trap_13_wwl((short)(0x05),(short)(vnum),(long)(vptr))
#define	       Tickcal()					       \
       (long)trap_13_w((short)(0x06))
#define	       Getbpb(d)					       \
       (void *)trap_13_ww((short)(0x07),(short)(d))
#define	       Bcostat(dev)					       \
       (short)trap_13_ww((short)(0x08),(short)(dev))
#define	       Mediach(dev)					       \
       (short)trap_13_ww((short)(0x09),(short)(dev))
#define	       Drvmap()						       \
       (long)trap_13_w((short)(0x0A))
#define	       Kbshift(data)					       \
       (long)trap_13_ww((short)(0x0B),(short)(data))
#define	       Getshift()					       \
	Kbshift(-1)


/*
 *     XBIOS   (trap14)
 */

#define	       Initmous(type,param,vptr)			       \
       (void)trap_14_wwll((short)(0x00),(short)(type),	       \
			  (long)(param),(long)(vptr))
#define Ssbrk(size)					       \
       (void *)trap_14_ww((short)(0x01),(short)(size))
#define	       Physbase()					       \
       (void *)trap_14_w((short)(0x02))
#define	       Logbase()					       \
       (void *)trap_14_w((short)(0x03))
#define	       Getrez()						       \
       (short)trap_14_w((short)(0x04))
#define	       Setscreen(lscrn,pscrn,rez)			       \
       (void)trap_14_wllw((short)(0x05),(long)(lscrn),(long)(pscrn), \
			  (short)(rez))
#define	       Setpalette(palptr)				       \
       (void)trap_14_wl((short)(0x06),(long)(palptr))
#define	       Setcolor(colornum,mixture)			       \
       (short)trap_14_www((short)(0x07),(short)(colornum),(short)(mixture))
#define	       Floprd(buf,x,d,sect,trk,side,n)			       \
       (short)trap_14_wllwwwww((short)(0x08),(long)(buf),(long)(x), \
	 (short)(d),(short)(sect),(short)(trk),(short)(side),(short)(n))
#define	       Flopwr(buf,x,d,sect,trk,side,n)			       \
       (short)trap_14_wllwwwww((short)(0x09),(long)(buf),(long)(x), \
	       (short)(d),(short)(sect),(short)(trk),(short)(side),(short)(n))
#define	       Flopfmt(buf,x,d,spt,t,sd,i,m,v)		       \
       (short)trap_14_wllwwwwwlw((short)(0x0A),(long)(buf),(long)(x), \
	  (short)(d),(short)(spt),(short)(t),(short)(sd),(short)(i),  \
	  (long)(m),(short)(v))
#define	       Midiws(cnt,ptr)					       \
       (void)trap_14_wwl((short)(0x0C),(short)(cnt),(long)(ptr))
#define	       Mfpint(vnum,vptr)				       \
       (void)trap_14_wwl((short)(0x0D),(short)(vnum),(long)(vptr))
#define	       Iorec(ioDEV)					       \
       (void *)trap_14_ww((short)(0x0E),(short)(ioDEV))
#define	       Rsconf(baud,flow,uc,rs,ts,sc)			       \
       (long)trap_14_wwwwwww((short)(0x0F),(short)(baud),(short)(flow), \
			  (short)(uc),(short)(rs),(short)(ts),(short)(sc))
	/* ret old val: MSB -> ucr:8, rsr:8, tsr:8, scr:8 <- LSB */
#define	       Keytbl(nrml,shft,caps)				       \
       (void *)trap_14_wlll((short)(0x10),(long)(nrml), \
			    (long)(shft),(long)(caps))
#define	       Random()						       \
       (long)trap_14_w((short)(0x11))
#define	       Protobt(buf,serial,dsktyp,exec)			       \
       (void)trap_14_wllww((short)(0x12),(long)(buf),(long)(serial), \
			   (short)(dsktyp),(short)(exec))
#define	       Flopver(buf,x,d,sect,trk,sd,n)			       \
       (short)trap_14_wllwwwww((short)(0x13),(long)(buf),(long)(x),(short)(d),\
	       (short)(sect),(short)(trk),(short)(sd),(short)(n))
#define	       Scrdmp()						       \
       (void)trap_14_w((short)(0x14))
#define	       Cursconf(rate,attr)				       \
       (short)trap_14_www((short)(0x15),(short)(rate),(short)(attr))
#define	       Settime(time)					       \
       (void)trap_14_wl((short)(0x16),(long)(time))
#define	       Gettime()					       \
       (long)trap_14_w((short)(0x17))
#define	       Bioskeys()					       \
       (void)trap_14_w((short)(0x18))
#define	       Ikbdws(len_minus1,ptr)				       \
       (void)trap_14_wwl((short)(0x19),(short)(len_minus1),(long)(ptr))
#define	       Jdisint(vnum)					       \
       (void)trap_14_ww((short)(0x1A),(short)(vnum))
#define	       Jenabint(vnum)					       \
       (void)trap_14_ww((short)(0x1B),(short)(vnum))
#define	       Giaccess(data,reg)				       \
       (short)trap_14_www((short)(0x1C),(short)(data),(short)(reg))
#define	       Offgibit(ormask)					       \
       (void)trap_14_ww((short)(0x1D),(short)(ormask))
#define	       Ongibit(andmask)					       \
       (void)trap_14_ww((short)(0x1E),(short)(andmask))
#define	       Xbtimer(timer,ctrl,data,vptr)			       \
       (void)trap_14_wwwwl((short)(0x1F),(short)(timer),(short)(ctrl), \
			   (short)(data),(long)(vptr))
#define	       Dosound(ptr)					       \
       (void)trap_14_wl((short)(0x20),(long)(ptr))
#define	       Setprt(config)					       \
       (short)trap_14_ww((short)(0x21),(short)(config))
#define	       Kbdvbase()					       \
       (_KBDVECS*)trap_14_w((short)(0x22))
#define	       Kbrate(delay,reprate)				       \
       (short)trap_14_www((short)(0x23),(short)(delay),(short)(reprate))
#define	       Prtblk(pblkptr)					       \
       (void)trap_14_wl((short)(0x24),(long)(pblkptr)) /* obsolete ? */
#define	       Vsync()						       \
       (void)trap_14_w((short)(0x25))
#define	       Supexec(funcptr)					       \
       (long)trap_14_wl((short)(0x26),(long)(funcptr))
#define			 Puntaes() \
		 (void)trap_14_w((short)(0x27))
#define	       Floprate(drive,rate)				       \
       (short)trap_14_www((short)(0x29),(short)(drive),(short)(rate))
#define	       Blitmode(flag)					       \
       (short)trap_14_ww((short)(0x40),(short)(flag))
/*
 * Flag:
 *  -1: get config
 * !-1: set config	previous config returned
 *	bit
 *	 0	0 blit mode soft	1 blit mode hardware
 *	 1	0 no blitter		1 blitter present
 *	2..14   reserved
 *	 15	must be zero on set/returned as zero
 * blitmode (bit 0) forced to soft if no blitter(bit 1 == 0).
 */

/*
 * extensions for TT TOS
 */

#define         Mxalloc(amt,flag)					\
	(void *)trap_1_wlw((short)(0x44),(long)(amt),(short)(flag))
#define		Maddalt(start,size)					\
	(long)trap_1_wll((short)(0x14),(long)(start),(long)(size))

#define         EsetShift(mode)						\
	(short)trap_14_ww((short)(80),(short)mode)
#define         EgetShift()						\
	(short)trap_14_w((short)(81))
#define         EsetBank(bank)						\
	(short)trap_14_ww((short)(82),(short)bank)
#define         EsetColor(num,val)					\
	(short)trap_14_www((short)(83),(short)num,(short)val)
#define         EsetPalette(start,count,ptr)				\
	(void)trap_14_wwwl((short)(84),(short)start,(short)count,(long)ptr)
#define         EgetPalette(start,count,ptr)				\
	(void)trap_14_wwwl((short)(85),(short)start,(short)count,(long)ptr)
#define         EsetGray(mode)						\
	(short)trap_14_ww((short)(86),(short)mode)
#define         EsetSmear(mode)						\
	(short)trap_14_ww((short)(87),(short)mode)

#define		DMAread(sector,count,buffer,devno)			\
	(long)trap_14_wlwlw((short)0x2a,(long)sector,(short)count,(long)buffer, \
			    (short)devno)
#define		DMAwrite(sector,count,buffer,devno)			\
	(long)trap_14_wlwlw((short)0x2b,(long)sector,(short)count,(long)buffer, \
			(short)devno)
#define		Bconmap(dev)						\
	(long)trap_14_ww((short)0x2c,(short)(dev))
#define		NVMaccess(op,start,count,buf)				\
	(short)trap_14_wwwwl((short)0x2e,(short)op,(short)start,(short)count, \
			(long)buf)

/*  Wake-up call for ST BOOK -- takes date/time pair in DOS format. */

#define	       Waketime(w_date, w_time)					\
       (void)trap_14_www((short)(0x2f),(unsigned short)(w_date),	\
				       (unsigned short)(w_time))

#define CacheCtrl(a, b) trap_14_www((short)(160),(unsigned short)(a), (unsigned short)(b))
#define WdgCtrl(a) trap_14_ww((short)(161),(unsigned short)(a))
#define ExtRsConf(a, b, c) trap_14_wwwl((short)(162),(unsigned short)(a),(unsigned short)(b),(unsigned long)(c))

#endif /* __GNUC__ */


__END_DECLS


#endif /* _MINT_OSBIND_H */
