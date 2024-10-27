/* Raven CPX
 * Ported from ct60conf.cpx v1.04
 *
 * Original CT60 source (c) 2009 Didier Mequignon
 * Cleaned CT60 source by Miro Kropacek
 *  github.com/mikrosk/ct60tos
 *
 */
 
#include <portab.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <tos.h>
#include <vdi.h>
#include <mt_aes.h>

#include <falcon.h>
#include "cpx.h"
#include "ct60.h"


long _FilSysV;

#define MIN_FREQ		(  4*1000UL)
#define MAX_FREQ		(125*1000UL)

#define LIGHT
/* #define DEBUG */
/* #define TEST */


#define ID_CPX 			(long)'RV60'
#define VA_START 		0x4711
#define ITIME 			1000L	/* mS */
#define MAX_CPULOAD 	10000
#define MAX_TEMP 		90

#define KER_GETINFO 	0x0100

#define PAGE_CPULOAD 	0
#define PAGE_TEMP    	1
#define PAGE_MEMORY  	2
#define PAGE_LANG    	3
#define PAGE_BOOT    	4

#define PAGE_DEFAULT	PAGE_MEMORY


#define Suptime(uptime,avenrun) gemdos(0x13f,(long)(uptime),(long)(avenrun))
#define Sync() gemdos(0x150)
#define Shutdown(mode) gemdos(0x151,(long)(mode))

typedef struct
{
	unsigned int bootpref;
	unsigned char language;
	unsigned char keyboard;
	unsigned char datetime;
	char separator;
	unsigned char bootdelay;
	unsigned int vmode;
	unsigned char scsi;
	unsigned char tosram;
	unsigned int trigger_temp;
	unsigned int daystop;
	unsigned int timestop;
	unsigned char blitterspeed;
	unsigned char cachedelay;
	unsigned char bootorder;
	unsigned char cpufpu;
	unsigned long frequency;
	unsigned char beep;
	unsigned char bootlog;
	unsigned char idectpci;
} HEAD;

typedef struct
{
	unsigned int bootpref;
	char reserved[4];
	unsigned char language;
	unsigned char keyboard;
	unsigned char datetime;
	char separator;
	unsigned char bootdelay;
	char reserved2[3];
	unsigned int vmode;
	unsigned char scsi;
} NVM;

typedef struct
{
	long ident;
	union
	{
		long l;
		short i[2];
		char c[4];
	} v;
} COOKIE;

typedef struct {
	WORD version;
	void (*fast_clrmem)( void *von, void *bis );
	char (*toupper)( char c );
	void (*_sprintf)( char *dest, char *source, LONG *p );
	BASPAG **act_pd;
 	void *act_appl;
	void *keyb_app;
	WORD *pe_slice;
	WORD *pe_timer;
	void (*appl_yield)( void );
	void (*appl_suspend)( void );
	void (*appl_begcritic)( void );
	void (*appl_endcritic)( void );
	long (*evnt_IO)( LONG ticks_50hz, void *unsel );
	void (*evnt_mIO)( LONG ticks_50hz, void *unsel, WORD cnt );
	void (*evnt_emIO)( void *ap );
	void (*appl_IOcomplete)( void *ap );
	long (*evnt_sem)( WORD mode, void *sem, LONG timeout );
	void (*Pfree)( void *pd );
	WORD int_msize;
	void *int_malloc( void );
	void int_mfree( void *memblk );
	void resv_intmem( void *mem, LONG bytes );
	LONG diskchange( WORD drv );
	LONG DMD_rdevinit( void *dmd );
	LONG proc_info( WORD code, BASPAG *pd );
	LONG mxalloc( LONG amount, WORD mode, BASPAG *pd );
	LONG mfree( void *block );
	LONG mshrink( LONG newlen, void *block );
} MX_KERNEL;


/* prototypes */

int cdecl cpx_call(GRECT *work);
void cdecl cpx_draw(GRECT *clip);
void cdecl cpx_wmove(GRECT *work);
void cdecl cpx_timer(int *event);
void cdecl cpx_key(int kstate,int key,int *event);
void cdecl cpx_button(MRETS *mrets,int nclicks,int *event);
int cdecl cpx_hook(int event,WORD *msg,MRETS *mrets,int *key,int *nclicks);
void cdecl cpx_close(int flag);
int init_rsc(void);
OBJECT* adr_tree(int num_tree);
void init_slider(void);
void display_slider(GRECT *work);
int read_hexa(char *p);
int ascii_hexa(char *p);
void write_hexa(int val,char *p);
void hexa_ascii(int val,char *p);
int cdecl trace_temp(PARMBLK *parmblock);
int cdecl cpu_load(PARMBLK *parmblock);
CPXNODE* get_header(long id);
HEAD *fix_header(void);
void save_header(void);
void display_selection(int selection,int flag_aff);
void change_objc(int objc,int state,GRECT *clip);
void display_objc(int objc,GRECT *clip);
void move_cursor(void);
int hndl_form(OBJECT *tree,int objc);
int MT_form_xalert(int fo_xadefbttn,char *fo_xastring,long time_out,void (*call)(),WORD *global);
void (*function)(void);
void display_error(int err);
void SendIkbd(int count, char *buffer);
int current_temp(void);
int fill_tab_temp(void);
int fill_tab_cpuload(void);
unsigned long bogomips(void);
void delay_loop(long loops);
int get_MagiC_ver(unsigned long *crdate);
COOKIE *fcookie(void);
COOKIE *ncookie(COOKIE *p);
COOKIE *get_cookie(long id);
int add_cookie(COOKIE *cook);
int test_060(void);
int copy_string(char *p1,char *p2);
int long_deci(char *p,int *lg);
int val_string(char *p);
void reboot(void);
void (*reset)(void);


extern long ct60_read_clock(void);
extern long ct60_read_temp(void);
extern long ct60_stop(void);
extern long ct60_cpu_revision(void);
extern long mes_delay(void);

long ct60_rw_param(int mode,long type_param,long value)
{
	return 0;
}


/* global variables in the 1st position of DATA segment */

HEAD config={0,2,2,0x11,'/',1,0x1b2,0x87,0,50,0,0,1,0,1,1,MIN_FREQ,1,1};


/* global variables */

int vdi_handle,work_in[11]={1,1,1,1,1,1,1,1,1,1,2},work_out[57];
int errno;
WORD global[15];
int gr_hwchar,gr_hhchar;
int	wi_id=-1;
int mint,magic,flag_frequency,flag_cpuload,flag_xbios,thread=0;
unsigned long magic_date,st_ram,fast_ram,loops_per_sec=0,frequency=MIN_FREQ,min_freq=MIN_FREQ,max_freq=MAX_FREQ;
unsigned long step_frequency=1;
long cpu_cookie=0;
char *eiffel_temp=NULL;
extern unsigned long value_supexec;
XCPB *Xcpb;
GRECT *Work;
CPXNODE *head;
CPXINFO	cpxinfo={cpx_call,cpx_draw,cpx_wmove,cpx_timer,cpx_key,cpx_button,0,0,cpx_hook,cpx_close};
NVM nvram;
USERBLK spec_trace={0,0};
USERBLK spec_cpuload={0,0};
int ed_objc,new_objc,ed_pos,new_pos;
int start_lang,selection,no_jumper;
int language,keyboard,datetime,vmode,bootpref,bootdelay,scsi,cpufpu;
int tosram,blitterspeed,cachedelay,bootorder,bootlog,idectpci,nv_magic_code;
unsigned int trigger_temp,daystop,timestop,beep;
unsigned short tab_temp[61],tab_temp_eiffel[61],tab_cpuload[61];

/* ressource */

#define MENUBOX 0
#define MENUBSELECT 3
#define MENUBOXSTATUS 5
#define MENUTEMP 7
#define MENUBARTEMP 8
#define MENUTRIGGER 12
#define MENUTRACE 13
#define MENUSTATUS 18
#define MENUBOXRAM 19
#define MENUSTRAMTOT 21
#define MENUFASTRAMTOT 23
#define MENUSTRAM 25
#define MENUFASTRAM 27
#define MENUMIPS 28
#define MENUBFPU 30
#define MENUBLEFT 31
#define MENUBOXSLIDER 32
#define MENUSLIDER 33
#define MENUBRIGHT 34
#define MENURAM 35
#define MENUBOXLANG 36
#define MENUBLANG 38
#define MENUBKEY 40
#define MENUBDATE 42
#define MENUBTIME 44
#define MENUSEP 45
#define MENULANG 46
#define MENUBOXVIDEO 47
#define MENUBVIDEO 49
#define MENUBMODE 51
#define MENUBRES 53
#define MENUBCOUL 55
#define MENUSTMODES 56
#define MENUOVERSCAN 57
#define MENUNVM 58
#define MENUVIDEO 59
#define MENUBOXBOOT 60
#define MENUBBOOTORDER 61
#define MENUBOS 63
#define MENUBARBIT 65
#define MENUBIDSCSI 67
#define MENUDELAY 68
#define MENUBBLITTER 70
#define MENUBTOSRAM 72
#define MENUBCACHE 74
#define MENUBBOOTLOG 76
#define MENUBIDECTPCI 78
#define MENUBOOT 79
#define MENUBOXSTOP 80
#define MENUBDAY 82
#define MENUTIME 83
#define MENUBBEEP 85
#define MENUSTOP 86

#define MENUBSAVE 87
#define MENUBLOAD 88
#define MENUBOK 89
#define MENUBCANCEL 90
#define MENUBINFO 91

#define INFOBOX 0
#define INFOLOGO 1
#define INFOOK 13
#define INFOSDRAM 14
#define INFOHELP 15

#define ALERTBOX 0
#define ALERTTITLE 1
#define ALERTNOTE 2
#define ALERTWAIT 3
#define ALERTSTOP 4
#define ALERTLINE1 5
#define ALERTLINE2 6
#define ALERTLINE3 7
#define ALERTLINE4 8
#define ALERTLINE5 9
#define ALERTLINE6 10
#define ALERTLINE7 11
#define ALERTLINE8 12
#define ALERTLINE9 13
#define ALERTLINE10 14
#define ALERTLINE11 15
#define ALERTLINE12 16
#define ALERTLINE13 17
#define ALERTLINE14 18
#define ALERTLINE15 19
#define ALERTLINE16 20
#define ALERTLINE17 21
#define ALERTLINE18 22
#define ALERTLINE19 23
#define ALERTLINE20 24
#define ALERTLINE21 25
#define ALERTLINE22 26
#define ALERTLINE23 27
#define ALERTLINE24 28
#define ALERTLINE25 29
#define ALERTB1 30
#define ALERTB2 31
#define ALERTB3 32

#define OFFSETTLV 1
#define OFFSETOK 2
#define OFFSETCANCEL 3

char *rs_strings_en[] = {
	"Raven Configuration","","",
	"Selection:",
	"Temperature","","",
	" Temperature ","","",
	"Temp:",
	"xxx øC","","",
	"yy","Thres: __","99",
#if 0
	"00:00 00:10 00:20 00:30 00:40 00:50 01:xx","","",
#else
	"","","",
#endif
	"80","","",
	"40","","",
	"0ø","","",
	" Memory / æP ","","",
	"ST RAM:",
	"uuuuuuuuu KByte",
	"TT RAM:",
	"vvvvvvvvv KByte",	
	"ST RAM free:",
	"yyyyyyyyy KByte","","",
	"TT RAM free:",
	"zzzzzzzzz KByte","","",
	"æP:   0.00 Mips     0000 tr/mn","","",
	"FPU:",
	"No","","",
	"012.345","","",
	" Language ","","",
	"Language:",
	"English","","",
	"Keyboard:",
	"England","","",
	"Date format:",
	"DD/MM/YY","","",
	"Time:",
	"24","","",
	"/","Separator: _","X",
	" Video (boot) ","","",
	"Video:",
	"VGA","","",
	"Mode:",
	"PAL","","",
	"Resolution:",
	"320x200","","",
	"Colors:",
	"xxxxx","","",
	"Mode ST",
	"Overscan",
	"Replace NVRAM",
	" Boot ","","",
	"IDE->SCSI","","",
	"OS:",
	"TOS","","",
	"SCSI arbitration:",
	"Yes","","",
	"ID:",
	"x","","",
	"zz","Delay: __ S","99",
	"Blitter:",
	"Slow","","",
	"TOS in RAM:",
	"No","","",
	"TOS:",
	"Normal","","",
	"boot.log:",
	"Without","","",
	"IDE:",
	"FALCON","","",
	" Stop ","","",
	"Stop programmed:",
	"Without","","",
	"xxxx","at: __:__","9999",
	"Beep alarm:",
	"Yes","","",
	
	"Save",
	"Load",
	"OK",
	"Cancel",

	"Raven Configuration","","",
	"This CPX and system:","","",
	"Didier MEQUIGNON","","",
	"aniplay@wanadoo.fr","","",
	"System:","","",
	"Xavier JOUBERT","","",
	"xavier.joubert@free.fr","","",
	"Hardware:","","",
	"Rodolphe CZUBA","","",
	"rczuba@free.fr","","",
	"http://www.powerphenix.com","","",
	"OK",
	"SDRAM",
	"Help",
	
	"CT60 Temperature","","",
	"line1x",
	"line2x",
	"line3x",
	"line4x",
	"line5x",
	"line6x",
	"line7x",
	"line8x",
	"line9x",	
	"line10x",
	"line11x",
	"line12x",
	"line13x",
	"line14x",
	"line15x",
	"line16x",
	"line17x",
	"line18x",
	"line19x",
	"line20x",
	"line21x",
	"line22x",
	"line23x",
	"line24x",
	"line25x",
	"button1x",
	"button2x",
	"button3x",
	
	"-00","Offset TLV 2.8øC/unit: ___ unit","X99",
	"OK",
	"Cancel" };

long rs_frstr[] = {0};

BITBLK rs_bitblk[] = {
	(int *)0L,36,72,0,0,2,
	(int *)1L,4,32,0,0,1,
	(int *)2L,4,32,0,0,4,
	(int *)3L,4,32,0,0,2 };

long rs_frimg[] = {0};
ICONBLK rs_iconblk[] = {0};

TEDINFO rs_tedinfo[] = {
	(char *)0L,(char *)1L,(char *)2L,IBM,0,2,0x1180,0,0,32,1,
	(char *)4L,(char *)5L,(char *)6L,IBM,0,2,0x1180,0,-1,16,1,
	(char *)7L,(char *)8L,(char *)9L,IBM,0,2,0x1180,0,0,14,1,
	(char *)11L,(char *)12L,(char *)13L,IBM,0,0,0x1180,0,0,7,1,
	(char *)14L,(char *)15L,(char *)16L,IBM,0,0,0x1180,0,0,3,10,
	(char *)17L,(char *)18L,(char *)19L,SMALL,0,2,0x1180,0,0,42,1,
	(char *)20L,(char *)21L,(char *)22L,SMALL,0,2,0x1180,0,0,3,1,
	(char *)23L,(char *)24L,(char *)25L,SMALL,0,2,0x1180,0,0,3,1,
	(char *)26L,(char *)27L,(char *)28L,SMALL,0,2,0x1180,0,0,3,1,
	(char *)29L,(char *)30L,(char *)31L,IBM,0,2,0x1180,0,0,15,1,
	(char *)37L,(char *)38L,(char *)39L,IBM,0,0,0x1180,0,0,16,1,
	(char *)41L,(char *)42L,(char *)43L,IBM,0,0,0x1180,0,0,16,1,
	(char *)44L,(char *)45L,(char *)46L,IBM,0,0,0x1180,0,0,16,1,
	(char *)48L,(char *)49L,(char *)50L,IBM,0,2,0x1180,0,-1,4,1,
	(char *)51L,(char *)52L,(char *)53L,SMALL,0,2,0x1180,0,-1,8,1,
	(char *)54L,(char *)55L,(char *)56L,IBM,0,2,0x1180,0,0,9,1,
	(char *)58L,(char *)59L,(char *)60L,IBM,0,2,0x1180,0,-1,16,1,
	(char *)62L,(char *)63L,(char *)64L,IBM,0,2,0x1180,0,-1,16,1,
	(char *)66L,(char *)67L,(char *)68L,IBM,0,2,0x1180,0,-1,16,1,
	(char *)70L,(char *)71L,(char *)72L,IBM,0,2,0x1180,0,-1,4,1,	
	(char *)73L,(char *)74L,(char *)75L,IBM,0,0,0x1180,0,0,2,14,
	(char *)76L,(char *)77L,(char *)78L,IBM,0,2,0x1180,0,0,9,1,
	(char *)80L,(char *)81L,(char *)82L,IBM,0,2,0x1180,0,-1,8,1,
	(char *)84L,(char *)85L,(char *)86L,IBM,0,2,0x1180,0,-1,8,1,
	(char *)88L,(char *)89L,(char *)90L,IBM,0,2,0x1180,0,-1,16,1,
	(char *)92L,(char *)93L,(char *)94L,IBM,0,2,0x1180,0,-1,16,1,
	(char *)98L,(char *)99L,(char *)100L,IBM,0,2,0x1180,0,0,6,1,
	(char *)101L,(char *)102L,(char *)103L,IBM,0,2,0x1180,0,-1,10,1,
	(char *)105L,(char *)106L,(char *)107L,IBM,0,2,0x1180,0,-1,16,1,
	(char *)109L,(char *)110L,(char *)111L,IBM,0,2,0x1180,0,-1,5,1,
	(char *)113L,(char *)114L,(char *)115L,IBM,0,2,0x1180,0,-1,3,1,		
	(char *)116L,(char *)117L,(char *)118L,IBM,0,0,0x1180,0,0,3,13,
	(char *)120L,(char *)121L,(char *)122L,IBM,0,2,0x1180,0,-1,7,1,
	(char *)124L,(char *)125L,(char *)126L,IBM,0,2,0x1180,0,-1,4,1,
	(char *)128L,(char *)129L,(char *)130L,IBM,0,2,0x1180,0,-1,8,1,
	(char *)132L,(char *)133L,(char *)134L,IBM,0,2,0x1180,0,-1,7,1,
	(char *)136L,(char *)137L,(char *)138L,IBM,0,2,0x1180,0,-1,7,1,
	(char *)139L,(char *)140L,(char *)141L,IBM,0,2,0x1180,0,0,14,1,
	(char *)143L,(char *)144L,(char *)145L,IBM,0,2,0x1180,0,-1,16,1,
	(char *)146L,(char *)147L,(char *)148L,IBM,0,0,0x1180,0,0,5,10,
	(char *)150L,(char *)151L,(char *)152L,IBM,0,2,0x1180,0,-1,5,1,

	(char *)157L,(char *)158L,(char *)159L,IBM,0,2,0x1480,0,0,38,1,
	(char *)160L,(char *)161L,(char *)162L,IBM,0,2,0x1180,0,0,38,1,
	(char *)163L,(char *)164L,(char *)165L,IBM,0,2,0x1180,0,0,38,1,
	(char *)166L,(char *)167L,(char *)168L,IBM,0,2,0x1180,0,0,38,1,
	(char *)169L,(char *)170L,(char *)171L,IBM,0,2,0x1180,0,0,38,1,
	(char *)172L,(char *)173L,(char *)174L,IBM,0,2,0x1180,0,0,38,1,
	(char *)175L,(char *)176L,(char *)177L,IBM,0,2,0x1180,0,0,38,1,
	(char *)178L,(char *)179L,(char *)180L,IBM,0,2,0x1180,0,0,38,1,
	(char *)181L,(char *)182L,(char *)183L,IBM,0,2,0x1180,0,0,38,1,
	(char *)184L,(char *)185L,(char *)186L,IBM,0,2,0x1180,0,0,38,1,
	(char *)187L,(char *)188L,(char *)189L,IBM,0,2,0x1180,0,0,38,1,

	(char *)193L,(char *)194L,(char *)195L,IBM,0,2,0x1480,0,-1,17,1,
	
	(char *)224L,(char *)225L,(char *)226L,IBM,0,0,0x1180,0,0,4,32 };
	
