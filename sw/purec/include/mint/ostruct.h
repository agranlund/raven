
#ifndef _MINT_OSTRUCT_H
#define _MINT_OSTRUCT_H	1

#ifndef _FEATURES_H
# include <features.h>
#endif

#include <mint/basepage.h>

__BEGIN_DECLS

/*
 * General OS specific codes here
 *
 */

/*
 * GEMDOS defines and structures
 */

/* Structure used by Dfree() */
typedef struct {
    unsigned long b_free;	/* number of free clusters */
    unsigned long b_total;	/* total number of clusters */
    unsigned long b_secsiz;	/* number of bytes per sector */
    unsigned long b_clsiz;	/* number of sectors per cluster */
} _DISKINFO;

/* Structure returned by Fdatime() */
typedef struct {
  unsigned short time;
  unsigned short date;
} _DOSTIME;

/* Structure used by Cconrs */
typedef struct
{
        unsigned char maxlen;
        unsigned char actuallen;
        char    buffer[255];
} _CCONLINE;

/* Structure used by Fgetdta(), Fsetdta(), Fsfirst(), Fsnext() */
typedef struct _dta {
    char 	    dta_buf[21];	/* reserved */
    char            dta_attribute;	/* file attribute */
    unsigned short  dta_time;		/* file time stamp */
    unsigned short  dta_date;		/* file date stamp */
    long            dta_size;		/* file size */
    char            dta_name[14];	/* file name */
} _DTA;

typedef struct
{
	unsigned hour   : 5;
	unsigned minute : 6;
	unsigned second : 5;
	unsigned year   : 7;
	unsigned month  : 4;
	unsigned day    : 5;
} _DATETIME;

/****** GEMDOS *************************************************************/

/* Cconis() return values */
#define DEV_READY		-1
#define DEV_BUSY		0

/* Super() constants */
#define SUP_SET			0L
#define SUP_INQUIRE		1L
#define SUP_USER		0L
#define SUP_SUPER		-1L

/* Fopen() modes */
#define S_READ			0x00
#define S_WRITE			0x01
#define S_READWRITE		0x02
#define S_COMPAT		0x00
#define S_DENYREADWRITE	0x10
#define S_DENYWRITE		0x20
#define S_DENYREAD		0x30
#define S_DENYNONE		0x40
#define S_INHERIT		0x70

#define FO_READ         	S_READ
#define FO_WRITE        	S_WRITE
#define FO_RW           	S_READWRITE

/* Fseek() modes */
#define SEEK_SET		0
#define SEEK_CUR		1
#define SEEK_END		2

/* Fattrib() params */
#define FA_INQUIRE		0
#define FA_SET			1

/* Modes for Mxalloc() */
#define MX_STRAM		0x00
#define MX_TTRAM		0x01
#define MX_PREFSTRAM	0x02
#define MX_PREFTTRAM	0x03
/* Protection bits.  */
#define MX_MPROT		0x08			/* change protection of already allocated block */

#define MX_HEADER		0x00			/* default protection from program header */
#define MX_PRIVATE		0x10			/* only owner can read or write */
#define MX_GLOBAL		0x20 			/* anyone can read or write */
#define MX_SUPERVISOR	0x30			/* any super access OK */
#define MX_READABLE		0x40			/* any read OK, no write */

#define MX_KEEP			0x4000

/* Fforce() params */
#define GSH_CONIN		0
#define GSH_CONOUT		1
#define GSH_AUX			2
#define GSH_PRN			3
#define GSH_BIOSCON		-1
#define GSH_BIOSAUX		-2
#define GSH_BIOSPRN		-3
#define GSH_MIDIIN		-4
#define GSH_MIDIOUT		-5

/* Dgetpath() param */
#define DEFAULT_DRIVE	0

/* Pterm() modes */
#define TERM_OK			0
#define TERM_ERROR		1
#define TERM_BADPARAMS	2
#define TERM_CRASH		-1
#define TERM_CTRLC		-32

/* Return value for i.e. Crawcin() */
#define MINT_EOF		0xFF1A

/* Codes used with Fcreate()/Fsfirst() */

