/************************************************************************/
/*	OSBINDS.H	#defines for GEMDOS,BIOS & XBIOS binding	*/
/* 		started 5/2/85 .. Rob Zdybel				*/
/*		Copyright 1985 Atari Corp.				*/
/************************************************************************/
#ifndef DL_OSBIND
#define DL_OSBIND

extern	long	bios();
extern	long	xbios();
extern	long	gemdos();


/*
     These are the data structures that are used by some of the
	 BIOS functions.  rpt
*/
typedef struct {
	int (*midivec)();   /* MIDI-input */
	int (*vkbderr)();   /* keyboard error */
	int (*vmiderr)();   /* MIDI error */
	int (*statvec)();   /* ikbd status packet */
	int (*mousevec)();  /* mouse packet */
	int (*clockvec)();  /* clock packet */
	int (*joyvec)();    /* joystick packet */
	int (*midisys)();   /* system MIDI vector */
	int (*ikbdsys)();   /* system IKBD vector */
} kbdvecs;

/*
	Used in function Iorec()
*/
typedef struct {
	char *ibuf; /* pointer to queue */
	int ibufsiz;    /* size of queue in bytes */
	int ibufhd; /* head index of queue */
	int ibuftl; /* tail index of queue */
	int ibuflow;    /* low water mark */
	int ibufhigh;   /* high water mark */
} iorec;


/*
	Used by function Dfree().
*/
typedef struct {
	long b_free;    /* no. of free clusters on drive */
	long b_total;   /* total no. of clusters on drive */
	long b_secsiz;  /* no. of bytes in a sector */
	long b_clsiz;   /* no. of sectors in a cluster */
} disk_info;


/*
	Used by function Getmpb().
*/
typedef struct md {
	struct md *m_link;   /* next memory block       */
	long       m_start;  /* start address of block  */
	long       m_length; /* No. of bytes in block   */
	long       m_own;    /* Memory block's owner ID */
} md;

typedef struct {
	md *mp_mfl;      /* memory free list        */
	md *mp_mal;      /* memory allocated list   */
	md *mp_rover;    /* roving pointer  (woof!) */
} mpb;

/*
	Used by function Getbpb().
*/
typedef struct _bpb {
	int sector_size_bytes;
	int cl_sectors;
	int cl_bytes;
	int dir_length_sectors;
	int FAT_size_sectors;
	int FAT_sector;				/*  sector number of the second FAT. 		*/
	int data_sector;			/*  sector number of the first data cluster */
	int total_data_clusters;	/*	number of data clusters on the disk     */
	int flags;					/*  Miscellaneous Flags.  					*/
} bpb;


/*
	This structure is a bit field that represents the different components of
	the date and time words.   A union structure was used so that a long 
	could be used for the assignment from the gettime() function and the
	bit-field structure could be used to easily decode the long word.

	Note:  This data structure was designed to work with Megamax C.  Not all
			compilers allocate bit-fields in the same manner.  rpt
*/
typedef union {
	struct {
		unsigned day	 : 5;
		unsigned month	 : 4;
		unsigned year	 : 7;
		unsigned seconds : 5;
		unsigned minutes : 6;
		unsigned hours	 : 5;
	} part;
	long realtime;
} datetime;


typedef union {
	struct {
		unsigned day	: 5;
		unsigned month	: 4;
		unsigned year	: 7;
	} part;
	unsigned realdate;
} dateinfo;

typedef union {
	struct {
		unsigned seconds : 5;
		unsigned minutes : 6;
		unsigned hours	 : 5;
	} part;
	unsigned realtime;
} timeinfo;


/*	BIOS	(trap13)	*/
#define Getmpb(a) bios(0, a)
#define	Bconstat(a)	(int)bios(1,a)
#define	Bconin(a)	bios(2,a)
#define	Bconout(a,b)	bios(3,a,b)
#define	Rwabs(a,b,c,d,e)	bios(4,a,b,c,d,e)
#define	Setexc(a,b)	bios(5,a,b)
#define Tickcal()	bios(6)
#define Getbpb(a)	(bpb *)bios(7, a)
#define	Bcostat(a)	bios(8,a)
#define	Mediach(a)	bios(9,a)
#define	Drvmap()	bios(10)
#define	Kbshift(a)	bios(11,a)