OBJECT rs_object[] = {
	-1,1,91,G_BOX,FL3DBAK,NORMAL,0x1100L,0,0,32,11,
	2,-1,-1,G_TEXT,FL3DBAK,SELECTED,0L,0,0,32,1,
	3,-1,-1,G_STRING,NONE,NORMAL,3L,1,1,14,1,
	4,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,1L,16,1,15,1,								/* popup selection */
	5,-1,-1,G_BOX,FL3DBAK,NORMAL,0x1100L,1,2,14,1,
	18,6,17,G_BOX,FL3DIND,NORMAL,0xff1100L,0,2,32,6,								/* state box */
	7,-1,-1,G_STRING,NONE,NORMAL,10L,1,1,6,1,
	8,-1,-1,G_TEXT,FL3DBAK,NORMAL,3L,8,1,6,1,										/* temperature */
	12,9,11,G_BOX,NONE,NORMAL,0xff11f1L,15,1,6,1,
	10,-1,-1,G_BOX,NONE,NORMAL,0x11f3L,0,0,2,1,										/* green */
	11,-1,-1,G_BOX,NONE,NORMAL,0x11f6L,2,0,2,1,										/* yellow */
	8,-1,-1,G_BOX,NONE,NORMAL,0x11f2L,4,0,2,1,										/* red */
	13,-1,-1,G_FTEXT,HIDETREE|TOUCHEXIT|FL3DBAK,NORMAL,4L,22,1,9,1,					/* threshold */
	14,-1,-1,G_BOX,NONE,NORMAL,0L,1,2,30,4,											/* trace */
	15,-1,-1,G_TEXT,FL3DBAK,NORMAL,5L,1,6,31,1,
	16,-1,-1,G_TEXT,FL3DBAK,NORMAL,6L,0,2,3,1,
	17,-1,-1,G_TEXT,FL3DBAK,NORMAL,7L,0,4,3,1,
	5,-1,-1,G_TEXT,FL3DBAK,NORMAL,8L,0,5,3,1,
	19,-1,-1,G_TEXT,FL3DBAK,NORMAL,2L,1,2,13,1,
	35,20,34,G_BOX,FL3DIND,NORMAL,0xff1100L,0,2,32,6,								/* memory box */
	21,-1,-1,G_STRING,NONE,NORMAL,32L,1,1,15,1,
	22,-1,-1,G_STRING,NONE,NORMAL,33L,16,1,16,1,									/* total ST-Ram */
	23,-1,-1,G_STRING,NONE,NORMAL,34L,1,2,15,1,
	24,-1,-1,G_STRING,NONE,NORMAL,35L,16,2,16,1,									/* total Fast-Ram */	
	25,-1,-1,G_STRING,NONE,NORMAL,36L,1,3,15,1,
	26,-1,-1,G_TEXT,FL3DBAK,NORMAL,10L,16,3,16,1,									/* free ST-Ram */
	27,-1,-1,G_STRING,NONE,NORMAL,40L,1,4,15,1,
	28,-1,-1,G_TEXT,FL3DBAK,NORMAL,11L,16,4,16,1,									/* free Fast-Ram */
	29,-1,-1,G_TEXT,/* TOUCHEXIT| */ FL3DBAK,NORMAL,12L,1,5,30,1,					/* Mips & tr/mn */
	30,-1,-1,G_STRING,NONE,NORMAL,47L,1,6,4,1,
	31,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,13L,6,6,6,1,								/* popup PFU */
	32,-1,-1,G_BOXCHAR,TOUCHEXIT,NORMAL,0x4ff1100L,16,6,2,1,						/*  */
	34,33,33,G_BOX,TOUCHEXIT,NORMAL,0xff1111L,18,6,11,1,
	32,-1,-1,G_BOXTEXT,TOUCHEXIT,NORMAL,14L,0,0,5,1,
	19,-1,-1,G_BOXCHAR,TOUCHEXIT,NORMAL,0x3ff1100L,29,6,2,1,						/*  */
	36,-1,-1,G_TEXT,FL3DBAK,NORMAL,9L,1,2,14,1,
	46,37,45,G_BOX,FL3DIND,NORMAL,0xff1100L,0,2,32,6,								/* language box */
	38,-1,-1,G_STRING,NONE,NORMAL,57L,1,1,15,1,
	39,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,16L,16,1,15,1,							/* popup language */
	40,-1,-1,G_STRING,NONE,NORMAL,61L,1,2,15,1,
	41,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,17L,16,2,15,1,							/* popup keyboard */
	42,-1,-1,G_STRING,NONE,NORMAL,65L,1,3,15,1,
	43,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,18L,16,3,15,1,							/* popup date format */
	44,-1,-1,G_STRING,NONE,NORMAL,69L,1,5,7,1,
	45,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,19L,8,5,3,1,								/* popup time format */
	36,-1,-1,G_FTEXT,EDITABLE|FL3DBAK,NORMAL,20L,16,5,14,1,
	47,-1,-1,G_TEXT,FL3DBAK,NORMAL,15L,1,2,10,1,
	59,48,58,G_BOX,FL3DIND,NORMAL,0xff1100L,0,2,32,6,								/* video box */
	49,-1,-1,G_STRING,NONE,NORMAL,79L,1,1,7,1,
	50,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,22L,8,1,7,1,								/* popup video TV/VGA */
	51,-1,-1,G_STRING,NONE,NORMAL,83L,16,1,7,1,
	52,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,23L,24,1,7,1,								/* popup mode NTSC/PAL */
	53,-1,-1,G_STRING,NONE,NORMAL,87L,1,2,15,1,
	54,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,25L,16,2,15,1,							/* popup resolution */
	55,-1,-1,G_STRING,NONE,NORMAL,91L,1,3,9,1,
	56,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,24L,10,3,5,1,								/* popup colors */
	57,-1,-1,G_BUTTON,SELECTABLE|TOUCHEXIT|FL3DIND,NORMAL,95L,16,3,15,1,            /* mode ST */
	58,-1,-1,G_BUTTON,SELECTABLE|TOUCHEXIT|FL3DIND,NORMAL,96L,16,5,15,1,            /* overscan */
	47,-1,-1,G_BUTTON,SELECTABLE|TOUCHEXIT|FL3DIND,NORMAL,97L,1,5,14,1,             /* replace NVRAM */
	60,-1,-1,G_TEXT,FL3DBAK,NORMAL,21L,1,2,14,1,
	79,61,78,G_BOX,FL3DIND,NORMAL,0xff1100L,0,2,32,6,								/* boot box */
	62,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,27L,1,1,17,1,								/* popup boot order */
	63,-1,-1,G_STRING,NONE,NORMAL,104L,20,1,3,1,
	64,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,28L,24,1,7,1,								/* popup favourite OS */
	65,-1,-1,G_STRING,NONE,NORMAL,108L,1,2,17,1,
	66,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,29L,18,2,4,1,								/* popup arbitration */
	67,-1,-1,G_STRING,NONE,NORMAL,112L,24,2,3,1,
	68,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,30L,28,2,2,1,								/* popup ID SCSI */
	69,-1,-1,G_FTEXT,EDITABLE|FL3DBAK,NORMAL,31L,1,3,13,1,
	70,-1,-1,G_STRING,NONE,NORMAL,119L,15,3,8,1,
	71,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,32L,24,3,6,1,								/* popup speed blitter */
	72,-1,-1,G_STRING,NONE,NORMAL,123L,1,4,11,1,
	73,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,33L,12,4,4,1,								/* popup 2transfer TOS in RAM */
	74,-1,-1,G_STRING,NONE,NORMAL,127L,18,4,4,1,
	75,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,34L,23,4,7,1,								/* popup TOS cache delay */
	76,-1,-1,G_STRING,NONE,NORMAL,131L,1,5,9,1,
	77,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,35L,10,5,7,1,								/* popup boot.log */
	78,-1,-1,G_STRING,NONE,NORMAL,135L,19,5,5,1,
	60,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,36L,23,5,7,1,								/* popup IDE CTPCI */
	80,-1,-1,G_TEXT,FL3DBAK,NORMAL,26L,1,2,6,1,
	86,81,85,G_BOX,FL3DIND,NORMAL,0xff1100L,0,2,32,6,								/* stop box */
	82,-1,-1,G_STRING,NONE,NORMAL,142L,1,1,15,1,
	83,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,38L,16,1,15,1,							/* popup stop */
	84,-1,-1,G_FTEXT,EDITABLE|FL3DBAK,NORMAL,39L,1,3,13,1,							/* time */
	85,-1,-1,G_STRING,NONE,NORMAL,149L,1,5,15,1,
	80,-1,-1,G_BOXTEXT,TOUCHEXIT,SHADOWED,40L,16,5,4,1,								/* popup beep */
	87,-1,-1,G_TEXT,FL3DBAK,NORMAL,37L,1,2,6,1,

	88,-1,-1,G_BUTTON,SELECTABLE|EXIT|FL3DIND|FL3DBAK,NORMAL,153L,1,9,5,1,			/* Save */
	89,-1,-1,G_BUTTON,SELECTABLE|EXIT|FL3DIND|FL3DBAK,NORMAL,154L,8,9,6,1,			/* Load */
	90,-1,-1,G_BUTTON,SELECTABLE|EXIT|FL3DIND|FL3DBAK,NORMAL,155L,16,9,3,1,			/* OK */
	91,-1,-1,G_BUTTON,SELECTABLE|DEFAULT|EXIT|FL3DIND|FL3DBAK,NORMAL,156L,21,9,6,1,	/* Cancel */
	0,-1,-1,G_BOXCHAR,SELECTABLE|EXIT|LASTOB|FL3DIND|FL3DBAK,NORMAL,0x69ff1100L,29,9,2,1,	/* i */

	/* info box */
	-1,1,15,G_BOX,FL3DBAK,OUTLINED,0x21100L,0,0,40,16,
	2,-1,-1,G_IMAGE,HIDETREE|NONE,NORMAL,0L,2,1,36,5,
	3,-1,-1,G_TEXT,FL3DBAK,NORMAL,41L,1,1,38,1,
	4,-1,-1,G_TEXT,FL3DBAK,NORMAL,42L,1,3,38,1,
	5,-1,-1,G_TEXT,FL3DBAK,NORMAL,43L,1,4,38,1,
	6,-1,-1,G_TEXT,FL3DBAK,NORMAL,44L,1,5,38,1,
	7,-1,-1,G_TEXT,FL3DBAK,NORMAL,45L,1,6,38,1,
	8,-1,-1,G_TEXT,FL3DBAK,NORMAL,46L,1,7,38,1,
	9,-1,-1,G_TEXT,FL3DBAK,NORMAL,47L,1,8,38,1,
	10,-1,-1,G_TEXT,FL3DBAK,NORMAL,48L,1,9,38,1,
	11,-1,-1,G_TEXT,FL3DBAK,NORMAL,49L,1,10,38,1,
	12,-1,-1,G_TEXT,FL3DBAK,NORMAL,50L,1,11,38,1,
	13,-1,-1,G_TEXT,FL3DBAK,NORMAL,51L,1,12,38,1,
	14,-1,-1,G_BUTTON,SELECTABLE|DEFAULT|EXIT|FL3DIND|FL3DBAK,NORMAL,190L,16,14,8,1,	/* OK */		
	15,-1,-1,G_BUTTON,HIDETREE|SELECTABLE|EXIT|FL3DIND|FL3DBAK,NORMAL,191L,28,14,8,1,		/* SDRAM */
	0,-1,-1,G_BUTTON,HIDETREE|SELECTABLE|EXIT|LASTOB|FL3DIND|FL3DBAK,NORMAL,192L,4,14,8,1,	/* Help */	

	/* alert box */
	-1,1,32,G_BOX,FL3DBAK,OUTLINED,0x21100L,0,0,42,30,
	2,-1,-1,G_BOXTEXT,FL3DIND,NORMAL,52L,0,0,42,1,
	3,-1,-1,G_IMAGE,NONE,NORMAL,1L,1,2,4,2,
	4,-1,-1,G_IMAGE,NONE,NORMAL,2L,1,2,4,2,
	5,-1,-1,G_IMAGE,NONE,NORMAL,3L,1,2,4,2,
	6,-1,-1,G_STRING,NONE,NORMAL,196L,1,2,40,1,
	7,-1,-1,G_STRING,NONE,NORMAL,197L,1,3,40,1,
	8,-1,-1,G_STRING,NONE,NORMAL,198L,1,4,40,1,
	9,-1,-1,G_STRING,NONE,NORMAL,199L,1,5,40,1,
	10,-1,-1,G_STRING,NONE,NORMAL,200L,1,6,40,1,
	11,-1,-1,G_STRING,NONE,NORMAL,201L,1,7,40,1,
	12,-1,-1,G_STRING,NONE,NORMAL,202L,1,8,40,1,
	13,-1,-1,G_STRING,NONE,NORMAL,203L,1,9,40,1,
	14,-1,-1,G_STRING,NONE,NORMAL,204L,1,10,40,1,
	15,-1,-1,G_STRING,NONE,NORMAL,205L,1,11,40,1,
	16,-1,-1,G_STRING,NONE,NORMAL,206L,1,12,40,1,
	17,-1,-1,G_STRING,NONE,NORMAL,207L,1,13,40,1,
	18,-1,-1,G_STRING,NONE,NORMAL,208L,1,14,40,1,
	19,-1,-1,G_STRING,NONE,NORMAL,209L,1,15,40,1,
	20,-1,-1,G_STRING,NONE,NORMAL,210L,1,16,40,1,
	21,-1,-1,G_STRING,NONE,NORMAL,211L,1,17,40,1,
	22,-1,-1,G_STRING,NONE,NORMAL,212L,1,18,40,1,
	23,-1,-1,G_STRING,NONE,NORMAL,213L,1,19,40,1,
	24,-1,-1,G_STRING,NONE,NORMAL,214L,1,20,40,1,
	25,-1,-1,G_STRING,NONE,NORMAL,215L,1,21,40,1,
	26,-1,-1,G_STRING,NONE,NORMAL,216L,1,22,40,1,
	27,-1,-1,G_STRING,NONE,NORMAL,217L,1,23,40,1,
	28,-1,-1,G_STRING,NONE,NORMAL,218L,1,24,40,1,
	29,-1,-1,G_STRING,NONE,NORMAL,219L,1,25,40,1,
	30,-1,-1,G_STRING,NONE,NORMAL,220L,1,26,40,1,
	31,-1,-1,G_BUTTON,SELECTABLE|DEFAULT|EXIT|FL3DIND|FL3DBAK,NORMAL,221L,1,28,10,1,
	32,-1,-1,G_BUTTON,SELECTABLE|EXIT|FL3DIND|FL3DBAK,NORMAL,222L,12,28,10,1,
	0,-1,-1,G_BUTTON,SELECTABLE|EXIT|LASTOB|FL3DIND|FL3DBAK,NORMAL,223L,23,28,10,1,

	/* TLV offset */
	-1,1,3,G_BOX,FL3DBAK,OUTLINED,0x21100L,0,0,33,5,
	2,-1,-1,G_FTEXT,EDITABLE|FL3DBAK,NORMAL,53L,1,1,31,1,
	3,-1,-1,G_BUTTON,SELECTABLE|EXIT|FL3DIND|FL3DBAK,NORMAL,227L,7,3,6,1,					/* OK */
	0,-1,-1,G_BUTTON,SELECTABLE|DEFAULT|EXIT|LASTOB|FL3DIND|FL3DBAK,NORMAL,228L,20,3,6,1 };	/* Cancel */