#define        FA_RDONLY           0x01
#define        FA_HIDDEN           0x02
#define        FA_SYSTEM           0x04
#define        FA_LABEL            0x08
#define        FA_DIR              0x10
#define        FA_CHANGED          0x20
#define        FA_SYMLINK          0x40			/* MagiC 3.0 */
/* alternative names: */
#define        FA_READONLY         FA_RDONLY
#define        FA_VOLUME           FA_LABEL
#define        FA_SUBDIR           FA_DIR
#ifndef FA_DIREC
#define        FA_DIREC            FA_DIR
#endif
#define        FA_ARCHIVE          FA_CHANGED
#ifndef FA_ARCH
#define        FA_ARCH             FA_CHANGED
#endif

/* Codes used with Pexec */

#define        PE_LOADGO           0           /* load & go */
#define        PE_LOAD             3           /* just load */
#define        PE_GO               4           /* just go */
#define        PE_CBASEPAGE        5           /* just create basepage */
/* Tos 1.4: like 4, but memory ownership changed to child, and freed
   on exit
 */
#define        PE_GO_FREE          6           /* just go, then free */

#ifdef __MINT__
#define        PE_CBASEPAGE_FLAGS  7           /* create basepage defining the program flags */
#define        PE_ASYNC_LOADGO	   100	       /* load and asynchronously go */
#define        PE_INIT             101         /* Inherits path- and file-handles (MagiC) */
#define        PE_TERM             102         /* The process will be deleted (MagiC) */
#define        PE_ASYNC_GO         104	       /* asynchronously go	     */
#define        PE_ASYNC_GO_FREE    106	       /* asynchronously go and free */
#define        PE_XBASE            107         /* As Mode 7, but in place of a command line the process name will be passed. (MagiC) */
#define        PE_EXACC            108         /* Used internally by MagiC for starting an ACC */
#define        PE_OVERLAY	       200	       /* load and overlay	     */
#define        PE_OVERLAY_GO       204         /* overlay and go */
#define        PE_OVERLAY_GO_FREE  206         /* overlay, go and free */
#endif

#define        PE_PTRACE           0x8000      /* activate tracing */

/****** KAOS 1.2 Gemdos Extension ***************************************/

/* Sconfig - Modes */

#define SC_GETCONF   0                                      /* KAOS 1.2 */
#define SC_SETCONF   1                                      /* KAOS 1.2 */
#define SC_DOSVARS   2                                      /* KAOS 1.4 */
#define SC_MOWNER    3                                      /* MagiC 3.00 */
#define SC_WBACK     4                                      /* MagiC 4.01 */
#define SC_INTMAVAIL 5                                      /* MagiC 4.01 */
#define SC_INTGARBC  6                                      /* MagiC 4.01 */

#define  SCWB_GET	 0		/* (for WBDAEMON) */	/* MagiC 4.01 */
#define  SCWB_SET    1								/* MagiC 4.01 */
#define  SCWB_RESET  2								/* MagiC 4.01 */

/* old names */
#define SC_GET SC_GETCONF
#define SC_SET SC_SETCONF
#define SC_VARS SC_DOSVARS

/* Sconfig Bits */

#define SCB_PTHCK   0x001                                   /* KAOS 1.2 */
#define SCB_DSKCH   0x002                                   /* KAOS 1.2 */
#define SCB_INSRT   SCB_DSKCH                               /* MagiC 3.0 */
#define SCB_BREAK   0x004                                   /* KAOS 1.2 */
#define SCB_NCTLC   0x008                                   /* KAOS 1.2 */
#define SCB_NFAST   0x010                                   /* KAOS 1.2 */
#define SCB_CMPTB   0x020                                   /* KAOS 1.4 */
#define SCB_NSMRT   0x040                                   /* KAOS 1.4 */
#define SCB_NGRSH   0x080                                   /* KAOS 1.4 */
#define SCB_NHALT   0x100                                   /* KAOS 1.4 */
#define SCB_RESVD   0x200                                   /* KAOS 1.4 */
#define SCB_PULLM   0x400                                   /* KAOS 1.4 */
#define SCB_FLPAR   0x800                                   /* MagiC 3.0 */

/* Sconfig(2) -> */

