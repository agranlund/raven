/* definitions for various GEMDOS system variables in low memory */
/* WARNING: this file is not compatible with the old one */

/*
 * Convention:
 * 	the address for each variable is defined.
 */
#ifndef _SYSVARS_H
# define _SYSVARS_H 1

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS


/* Processor state and post mortem dump area */

#define PROC_LIVES_MAGIC 0x12345678L	/* proc_lives if dump is valid */
#define proc_lives	((unsigned long *) 0x380L)
struct __post_mortem_dump {
    unsigned long	d0, d1, d2, d3, d4, d5, d6, d7;
    void   		*a0, *a1, *a2, *a3, *a4, *a5, *a6, *a7; /* a7 == ssp */
    void		*pc;	  /* first byte is exception #   */
    void		*usp;
    unsigned short	stk[16];  /* sixteen word of super stack */
};
#define proc_post_mortem_dump_p	((struct __post_mortem_dump *) 0x384L)
#define proc_dregs	(&proc_post_mortem_dump_p->d0)
#define proc_aregs	(&proc_post_mortem_dump_p->a0)
#define proc_pc		(proc_post_mortem_dump_p->pc)
#define proc_enum	((unsigned char *)(&proc_post_mortem_dump->pc))
#define proc_usp	(proc_post_mortem_dump_p->usp)
#define proc_utk	(proc_post_mortem_dump_p->stk)


/* timer, crit error and process termination handoff vectors */
#define etv_timer	((void (**)(void)) 0x400L)
#define etv_critic	((void (**)(void)) 0x404L)
#define etv_term	((void (**)(void)) 0x408L)
#define etv_xtra	((void (**)(void)) 0x40CL)

/* memory controller */
#define MEMVALID_MAGIC	0x752019F3L	/* once memory is sized */
#define memvalid	((unsigned long *) 0x420L)
#define mencntlr	((unsigned char *) 0x424L)
 			/* 0 = 128K, 4 = 512K 0 = 256k(2banks) 5 = 1M */

/* reset vector, jump through resvector if resvalid on reset */
#define RESVALID_MAGIC	0x31415926L
#define resvalid	((unsigned long *) 0x426L)
#define resvector	((void (**)(void)) 0x42aL)
			/* do a jmp 0x24L(a6) at end to go to system reset */

/* mem */
#define phystop		((unsigned long *) 0x42eL) /* physical top of st ram */
#define _membot		((unsigned long *) 0x432L) /* bottom of avail     */
#define _memtop		((unsigned long *) 0x436L) /* top    of avail     */
#define MEMVAL2_MAGIC	0x237698AAL /* after suc. coldstart && memvalid     */
#define memval2		((unsigned long *) 0x43aL)
#define ramtop		((unsigned long *) 0x5a4L) /* physical top of tt ram */

/* floppy */
#define flock		((volatile short *) 0x43eL) /* lock usage of DMA   chip */
#define seekrate	((short *) 0x440L) /* 0=6ms 1=12ms 2=2ms 3=3ms */
#define _timr_ms	((short *) 0x442L) /* timer calib == 20ms      */
#define _fverify	((short *) 0x444L) /* write verify flag        */
#define _bootdev	((short *) 0x446L)

/* video */
#define palmode		((short *) 0x448L) /* PAL video mode flag         */
#define defshiftmd	((unsigned char *) 0x44aL) /* default video rez   */
#define sshiftmd	((short *) 0x44cL) /* shadow of hdwr. shiftmd reg */
 					   /* 0=Lo 1=med 2=Hi rez         */
#define _v_bas_ad	((volatile void **) 0x44eL)  /* screen mem base             */
#define vblsem		((volatile short *) 0x452L) /* vbl semaphore               */
#define nvbls		((short *) 0x454L) /* # of vbl entries def. == 8  */
#define _vblqueue	((void (***)(void)) 0x456L) /* vbl queue pointer      */
#define colorptr	((volatile short **) 0x45aL) /* pal. on next vblank if!NULL */
#define screenptr	((volatile unsigned long *) 0x45eL) /* screen mem on next vblank if !NULL */
#define _vbclock	((volatile unsigned long *) 0x462L) /* vbi counter         */
#define _frclock	((volatile unsigned long *) 0x466L) /* #vbi not vblsem'ed  */