long rs_trindex[] = {0L,92L,108L,141L};
struct foobar {
	int dummy;
	int *image;
	} rs_imdope[] = {0};

UWORD pic_note[]={
	0x0003,0xC000,0x0006,0x6000,0x000D,0xB000,0x001B,0xD800,
	0x0037,0xEC00,0x006F,0xF600,0x00DC,0x3B00,0x01BC,0x3D80,
	0x037C,0x3EC0,0x06FC,0x3F60,0x0DFC,0x3FB0,0x1BFC,0x3FD8,
	0x37FC,0x3FEC,0x6FFC,0x3FF6,0xDFFC,0x3FFB,0xBFFC,0x3FFD,
	0xBFFC,0x3FFD,0xDFFC,0x3FFB,0x6FFC,0x3FF6,0x37FC,0x3FEC,
	0x1BFF,0xFFD8,0x0DFF,0xFFB0,0x06FC,0x3F60,0x037C,0x3EC0,
	0x01BC,0x3D80,0x00DC,0x3B00,0x006F,0xF600,0x0037,0xEC00,
	0x001B,0xD800,0x000D,0xB000,0x0006,0x6000,0x0003,0xC000 };

UWORD pic_wait[]={
	0x3FFF,0xFFFC,0xC000,0x0003,0x9FFF,0xFFF9,0xBFFF,0xFFFD,
	0xDFF8,0x3FFB,0x5FE0,0x0FFA,0x6FC0,0x07F6,0x2F83,0x83F4,
	0x3787,0xC3EC,0x1787,0xC3E8,0x1BFF,0x83D8,0x0BFF,0x07D0,
	0x0DFE,0x0FB0,0x05FC,0x1FA0,0x06FC,0x3F60,0x02FC,0x3F40,
	0x037C,0x3EC0,0x017C,0x3E80,0x01BF,0xFD80,0x00BF,0xFD00,
	0x00DC,0x3B00,0x005C,0x3A00,0x006C,0x3600,0x002F,0xF400,
	0x0037,0xEC00,0x0017,0xE800,0x001B,0xD800,0x000B,0xD000,
	0x000D,0xB000,0x0005,0xA000,0x0006,0x6000,0x0003,0xC000 };

UWORD pic_stop[]={
	0x007F,0xFE00,0x00C0,0x0300,0x01BF,0xFD80,0x037F,0xFEC0,
	0x06FF,0xFF60,0x0DFF,0xFFB0,0x1BFF,0xFFD8,0x37FF,0xFFEC,
	0x6FFF,0xFFF6,0xDFFF,0xFFFB,0xB181,0x860D,0xA081,0x0205,
	0xA4E7,0x3265,0xA7E7,0x3265,0xA3E7,0x3265,0xB1E7,0x3205,
	0xB8E7,0x320D,0xBCE7,0x327D,0xA4E7,0x327D,0xA0E7,0x027D,
	0xB1E7,0x867D,0xBFFF,0xFFFD,0xDFFF,0xFFFB,0x6FFF,0xFFF6,
	0x37FF,0xFFEC,0x1BFF,0xFFD8,0x0DFF,0xFFB0,0x06FF,0xFF60,
	0x037F,0xFEC0,0x01BF,0xFD80,0x00C0,0x0300,0x007F,0xFE00 };

#define NUM_STRINGS 229	/* number of strings */
#define NUM_FRSTR 0		/* strings form_alert */
#define NUM_IMAGES 0
#define NUM_BB 3		/* number of BITBLK */
#define NUM_FRIMG 0
#define NUM_IB 0		/* number of ICONBLK */
#define NUM_TI 54		/* number of TEDINFO */
#define NUM_OBS 145		/* number of objects */
#define NUM_TREE 4		/* number of trees */ 

#define TREE1 0
#define TREE2 1
#define TREE3 2
#define TREE4 3

#define MAX_SELECT 5

#define USA 0
#define FRG 1
#define FRA 2
#define UK 3
#define SPA 4
#define ITA 5
#define SWE 6
#define SWF 7
#define SWG 8

/* popups */

char *spec_select[7]={"Average load","Temperature","Memory / æP","Language","Boot","Stop","Video"};
char *_select[7]={"  Average load   ","  Temperature    ","  Memory / æP    ","  Language       ","  Boot           ","  Stop           ","  Video (boot)   "};
char *spec_fpu[2]={"No","Yes"};
char *fpu[2]={"  No  ","  Yes "};
char *spec_lang[]={"English","Deutsch","Fran‡ais","Espa¥ol","Italiano","Suisse","Schweiz"};
char *lang[]={ "  English     ",
               "  Deutsch     ",
               "  Fran‡ais    ",
               "  Espa¥ol     ",
               "  Italiano    ",
               "  Suisse      ",
               "  Schweiz     " };
char *spec_key[]={"USA","Deutsch","France","England","Espa¥a","Italia","Sweden","Suisse","Schweiz"};
char *key[]={ "  USA            ",
              "  Deutschland    ",
              "  France         ",
              "  England & Eire ",
              "  Espa¥a         ",
              "  Italia         ",
              "  Sweden         ",
              "  Suisse         ",
              "  Schweiz        " };
char *spec_date[]={"MM/DD/YY","DD/MM/YY","YY/MM/DD","YY/DD/MM"};
char *date[]={"  MM/DD/YY ","  DD/MM/YY ","  YY/MM/DD ","  YY/DD/MM "};
char *spec_time[]={"12","24"};
char *_time[]={"  12H ","  24H "};

unsigned char code_lang[]= { USA, FRG, FRA, SPA, ITA, SWF, SWG };
unsigned char code_key[]=  { USA, FRG, FRA, UK, SPA, ITA, SWE, SWF, SWG };
unsigned char code_os[]={0,0x80,8,0x40,0x10,0x20};

char *spec_os[]={"-","TOS","MagiC","TT SVR4","Linux","NetBSD"};
char *os[]={"  -       ","  TOS     ","  MagiC   ","  TT SVR4 ","  Linux   ","  NetBSD  "};
char *spec_arbit[2][2]={"Non","Oui","No","Yes"};
char *arbit[2][2]={"  Non ","  Oui ","  No  ","  Yes "};
char *spec_idscsi[]={"0","1","2","3","4","5","6","7"};
char *idscsi[]={"  0 ","  1 ","  2 ","  3 ","  4 ","  5 ","  6 ","  7 "};	
char *spec_blitter_speed[2][2]={"Lent","Rapide","Slow","Fast"};
char *blitter_speed[2][2]={"  Lent   ","  Rapide ","  Slow ","  Fast "};
char *spec_tos_ram[2][2]={"Non","Oui","No","Yes"};
char *tos_ram[2][2]={"  Non ","  Oui ","  No  ","  Yes "};
char *spec_cache_delay[2][4]={"Normal","Cache 5","Alerte","Cache 5","Normal","Cache 5","Alert","Cache 5"};
char *cache_delay[2][4]={"  Cache normal / Sans alerte copyback ",
                         "  Cache delais 5 S / Sans alerte      ",
                         "  Cache normal / Alerte copyback      ",
                         "  Cache delais 5 S / Alerte copyback  ",
                         "  Normal cache / No copyback alert    ",
                         "  Delay cache 5 S / No copyback alert ",
                         "  Normal cache / Copyback alert       ",
                         "  Delay cache 5 S / Copyback alert    "};
char *spec_boot_order[2][8]={"SCSI0-7 -> IDE0-1","IDE0-1 -> SCSI0-7","SCSI7-0 -> IDE1-0","IDE1-0 -> SCSI7-0",
                             "SCSI0-7 -> IDE0-1","IDE0-1 -> SCSI0-7","SCSI7-0 -> IDE1-0","IDE1-0 -> SCSI7-0",
                             "SCSI0-7 -> IDE0-1","IDE0-1 -> SCSI0-7","SCSI7-0 -> IDE1-0","IDE1-0 -> SCSI7-0",
                             "SCSI0-7 -> IDE0-1","IDE0-1 -> SCSI0-7","SCSI7-0 -> IDE1-0","IDE1-0 -> SCSI7-0"};
char *boot_order[2][8]={"  Nouv. boot SCSI0-7 -> IDE0-1 ",
                        "  Nouv. boot IDE0-1 -> SCSI0-7 ",
                        "  Nouv. boot SCSI7-0 -> IDE1-0 ",
                        "  Nouv. boot IDE1-0 -> SCSI7-0 ",
                        "  Vieux boot SCSI0-7 -> IDE0-1 ",
                        "  Vieux boot IDE0-1 -> SCSI0-7 ",
                        "  Vieux boot SCSI7-0 -> IDE1-0 ",
                        "  Vieux boot IDE1-0 -> SCSI7-0 ",
                        "  New boot SCSI0-7 -> IDE0-1 ",
                        "  New boot IDE0-1 -> SCSI0-7 ",
                        "  New boot SCSI7-0 -> IDE1-0 ",
                        "  New boot IDE1-0 -> SCSI7-0 ",
                        "  Old boot SCSI0-7 -> IDE0-1 ",
                        "  Old boot IDE0-1 -> SCSI0-7 ",
                        "  Old boot SCSI7-0 -> IDE1-0 ",
                        "  Old boot IDE1-0 -> SCSI7-0 "};
char *spec_boot_log[2][2]={"Avec","Sans","With","Without"};
char *boot_log[2][2]={"  Avec ","  Sans ","  With    ","  Without "};
char *spec_ide_ctpci[2][2]={"FALCON","CTPCI","FALCON","CTPCI"};
char *ide_ctpci[2][2]={"  FALCON ","  CTPCI  ","  FALCON ","  CTPCI  "};

CPXINFO* cdecl cpx_init(XCPB *xcpb)

{
	register int i;
	long value,stack;
	COOKIE apk;
	COOKIE idt;
	HEAD *header;
	MX_KERNEL *mx_kernel;
	Xcpb=xcpb;

	if(NVMaccess(0,0,(int)(sizeof(NVM)),&nvram)<0)	/* read */
	{
		NVMaccess(2,0,0,&nvram);					/* init */
		NVMaccess(0,0,(int)(sizeof(NVM)),&nvram);	/* read */
	}

	if((*Xcpb->get_cookie)('MiNT',&value))
		mint=1;
	else
		mint=0;
	magic_date=0L;
	magic=get_MagiC_ver(&magic_date);
	if(magic)
		mx_kernel=(MX_KERNEL *)Dcntl(KER_GETINFO,NULL,0L);
	if(mint || (magic && *mx_kernel->pe_slice>=0))	/* preemptive */
		flag_cpuload=1;
	else
		flag_cpuload=0;
	stack=Super(0L);
	st_ram = *(long *)0x42e;					/* phystop, end ST-Ram */
	fast_ram = *(long *)0x5a4;					/* end Fast-Ram */
	if(fast_ram && *(long *)0x5a8==0x1357BD13L	/* ramvalid */
	 && Mxalloc(-1L,1))							/* free Fast-Ram */
		fast_ram-=0x1000000L;
	else
		fast_ram=0L;
	Super((void *)stack);
	if(init_rsc()==0)
		return(0);
#if 0
	if(!(*Xcpb->get_cookie)('_AKP',&value))
	{
		apk.ident='_AKP';						/* cookie created if not exists */
		apk.v.l=(((long)nvram.language)<<8) + (long)nvram.keyboard;
		add_cookie(&apk);
	}
	if(!(*Xcpb->get_cookie)('_IDT',&value))
	{
		idt.ident='_IDT';						/* cookie created if not exists */
		idt.v.l=(((long)nvram.datetime)<<8) + (long)nvram.separator;
		add_cookie(&idt);
	}
#endif

	eiffel_temp=NULL;
	if((*Xcpb->get_cookie)('Temp',&value))
		eiffel_temp=(char *)value;
	for(i=0;i<61;i++)
		tab_temp[i]=tab_temp_eiffel[i]=tab_cpuload[i]=0;
	
	if((head=get_header(ID_CPX))==0)
		return(0);
	header=fix_header();
	trigger_temp=header->trigger_temp;
	if(trigger_temp==0)
		trigger_temp=(MAX_TEMP*3)/4;
	if(trigger_temp>99)
		trigger_temp=99;
	daystop=header->daystop;
    if(daystop>11)
    	daystop=0;	
	timestop=header->timestop;
	beep=(int)header->beep&1;
	
	
	if(!loops_per_sec)
	{
		loops_per_sec=bogomips();
	}

	flag_xbios=1;

	return(&cpxinfo);
}

int cdecl cpx_call(GRECT *work)

{
	GRECT menu;
	TEDINFO *t_edinfo;
	long value,stack;
	static char mess_alert[256];
	HEAD *header;
	int ret;
	register int i;
	
	if((vdi_handle=Xcpb->handle)>0)
	{
		v_opnvwk(work_in,&vdi_handle,work_out);
		if(vdi_handle<=0 || (head=get_header(ID_CPX))==0)
			return(0);
	}
	else
	{
		return(0);
	}
	
	selection=PAGE_DEFAULT;
	rs_object[MENUBOX].ob_x=work->g_x;
	rs_object[MENUBOX].ob_y=work->g_y;
	rs_object[MENUBOX].ob_width=work->g_w;
	rs_object[MENUBOX].ob_height=work->g_h;

	NVMaccess(0,0,(int)(sizeof(NVM)),&nvram);

	header=fix_header();


	spec_cpuload.ub_code=cpu_load;
	spec_trace.ub_code=trace_temp;
	spec_trace.ub_parm=(long)tab_temp;
	rs_object[MENUTRACE].ob_type=G_USERDEF;
	rs_object[MENUTRACE].ob_spec.userblk=(USERBLK *)&spec_trace;
	sprintf(rs_object[MENUSTRAMTOT].ob_spec.free_string,"%9ld",st_ram/1024);
	rs_object[MENUSTRAMTOT].ob_spec.free_string[9]=' ';
	sprintf(rs_object[MENUFASTRAMTOT].ob_spec.free_string,"%9ld",fast_ram/1024);
	rs_object[MENUFASTRAMTOT].ob_spec.free_string[9]=' ';
	t_edinfo=rs_object[MENUMIPS].ob_spec.tedinfo;
	
	sprintf(t_edinfo->te_ptext,"Speed:               %3lu Mips", ((loops_per_sec+250000)/500000));
	
	language=0;
	while(language<7 && nvram.language!=code_lang[language])
		language++;
	if(language>=7)
		language=0;	
	keyboard=0;
	while(keyboard<9 && nvram.keyboard!=code_key[keyboard])
		keyboard++;
	if(keyboard>=9)
		keyboard=0;
		
	datetime=(int)nvram.datetime & 0x13;
	vmode=(int)nvram.vmode & (VERTFLAG|STMODES|OVERSCAN|PAL|VGA|COL80|NUMCOLS);

	t_edinfo=rs_object[MENUBLANG].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_lang[language];
	t_edinfo=rs_object[MENUBKEY].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_key[keyboard];
	t_edinfo=rs_object[MENUBDATE].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_date[datetime & 3];
	t_edinfo=rs_object[MENUBTIME].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_time[(datetime>>4) & 1];
	t_edinfo=rs_object[MENUSEP].ob_spec.tedinfo;
	t_edinfo->te_ptext[0]=nvram.separator;

	bootpref=0;
	while(bootpref<6 && nvram.bootpref!=(int)code_os[bootpref])
		bootpref++;
	if(bootpref>=6)
		bootpref=0;	
	t_edinfo=rs_object[MENUBOS].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_os[bootpref];
	scsi=(int)nvram.scsi & 0x87;
	i=0;
	if(scsi & 0x80)
		i++;
	t_edinfo=rs_object[MENUBARBIT].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_arbit[start_lang][i];
	t_edinfo=rs_object[MENUBIDSCSI].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_idscsi[scsi & 7];
	bootdelay=(int)nvram.bootdelay;
	if(bootdelay>99)
		bootdelay=99;
	t_edinfo=rs_object[MENUDELAY].ob_spec.tedinfo;
	sprintf(t_edinfo->te_ptext,"%d",bootdelay);
	if(flag_xbios)
	{
		tosram=(int)ct60_rw_parameter(CT60_MODE_READ,CT60_PARAM_TOSRAM,0L)&1;
		blitterspeed=(int)ct60_rw_parameter(CT60_MODE_READ,CT60_BLITTER_SPEED,0L)&1;
		cachedelay=(int)ct60_rw_parameter(CT60_MODE_READ,CT60_CACHE_DELAY,0L)&3;
		bootorder=(int)ct60_rw_parameter(CT60_MODE_READ,CT60_BOOT_ORDER,0L)&7;
		bootlog=(int)ct60_rw_parameter(CT60_MODE_READ,CT60_BOOT_LOG,0L)&1;
		cpufpu=(int)ct60_rw_parameter(CT60_MODE_READ,CT60_CPU_FPU,0L)&1;
		nv_magic_code=(int)((ct60_rw_parameter(CT60_MODE_READ,CT60_SAVE_NVRAM_1,0L)>>16) & 0xffff);
		frequency=(int)ct60_rw_parameter(CT60_MODE_READ,CT60_CLOCK,0L);
		switch(ct60_rw_parameter(CT60_MODE_READ,CT60_PARAM_CTPCI,0L))
		{
			case 0: idectpci=0; break; /* boot IDE F030 - no DMA */
			case 1: idectpci=1; break; /* boot IDE CTPCI - no DMA */
			case 2: idectpci=2; break; /* boot IDE F030 - DMA */
			case 3: idectpci=3; break; /* boot IDE CTPCI - DMA */
			default: idectpci=0; break;
		}
	}
	else
	{
		stack=Super(0L);
		tosram=(int)ct60_rw_param(CT60_MODE_READ,CT60_PARAM_TOSRAM,0L)&1;
		blitterspeed=(int)ct60_rw_param(CT60_MODE_READ,CT60_BLITTER_SPEED,0L)&1;
		cachedelay=(int)ct60_rw_param(CT60_MODE_READ,CT60_CACHE_DELAY,0L)&3;
		bootorder=(int)ct60_rw_param(CT60_MODE_READ,CT60_BOOT_ORDER,0L)&7;
		bootlog=(int)ct60_rw_param(CT60_MODE_READ,CT60_BOOT_LOG,0L)&1;
		cpufpu=(int)ct60_rw_param(CT60_MODE_READ,CT60_CPU_FPU,0L)&1;
		nv_magic_code=(int)((ct60_rw_param(CT60_MODE_READ,CT60_SAVE_NVRAM_1,0L)>>16) & 0xffff);
		frequency=(int)ct60_rw_param(CT60_MODE_READ,CT60_CLOCK,0L);
		switch(ct60_rw_param(CT60_MODE_READ,CT60_PARAM_CTPCI,0L))
		{
			case 0: idectpci=0; break; /* boot IDE F030 - no DMA */
			case 1: idectpci=1; break; /* boot IDE CTPCI - no DMA */
			case 2: idectpci=2; break; /* boot IDE F030 - DMA */
			case 3: idectpci=3; break; /* boot IDE CTPCI - DMA */
			default: idectpci=0; break;
		}
		Super((void *)stack);
	}

	frequency = ((loops_per_sec+500000)/1000000) * 1000;
	flag_frequency=0; /* modif */
	step_frequency=1000;
	min_freq=MIN_FREQ;
	max_freq=MAX_FREQ;
	if(frequency<min_freq || frequency>max_freq)
		frequency=min_freq;

	if(nv_magic_code=='NV')
		rs_object[MENUNVM].ob_state |= SELECTED;
	else
		rs_object[MENUNVM].ob_state &= ~SELECTED;

	t_edinfo=rs_object[MENUBBLITTER].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_blitter_speed[start_lang][blitterspeed];
	t_edinfo=rs_object[MENUBTOSRAM].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_tos_ram[start_lang][tosram];
	t_edinfo=rs_object[MENUBCACHE].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_cache_delay[start_lang][cachedelay];
	t_edinfo=rs_object[MENUBBOOTORDER].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_boot_order[start_lang][bootorder];
	t_edinfo=rs_object[MENUBBOOTLOG].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_boot_log[start_lang][bootlog];
	t_edinfo=rs_object[MENUBIDECTPCI].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_ide_ctpci[start_lang][idectpci&1];
	t_edinfo=rs_object[MENUBFPU].ob_spec.tedinfo;
	t_edinfo->te_ptext=spec_fpu[cpufpu];
    ed_pos=ed_objc=0;
	Work=work;
	t_edinfo=rs_object[MENUBSELECT].ob_spec.tedinfo;

	t_edinfo->te_ptext=spec_select[PAGE_DEFAULT];
	display_selection(PAGE_DEFAULT,0);
	selection=-1;
	cpx_timer(&ret);
	selection=PAGE_DEFAULT;
	no_jumper=0;

	init_slider();
	objc_edit(rs_object,ed_objc,0,&ed_pos,ED_INIT);
	new_objc=ed_objc;
	new_pos=ed_pos;
	cpx_draw(work);
	(*Xcpb->Set_Evnt_Mask)(MU_KEYBD|MU_BUTTON|MU_TIMER,0L,0L,ITIME);
#ifdef DEBUG
	printf("\r\nCPX call finished");
#endif
	return(1);					/* CPX isn't finished */
}

