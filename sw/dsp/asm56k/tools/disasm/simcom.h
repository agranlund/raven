#ifndef	__dsp_simcom_h__
#define __dsp_simcom_h__ 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>

#define DSP_LINE_LEN    256	/* default line length */
#ifndef ADSx
#define ADSx		0	/* ADS */
#endif
#ifndef CURSES
#define CURSES		0	/* CURSES */
#endif
#ifndef VAXC
#define VAXC		0	/* VAX C compiler */
#endif
#ifndef PC9801
#define PC9801 0		/* nec 9801 */
#endif
#ifndef SUN4
#define SUN4 0
#endif
#ifndef NEXT
#define NEXT 0
#endif
#if PC9801
#ifndef MSDOS
#define MSDOS 1
#endif
#endif
#ifndef MSDOS
#define MSDOS  0		/* MSDOS operating system */
#endif
#ifndef ATT
#define ATT    0		/* AT&T Unix */
#endif
#ifndef BSD
#define BSD    0		/* BSD Unix */
#endif
#ifndef UNIX
#define UNIX   (ATT || BSD)	/* generic Unix operating system */
#endif
#ifndef VMS
#define VMS    0		/* VMS operating system */
#endif
#ifndef IBMC
#define IBMC   0		/* for CMS IBMC compiler */
#endif
#ifndef FULLSIM
#define FULLSIM 1		/* Full window version of simulator as default */
#endif
#ifndef macintosh
#define macintosh  0		/* MAC II operating system */
#endif
#ifndef APLC
#define APLC 0			/* apollo c */
#endif
#ifndef BLM
#define BLM 0			/* quicksim Behavioral Level Model */
#endif
#ifndef SYS5
#define SYS5 0
#endif

#if MSDOS
#ifndef BSD
#define BSD 0
#endif

#ifndef CURSES
#define CURSES UNIX		/* Curses screen management funcions */
#endif

#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#define BIG_ENDIAN 0
#ifdef LABWIN
#include <ansi_c.h>
#else
#include <dos.h>
#endif
#ifndef __GNUC__
#ifndef LABWIN
#include <stdlib.h>
#include <process.h>
#include <direct.h>
#endif
#endif
#ifdef LABWIN
#include <lowlvlio.h>
#else
#include <io.h>
#endif
#endif /* MSDOS */

#if UNIX
extern int errno;
extern char *sys_errlist[];

#if !APLC
#include <memory.h>
#endif
#endif

#if IBMC
#include <stddef.h>
#endif

#if macintosh
#include <StdIO.h>
#include <CType.h>
#include <Math.h>
void *malloc ();

#if FULLSIM
#include <ErrNo.h>
#include <ctype.h>
#endif
#else

#if SUN4
#include <sgtty.h>
extern int stty(int fd, struct sgttyb *buf);
#include <curses.h>
#include <sys/types.h>
extern int _filbuf(FILE *);
extern int fscanf(FILE *, const char *, ...);
extern int fprintf(FILE *, const char *, ...);
extern void rewind(FILE *);
extern int fseek(FILE *, long, int);
extern int fflush(FILE *);
extern int sscanf(const char *, const char *, ...);
extern int fclose(FILE *);
extern int tolower(int);
extern int fgetc(FILE *);
extern long int strtol(const char *, char **, int);
extern int fwrite(const void *, int, int, FILE *);
extern time_t time(time_t *tloc);
extern int endwin(void);
extern int wmove(WINDOW *win, int y, int x);
extern int wstandout(WINDOW *win);
extern int wstandend(WINDOW *win);
extern int wgetch(WINDOW *win);
extern int wclear(WINDOW *win);
extern int wrefresh(WINDOW *win);
extern int touchwin(WINDOW *win);
extern int waddstr(WINDOW *win, char *str);
extern int fread(void *, int, int, FILE *);
extern int fputs(const char *, FILE *);
extern int printf(const char *, ...);
extern int system(const char *string);
extern int rename(char *, char *);
extern int ioctl(int, int, caddr_t);
extern void perror(char *);
extern int setvbuf(FILE *, char *, int , int);
extern int _flsbuf (unsigned int, FILE *);
#endif

#if CURSES
#include <curses.h>
#endif

#include <stddef.h>
#if UNIX
#include <unistd.h>
#endif
#if FULLSIM
#include <errno.h>
#endif
#endif
#if BLM
#include "blmredef.h"
#endif
#include "coreaddr.h"
#include "maout.h"

union sev_dubl
{
    double df;
#if BIG_ENDIAN
    struct
    {
	unsigned long hi,
	 lo;
    }
    dul;
#else
    struct
    {
	unsigned long lo,
	 hi;
    }
    dul;			/* integer from FPT */
#endif
};

struct sev_evres
{				/* evaluation result */
    union sev_dubl uval;
    unsigned long xval[3];
    CORE_ADDR addr;
    long attr;			/* word size and type of evaluation result INT, FPT, FRC */
    unsigned short g1,
     g2,
     g3,
     g4;
};