/* maybe also declared in GEM headers */
#ifndef _DOSVARS
#define _DOSVARS
typedef struct
{
	char		*in_dos;				/* Address of DOS flags       */
	short		*dos_time;				/* Address of DOS time        */
	short		*dos_date;				/* Address of DOS date        */
	long		dos_stack;				/* NULL since Mag!X 2.00      */
	long		pgm_superst;			/* user pgm. super stk        */
	void        **memlist;				/* address of 3 MD lists      */
	void		*act_pd;				/* Running program            */
	void		*fcbx;					/* files                      */
	short		fcbn;					/* length of fcbx[]           */
	void		*dmdx;					/* DMDs                       */
	void		*imbx;					/* Internal DOS-memory list   */
	void		(*resv_intmem)(void);	/* Extend DOS memory          */
#ifdef __NO_CDECL
	void *etv_critic;
#else
	long      __CDECL (*etv_critic)(short err);         /* etv_critic of GEMDOS      */
#endif
	char *	((*err_to_str)(signed char e));	/* Conversion code->plaintext */
	void		*xaes_appls;
	void		*mem_root;
	void		*ur_pd;
} DOSVARS;
#endif

#ifndef _AESVARS
#define _AESVARS
typedef struct
{
	long	magic;							/* Must be $87654321               */
	void	*membot;						/* End of the AES-variables        */
	void	*aes_start;						/* Start address                   */
	long	magic2;							/* Is 'MAGX'                       */
	long	date;							/* Creation date mmddyyyy          */
    void    (*chgres)(int res, int txt);    /* Change resolution               */
    long    (**shel_vector)(void);          /* Resident desktop                */
	char    *aes_bootdrv;                   /* Booting took place from here    */
	short *vdi_device;                      /* VDI-driver used by AES          */
	void	**nvdi_workstation;				/* workstation used by AES         */
	short	*shelw_doex;					/* last <doex> for APP #0          */
	short	*shelw_isgr;					/* last <isgr> for APP #0          */
	short	version;						/* e.g. $0201 is V2.1              */
	short	release;						/* 0=alpha..3=release              */
	void	*_basepage;						/* basepage of AES                 */
	short	*moff_cnt;						/* global mouse off counter        */
	unsigned long shel_buf_len;				/* length of the shell buffer      */
	void	*shel_buf;						/* pointer to shell buffer         */
	void	**notready_list;				/* waiting applications            */
	void	**menu_app;						/* application owning the menu     */
	void	**menutree;						/* active menu tree                */
	void	**desktree;						/* active desktop background       */
	short	*desk_1stob;					/*   its first object              */
	void	*dos_magic;
	void	*windowinfo;
	short	(**fsel)(char *path, char *name, short *button, const char *title);
	long (*ctrl_timeslice) (long settings);
	void	**topwind_app;					/* app. of topmost window          */
	void	**mouse_app;					/* app. owning the mouse           */
	void	**keyb_app;						/* app. owning the keyboard        */
	long	dummy;
} AESVARS;
#endif

#ifndef _MAGX_COOKIE
#define _MAGX_COOKIE
typedef struct
{
	long		config_status;
	DOSVARS	*dosvars;
	AESVARS	*aesvars;
	void		*res1;
	void		*hddrv_functions;
    long         status_bits;               /* MagiC 3 from 24.5.95 on         */
} MAGX_COOKIE;
#endif

/* Bits for <status_bits> in MAGX_COOKIE (read only!) */

#define MGXSTB_TSKMAN_ACTIVE  1    /* MagiC task manager is currently active */

/****** BIOS *************************************************************/

/*
 * BIOS defines and structures
 */

/* Device codes for Bconin(), Bconout(), Bcostat(), Bconstat() */
#define _PRT    0
#define _AUX    1
#define _CON    2
#define _MIDI   3
#define _IKBD   4
#define _RAWCON 5

#define DEV_PRINTER		_PRT
#define DEV_AUX			_AUX
#define DEV_CONSOLE		_CON
#define DEV_MIDI		_MIDI
#define DEV_IKBD		_IKBD
#define DEV_RAW			_RAWCON

/* Mode bitmask used in Rwabs() */
#define RW_READ			0
#define RW_WRITE		0
#define RW_NOMEDIACH	1
#define RW_NORETRIES	2
#define RW_NOTRANSLATE	3

/* Vector numbers used in Setexc() */
#define VEC_BUSERROR	0x02
#define VEC_ADDRESSERROR	0x03
#define VEC_ILLEGALINSTRUCTION	0x04
#define VEC_GEMDOS		0x21
#define VEC_GEM			0x22
#define VEC_BIOS		0x2d
#define VEC_XBIOS		0x2e
#define VEC_TIMER		0x100
#define VEC_CRITICALERROR	0x101
#define VEC_CRITICALERR	VEC_CRITICALERROR
#define VEC_TERMINATE	0x102
#define VEC_PROCTERM	VEC_TERMINATE
#define VEC_INQUIRE		((void(*)(void))-1)