void cdecl cpx_draw(GRECT *clip)

{
	display_objc(0,clip);
}

void cdecl cpx_wmove(GRECT *work)

{
	rs_object[MENUBOX].ob_x=work->g_x;
	rs_object[MENUBOX].ob_y=work->g_y;
	rs_object[MENUBOX].ob_width=work->g_w;
	rs_object[MENUBOX].ob_height=work->g_h;
}

int cdecl cpx_hook(int event,WORD *msg,MRETS *mrets,int *key,int *nclicks)

{
	register int i;
	register long ret;
	register TEDINFO *t_edinfo;
	register unsigned short *p;
	if(mrets && key && nclicks);
	if(event & MU_MESAG)
	{
		switch(msg[0])
		{
		case WF_TOP:
			wi_id=msg[3];
			break;		
		case WM_REDRAW:
			wi_id=msg[3];
			break;
		}
	}
	return(0);
}

void cdecl cpx_timer(int *event)

{
	register int i,j,ret,mn;
	long value;
	unsigned int time,new_trigger,new_timestop;
	static unsigned int old_daystop=0;
	register TEDINFO *t_edinfo;
	char *p;
	if(*event);

	if (1)
	{
		switch(selection)
		{
		case PAGE_MEMORY:
			break;
		
		case PAGE_CPULOAD:
			fill_tab_cpuload();
			t_edinfo=rs_object[MENUTEMP].ob_spec.tedinfo;
			sprintf(t_edinfo->te_ptext,"%3ld  ",(long) (((long)tab_cpuload[60])*100)/MAX_CPULOAD);
			t_edinfo->te_ptext[4]='%';
			display_objc(MENUTEMP,Work);
			display_objc(MENUBARTEMP,Work);
			display_objc(MENUTRACE,Work);
			display_objc(MENUTRACE+1,Work);				
			break;


		case PAGE_TEMP:
		case -1:
			ret=current_temp();										/* temperature 68060 */
			t_edinfo=rs_object[MENUTEMP].ob_spec.tedinfo;
			sprintf(t_edinfo->te_ptext,"%3d øC",ret);
			if(ret>MAX_TEMP)
				ret=MAX_TEMP;
			i=rs_object[MENUBARTEMP].ob_width;
			ret=(i*ret)/MAX_TEMP;
			i/=3;
			rs_object[MENUBARTEMP+1].ob_flags &= ~HIDETREE;			/* green */
			if(ret<i)
			{
				rs_object[MENUBARTEMP+2].ob_flags |= HIDETREE;		/* no yellow */
				rs_object[MENUBARTEMP+3].ob_flags |= HIDETREE;		/* no red */
				rs_object[MENUBARTEMP+1].ob_width=ret;
				rs_object[MENUBARTEMP+2].ob_width=rs_object[MENUBARTEMP+3].ob_width=0;
			}
			else
			{
				if(ret<(i*2))
				{
					rs_object[MENUBARTEMP+2].ob_flags &= ~HIDETREE;	/* yellow */
					rs_object[MENUBARTEMP+3].ob_flags |= HIDETREE;	/* no red */
					rs_object[MENUBARTEMP+1].ob_width=i;			
					rs_object[MENUBARTEMP+2].ob_width=ret-i;
					rs_object[MENUBARTEMP+3].ob_width=0;
				}
				else
				{
					rs_object[MENUBARTEMP+2].ob_flags &= ~HIDETREE;	/* yellow */
					rs_object[MENUBARTEMP+3].ob_flags &= ~HIDETREE;	/* no red */
					rs_object[MENUBARTEMP+1].ob_width=i;			
					rs_object[MENUBARTEMP+2].ob_width=i;			
					rs_object[MENUBARTEMP+3].ob_width=ret-(i*2);
				}
			}
			if(selection==PAGE_TEMP)
			{
				fill_tab_temp();
				display_objc(MENUTEMP,Work);
				display_objc(MENUBARTEMP,Work);
				display_objc(MENUTRACE,Work);
				display_objc(MENUTRACE+1,Work);
			}
			break;
		}
	}
	
	t_edinfo=rs_object[MENUSTRAM].ob_spec.tedinfo;
	sprintf(t_edinfo->te_ptext,"%9ld",((unsigned long)Mxalloc(-1L,0))/1024);				/* ST-Ram */
	t_edinfo->te_ptext[9]=' ';
	t_edinfo=rs_object[MENUFASTRAM].ob_spec.tedinfo;
	sprintf(t_edinfo->te_ptext,"%9ld",((unsigned long)Mxalloc(-1L,1))/1024);				/* Fast-Ram */
	t_edinfo->te_ptext[9]=' ';
	if(selection==PAGE_MEMORY)										/* memory */
	{
		display_objc(MENUSTRAM,Work);
		display_objc(MENUFASTRAM,Work);
	}
	
	
	t_edinfo=rs_object[MENUTRIGGER].ob_spec.tedinfo;
	new_trigger=(unsigned int)atoi(t_edinfo->te_ptext);
	t_edinfo=rs_object[MENUTIME].ob_spec.tedinfo;	
	new_timestop=(((unsigned int)atoi(t_edinfo->te_ptext)/100)<<11)
                +(((unsigned int)atoi(t_edinfo->te_ptext)%100)<<5);
	if((new_trigger!=trigger_temp)
	 || (new_timestop!=timestop) || (daystop!=old_daystop))
	{
		trigger_temp=new_trigger;
		timestop=new_timestop;
		old_daystop=daystop;	
	}
}

void cdecl cpx_key(int kstate,int key,int *event)

{
	register int i,j,dial;
	register TEDINFO *t_edinfo;
	if(kstate);
	if(*event);
	dial=form_keybd(rs_object,ed_objc,ed_objc,key,&new_objc,&key);
	if(!key && dial)
	{
		if(new_objc)
		{
			t_edinfo=(TEDINFO *)rs_object[new_objc].ob_spec.tedinfo;
			for(i=0;t_edinfo->te_ptext[i];i++);
			new_pos=i;				/* cursor in end of zone edited */
		}
	}
	else
	{
		if(rs_object[ed_objc].ob_flags & EDITABLE)
		{
			switch(key & 0xff00)
			{
			case 0x7300:			/* ctrl + left */
				new_objc=ed_objc;	/* same zone */
				new_pos=0;			/* cursor at left */
				key=0;
				break;
			case 0x7400:			/* ctrl + right */
				new_objc=ed_objc;	/* same zone */
				key=0;
				t_edinfo=(TEDINFO *)rs_object[new_objc].ob_spec.tedinfo;
				for(i=0;t_edinfo->te_ptext[i];i++);
				new_pos=i;			/* cursor in end of zone */
			}
		}
	}
	if(key>0)
	{
		objc_edit(rs_object,ed_objc,key,&ed_pos,ED_CHAR);	/* text edited in usual zone */
		new_objc=ed_objc;
		new_pos=ed_pos;
	}
	if(dial)						/* if 0 => new_objc contains object EXIT */
		move_cursor();
	else
	{
		change_objc(new_objc,NORMAL,Work);
		*event=1;					/* end */
		cpx_close(0);
	}	
}

void cdecl cpx_button(MRETS *mrets,int nclicks,int *event)