struct sev_obj
{
    int count;			/* word count (1 or 2 or 3) */
    unsigned long opcode;	/* base opcode */
    unsigned long postword;	/* post word */
    unsigned long thirdword;	/* post word */
    char *errmsg;
    int total_count;            /* word count for multi instructions */
    unsigned long opcodes[10];  /* opcodes for multi instructions */
};

#define DSP_MFP 1		/* flag bits used in memflag below in sdb_dasm */
#define DSP_MFX 2
#define DSP_MFY 4
#define DSP_MFL 8
#define DSP_MFY2 0x10
#define DSP_MFX2 0x20
#define DSP_MFPX 0x40
#define DSP_MFPX2 0x80

/* this is state information required by SC100 disassembler */
struct asm_state
{
    unsigned long prefix_low,prefix_high;
    int dalu_position;
    int move_position;
};

struct sdb_dasm
{				/* symbolic debug info returned by disassembers */
    struct asm_state asm_state;
    unsigned long pc;		/* current pc and r,n,m registers sent to disassembler */
    unsigned long rn[8];
    unsigned long nn[8];
    unsigned long mn[8];
    int numwords;		/* return number of words from disassembler */
    int memflag;		/* return flags for p,x,y addresses below */
    unsigned long paddr;
    unsigned long xaddr;
    unsigned long yaddr;
    unsigned long sp;           /* user stack pointer register */
};

/* size of target device word - this goes in the device attributes */
#define DSP_WRD16 0X10000000L	/* should be the same as the SIZEN attributes in simdev.h */
#define DSP_WRD24 0X04000000L
#define DSP_WRD32 0X02000000L
#define DSP_WRD36 0X01000000L
#define DSP_WRD16_36 0X11000000L   /* 16 bit word with 36 bit accumulator */
#define DSP_SIZEN 0x16100000L	/* the above word size bits */
#define DSP_ADDR16 0X8L
#define DSP_ADDR32 0X10L
#define DSP_IEEE  0x80L		/* ieee floating point for 96k */
#define DSP_ICACHE 0x100L	/* icache version */
#define DSP_ADDR24 0x200L
#define DSP_ADDR19 0X400L
#define DSP_SBM 0x800L          /* sixteen bit address mode of 56301 */
#define DSP_SA  0x1000L         /* sixteen bit arithmetic mode of 56301 */
/* flag bits int the sev_evres attr field */

#define DSP_WRDN 0X7L
#define DSP_WRD1 0X1L		/* Number of words involved in integer result */
#define DSP_WRD2 0X2L
#define DSP_WRD3 0X4L
#define DSP_FORCEL 0X10L	/* FORCE LONG */
#define DSP_FORCES 0X20L	/* FORCE SHORT */
#define DSP_FORCEI 0X40L	/* FORCE IO */
#define DSP_INT 0X100L		/* Type of result: int,float,fraction,unsigned int */
#define DSP_FPT 0X200L
#define DSP_FRC 0X400L
#define DSP_UINT 0X800L
#define DSP_UFRAC 0X1000L
#define DSP_LOG 0X2000L		/* logical type */
#define DSP_ADD 0x4000L		/* address type */
#define DSP_EVIO 0X8000L	/* Special value flag for I/O input files */
#define DSP_REGVAL 0x10000L     /* value is from a register lookup */

#define DSP_ERR	(-1)
#define DSP_YES	(1)
#define DSP_NO	   (0)

#if MSDOS||IBMC
#define DSP_WRITEBIN "w+b"
#define DSP_PATHSEP '\\'
#define DSP_PSSTRING "\\"
#endif
#if UNIX||VMS
#define DSP_WRITEBIN "w+"
#define DSP_PATHSEP '/'
#define DSP_PSSTRING "/"
#define DSP_COLS_MAX	256
#define DSP_LINES_MAX	100
#else
#if WIN32
#define DSP_COLS_MAX	256
#define DSP_LINES_MAX	100
#else
#define DSP_COLS_MAX	80
#define DSP_LINES_MAX	24
#endif
#endif
#if macintosh
#define DSP_WRITEBIN "w+"
#define DSP_PATHSEP ':'
#define DSP_PSSTRING ":"
#endif

#define DSP_GUI_COLS_MAX    256
#define DSP_GUI_LINES_MAX   100

/* OPCODE FOR LONG JUMP */
#define	DSP_LONGJUMP_56K    0xaf080l
#define	DSP_LONGJUMP_56100  0x134l
#define	DSP_LONGJUMP_56800  0xe984l
#define	DSP_LONGJUMP_96K    0x30c3f80l

#define DSP_OK      0
#define DSP_ERROR (-1)

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define DSP_HILITE    1
#define DSP_NO_HILITE 0

#endif /* __dsp_simcom_h__ */