#define hdv_init	((void (**)(void)) 0x46aL)	/* hard disk init */
#define swv_vec		((void (**)(void)) 0x46eL)	/* resolution change */
#define hdv_bpb		((_BPB (**)(short)) 0x472L)	/* get BPB for harddisk */
#define hdv_rw		((void **) 0x476L)			/* read/write for harddisk */
#define hdv_boot	((void (**)(void)) 0x47aL)	/* hard disk boot */
#define hdv_mediach	((long (**)(short)) 0x47eL)	/* hard disk media change */


#define _cmd_load	((short *)0x482L)
#define conterm		((char *) 0x484L)
#define trp14ret	((unsigned long *)0x486L)
#define criticret	((unsigned long *)0x48AL)
#define themd		((_MD *) 0x48eL)
#define themdmd		((_MD **) 0x49EL)
#define savptr		((long *) 0x4A2L)
#define _nflops		((short *) 0x4A6L)
#define con_state	((long *) 0x4a8L)
#define sav_row		((short *) 0x4acL)
#define sav_context	((long *) 0x4aeL)
#define _bufl		((long *) 0x4b2L) /* 2 longs */
#define _hz_200		((volatile unsigned long *) 0x4baL)
#define the_env		((unsigned long *) 0x4beL)
#define _drvbits	((unsigned long *) 0x4c2L)
#define _dskbufp	((unsigned long *) 0x4c6L)
#define _autopath	((unsigned long *) 0x4caL)
#define _vbl_list	((unsigned long *) 0x4ceL) /* 8 longs */
#define _prt_cnt	((volatile short *) 0x4eeL)
#define _prtabt		((volatile short *) 0x4f0L)

#define _sysbase	((long *) 0x4F2L)
#define _shell_p	((long *) 0x4F6L)
#define end_os		((long *) 0x4FAL)
#define exec_os		((long *) 0x4FEL)

#define dump_vec	((long *) 0x502L)
#define prt_stat	((long *) 0x506L)
#define prt_vec		((long *) 0x50aL)
#define aux_stat	((long *) 0x50eL)
#define aux_vec		((long *) 0x512L)

typedef struct {
  short	puns;
  char	v_p_un[16];
  long	pstart[16];
  short	bpbs[1];		/* really 16 BPB's (bios parameter block) */
} HDINFO;

#define pun_ptr		((HDINFO **) 0x516L)
#define _p_cookies	((long **) 0x5A0L)

typedef struct
{
	long gem_magic;			/* $87654321 if GEM present */
	long gem_end;			/* End address of OS RAM usage */
	long gem_entry;			/* Execution address of GEM */
} GEM_MUPB;

/* os header */
typedef struct _osheader 
{
    unsigned short	os_entry;   	 /* 0x00  BRA to reset handler	*/
    unsigned short	os_version; 	 /* 0x02  TOS version		*/
    void		(*reseth) (void); /* 0x04 -> reset handler	*/
    struct _osheader	*os_beg;	 /* 0x08 -> base of OS		*/
    void		*os_end;	 /* 0x0c -> end of OS ram usage */
    char		*os_rsv1;	 /* 0x10 reserved		*/
	GEM_MUPB	*os_magic;	 /* 0x14 GEM memory usage param */
    long		os_date;	 /* 0x18 Build date 0xMMDDYYYY	*/
    unsigned short	os_conf;	 /* 0x1c OS conf bits		*/
    unsigned short	os_dosdate;	 /* 0x1e DOS format build date  */
    /* the following available on TOS version >= 1.2 */
    char		**p_root;	 /* 0x20 -> base of OS pool	*/
    char		**pkbshift;	 /* 0x24 -> kbd shift state var */
    char		**p_run;	 /* 0x28 -> PID of current proc */
    char		*p_rsv2;	 /* 0x2c reserved		*/
} OSHEADER;

#define _longframe ((short *)0x59EL)
    
/* zzzz to-do more */

extern unsigned long get_sysvar (volatile void *var) __THROW;
extern unsigned long get_sysvar_size (volatile void *var, int size) __THROW;
extern void set_sysvar_to_long (void *var, long val) __THROW;


__END_DECLS

#endif /* _SYSVARS_H */