{
	register int i,j,k,objc_clic,pos_clic;
	register TEDINFO *t_edinfo;
	int ret,xoff,yoff,attrib[10];
	static char mess_alert[256];
	long value,stack,offset;
	GRECT menu;
	HEAD *header;
	OBJECT *info_tree, *offset_tree;
	static MRETS old_mouse;
	EVNTDATA mouse;
	if((mrets->buttons & 2)!=0					/* right button */
	 || ((mrets->buttons & 2)==0 && (old_mouse.buttons & 2)!=0))
	{
		old_mouse.buttons=mrets->buttons;
		return;
	}
	old_mouse.buttons=mrets->buttons;
	header=(HEAD *)head->cpxhead.buffer;
	if((objc_clic=objc_find(rs_object,0,MAX_DEPTH,mrets->x,mrets->y))>=0)
	{
		if(form_button(rs_object,objc_clic,nclicks,&new_objc))
		{
			if(new_objc>0)
			{
				objc_offset(rs_object,objc_clic,&xoff,&yoff);
				t_edinfo=(TEDINFO *)rs_object[objc_clic].ob_spec.tedinfo;
				vqt_attributes(vdi_handle,attrib);
				/* attrib[8] = largeur du cadre des caractŠres */
				for(i=0;t_edinfo->te_ptmplt[i];i++);	/* size of mask string */
				if((pos_clic=rs_object[objc_clic].ob_width-i*attrib[8])>=0)
				{
					switch(t_edinfo->te_just)
					{
					case TE_RIGHT: 			/* justified to right */
						pos_clic=mrets->x-xoff-pos_clic;
						break;
					case TE_CNTR:			/* centred */
						pos_clic=mrets->x-xoff-pos_clic/2;
						break;
					case TE_LEFT:			/* justified to left */
					default:
						pos_clic=mrets->x-xoff;
					}
				}
				else
					pos_clic=mrets->x-xoff;
				new_pos=-1;
				pos_clic/=attrib[8];		/* position character checked */
				j=-1;
				do
				{
					if(t_edinfo->te_ptmplt[++j]=='_')
						new_pos++;
				}
				while(j<i && j<pos_clic);	/* end if cursor on end of string or position character checked */
				if(j>=i)
					new_pos=-1;						/* cursor at end of string */
				else
				{
					j--;
					while(t_edinfo->te_ptmplt[++j]!='_' && j<i);
					if(j>=i)
						new_pos=-1;					/* cursor at end of string */
				}
				for(i=0;t_edinfo->te_ptext[i];i++);	/* size of string text */
				if(new_pos<0 || i<new_pos)
					new_pos=i;
			}
			move_cursor();
		}
		else
		{
			switch(objc_clic)
			{
			case MENUBSELECT:
				objc_offset(rs_object,MENUBSELECT,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBSELECT].ob_width;
				menu.g_h=rs_object[MENUBSELECT].ob_height;
				i=MAX_SELECT;
				j=selection;
				k=0;
				if (!flag_cpuload)
				{
					i--;
					j--;
					k++;
				}

				ret=(*Xcpb->Popup)(&_select[k],i,j,IBM,&menu,Work);
				if(ret>=0 && k!=0)
					ret+=k; 

				if(ret>=0 && ret!=selection)
				{
					t_edinfo=rs_object[MENUBSELECT].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_select[ret];
					display_objc(MENUBSELECT,Work);
					selection=ret;
					display_selection(selection,1);
				}
				break;
					
			case MENUMIPS:
				loops_per_sec=bogomips();
				t_edinfo=rs_object[MENUMIPS].ob_spec.tedinfo;
				sprintf(t_edinfo->te_ptext,"æP: %3lu.%02lu Mips",loops_per_sec/500000,(loops_per_sec/5000) % 100);
				display_objc(MENUMIPS,Work);	
				break;
				
			case MENUBFPU:
				objc_offset(rs_object,MENUBFPU,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBFPU].ob_width;
				menu.g_h=rs_object[MENUBFPU].ob_height;
				ret=(*Xcpb->Popup)(fpu,2,cpufpu,IBM,&menu,Work);
				if(ret>=0 && ret!=cpufpu)
				{
					t_edinfo=rs_object[MENUBFPU].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_fpu[ret];
					display_objc(MENUBFPU,Work);				
					cpufpu=ret;
				}
				break;
				
			case MENUBLEFT:
			case MENUBOXSLIDER:
			case MENUSLIDER:
			case MENUBRIGHT:
				break;

			case MENUBLANG:
				objc_offset(rs_object,MENUBLANG,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBLANG].ob_width;
				menu.g_h=rs_object[MENUBLANG].ob_height;
				ret=(*Xcpb->Popup)(lang,7,language,IBM,&menu,Work);
				if(ret>=0 && ret!=language)
				{
					t_edinfo=rs_object[MENUBLANG].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_lang[ret];
					display_objc(MENUBLANG,Work);
					language=ret;
				}		
				break;
			case MENUBKEY:
				objc_offset(rs_object,MENUBKEY,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBKEY].ob_width;
				menu.g_h=rs_object[MENUBKEY].ob_height;
				ret=(*Xcpb->Popup)(key,9,keyboard,IBM,&menu,Work);
				if(ret>=0 && ret!=keyboard)
				{
					t_edinfo=rs_object[MENUBKEY].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_key[ret];
					display_objc(MENUBKEY,Work);
					keyboard=ret;
				}		
				break;
			case MENUBDATE:
				objc_offset(rs_object,MENUBDATE,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBDATE].ob_width;
				menu.g_h=rs_object[MENUBDATE].ob_height;
				ret=(*Xcpb->Popup)(date,4,datetime & 3,IBM,&menu,Work);
				if(ret>=0 && ret!=(datetime & 3))
				{
					t_edinfo=rs_object[MENUBDATE].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_date[ret];
					display_objc(MENUBDATE,Work);
					datetime &= ~3;
					datetime |= ret;
				}
				break;
			case MENUBTIME:
				objc_offset(rs_object,MENUBTIME,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBTIME].ob_width;
				menu.g_h=rs_object[MENUBTIME].ob_height;
				ret=(*Xcpb->Popup)(_time,2,(datetime>>4) & 1,IBM,&menu,Work);
				if(ret>=0 && ret!=((datetime>>4) & 1))
				{
					t_edinfo=rs_object[MENUBTIME].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_time[ret];
					display_objc(MENUBTIME,Work);
					datetime &= 3;
					datetime |= (ret<<4);
				}
				break;

			case MENUBBOOTORDER:
				objc_offset(rs_object,MENUBBOOTORDER,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBBOOTORDER].ob_width;
				menu.g_h=rs_object[MENUBBOOTORDER].ob_height;
				ret=(*Xcpb->Popup)(boot_order[start_lang],8,bootorder,IBM,&menu,Work);
				if(ret>=0 && ret!=bootorder)
				{
					t_edinfo=rs_object[MENUBBOOTORDER].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_boot_order[start_lang][ret];
					display_objc(MENUBBOOTORDER,Work);				
					bootorder=ret;
				}
				break;

			case MENUBOS:
				objc_offset(rs_object,MENUBOS,&menu.g_x,&menu.g_y);
					menu.g_w=rs_object[MENUBOS].ob_width;
				menu.g_h=rs_object[MENUBOS].ob_height;
				ret=(*Xcpb->Popup)(os,6,bootpref,IBM,&menu,Work);
				if(ret>=0 && ret!=bootpref)
				{
					t_edinfo=rs_object[MENUBOS].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_os[ret];
					display_objc(MENUBOS,Work);
					bootpref=ret;
				}
				break;

			case MENUBARBIT:
				objc_offset(rs_object,MENUBARBIT,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBARBIT].ob_width;
				menu.g_h=rs_object[MENUBARBIT].ob_height;
				ret=(*Xcpb->Popup)(arbit[start_lang],2,(scsi>>7) & 1,IBM,&menu,Work);
				if(ret>=0 && ret!=((scsi>>7) & 1))
				{
					t_edinfo=rs_object[MENUBARBIT].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_arbit[start_lang][ret];
					display_objc(MENUBARBIT,Work);
					scsi &= 0x7f;
					if(ret)
						scsi |= 0x80;
				}
				break;

			case MENUBIDSCSI:
				objc_offset(rs_object,MENUBIDSCSI,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBIDSCSI].ob_width;
				menu.g_h=rs_object[MENUBIDSCSI].ob_height;
				ret=(*Xcpb->Popup)(idscsi,8,scsi & 7,IBM,&menu,Work);
				if(ret>=0 && ret!=(scsi & 7))
				{
					t_edinfo=rs_object[MENUBIDSCSI].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_idscsi[ret];
					display_objc(MENUBIDSCSI,Work);
					scsi &= ~7;
					scsi |= ret;
				}
				break;
				
			case MENUBBLITTER:
				objc_offset(rs_object,MENUBBLITTER,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBBLITTER].ob_width;
				menu.g_h=rs_object[MENUBBLITTER].ob_height;
				ret=(*Xcpb->Popup)(blitter_speed[start_lang],2,blitterspeed,IBM,&menu,Work);
				if(ret>=0 && ret!=blitterspeed)
				{
					t_edinfo=rs_object[MENUBBLITTER].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_blitter_speed[start_lang][ret];
					display_objc(MENUBBLITTER,Work);				
					blitterspeed=ret;
				}
				break;
				
			case MENUBTOSRAM:
				objc_offset(rs_object,MENUBTOSRAM,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBTOSRAM].ob_width;
				menu.g_h=rs_object[MENUBTOSRAM].ob_height;
				ret=(*Xcpb->Popup)(tos_ram[start_lang],2,tosram,IBM,&menu,Work);
				if(ret>=0 && ret!=tosram)
				{
					t_edinfo=rs_object[MENUBTOSRAM].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_tos_ram[start_lang][ret];
					display_objc(MENUBTOSRAM,Work);				
					tosram=ret;
				}
				break;
				
			case MENUBCACHE:
				objc_offset(rs_object,MENUBCACHE,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBCACHE].ob_width;
				menu.g_h=rs_object[MENUBCACHE].ob_height;
				ret=(*Xcpb->Popup)(cache_delay[start_lang],4,cachedelay,IBM,&menu,Work);
				if(ret>=0 && ret!=cachedelay)
				{
					t_edinfo=rs_object[MENUBCACHE].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_cache_delay[start_lang][ret];
					display_objc(MENUBCACHE,Work);				
					cachedelay=ret;
				}
				break;
				
			case MENUBBOOTLOG:
				objc_offset(rs_object,MENUBBOOTLOG,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBBOOTLOG].ob_width;
				menu.g_h=rs_object[MENUBBOOTLOG].ob_height;
				ret=(*Xcpb->Popup)(boot_log[start_lang],2,bootlog,IBM,&menu,Work);
				if(ret>=0 && ret!=bootlog)
				{
					t_edinfo=rs_object[MENUBBOOTLOG].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_boot_log[start_lang][ret];
					display_objc(MENUBBOOTLOG,Work);				
					bootlog=ret;
				}
				break;
				
			case MENUBIDECTPCI:
				objc_offset(rs_object,MENUBIDECTPCI,&menu.g_x,&menu.g_y);
				menu.g_w=rs_object[MENUBIDECTPCI].ob_width;
				menu.g_h=rs_object[MENUBIDECTPCI].ob_height;
				ret=(*Xcpb->Popup)(ide_ctpci[start_lang],2,idectpci&1,IBM,&menu,Work);
				if(ret>=0 && ret!=(idectpci&1))
				{
					t_edinfo=rs_object[MENUBIDECTPCI].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_ide_ctpci[start_lang][ret];
					display_objc(MENUBIDECTPCI,Work);
					idectpci &= ~1;
					idectpci |= ret;
				}
				break;

			case MENUBSAVE:
				head->cpxhead.flags.ram_resident=head->cpxhead.flags.boot_init=1;
				header->language=code_lang[language];
				header->keyboard=code_key[keyboard];
				header->datetime=(unsigned char)datetime;
				t_edinfo=rs_object[MENUSEP].ob_spec.tedinfo;
				header->separator=t_edinfo->te_ptext[0];
				header->vmode=(unsigned int)vmode;
				header->bootpref=(unsigned int)code_os[bootpref];
				header->scsi=(unsigned char)scsi;
				t_edinfo=rs_object[MENUDELAY].ob_spec.tedinfo;
				header->bootdelay=(unsigned char)atoi(t_edinfo->te_ptext);
				header->blitterspeed=(unsigned char)blitterspeed;
				header->tosram=(unsigned char)tosram;
				header->cachedelay=(unsigned char)cachedelay;
				header->bootorder=(unsigned char)bootorder;
				header->bootlog=(unsigned char)bootlog;
				header->cpufpu=(unsigned char)cpufpu;
				header->idectpci=(unsigned char)idectpci;
				t_edinfo=rs_object[MENUTRIGGER].ob_spec.tedinfo;
				header->trigger_temp=(unsigned int)atoi(t_edinfo->te_ptext);
				header->daystop=daystop;
				t_edinfo=rs_object[MENUTIME].ob_spec.tedinfo;				
				header->timestop=(((unsigned int)atoi(t_edinfo->te_ptext)/100)<<11)
				                +(((unsigned int)atoi(t_edinfo->te_ptext)%100)<<5);
                header->frequency=frequency;
                header->beep=(unsigned char)beep;
                if(nclicks<=1)
                {
					save_header();

				}
				change_objc(MENUBSAVE,NORMAL,Work);
				break;
				
 			case MENUBLOAD:
				ret=form_alert(1,"[2][Load saved parameters?][Load|Cancel]");
				if(ret==1)
				{
					selection=PAGE_DEFAULT;
					t_edinfo=rs_object[MENUBSELECT].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_select[selection];
					language=0;
					while(language<7 && header->language!=code_lang[language])
					language++;
					if(language>=7)
						language=0;	
					keyboard=0;
					while(keyboard<9 && header->keyboard!=code_key[keyboard])
						keyboard++;
					if(keyboard>=9)
						keyboard=0;
					datetime=(int)header->datetime;
					vmode=(int)header->vmode;

					t_edinfo=rs_object[MENUBLANG].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_lang[language];
					t_edinfo=rs_object[MENUBKEY].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_key[keyboard];
					t_edinfo=rs_object[MENUBDATE].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_date[datetime & 3];
					t_edinfo=rs_object[MENUBTIME].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_time[(datetime>>4) & 1];
					t_edinfo=rs_object[MENUSEP].ob_spec.tedinfo;
					t_edinfo->te_ptext[0]=header->separator;

					bootpref=0;
					while(bootpref<6 && header->bootpref!=(int)code_os[bootpref])
						bootpref++;
					if(bootpref>=6)
						bootpref=0;	
					t_edinfo=rs_object[MENUBOS].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_os[bootpref];
					scsi=(int)header->scsi;
					i=0;
					if(scsi & 0x80)
						i++;
					t_edinfo=rs_object[MENUBARBIT].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_arbit[start_lang][i];
					t_edinfo=rs_object[MENUBIDSCSI].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_idscsi[scsi & 7];
					bootdelay=(int)header->bootdelay;
					if(bootdelay>99)
						bootdelay=99;
					t_edinfo=rs_object[MENUDELAY].ob_spec.tedinfo;
					sprintf(t_edinfo->te_ptext,"%d",bootdelay);
					blitterspeed=(int)header->blitterspeed;
					t_edinfo=rs_object[MENUBBLITTER].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_blitter_speed[start_lang][blitterspeed];
					tosram=(int)header->tosram;
					t_edinfo=rs_object[MENUBTOSRAM].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_tos_ram[start_lang][tosram];
					cachedelay=(int)header->cachedelay;
					t_edinfo=rs_object[MENUBCACHE].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_cache_delay[start_lang][cachedelay];
					bootorder=(int)header->bootorder;					
					t_edinfo=rs_object[MENUBBOOTORDER].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_boot_order[start_lang][bootorder];
					bootlog=(int)header->bootlog;					
					t_edinfo=rs_object[MENUBBOOTLOG].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_boot_log[start_lang][bootlog];
					idectpci=(int)header->idectpci;					
					t_edinfo=rs_object[MENUBIDECTPCI].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_ide_ctpci[start_lang][idectpci&1];
					cpufpu=(int)header->cpufpu;
					t_edinfo=rs_object[MENUBFPU].ob_spec.tedinfo;
					t_edinfo->te_ptext=spec_fpu[cpufpu];

					init_slider();
					display_objc(MENUBSELECT,Work);
					display_selection(selection,1);
				}
				change_objc(MENUBLOAD,NORMAL,Work);
				break;
				
			case MENUBOK:
_ok_:

				nvram.language=code_lang[language];
				nvram.keyboard=code_key[keyboard];
				nvram.datetime=(unsigned char)datetime;
				t_edinfo=rs_object[MENUSEP].ob_spec.tedinfo;
				nvram.separator=t_edinfo->te_ptext[0];
				nvram.vmode=(unsigned int)vmode;
				nvram.bootpref=(unsigned int)code_os[bootpref];
				nvram.scsi=(unsigned char)scsi;
				t_edinfo=rs_object[MENUDELAY].ob_spec.tedinfo;
				nvram.bootdelay=(unsigned char)atoi(t_edinfo->te_ptext);
				NVMaccess(1,0,(int)(sizeof(NVM)),&nvram);		/* write */
				if(flag_xbios)
				{
					tosram=(int)ct60_rw_parameter(CT60_MODE_WRITE,CT60_PARAM_TOSRAM,(long)tosram);
					blitterspeed=(int)ct60_rw_parameter(CT60_MODE_WRITE,CT60_BLITTER_SPEED,(long)blitterspeed);
					cachedelay=(int)ct60_rw_parameter(CT60_MODE_WRITE,CT60_CACHE_DELAY,(long)cachedelay);
					bootorder=(int)ct60_rw_parameter(CT60_MODE_WRITE,CT60_BOOT_ORDER,(long)bootorder);
					bootlog=(int)ct60_rw_parameter(CT60_MODE_WRITE,CT60_BOOT_LOG,(long)bootlog);
					cpufpu=(int)ct60_rw_parameter(CT60_MODE_WRITE,CT60_CPU_FPU,(long)cpufpu);
					ct60_rw_parameter(CT60_MODE_WRITE,CT60_CLOCK,(long)frequency);
#ifndef LIGHT
					ct60_rw_parameter(CT60_MODE_WRITE,CT60_SAVE_NVRAM_1,
					 (long)((((unsigned long)nv_magic_code)<<16)+(unsigned long)nvram.bootpref));
					ct60_rw_parameter(CT60_MODE_WRITE,CT60_SAVE_NVRAM_2,
					 (long)((((unsigned long)nvram.language)<<24)
					 + (((unsigned long)nvram.keyboard)<<16) 
					 + (((unsigned long)nvram.datetime)<<8)
					 + (unsigned long)nvram.separator));
					ct60_rw_parameter(CT60_MODE_WRITE,CT60_SAVE_NVRAM_3,
					 (long)((((unsigned long)nvram.bootdelay)<<24)
					 + (((unsigned long)nvram.vmode)<<8)
					 + (unsigned long)nvram.scsi));
#endif
					ct60_rw_parameter(CT60_MODE_WRITE,CT60_PARAM_CTPCI,(long)idectpci);
				}
				else
				{
					stack=Super(0L);
					tosram=(int)ct60_rw_param(CT60_MODE_WRITE,CT60_PARAM_TOSRAM,(long)tosram);
					blitterspeed=(int)ct60_rw_param(CT60_MODE_WRITE,CT60_BLITTER_SPEED,(long)blitterspeed);
					cachedelay=(int)ct60_rw_param(CT60_MODE_WRITE,CT60_CACHE_DELAY,(long)cachedelay);
					bootorder=(int)ct60_rw_param(CT60_MODE_WRITE,CT60_BOOT_ORDER,(long)bootorder);
					bootlog=(int)ct60_rw_param(CT60_MODE_WRITE,CT60_BOOT_LOG,(long)bootlog);
					cpufpu=(int)ct60_rw_param(CT60_MODE_WRITE,CT60_CPU_FPU,(long)cpufpu);
					ct60_rw_param(CT60_MODE_WRITE,CT60_CLOCK,(long)frequency);
#ifndef LIGHT
					ct60_rw_param(CT60_MODE_WRITE,CT60_SAVE_NVRAM_1,
					 (long)((((unsigned long)nv_magic_code)<<16)+(unsigned long)nvram.bootpref));
					ct60_rw_param(CT60_MODE_WRITE,CT60_SAVE_NVRAM_2,
					 (long)((((unsigned long)nvram.language)<<24)
					 + (((unsigned long)nvram.keyboard)<<16) 
					 + (((unsigned long)nvram.datetime)<<8)
					 + (unsigned long)nvram.separator));
					ct60_rw_param(CT60_MODE_WRITE,CT60_SAVE_NVRAM_3,
					 (long)((((unsigned long)nvram.bootdelay)<<24)
					 + (((unsigned long)nvram.vmode)<<8)
					 + (unsigned long)nvram.scsi));
#endif
					ct60_rw_param(CT60_MODE_WRITE,CT60_PARAM_CTPCI,(long)idectpci);
					Super((void *)stack);
				}
				rs_object[MENUBOK].ob_state &= ~SELECTED;
				*event=1;	/* end */
				cpx_close(0);
				break;
			case MENUBCANCEL:
				rs_object[MENUBCANCEL].ob_state &= ~SELECTED;
				*event=1;	/* end */
				cpx_close(0);
				break;
			case MENUBINFO:
				change_objc(MENUBINFO,NORMAL,Work);
				if((info_tree=adr_tree(TREE2))!=0)
				{
					info_tree[INFOSDRAM].ob_flags |= (SELECTABLE|EXIT);
					info_tree[INFOSDRAM].ob_state &= ~DISABLED;

					switch(hndl_form(info_tree,0))
					{
					case INFOSDRAM :
						break;
					case INFOHELP:
						break;
					}
				}
				break;
			}
		}
	}
}

void cdecl cpx_close(int flag)

{
	int x,y,m,k;
	if(flag);
	objc_edit(rs_object,ed_objc,0,&ed_pos,ED_END);
	v_clsvwk(vdi_handle);
}

int init_rsc(void)