/* Values returned by Mediach() */
#define MED_NOCHANGE	0
#define MED_UNKNOWN		1
#define MED_CHANGED		2

/* Mode bitmask for Kbshift() */
#define K_RSHIFT		0x0001
#define K_LSHIFT		0x0002
#define K_CTRL			0x0004
#define K_ALT			0x0008
#define K_CAPSLOCK		0x0010
#define K_CLRHOME		0x0020
#define K_INSERT		0x0040


/* Structure returned by Getbpb() */
typedef struct {
	unsigned short recsiz;    /* bytes per sector */
	short clsiz;        	  /* sectors per cluster */
	unsigned short clsizb;    /* bytes per cluster */
	short rdlen;              /* root directory size */
	short fsiz;               /* size of file allocation table */
	short fatrec;             /* startsector of second FAT */
	short datrec;             /* first data sector */
	unsigned short numcl;     /* total number of clusters */
	short	bflags;		/* bit 0: 0 = FAT12, 1 = FAT16
						 * bit 1: 0 = 2 FATs, 1 = 1 FAT
						 * bit 2: 0 = BPB, 1 = EXTENDED_BPB
						 */
} _BPB;

/* Structures used by Getmpb() */

/* Memory descriptor */
typedef struct _md {
    struct _md	*m_link;	/* next descriptor in the chain */
    long	 m_start;		/* starting address of block */
    long	 m_length;		/* length of the block */
    BASEPAGE	*m_own;		/* owner's process descriptor */
} _MD;

/* Memory parameter block */
typedef struct {
    _MD *mp_mfl;		/* free memory chunks */
    _MD *mp_mal;		/* used memory descriptors */
    _MD *mp_rover;		/* rover memory descriptor */
} _MPB;

/*
	buffer control block
*/
#ifndef __bcb_defined
#define __bcb_defined 1
typedef struct _bcb {
   struct _bcb *b_link;
   short       b_bufdrv;
   short       b_buftyp;
   short       b_bufrec;
   short       b_dirty;
   char       *b_dmd;      /* actually pointer to DMD */
   char       *b_bufr;
} _BCB;
#endif

/****** XBIOS *************************************************************/

/*
 * XBIOS defines and structures
 */

/* Codes used with Cursconf() */
#define CURS_HIDE   	0
#define CURS_SHOW   	1
#define CURS_BLINK  	2
#define CURS_NOBLINK    3
#define CURS_SETRATE    4
#define CURS_GETRATE    5

/* Modes for Initmous() */
#define IM_DISABLE		0
#define IM_RELATIVE		1
#define IM_ABSOLUTE		2
#define IM_KEYCODE		4

#define IM_YBOT			0
#define IM_YTOP			1
#define IM_PACKETS		2
#define IM_KEYS			3

/* VsetScreen() modes */
#define SCR_NOCHANGE	-1
#define SCR_MODECODE	3

#define COL_INQUIRE		-1

/* Floprd() devices */
#define FLOP_DRIVEA		0
#define FLOP_DRIVEB		1

/* Flopfmt() params */
#define FLOP_NOSKEW		 1
#define FLOP_SKEW		-1

#define FLOP_MAGIC		0x8754321
#define FLOP_VIRGIN		0xe5e5

#define FLOPPY_DSDD		0
#define FLOPPY_DSHD		1
#define FLOPPY_DSED		2

/* Dbmsg() messages */
#define DB_NULLSTRING	0xf000
#define DB_COMMAND		0xf100

/* Mfpint() vector indices */
#define MFP_PARALLEL	0
#define	MFP_DCD			1
#define MFP_CTS			2
#define MFP_BITBLT		3
#define MFP_TIMERD		4
#define MFP_BAUDRATE	MFP_TIMERD
#define MFP_200HZ		5
#define MFP_ACIA		6
#define MFP_DISK		7
#define MFP_TIMERB		8
#define MFP_HBLANK		MFP_TIMERB
#define MFP_TERR		9
#define MFP_TBE			10
#define MFP_RERR		11
#define MFP_RBF			12
#define MFP_TIMERA		13
#define MFP_DMASOUND	MFP_TIMERA
#define MFP_RING		14
#define MFP_MONODETECT	15

/* Iorec() devices */
#define IO_SERIAL		0
#define IO_KEYBOARD		1
#define IO_MIDI			2