/*	XBIOS	(trap14)	*/
#define	Initmous(a,b,c)	xbios(0,a,b,c)
#define	Physbase()	xbios(2)
#define	Logbase()	xbios(3)
#define	Getrez()	(int)xbios(4)
#define	Setscreen(a,b,c)	xbios(5,a,b,c)
#define	Setpalette(a)	xbios(6,a)
#define	Setcolor(a,b)	(int)xbios(7,a,b)
#define	Floprd(a,b,c,d,e,f,g)	(int)xbios(8,a,b,c,d,e,f,g)
#define	Flopwr(a,b,c,d,e,f,g)	(int)xbios(9,a,b,c,d,e,f,g)
#define	Flopfmt(a,b,c,d,e,f,g,h,i)	(int)xbios(10,a,b,c,d,e,f,g,h,i)
#define	Midiws(a,b)	xbios(12,a,b)
#define	Mfpint(a,b)	xbios(13,a,b)
#define	Iorec(a)	(iorec *)xbios(14,a)
#define	Rsconf(a,b,c,d,e,f)	xbios(15,a,b,c,d,e,f)
#define	Keytbl(a,b,c)	xbios(16,a,b,c)
#define	Random()	xbios(17)
#define	Protobt(a,b,c,d)	xbios(18,a,b,c,d)
#define	Flopver(a,b,c,d,e,f,g)	(int)xbios(19,a,b,c,d,e,f,g)
#define	Scrdmp()	xbios(20)	/* WARNING: This Bind Incomplete */
#define	Cursconf(a,b)	(int)xbios(21,a,b)
#define	Settime(a)	xbios(22,a)
#define	Gettime()	xbios(23)
#define	Bioskeys()	xbios(24)
#define	Ikbdws(a,b)	xbios(25,a,b)
#define	Jdisint(a)	xbios(26,a)
#define	Jenabint(a)	xbios(27,a)
#define	Giaccess(a,b)	(char)xbios(28,a,b)
#define	Offgibit(a)	xbios(29,a)
#define	Ongibit(a)	xbios(30,a)
#define	Xbtimer(a,b,c,d)	xbios(31,a,b,c,d)
#define	Dosound(a)	xbios(32,a)
#define	Setprt(a)	(int) xbios(33,a)
#define	Kbdvbase()	(kbdvecs *)xbios(34)
#define	Kbrate(a,b)	(int)xbios(35,a,b)
#define	Prtblk()	xbios(36)
#define Vsync()		xbios(37)
#define Supexec(a)	xbios(38, a)
#define Puntaes()	xbios(39)

/*	GEMDOS	(trap1)		*/
#define	Pterm0()	gemdos(0x0)
#define	Cconin()	(int)gemdos(0x1)
#define	Cconout(a)	gemdos(0x2,a)
#define	Cauxin()	(int)gemdos(0x3)
#define	Cauxout(a)	gemdos(0x4,a)
#define	Cprnout(a)	gemdos(0x5,a)
#define	Crawio(a)	gemdos(0x6,a)
#define	Crawcin()	gemdos(0x7)
#define	Cnecin()	gemdos(0x8)
#define	Cconws(a)	gemdos(0x9,a)
#define	Cconrs(a)	gemdos(0x0a,a)
#define	Cconis()	(int)gemdos(0x0b)
#define	Dsetdrv(a)	gemdos(0x0e,a)
#define	Cconos()	(int)gemdos(0x10)
#define	Cprnos()	(int)gemdos(0x11)
#define	Cauxis()	(int)gemdos(0x12)
#define	Cauxos()	(int)gemdos(0x13)
#define	Dgetdrv()	(int)gemdos(0x19)
#define	Fsetdta(a)	gemdos(0x1a,a)
#define	Super(a)	gemdos(0x20,a)		/* NOTE:This name may change */
#define	Tgetdate()	(int)gemdos(0x2a)
#define	Tsetdate(a)	gemdos(0x2b,a)
#define	Tgettime()	(int)gemdos(0x2c)
#define	Tsettime(a)	gemdos(0x2d,a)
#define	Fgetdta()	gemdos(0x2f)
#define	Sversion()	(int)gemdos(0x30)
#define	Ptermres(a,b)	gemdos(0x31,a,b)
#define	Dfree(a,b)	gemdos(0x36,a,b)
#define	Dcreate(a)	(int)gemdos(0x39,a)
#define	Ddelete(a)	(int)gemdos(0x3a,a)
#define	Dsetpath(a)	(int)gemdos(0x3b,a)
#define	Fcreate(a,b)	(int)gemdos(0x3c,a,b)
#define	Fopen(a,b)	(int)gemdos(0x3d,a,b)
#define	Fclose(a)	gemdos(0x3e,a)
#define	Fread(a,b,c)	gemdos(0x3f,a,b,c)
#define	Fwrite(a,b,c)	gemdos(0x40,a,b,c)
#define	Fdelete(a)	(int)gemdos(0x41,a)
#define	Fseek(a,b,c)	gemdos(0x42,a,b,c)
#define	Fattrib(a,b,c)	(int)gemdos(0x43,a,b,c)
#define	Fdup(a)		(int)gemdos(0x45,a)
#define	Fforce(a,b)	(int)gemdos(0x46,a,b)
#define	Dgetpath(a,b)	(int)gemdos(0x47,a,b)
#define	Malloc(a)	gemdos(0x48,a)
#define	Mfree(a)	(int)gemdos(0x49,a)
#define	Mshrink(a,b)	(int)gemdos(0x4a,0,a,b)	/* NOTE:Null parameter added */
#define	Pexec(a,b,c,d)	gemdos(0x4b,a,b,c,d)
#define	Pterm(a)	gemdos(0x4c,a)
#define	Fsfirst(a,b)	(int)gemdos(0x4e,a,b)
#define	Fsnext()	(int)gemdos(0x4f)
#define	Frename(a,b,c)	(int)gemdos(0x56,a,b,c)
#define	Fdatime(a,b,c)	(int)gemdos(0x57,a,b,c)

#endif