{
	OBJECT *info_tree,*alert_tree;
	BITBLK *b_itblk;
	char **rs_str;
	register int h;
	long value;
	if(rs_object[MENUBINFO].ob_height==1)
	/* flag SkipRshFix is bugged with XCONTROL and ZCONTROL when it's used in cpx_init */
	{
		start_lang=1;
		rs_str=rs_strings_en;

		(*Xcpb->rsh_fix)(NUM_OBS,NUM_FRSTR,NUM_FRIMG,NUM_TREE,rs_object,rs_tedinfo,rs_str,rs_iconblk,rs_bitblk,rs_frstr,rs_frimg,rs_trindex,rs_imdope);
		
		if(rs_object[MENUBINFO].ob_height==1)
			return(0);	/* error */
		else
		{
			gr_hwchar=rs_object[MENUBINFO].ob_width>>1;
			gr_hhchar=rs_object[MENUBINFO].ob_height;
			h=gr_hhchar>>1;
			rs_object[MENUBSELECT].ob_y+=2;
			rs_object[MENUBOXSTATUS].ob_y+=(h+1);
			rs_object[MENUBOXSTATUS].ob_height+=h;
			rs_object[MENUTEMP-1].ob_y-=h;
			rs_object[MENUTEMP].ob_y-=h;		
			rs_object[MENUBARTEMP+1].ob_height=rs_object[MENUBARTEMP+2].ob_height=rs_object[MENUBARTEMP+3].ob_height=h;
			rs_object[MENUTRIGGER].ob_y-=h;
			rs_object[MENUTRACE].ob_x+=6;
			rs_object[MENUTRACE].ob_y=(7*gr_hhchar)>>2;
			rs_object[MENUTRACE].ob_height=gr_hhchar<<2;
			rs_object[MENUTRACE+1].ob_height=rs_object[MENUTRACE+2].ob_height=rs_object[MENUTRACE+3].ob_height=rs_object[MENUTRACE+4].ob_height=6;
			rs_object[MENUTRACE+2].ob_width=rs_object[MENUTRACE+3].ob_width=rs_object[MENUTRACE+4].ob_width=12;	
			rs_object[MENUTRACE+2].ob_x++;
			rs_object[MENUTRACE+3].ob_x++;
			rs_object[MENUTRACE+4].ob_x++;
			rs_object[MENUTRACE+4].ob_y+=h;
			rs_object[MENUBOXRAM].ob_y+=(h+1);
			rs_object[MENUBOXRAM].ob_height+=h;
			rs_object[MENUSTRAMTOT-1].ob_y-=(h+1);
			rs_object[MENUSTRAMTOT].ob_y-=(h+1);
			rs_object[MENUFASTRAMTOT-1].ob_y-=(h+2);
			rs_object[MENUFASTRAMTOT].ob_y-=(h+2);			
			rs_object[MENUSTRAM-1].ob_y-=(h+3);
			rs_object[MENUSTRAM].ob_y-=(h+3);
			rs_object[MENUFASTRAM-1].ob_y-=(h+4);
			rs_object[MENUFASTRAM].ob_y-=(h+4);				
			rs_object[MENUMIPS].ob_y-=(h+5);
			rs_object[MENUBFPU-1].ob_y-=(h+3);
			rs_object[MENUBFPU].ob_y-=(h+3);
			rs_object[MENUBLEFT].ob_x--;
			rs_object[MENUBLEFT].ob_y-=(h+2);
			rs_object[MENUBOXSLIDER].ob_y-=(h+2);
			rs_object[MENUBRIGHT].ob_x++;
			rs_object[MENUBRIGHT].ob_y-=(h+2);
			rs_object[MENUBOXLANG].ob_y+=(h+1);
			rs_object[MENUBOXLANG].ob_height+=h;
			rs_object[MENUBLANG-1].ob_y-=h;
			rs_object[MENUBLANG].ob_y-=h;
			rs_object[MENUBDATE-1].ob_y+=h;
			rs_object[MENUBDATE].ob_y+=h;
			rs_object[MENUBOXVIDEO].ob_y+=(h+1);
			rs_object[MENUBOXVIDEO].ob_height+=h;
			rs_object[MENUBVIDEO-1].ob_y-=h;
			rs_object[MENUBVIDEO].ob_y-=h;
			rs_object[MENUBMODE-1].ob_y-=h;
			rs_object[MENUBMODE].ob_y-=h;		
			rs_object[MENUBCOUL-1].ob_y+=h;
			rs_object[MENUBCOUL].ob_y+=h;
			rs_object[MENUSTMODES].ob_y+=h;
			rs_object[MENUBOXBOOT].ob_y+=(h+1);
			rs_object[MENUBOXBOOT].ob_height+=h;
			rs_object[MENUBBOOTORDER].ob_y-=h;
			rs_object[MENUBOS-1].ob_y-=h;
			rs_object[MENUBOS].ob_y-=h;
			rs_object[MENUBARBIT-1].ob_y-=(h-3);
			rs_object[MENUBARBIT].ob_y-=(h-3);
			rs_object[MENUBIDSCSI-1].ob_y-=(h-3);
			rs_object[MENUBIDSCSI].ob_y-=(h-3);
			rs_object[MENUDELAY].ob_y-=(h-6);
			rs_object[MENUBBLITTER-1].ob_y-=(h-6);
			rs_object[MENUBBLITTER].ob_y-=(h-6);
			rs_object[MENUBTOSRAM-1].ob_y+=(h-7);
			rs_object[MENUBTOSRAM].ob_y+=(h-7);
			rs_object[MENUBCACHE-1].ob_y+=(h-7);
			rs_object[MENUBCACHE].ob_y+=(h-7);
			rs_object[MENUBBOOTLOG-1].ob_y+=(h-4);
			rs_object[MENUBBOOTLOG].ob_y+=(h-4);
			rs_object[MENUBIDECTPCI-1].ob_y+=(h-4);
			rs_object[MENUBIDECTPCI].ob_y+=(h-4);
			rs_object[MENUBIDECTPCI-1].ob_flags |= HIDETREE;	
			rs_object[MENUBIDECTPCI].ob_flags |= HIDETREE;

			rs_object[MENUBOXSTOP].ob_y+=(h+1);
			rs_object[MENUBOXSTOP].ob_height+=h;
			rs_object[MENUBSAVE].ob_y+=h;
			rs_object[MENUBLOAD].ob_y+=h;
			rs_object[MENUBOK].ob_y+=h;
			rs_object[MENUBCANCEL].ob_y+=h;
			rs_object[MENUBINFO].ob_y+=h;
			if((info_tree=adr_tree(TREE2))!=0)
			{
				info_tree[INFOLOGO].ob_flags |= HIDETREE;		
			}
			if((alert_tree=adr_tree(TREE3))!=0)
			{
				b_itblk=alert_tree[ALERTNOTE].ob_spec.bitblk;
				b_itblk->bi_pdata=(int *)pic_note;
				b_itblk=alert_tree[ALERTWAIT].ob_spec.bitblk;
				b_itblk->bi_pdata=(int *)pic_wait;
				b_itblk=alert_tree[ALERTSTOP].ob_spec.bitblk;
				b_itblk->bi_pdata=(int *)pic_stop;
			}
		}
	}

	return(1);
}

OBJECT* adr_tree(int num_tree)

{
	register int i,tree;
	if(!num_tree)
		return(rs_object);
	for(i=tree=0;i<NUM_OBS;i++)
	{
		if(rs_object[i].ob_flags & LASTOB)
		{
			tree++;
			if(tree==num_tree)
				return(&rs_object[i+1]);
		}
	}
	return(0L);
}

void init_slider(void)

{
	TEDINFO *t_edinfo;
	if(frequency)
	{
		rs_object[MENUBLEFT].ob_flags &= ~HIDETREE;	
		rs_object[MENUBOXSLIDER].ob_flags &= ~HIDETREE;
		rs_object[MENUSLIDER].ob_flags &= ~HIDETREE;
		rs_object[MENUBRIGHT].ob_flags &= ~HIDETREE;
		rs_object[MENUSLIDER].ob_x = (int)
		 (((frequency-min_freq) * (unsigned long)(rs_object[MENUBOXSLIDER].ob_width - rs_object[MENUSLIDER].ob_width))/(max_freq-min_freq));
		t_edinfo=rs_object[MENUSLIDER].ob_spec.tedinfo;
		sprintf(t_edinfo->te_ptext,"%ldMhz",frequency/1000);
	}
	else
	{
		rs_object[MENUBLEFT].ob_flags |= HIDETREE;	
		rs_object[MENUBOXSLIDER].ob_flags |= HIDETREE;
		rs_object[MENUSLIDER].ob_flags |= HIDETREE;
		rs_object[MENUBRIGHT].ob_flags |= HIDETREE;
	}
}

void display_slider(GRECT *work)

{
	init_slider();
	display_objc(MENUBOXSLIDER,work);
}


int read_hexa(char *p)

{
	return((ascii_hexa(p)<<4) | ascii_hexa(p+1));
}

int ascii_hexa(char *p)

{
	register int val;
	if((val=p[0]-'0')<0)
		return(-1);
	if(val>9)
	{
		val-=7;
		if(val<10 || val>15)
			return(-1);
	}
	return(val);
}	

void write_hexa(int val,char *p)

{
	val&=0xff;
	hexa_ascii(val>>4,p++);
	hexa_ascii(val & 0xf,p);
}

void hexa_ascii(int val,char *p)

{
	val&=0xf;
	val+='0';
	if(val>'9')
		val+=7;
	*p=(char)val;
}

void display_error(int err)

{
	if(err<0)
	{
		switch(err)
		{
		case -33:
			(*Xcpb->XGen_Alert)(FILE_NOT_FOUND);
			break;
		case -39:
		case -35:
		case -40:
		case -67:
			(*Xcpb->XGen_Alert)(MEM_ERR);
			break;
		default:
			(*Xcpb->XGen_Alert)(FILE_ERR);
		}
	}
}

int cdecl trace_temp(PARMBLK *parmblock)

{
	register int i,color,temp,temp2,x,y,w,h;
	int tab_clip[4],tab_l[10],tab_f[4],attrib_l[6],attrib_f[5],xy[6];
	unsigned short *p;
	if(	parmblock->pb_prevstate==parmblock->pb_currstate)
	{
		tab_clip[0]=parmblock->pb_xc;
		tab_clip[1]=parmblock->pb_yc;
		tab_clip[2]=parmblock->pb_wc+tab_clip[0]-1;
		tab_clip[3]=parmblock->pb_hc+tab_clip[1]-1;
		vs_clip(vdi_handle,1,tab_clip);			/* clipping */
		tab_f[0]=x=parmblock->pb_x;
		tab_f[1]=y=parmblock->pb_y;
		w=parmblock->pb_w;
		h=parmblock->pb_h;
		tab_f[2]=w+tab_f[0]-1;
		tab_f[3]=h+tab_f[1]-1;
		tab_l[0]=tab_l[2]=tab_l[8]=tab_f[0]-1;
		tab_l[1]=tab_l[7]=tab_l[9]=tab_f[1]-1;
		tab_l[3]=tab_l[5]=tab_f[3]+1;
		tab_l[4]=tab_l[6]=tab_f[2]+1;
		vql_attributes(vdi_handle,attrib_l);	/* save lines attributes */
		vqf_attributes(vdi_handle,attrib_f);	/* save filling attributes */
		vsl_type(vdi_handle,1);
		vsl_color(vdi_handle,WHITE);			/* color line */
		vswr_mode(vdi_handle,MD_REPLACE);
		vsl_ends(vdi_handle,0,0);
		vsl_width(vdi_handle,1);
		vsf_interior(vdi_handle,1);				/* color */
		vsf_color(vdi_handle,BLACK);
		vsf_perimeter(vdi_handle,1);
		vr_recfl(vdi_handle,tab_f);
		v_pline(vdi_handle,5,tab_l);			/* border */
		vswr_mode(vdi_handle,MD_TRANS);
		vsl_type(vdi_handle,3);					/* type line: dotted line */
		for(i=10;i<60;i+=10)					/* vertical square */
		{
			xy[0]=xy[2]=x+((i * w) / 60);
			xy[1]=y;
			xy[3]=y+h-1;
			v_pline(vdi_handle,2,xy);
 		}
		p=(unsigned short *)parmblock->pb_parm;	/* trace */
		if(p==tab_cpuload)						/* cpu_load */
		{
			for(i=20;i<100;i+=20)				/* horizontal square */
			{
				xy[0]=x;
				xy[1]=xy[3]=y+h-((i * h) / 100);
				xy[2]=x+w-1;
				v_pline(vdi_handle,2,xy);
			}
			vsl_type(vdi_handle,1);				/* type line: full line */
			for(i=0;i<60;i++)					/* trace */
			{
				temp = (int)*p++;
				temp2 =(int)*p;
				vsl_color(vdi_handle,CYAN);
				xy[0]=x+((i * w) / 60);
				xy[1]=y+h-(int)(((long)temp * (long)h) / (long)MAX_CPULOAD);
				xy[2]=x+(((i+1) * w) / 60);
				xy[3]=y+h-(int)(((long)temp2 * (long)h) / (long)MAX_CPULOAD);
				v_pline(vdi_handle,2,xy);
			}
		}
		else										/* temperature */            
		{
			for(i=20;i<MAX_TEMP;i+=20)				/* horizontal square */
			{
				xy[0]=x;
				xy[1]=xy[3]=y+h-((i * h) / MAX_TEMP);
				xy[2]=x+w-1;
				v_pline(vdi_handle,2,xy);
			}
			vsl_type(vdi_handle,1);					/* type line: full line */
			if(p==tab_temp && eiffel_temp!=NULL)
			{
				p=tab_temp_eiffel;
				for(i=0;i<60;i++)					/* trace */
				{
					temp = (int)(*p++ & 0x3F);
					temp2 = (int)(*p & 0x3F);
					if(*p & 0x8000)					/* motor on */
						vsl_color(vdi_handle,RED);
					else
						vsl_color(vdi_handle,BLUE);
					xy[0]=x+((i * w) / 60);
					xy[1]=y+h-(int)(((long)temp * (long)h) / MAX_TEMP);
					xy[2]=x+(((i+1) * w) / 60);
					xy[3]=y+h-(int)(((long)temp2 * (long)h) / MAX_TEMP);
					v_pline(vdi_handle,2,xy);
				}
				p=tab_temp;
			}
			for(i=0;i<60;i++)						/* trace */
			{
				temp = (int)*p++;
				temp2 = (int)*p;
				if(temp < MAX_TEMP/3)
					color=GREEN;
				else
				{
					if(temp < (MAX_TEMP*2)/3)
						color=YELLOW;
					else
						color=RED;
				}
				vsl_color(vdi_handle,color);	/* beginning with the 1st color */
				xy[0]=x+((i * w) / 60);
				xy[1]=y+h-((temp * h) / MAX_TEMP);
				xy[4]=x+(((i+1) * w) / 60);
				xy[5]=y+h-((temp2 * h) / MAX_TEMP);
				xy[2]=(xy[0]+xy[4])>>1;
				xy[3]=(xy[1]+xy[5])>>1;
				v_pline(vdi_handle,2,xy);		/* 1st segment */
				if(temp2 < MAX_TEMP/3)
					color=GREEN;
				else
				{
					if(temp2 < (MAX_TEMP*2)/3)
						color=YELLOW;
					else
						color=RED;
				}
				vsl_color(vdi_handle,color);	/* end maybe with 2nd color */
				v_pline(vdi_handle,2,&xy[2]);	/* 2nd segment */
			}
		}		
		vs_clip(vdi_handle,0,tab_clip);
		vsl_type(vdi_handle,attrib_l[0]);		/* restore: type line */
		vsl_color(vdi_handle,attrib_l[1]);				/* color line */
		vswr_mode(vdi_handle,attrib_l[2]);				/* graphic modec */
		vsl_ends(vdi_handle,attrib_l[3],attrib_l[4]);	/* line aspect */
		vsl_width(vdi_handle,attrib_l[5]);				/* width line */
		vsf_interior(vdi_handle,attrib_f[0]);			/* type filling */
		vsf_color(vdi_handle,attrib_f[1]);				/* color fillinge */
		vsf_style(vdi_handle,attrib_f[2]);				/* motif filling */
		vsf_perimeter(vdi_handle,attrib_f[4]);			/* state border */
	}
	return(0);
}

int cdecl cpu_load(PARMBLK *parmblock)

{
	register int i,scale,x,y,w,h;
	int tab_clip[4],tab_f[4],attrib_l[6],attrib_f[5],xy[4];
	unsigned int *p;
	if(	parmblock->pb_prevstate==parmblock->pb_currstate)
	{
		tab_clip[0]=parmblock->pb_xc;
		tab_clip[1]=parmblock->pb_yc;
		tab_clip[2]=parmblock->pb_wc+tab_clip[0]-1;
		tab_clip[3]=parmblock->pb_hc+tab_clip[1]-1;
		vs_clip(vdi_handle,1,tab_clip);			/* clipping */
		tab_f[0]=x=parmblock->pb_x;
		tab_f[1]=y=parmblock->pb_y;
		w=parmblock->pb_w;
		h=parmblock->pb_h;
		tab_f[2]=w+tab_f[0]-1;
		tab_f[3]=h+tab_f[1]-1;
		vql_attributes(vdi_handle,attrib_l);	/* save lines attributes */
		vqf_attributes(vdi_handle,attrib_f);	/* save filling attributes */
		vsl_type(vdi_handle,1);
		vsl_color(vdi_handle,CYAN);				/* color line */
		vswr_mode(vdi_handle,MD_REPLACE);
		vsl_ends(vdi_handle,0,0);
		vsl_width(vdi_handle,1);
		vsf_interior(vdi_handle,1);				/* color */
		vsf_color(vdi_handle,BLACK);
		vsf_perimeter(vdi_handle,1);
		vr_recfl(vdi_handle,tab_f);
		scale=(int)((parmblock->pb_parm*((long)w-3L))/100L);
		if(scale > w-3)
			scale=w-3;
		xy[1]=y+3;
		xy[3]=y+h-4;
		for(i=3;i<scale;i+=4)
		{
			xy[0]=xy[2]=x+i;	
			v_pline(vdi_handle,2,xy);
			xy[0]++;
			xy[2]++;
			v_pline(vdi_handle,2,xy);
		}
		vs_clip(vdi_handle,0,tab_clip);
		vsl_type(vdi_handle,attrib_l[0]);		/* restore: type line */
		vsl_color(vdi_handle,attrib_l[1]);				/* color line */
		vswr_mode(vdi_handle,attrib_l[2]);				/* graphic modec */
		vsl_ends(vdi_handle,attrib_l[3],attrib_l[4]);	/* line aspect */
		vsl_width(vdi_handle,attrib_l[5]);				/* width line */
		vsf_interior(vdi_handle,attrib_f[0]);			/* type filling */
		vsf_color(vdi_handle,attrib_f[1]);				/* color fillinge */
		vsf_style(vdi_handle,attrib_f[2]);				/* motif filling */
		vsf_perimeter(vdi_handle,attrib_f[4]);			/* state border */
	}
	return(0);
}