/* Rsconf() speeds */
#define BAUD_19200		0
#define BAUD_9600		1
#define BAUD_4800		2
#define BAUD_3600		3
#define BAUD_2400		4
#define BAUD_2000		5
#define BAUD_1800		6
#define BAUD_1200		7
#define BAUD_600		8
#define BAUD_300		9
#define BAUD_200		10
#define BAUD_150		11
#define BAUD_134		12
#define BAUD_110		13
#define BAUD_75			14
#define BAUD_50			15
#define BAUD_INQUIRE	-2

/* Rsconf() params */
#define FLOW_NONE		0
#define FLOW_SOFT		1
#define FLOW_HARD		2
#define FLOW_BOTH		3

#define RS_RECVENABLE	0x01
#define RS_SYNCSTRIP	0x02
#define RS_MATCHBUSY	0x04
#define RS_BRKDETECT	0x08
#define RS_FRAMEERR		0x10
#define RS_PARITYERR	0x20
#define RS_OVERRUNERR	0x40
#define RS_BUFFUL		0x80

#define RS_ODDPARITY	0x02
#define RS_EVENPARITY	0x00
#define RS_PARITYENABLE	0x04

#define RS_NOSTOP		0x00
#define RS_1STOP		0x08
#define RS_15STOP		0x10
#define RS_2STOP		0x18

#define RS_8BITS		0x00
#define RS_7BITS		0x20
#define RS_6BITS		0x40
#define RS_5BITS		0x60

#define RS_CLK16		0x80

#define RS_INQUIRE		-1
#define RS_LASTBAUD		2

/* Keytbl() param */
#define KT_NOCHANGE		((char*)-1)

/* Protobt() params */
#define SERIAL_NOCHANGE	-1
#define SERIAL_RANDOM	0x01000001

#define DISK_NOCHANGE	-1
#define DISK_SSSD		0
#define DISK_DSSD		1
#define DISK_SSDD		2
#define DISK_DSDD		3
#define DISK_DSHD		4
#define DISK_DSED		5

#define EXEC_NOCHANGE	-1
#define EXEC_NO			0
#define EXEC_YES		1

/* Giaccess() registers */
#define PSG_APITCHLOW	0
#define PSG_APITCHHIGH	1
#define PSG_BPITCHLOW	2
#define PSG_BPTICHHIGH	3
#define PSG_CPITCHLOW	4
#define PSG_CPITCHHIGH	5
#define PSG_NOISEPITCH	6
#define PSG_MODE		7
#define PSG_AVOLUME		8
#define PSG_BVOLUME		9
#define PSG_CVOLUME		10
#define PSG_FREQLOW		11
#define PSG_FREQHIGH	12
#define PSG_ENVELOPE	13
#define PSG_PORTA		14
#define PSG_PORTB		15

#define PSG_ENABLEA		0x01
#define PSG_ENABLEB		0x02
#define PSG_ENABLEC		0x04
#define PSG_NOISEA		0x08
#define PSG_NOISEB		0x10
#define PSG_NOISEC		0x20
#define PSG_PRTAOUT		0x40
#define PSG_PRTBOUT		0x80

/* Bitmasks for Offgibit() */
#define GI_FLOPPYSIDE	0x01
#define GI_FLOPPYA		0x02
#define GI_FLOPPYB		0x04
#define GI_RTS			0x08
#define GI_DTR			0x10
#define GI_STROBE		0x20
#define GI_GPO			0x40
#define GI_SCCPORT		0x80

/* Xbtimer() values */
#define XB_TIMERA		0
#define XB_TIMERB		1
#define XB_TIMERC		2
#define XB_TIMERD		3

/* Dosound() param */
#define DS_INQUIRE		-1

/* Setprt() modes */
#define PRT_DOTMATRIX	0x01
#define PRT_MONO		0x02
#define PRT_ATARI		0x04
#define PRT_DRAFT		0x08
#define PRT_PARALLEL	0x10
#define PRT_CONTINUOUS	0x20

#define PRT_DAISY		0x01
#define PRT_COLOR		0x02
#define PRT_EPSON		0x04
#define PRT_FINAL		0x08
#define PRT_SERIAL		0x10
#define PRT_SINGLE		0x20

#define PRT_INQUIRE		-1

/* Kbrate() param */
#define	KB_INQUIRE		-1

/* Floprate() seek rates */
#define FRATE_6			0
#define FRATE_12		1
#define FRATE_2			2
#define FRATE_3			3
#define	FRATE_INQUIRE	-1