CPXNODE* get_header(long id)

{
	register CPXNODE *p;
	p=(CPXNODE *)(*Xcpb->Get_Head_Node)();	/* header 1st CPX */
	do
	{
		if(p->cpxhead.cpx_id==id)
			return(p);
	}
	while(p->vacant && (p=p->next)!=0);		/* no more headers */
	return(0L);
}

HEAD *fix_header(void)

{
	HEAD *header;
	header=(HEAD *)head->cpxhead.buffer;
	if(header->bootpref==0 && header->language==0 && header->keyboard==0
	 && header->datetime==0 && header->separator==0 && header->bootdelay==0
	 && header->vmode==0 && header->scsi==0 && header->tosram==0
	 && header->trigger_temp==0 && header->daystop==0 && header->timestop==0
	 && header->blitterspeed==0 && header->cachedelay==0 && header->bootorder==0
	 && header->bootlog==0 && header->cpufpu==0 && header->beep==0)
		*header=config;	/* buffer of header is always to 0 with ZCONTROL */
	return(header);
}

void save_header(void)

{
	HEAD *header;
	if((*Xcpb->XGen_Alert)(SAVE_DEFAULTS))
	{
		header=(HEAD *)head->cpxhead.buffer;
		if(((*Xcpb->Save_Header)(head))==0) /* not works with ZCONTROL */
			(*Xcpb->XGen_Alert)(FILE_ERR);
		config=*header;
		if(((*Xcpb->CPX_Save)((void *)&config,sizeof(HEAD)))==0)
			(*Xcpb->XGen_Alert)(FILE_ERR);	
	}
}

void display_selection(int selection,int flag_aff)

{
	static char *status[2]={" CPU load "," Temperature "};
	TEDINFO *t_edinfo;
	if(flag_aff)
		display_objc(MENUBSELECT+1,Work);
	switch(selection)
	{
	case PAGE_CPULOAD:			/* average load */
		rs_object[MENUBOXSTATUS].ob_flags &= ~HIDETREE;
		rs_object[MENUSTATUS].ob_flags &= ~HIDETREE;
		t_edinfo=rs_object[MENUSTATUS].ob_spec.tedinfo;
		t_edinfo->te_ptext=status[0];
		rs_object[MENUTEMP-1].ob_flags |= HIDETREE;
		t_edinfo=rs_object[MENUTEMP].ob_spec.tedinfo;
		sprintf(t_edinfo->te_ptext,"%3ld  ",spec_cpuload.ub_parm);
		t_edinfo->te_ptext[4]='%';
		rs_object[MENUTEMP].ob_x=gr_hwchar;
		rs_object[MENUBARTEMP].ob_type=G_USERDEF;
		rs_object[MENUBARTEMP].ob_spec.userblk=(USERBLK *)&spec_cpuload;
		rs_object[MENUBARTEMP].ob_x=gr_hwchar<<3;
		rs_object[MENUBARTEMP].ob_y=gr_hhchar>>1;
		rs_object[MENUBARTEMP].ob_width=gr_hwchar*23;
		rs_object[MENUBARTEMP].ob_height=gr_hhchar;
		rs_object[MENUBARTEMP+1].ob_flags |= HIDETREE;
		rs_object[MENUBARTEMP+2].ob_flags |= HIDETREE;
		rs_object[MENUBARTEMP+3].ob_flags |= HIDETREE;
		rs_object[MENUTRIGGER].ob_flags &= ~EDITABLE;
		rs_object[MENUTRIGGER].ob_flags |= HIDETREE;
		rs_object[MENUTRACE+2].ob_y=rs_object[MENUTRACE].ob_y+rs_object[MENUTRACE].ob_height
		                           -((80*rs_object[MENUTRACE].ob_height)/(MAX_CPULOAD/100))-2;
		rs_object[MENUTRACE+3].ob_y=rs_object[MENUTRACE].ob_y+rs_object[MENUTRACE].ob_height
		                           -((40*rs_object[MENUTRACE].ob_height)/(MAX_CPULOAD/100))-2;
		t_edinfo=rs_object[MENUTRACE+4].ob_spec.tedinfo;
		t_edinfo->te_ptext="0%";
		rs_object[MENUBOXRAM].ob_flags |= HIDETREE;
		rs_object[MENURAM].ob_flags |= HIDETREE;
		rs_object[MENUBOXLANG].ob_flags |= HIDETREE;
		rs_object[MENULANG].ob_flags |= HIDETREE;
		rs_object[MENUSEP].ob_flags &= ~EDITABLE;	
		rs_object[MENUBOXVIDEO].ob_flags |= HIDETREE;
		rs_object[MENUVIDEO].ob_flags |= HIDETREE;
		rs_object[MENUBOXBOOT].ob_flags |= HIDETREE;
		rs_object[MENUBOOT].ob_flags |= HIDETREE;
		rs_object[MENUDELAY].ob_flags &= ~EDITABLE;
		rs_object[MENUBOXSTOP].ob_flags |= HIDETREE;
		rs_object[MENUSTOP].ob_flags |= HIDETREE;
		rs_object[MENUTIME].ob_flags &= ~EDITABLE;
		spec_trace.ub_parm=(long)tab_cpuload;
		if(flag_aff)
		{	
			display_objc(MENUBOXSTATUS,Work);
			display_objc(MENUSTATUS,Work);
			new_objc=ed_objc=new_pos=ed_pos=0;
		}	
		break;
	case PAGE_TEMP:				/* temperature */
		rs_object[MENUBOXSTATUS].ob_flags &= ~HIDETREE;
		rs_object[MENUSTATUS].ob_flags &= ~HIDETREE;
		t_edinfo=rs_object[MENUSTATUS].ob_spec.tedinfo;
		t_edinfo->te_ptext=status[1];
		rs_object[MENUTEMP-1].ob_flags &= ~HIDETREE;
		t_edinfo=rs_object[MENUTEMP].ob_spec.tedinfo;
		sprintf(t_edinfo->te_ptext,"%3d øC",current_temp());		
		rs_object[MENUTEMP].ob_x=gr_hwchar<<3;
		rs_object[MENUBARTEMP].ob_type=G_BOX;
		rs_object[MENUBARTEMP].ob_spec.index=(long)0xff11f1L;
		rs_object[MENUBARTEMP].ob_x=gr_hwchar*15;
		rs_object[MENUBARTEMP].ob_y=(gr_hhchar*3)>>2;
		rs_object[MENUBARTEMP].ob_width=gr_hwchar*6;
		rs_object[MENUBARTEMP].ob_height=gr_hhchar>>1;
		rs_object[MENUBARTEMP+1].ob_flags &= ~HIDETREE;
		if(rs_object[MENUBARTEMP+2].ob_width)
		{
			rs_object[MENUBARTEMP+2].ob_flags &= ~HIDETREE;
			if(rs_object[MENUBARTEMP+3].ob_width)
				rs_object[MENUBARTEMP+3].ob_flags &= ~HIDETREE;
			else
				rs_object[MENUBARTEMP+3].ob_flags |= HIDETREE;
		}
		else
		{
			rs_object[MENUBARTEMP+2].ob_flags |= HIDETREE;
			rs_object[MENUBARTEMP+3].ob_flags |= HIDETREE;
		}
#if 0
		rs_object[MENUTRIGGER].ob_flags |= EDITABLE;
		rs_object[MENUTRIGGER].ob_flags &= ~HIDETREE;
#else
		rs_object[MENUTRIGGER].ob_flags &= ~EDITABLE;
		rs_object[MENUTRIGGER].ob_flags |= HIDETREE;
#endif
		rs_object[MENUTRACE+2].ob_y=rs_object[MENUTRACE].ob_y+rs_object[MENUTRACE].ob_height
		                           -((80*rs_object[MENUTRACE].ob_height)/MAX_TEMP)-2;
		rs_object[MENUTRACE+3].ob_y=rs_object[MENUTRACE].ob_y+rs_object[MENUTRACE].ob_height
		                           -((40*rs_object[MENUTRACE].ob_height)/MAX_TEMP)-2;
		t_edinfo=rs_object[MENUTRACE+4].ob_spec.tedinfo;
		t_edinfo->te_ptext="0ø";
		rs_object[MENUBOXRAM].ob_flags |= HIDETREE;
		rs_object[MENURAM].ob_flags |= HIDETREE;
		rs_object[MENUBOXLANG].ob_flags |= HIDETREE;
		rs_object[MENULANG].ob_flags |= HIDETREE;
		rs_object[MENUSEP].ob_flags &= ~EDITABLE;	
		rs_object[MENUBOXVIDEO].ob_flags |= HIDETREE;
		rs_object[MENUVIDEO].ob_flags |= HIDETREE;
		rs_object[MENUBOXBOOT].ob_flags |= HIDETREE;
		rs_object[MENUBOOT].ob_flags |= HIDETREE;
		rs_object[MENUDELAY].ob_flags &= ~EDITABLE;
		rs_object[MENUBOXSTOP].ob_flags |= HIDETREE;
		rs_object[MENUSTOP].ob_flags |= HIDETREE;
		rs_object[MENUTIME].ob_flags &= ~EDITABLE;
		spec_trace.ub_parm=(long)tab_temp;
		if(flag_aff)
		{	
			display_objc(MENUBOXSTATUS,Work);
			display_objc(MENUSTATUS,Work);
		#if 0
			ed_objc=MENUTRIGGER;
			objc_edit(rs_object,ed_objc,0,&ed_pos,ED_INIT);
			new_objc=ed_objc;
			new_pos=ed_pos;
		#else
			new_objc=ed_objc=new_pos=ed_pos=0;
		#endif
		}
		break;
	case PAGE_MEMORY:			/* memory */
		rs_object[MENUBOXSTATUS].ob_flags |= HIDETREE;
		rs_object[MENUSTATUS].ob_flags |= HIDETREE;
		rs_object[MENUTRIGGER].ob_flags &= ~EDITABLE;
		rs_object[MENUBOXRAM].ob_flags &= ~HIDETREE;
		rs_object[MENURAM].ob_flags &= ~HIDETREE;
		rs_object[MENUBOXLANG].ob_flags |= HIDETREE;
		rs_object[MENULANG].ob_flags |= HIDETREE;
		rs_object[MENUSEP].ob_flags &= ~EDITABLE;	
		rs_object[MENUBOXVIDEO].ob_flags |= HIDETREE;
		rs_object[MENUVIDEO].ob_flags |= HIDETREE;
		rs_object[MENUBOXBOOT].ob_flags |= HIDETREE;
		rs_object[MENUBOOT].ob_flags |= HIDETREE;
		rs_object[MENUDELAY].ob_flags &= ~EDITABLE;
		rs_object[MENUBOXSTOP].ob_flags |= HIDETREE;
		rs_object[MENUSTOP].ob_flags |= HIDETREE;
		rs_object[MENUTIME].ob_flags &= ~EDITABLE;
		if(flag_aff)
		{	
			display_objc(MENUBOXRAM,Work);
			display_objc(MENURAM,Work);
			new_objc=ed_objc=new_pos=ed_pos=0;
		}
		break;
	case PAGE_BOOT:				/* boot */
		rs_object[MENUBOXSTATUS].ob_flags |= HIDETREE;
		rs_object[MENUSTATUS].ob_flags |= HIDETREE;
		rs_object[MENUTRIGGER].ob_flags &= ~EDITABLE;
		rs_object[MENUBOXRAM].ob_flags |= HIDETREE;
		rs_object[MENURAM].ob_flags |= HIDETREE;
		rs_object[MENUBOXLANG].ob_flags |= HIDETREE;
		rs_object[MENULANG].ob_flags |= HIDETREE;
		rs_object[MENUSEP].ob_flags &= ~EDITABLE;
		rs_object[MENUBOXVIDEO].ob_flags |= HIDETREE;
		rs_object[MENUVIDEO].ob_flags |= HIDETREE;
		rs_object[MENUBOXBOOT].ob_flags &= ~HIDETREE;
		rs_object[MENUBOOT].ob_flags &= ~HIDETREE;
		rs_object[MENUDELAY].ob_flags |= EDITABLE;
		rs_object[MENUBOXSTOP].ob_flags |= HIDETREE;
		rs_object[MENUSTOP].ob_flags |= HIDETREE;
		rs_object[MENUTIME].ob_flags &= ~EDITABLE;
		if(flag_aff)
		{
			display_objc(MENUBOXBOOT,Work);
			display_objc(MENUBOOT,Work);
			ed_objc=MENUDELAY;
			objc_edit(rs_object,ed_objc,0,&ed_pos,ED_INIT);
			new_objc=ed_objc;
			new_pos=ed_pos;
		}
		break;
		
	case PAGE_LANG:				/* language */
		rs_object[MENUBOXSTATUS].ob_flags |= HIDETREE;
		rs_object[MENUSTATUS].ob_flags |= HIDETREE;
		rs_object[MENUTRIGGER].ob_flags &= ~EDITABLE;
		rs_object[MENUBOXRAM].ob_flags |= HIDETREE;
		rs_object[MENURAM].ob_flags |= HIDETREE;
		rs_object[MENUBOXLANG].ob_flags &= ~HIDETREE;
		rs_object[MENULANG].ob_flags &= ~HIDETREE;
		rs_object[MENUSEP].ob_flags |= EDITABLE;
		rs_object[MENUBOXVIDEO].ob_flags |= HIDETREE;
		rs_object[MENUVIDEO].ob_flags |= HIDETREE;				
		rs_object[MENUBOXBOOT].ob_flags |= HIDETREE;
		rs_object[MENUBOOT].ob_flags |= HIDETREE;
		rs_object[MENUDELAY].ob_flags &= ~EDITABLE;
		rs_object[MENUBOXSTOP].ob_flags |= HIDETREE;
		rs_object[MENUSTOP].ob_flags |= HIDETREE;
		rs_object[MENUTIME].ob_flags &= ~EDITABLE;		
		if(flag_aff)
		{
			display_objc(MENUBOXLANG,Work);
			display_objc(MENULANG,Work);
			ed_objc=MENUSEP;
			objc_edit(rs_object,ed_objc,0,&ed_pos,ED_INIT);
			new_objc=ed_objc;
			new_pos=ed_pos;
		}
		break;
	}
}

void change_objc(int objc,int state,GRECT *clip)

{
	switch(state)
	{
	case NORMAL:
		rs_object[objc].ob_state &= ~SELECTED;
		break;	
	case SELECTED:
		rs_object[objc].ob_state |= SELECTED;
		break;	
	default:
		rs_object[objc].ob_state=state;
		break;
	}
	display_objc(objc,clip);
}

void display_objc(int objc,GRECT *clip)

{
	register GRECT *rect;
	register int cursor=0;
	wind_update(BEG_UPDATE);
	if(objc==MENUBOX)
	{
		objc_edit(rs_object,ed_objc,0,&ed_pos,ED_END);		/* hide cursor */
		cursor=1;
	}
	rect=(GRECT *)(*Xcpb->GetFirstRect)(clip);
	while(rect)
	{
		objc_draw(rs_object,objc,MAX_DEPTH,rect->g_x,rect->g_y,rect->g_w,rect->g_h);
		rect=(GRECT *)(*Xcpb->GetNextRect)();
	}
	if(cursor)
		objc_edit(rs_object,ed_objc,0,&ed_pos,ED_END);		/* showm cursor */
	wind_update(END_UPDATE);
}

void move_cursor(void)

{
	if(new_objc>0 && (ed_objc!=new_objc || ed_pos!=new_pos))
	{
		objc_edit(rs_object,ed_objc,0,&ed_pos,ED_END);		/* hide cursor */
		ed_pos=new_pos;
		objc_edit(rs_object,new_objc,0,&ed_pos,ED_CHAR);	/* new position of cursor */
		objc_edit(rs_object,new_objc,0,&ed_pos,ED_END);		/* showm cursor */
		ed_objc=new_objc;									/* new zone edited */
		ed_pos=new_pos;										/* new position cursor */
	}
}

int hndl_form(OBJECT *tree,int objc)

{
	register int i,flag_exit,answer;
	long value;
	GRECT rect,kl_rect;
	void *flyinf;
	void *scantab=0;
	int	lastcrsr;
	wind_update(BEG_UPDATE);
	form_center(tree,&rect.g_x, &rect.g_y, &rect.g_w, &rect.g_h);
	answer=flag_exit=i=0;
	do
	{
		if(tree[i].ob_flags & EXIT)
			flag_exit=1;
	}
	while(!(tree[i++].ob_flags & LASTOB));
	if(magic && flag_exit)				/* MagiC dialog */ 
	{
		form_xdial(FMD_START,kl_rect.g_x,kl_rect.g_y,kl_rect.g_w,kl_rect.g_h,rect.g_x,rect.g_y,rect.g_w,rect.g_h,&flyinf);
		objc_draw(tree,0,MAX_DEPTH,rect.g_x,rect.g_y,rect.g_w,rect.g_h);
		answer = 0x7f & form_xdo(tree,objc,&lastcrsr,scantab,flyinf);
		form_xdial(FMD_FINISH,kl_rect.g_x,kl_rect.g_y,kl_rect.g_w,kl_rect.g_h,rect.g_x,rect.g_y,rect.g_w,rect.g_h,&flyinf);
	}
	else								/* TOS dialog */
	{
		form_dial(FMD_START,kl_rect.g_x,kl_rect.g_y,kl_rect.g_w,kl_rect.g_h,rect.g_x,rect.g_y,rect.g_w,rect.g_h);
		objc_draw(tree,0,MAX_DEPTH,rect.g_x,rect.g_y,rect.g_w,rect.g_h);
		if(flag_exit)
			answer = 0x7f & form_do(tree,objc);
		else
			evnt_timer((long)objc);		/* dialog without EXIT button */
		form_dial(FMD_FINISH,kl_rect.g_x,kl_rect.g_y,kl_rect.g_w,kl_rect.g_h,rect.g_x,rect.g_y,rect.g_w,rect.g_h);
	}
	wind_update(END_UPDATE);
	tree[answer].ob_state &= ~SELECTED;
	return(answer);
}	

int MT_form_xalert(int fo_xadefbttn,char *fo_xastring,long time_out,void (*call)(),WORD *global)

{
	register int i,j,w,max_length_lines,max_length_buttons;
	register char *p;
	int flag_img,nb_lines,nb_buttons;
	int answer,event,ret,objc_clic,key,nclicks,new_objc;
	GRECT rect,kl_rect;
	OBJECT *alert_tree;
	TEDINFO *t_edinfo;
	EVNTDATA mouse;
	WORD msg[8];
	char line[25][61];
	char button[3][21];
	if((alert_tree=adr_tree(TREE3))==0)
		return(0);
	alert_tree[ALERTB1].ob_state &= ~SELECTED;
	alert_tree[ALERTB2].ob_state &= ~SELECTED;
	alert_tree[ALERTB3].ob_state &= ~SELECTED;
	switch(fo_xadefbttn)
	{
		case 2:
			alert_tree[ALERTB1].ob_flags &= ~DEFAULT;
			alert_tree[ALERTB2].ob_flags |= DEFAULT;
			alert_tree[ALERTB3].ob_flags &= ~DEFAULT;
			break;
		case 3:
			alert_tree[ALERTB1].ob_flags &= ~DEFAULT;
			alert_tree[ALERTB2].ob_flags &= ~DEFAULT;
			alert_tree[ALERTB3].ob_flags |= DEFAULT;
			break;
		default:
			alert_tree[ALERTB1].ob_flags |= DEFAULT;
			alert_tree[ALERTB2].ob_state &= ~DEFAULT;
			alert_tree[ALERTB3].ob_state &= ~DEFAULT;			
			break;
	}
	if(fo_xastring[0]!='[' || fo_xastring[2]!=']' || fo_xastring[3]!='[')
		return(0);				/* error */
	switch(fo_xastring[1])
	{
	case '1':
		alert_tree[ALERTNOTE].ob_flags &= ~HIDETREE;
		alert_tree[ALERTWAIT].ob_flags |= HIDETREE;
		alert_tree[ALERTSTOP].ob_flags |= HIDETREE;
		flag_img=1;
		break;
	case '2':
		alert_tree[ALERTNOTE].ob_flags |= HIDETREE;
		alert_tree[ALERTWAIT].ob_flags &= ~HIDETREE;
		alert_tree[ALERTSTOP].ob_flags |= HIDETREE;
		flag_img=1;
		break;
	case '3':
		alert_tree[ALERTNOTE].ob_flags |= HIDETREE;
		alert_tree[ALERTWAIT].ob_flags |= HIDETREE;
		alert_tree[ALERTSTOP].ob_flags &= ~HIDETREE;		
		flag_img=1;
		break;			
	default:
		alert_tree[ALERTNOTE].ob_flags |= HIDETREE;
		alert_tree[ALERTWAIT].ob_flags |= HIDETREE;
		alert_tree[ALERTSTOP].ob_flags |= HIDETREE;		
		flag_img=0;
		break;
	}
	for(i=0;i<25;alert_tree[ALERTLINE1+i].ob_spec.free_string=&line[i][0],line[i++][0]=0);
	for(i=0;i<3;alert_tree[ALERTB1+i].ob_spec.free_string=&button[i][0],button[i++][0]=0);
	fo_xastring+=4;
	p=fo_xastring;				/* search the size of the box */
	max_length_buttons=nb_lines=nb_buttons=0;
	t_edinfo=alert_tree[ALERTTITLE].ob_spec.tedinfo;
	max_length_lines=t_edinfo->te_txtlen-1;
	for(i=0;*p!=']' && i<25;i++)
	{ 
		for(j=0;*p!=']' && *p!='|' && j<60;j++)
		{
			if(*p++ == 0)
				return(0);		/* error */
		}
		if(p[0]=='|')
			p++;
		else
		{
			if(j>=60)
				return(0);		/* error */
		}
		if(j>max_length_lines)
			max_length_lines=j;
		nb_lines++;
	}
	if(p[0]!=']' || p[1]!='[')
		return(0);				/* error */
	p+=2;
	for(i=0;*p!=']' && i<3;i++)
	{
		for(j=0;*p!=']' && *p!='|' && j<20;j++)
		{
			if(*p++ == 0)
				return(0);		/* error */
		}
		if(*p=='|')
			p++;
		else
		{
			if(j>=20)
				return(0);		/* error */
		}
		if(j>max_length_buttons)
			max_length_buttons=j;
		nb_buttons++;
	}
	if(p[0]!=']' || p[1]!=0)
		return(0);				/* error */
	if(!max_length_buttons)
		nb_buttons=0;
	else
		max_length_buttons++;
	i=max_length_buttons*nb_buttons;
	if(max_length_lines>i)
		i=max_length_lines;
	i+=3;
	if(flag_img)							/* NOTE, WAIT or STOP */
		i+=5;
	i*=gr_hwchar;							/* width of box */
	alert_tree[ALERTBOX].ob_width=i;
	alert_tree[ALERTTITLE].ob_x=gr_hwchar>>1;
	alert_tree[ALERTTITLE].ob_y=4;
	alert_tree[ALERTTITLE].ob_width=i-gr_hwchar;
	j=max_length_buttons*gr_hwchar;			/* width of button */
	w=(i-(j*nb_buttons))/(nb_buttons+1);	/* width between buttons */
	alert_tree[ALERTB1].ob_x=w;
	alert_tree[ALERTB1].ob_width=alert_tree[ALERTB2].ob_width=alert_tree[ALERTB3].ob_width=j;
	alert_tree[ALERTB2].ob_x=j+(w<<1);
	alert_tree[ALERTB3].ob_x=(j<<1)+(w*3);
	i=nb_lines+1;
	if(flag_img)							/* NOTE, WAIT or STOP */
	{
		j=(alert_tree[ALERTNOTE].ob_height/gr_hhchar)+2;
		if(j>i)
			i=j;
	}
	if(nb_buttons)
		j=(i+4)*gr_hhchar;	
	else
		j=(i+2)*gr_hhchar;
	alert_tree[ALERTBOX].ob_height=j;		/* height of box */
	alert_tree[ALERTB1].ob_y=alert_tree[ALERTB2].ob_y=alert_tree[ALERTB3].ob_y=j-(gr_hhchar<<1);
	for(i=0;i<25;i++)						/* copy texts of lines */
	{
		if(i<nb_lines)
		{
			alert_tree[ALERTLINE1+i].ob_flags &= ~HIDETREE;
			alert_tree[ALERTLINE1+i].ob_x=gr_hwchar<<1;
			if(flag_img)
				alert_tree[ALERTLINE1+i].ob_x+=alert_tree[ALERTNOTE].ob_width;
			alert_tree[ALERTLINE1+i].ob_width=max_length_lines*gr_hwchar;
			for(j=0;*fo_xastring!='|' && *fo_xastring!=']' && j<60;line[i][j++]=*fo_xastring++);
			line[i][j]=0;
			fo_xastring++;
		}
		else
		{
			alert_tree[ALERTLINE1+i].ob_flags |= HIDETREE;
			line[i][0]=0;
		}			
	}
	fo_xastring++;
	for(i=0;i<3;i++)						/* copy texts of buttons */
	{
		if(i<nb_buttons)
		{
			alert_tree[ALERTB1+i].ob_flags &= ~HIDETREE;
			for(j=0;*fo_xastring!='|' && *fo_xastring!=']' && j<20;button[i][j++]=*fo_xastring++);
			button[i][j]=0;
			fo_xastring++;
		}
		else
		{
			alert_tree[ALERTB1+i].ob_flags |= HIDETREE;
			button[i][0]=0;
		}
	}
	MT_wind_update(BEG_UPDATE,global);
	MT_form_center(alert_tree,&rect,global);
	MT_form_dial(FMD_START,&kl_rect,&rect,global);
	MT_objc_draw(alert_tree,0,MAX_DEPTH,&rect,global);
	MT_wind_update(BEG_MCTRL,global);
	answer=0;
	do
	{	event=(MU_KEYBD|MU_BUTTON);
		if(time_out!=0)
			event|=MU_TIMER;	
		event=MT_evnt_multi(event,2,1,1,0,&rect,0,&rect,msg,time_out,&mouse,&key,&nclicks,global);
		if(event & MU_TIMER)
			answer=fo_xadefbttn;
		if(event & MU_BUTTON)
		{
			if((objc_clic=MT_objc_find(alert_tree,0,MAX_DEPTH,mouse.x,mouse.y,global))>=0)
			{
				if(!MT_form_button(alert_tree,objc_clic,nclicks,&new_objc,global))
				{
					switch(objc_clic)		/* buttons */
					{
					case ALERTB1:
						answer=1;
						break;
					case ALERTB2:
						answer=2;
						break;	
					case ALERTB3:
						answer=3;
						break;
					}
				}
				else
				{
					if(time_out && nb_buttons==0) /* no buttons and clic inside the box */
						answer=fo_xadefbttn;		
				}
			}
		}
		if(event & MU_KEYBD)
		{
			if(!MT_form_keybd(alert_tree,0,0,key,&new_objc,&key,global))
				answer=fo_xadefbttn;				
		}		
	}
	while(!answer);
	if(call)
	{
		function=call;
		(*function)();
	}
	MT_wind_update(END_MCTRL,global);
	MT_form_dial(FMD_FINISH,&kl_rect,&rect,global);
	MT_wind_update(END_UPDATE,global);
	return(answer);
}

void SendIkbd(int count, char *buffer)
{
	while(count>=0)
	{
		Bconout(4,*buffer++);
		count--;
	}
}


int read_temp(void)
{
	int temp = 0;
#if 0
	temp=(int)Supexec(ct60_read_temp);
	if (temp < 0)
		temp = 0;
#endif
	return temp;
}

int current_temp(void)
{
	return (tab_temp_eiffel[60] & 0x3f);
}

int fill_tab_temp(void)

{
	register int i,temp;
	register unsigned int time;
	char buffer[2];
	static unsigned int old_time=9999;
#if 0
	time=(unsigned int)(Gettime() & 0xff1fL);	/* 2sec */
	if(time!=old_time)
#endif
	{
		for(i=0;i<60;i++)
			tab_temp[i]=tab_temp[i+1];
		temp=read_temp();
		if(temp<0)
			temp=0;
		if(temp>MAX_TEMP)
			temp=MAX_TEMP;
		tab_temp[60]=temp;
		old_time=time;
		if(eiffel_temp!=NULL)
		{
			buffer[0]=3;							/* get temp */
			Ikbdws(0,buffer);
			for(i=0;i<60;i++)
				tab_temp_eiffel[i]=tab_temp_eiffel[i+1];
			tab_temp_eiffel[60]=((unsigned short)(eiffel_temp[0]&0x3F))
			 | (((unsigned short)(eiffel_temp[2]&1))<<15);
		}
		return(1);
	}
	return(0);
}


int get_cpuload_suptime()
{
	int load;
	long uptime,avenrun[3];
	avenrun[0] = -1L;
	avenrun[1] = 0;
	avenrun[0] = 0;
	Suptime(&uptime,avenrun);
	return (int)avenrun[0];
}


int get_cpuload_stat()
{
	FILE* fp;
	long mcpu, mnice, msys, midle;
	long mused = 0;
	long mtotal = 0;
	
	static long mused_old = 0;
	static long mtotal_old = 0;
	
	int load = 0;
	char fbuf[129];
	fbuf[128] = 0;
	
	fp = fopen("u:/kern/stat", "r");
	if (!fp) {
		return get_cpuload_suptime();
	}
	
	fread(fbuf, 1, 128, fp);
	fclose(fp);
	sscanf(fbuf, "%*s %ld %ld %ld %ld", &mcpu, &mnice, &msys, &midle);
	mused = mcpu + msys + mnice;
	mtotal = mused + midle;

	if (mtotal - mtotal_old > 0) {
		load = (int) ((2000L * (mused-mused_old)) / (mtotal-mtotal_old));
		if (load < 0)
			load = 0;
	}
	
	mtotal_old = mtotal;
	mused_old = mused;
	return load;
}


int fill_tab_cpuload(void)

{
	register int i,load;
	register unsigned int time;
	char buffer[2];

	static unsigned int old_time=9999;
#if 0
	time=(unsigned int)(Gettime() & 0xff1fL);	/* 2sec */
	if(time!=old_time)
#endif
	{
		for(i=0;i<60;i++)
			tab_cpuload[i]=tab_cpuload[i+1];

		load=0;
		if(flag_cpuload)
		{
			if (mint)
			{
				load = get_cpuload_stat();
			}
			else
			{
				load = get_cpuload_suptime();
			}

		}	
		if(load<0)
			load=0;
		if(load>MAX_CPULOAD)
			load=MAX_CPULOAD;
		tab_cpuload[60]=load;
		old_time=time;
		return(1);
	}
	return(0);
}


unsigned long bogomips(void)

{
	long value;
	unsigned long loops_per_sec=1;
	unsigned long ticks;
	while((loops_per_sec<<=1))
	{
		if(!(*Xcpb->get_cookie)('MgMc',&value))
   		{	/* normal case ST, TT, FALCON, HADES, MILAN, CT60 */
   			value_supexec=loops_per_sec;
			ticks=(unsigned long)Supexec(mes_delay);
		}
		else
		{	/* _hz_200 seems not works in supervisor state under MagiCMac */
			ticks=clock();
			delay_loop(loops_per_sec);
			ticks=clock()-ticks;
		}
		if(ticks>=CLOCKS_PER_SEC)
		{
			loops_per_sec=(loops_per_sec/ticks) * CLOCKS_PER_SEC;
			break;
		}
	}
	return(loops_per_sec);
}

void delay_loop(long loops)

{
	while((--loops)>=0);
}

int get_MagiC_ver(unsigned long *crdate)

{
	long value;
	AESVARS *av;
	if(!(*Xcpb->get_cookie)('MagX',&value))
		return(0);
	av=((MAGX_COOKIE *)value)->aesvars;
	if(!av)
		return(0);
	if(crdate)
	{
		*crdate = av->date << 16L;
		*crdate |= av->date >> 24L;
		*crdate |= (av->date >> 8L) & 0xff00L;		/* yyyymmdd */
	}
	return(av->version);
}

COOKIE *fcookie(void)

{
	COOKIE *p;
	long stack;
	stack=Super(0L);
	p=*(COOKIE **)0x5a0;
	Super((void *)stack);
	if(!p)
		return((COOKIE *)0);
	return(p);
}

COOKIE *ncookie(COOKIE *p)

{
	if(!p->ident)
		return(0);
	return(++p);
}

COOKIE *get_cookie(long id)

{
	COOKIE *p;
	p=fcookie();
	while(p)
	{
		if(p->ident==id)
			return p;
		p=ncookie(p);
	}
	return((COOKIE *)0);
}

int add_cookie(COOKIE *cook)

{
	COOKIE *p;
	int i=0;
	p=fcookie();
	while(p)
	{
		if(p->ident==cook->ident)
			return(-1);
		if(!p->ident)
		{
			if(i+1 < p->v.l)
			{
				*(p+1)=*p;
				*p=*cook;
				return(0);
			}
			else
				return(-2);			/* problem */
		}
		i++;
		p=ncookie(p);
	}
	return(-1);						/* no cookie-jar */
}


int test_060(void)

{
	if(cpu_cookie==0)
		(*Xcpb->get_cookie)('_CPU',&cpu_cookie);
	return(cpu_cookie==60 ? 1 : 0);
}


int copy_string(char *p1,char *p2)

{
	register int i=0;
	while((*p2++ = *p1++)!=0)
		i++;
	return(i);
}

int long_deci(char *p,int *lg)

{
	register int i=0;
	*lg=0;
	while((*p>='0' && *p<='9') || (!i && *p==' '))
	{
		if(*p!=' ')
			i++;
		p++;
		(*lg)++;	/* number of characters */
	}
	return(i);		/* number of digits */
}

int val_string(char *p)

{
	static int tab_mul_10[5]={10000,1000,100,10,1};
	register int lg=0,i,j;
	register int *range;
	while(p[lg]>='0' && p[lg]<='9')
		lg++;
	if(!lg)
		return(0);
	range = &tab_mul_10[5-lg];
	for(i=j=0;j<lg;i+=(((int)p[j] & 0xf) * range[j]),j++);
	return(i);
}

void reboot(void)

{
	EVNTDATA mouse;
	do
		graf_mkstate(&mouse.x, &mouse.y, &mouse.bstate, &mouse.kstate);				/* wait end of clic */
	while(mouse.bstate);
	Super(0L);								/* supervisor state */
	reset=(void (*)())*(void **)0x4;		/* reset vector */
	(*reset)();								/* reset system */
}