/* Bconmap() params */
#define BMAP_CHECK		0
#define BMAP_INQUIRE	-1
#define BMAP_MAPTAB		-2

/* NVMaccess params */
#define NVM_READ		0
#define NVM_WRITE		1
#define NVM_RESET		2

/* Blitmode() modes */
#define BLIT_SOFT		0
#define BLIT_HARD		1

/* EsetShift() modes */
#define ST_LOW			0x0000
#define ST_MED			0x0100
#define ST_HIGH			0x0200
#define TT_MED			0x0400
#define TT_HIGH			0x0600
#define TT_LOW			0x0700

#define ES_GRAY			12
#define ES_SMEAR		15

/* Esetbank() params */
#define ESB_INQUIRE		-1
#define EC_INQUIRE		-1

/* EsetGray() modes */
#define ESG_INQUIRE		-1
#define ESG_COLOR		0
#define	ESG_GRAY		1

/* EsetSmear() modes */
#define ESM_INQUIRE		-1
#define ESM_NORMAL		0
#define	ESM_SMEAR		1

/* Structure returned by Iorec() */
typedef struct {
    char    *ibuf;
    short   ibufsiz;
    volatile short   ibufhd;
    volatile short   ibuftl;
    short   ibuflow;
    short   ibufhi;
} _IOREC;

/* Structure used by Initmouse() */
typedef struct {
	char	topmode;
	char	buttons;
	char	xparam;
	char	yparam;
	short	xmax;
	short	ymax;
	short	xstart;
	short	ystart;
} _PARAM;

/*
 * Structure returned by Kbdvbase()
 * no need to declare the functions cdecl,
 * since the arg is passed in a0 AND on stack
 */
typedef struct {
    void    (*midivec)	(void);
    void    (*vkbderr)	(void);
    void    (*vmiderr)	(void);
    void    (*statvec)	(void *);
    void    (*mousevec)	(void *);
    void    (*clockvec)	(void *);
    void    (*joyvec)	(void *);
    long    (*midisys)	(void);
    long    (*ikbdsys)	(void);
    char    kbstate;
} _KBDVECS;

/* Structure returned by Keytbl() */
typedef struct {
    char *unshift;	/* pointer to unshifted keys */
    char *shift;	/* pointer to shifted keys */
    char *caps;		/* pointer to capslock keys */

    /* Entries below available
     * when _AKP cookie is present.
     */
    char *alt;		/* pointers to alt translation tables */
    char *altshift;
    char *altcaps;

   /* Entry below is available
    * on MilanTOS and as of FreeMiNT 1.16.1
    */
    char *altgr;

} _KEYTAB;

/* Structure used by Prtblk() */
typedef struct
{
        void    *pb_scrptr;
        short   pb_offset;
        short   pb_width;
        short   pb_height;
        short   pb_left;
        short   pb_right;
        short   pb_screz;
        short   pb_prrez;
        void    *pb_colptr;
        short   pb_prtype;
        short   pb_prport;
        void    *pb_mask;
} _PBDEF;

/* Available from MetaDOS version 2.30 */
typedef struct
{
	unsigned short mi_version;
	long mi_magic;
	const char *mi_log2phys;
} META_INFO_2;

/* Structure used by Metainit() */
typedef struct
{
	unsigned long mi_drivemap;
	const char *mi_version_string;
	long reserved;

	META_INFO_2 *mi_info;	/* Available from MetaDOS version 2.30 */
} META_INFO_1;

#ifndef METAINFO
#define METAINFO META_INFO_1
#endif

/* Structure used by VgetRGB*/
typedef struct
{
	char reserved;
	char red;
	char green;
	char blue;
} _RGB;

/* Structure used by Bconmap() */
typedef struct
{
	short _CDECL (*Bconstat)(short dev);
	long _CDECL (*Bconin)(short dev);
	long _CDECL (*Bcostat)(short dev);
	void _CDECL (*Bconout)(short, short);
	unsigned long _CDECL (*Rsconf)(short, short, short, short, short, short);
	_IOREC *iorec;
} _MAPTAB;

/* Structure used by Bconmap() */
typedef struct
{
	_MAPTAB *maptab;
	short maptabsize;
} _BCONMAP;

/* Structure used by Settime */
typedef struct
{
	unsigned year:7;
	unsigned month:4;
	unsigned day:5;
	unsigned hour:5;
	unsigned minute:6;
	unsigned second:5;
} _BIOSTIME;


__END_DECLS


#endif /* _MINT_OSTRUCT_H */
