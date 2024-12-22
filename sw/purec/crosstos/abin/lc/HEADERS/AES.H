/**
 * $Id: aes.h,v 1.2 1993/11/14 14:26:10 AGK Rel $
 *
 * AES interface header file
 *
 * (c) Copyright 1992 HiSoft
**/

#ifndef _AES_H
#define _AES_H

#ifndef __TYPES_H
#include <__types.h>
#endif

extern void *_AESpb[6];

extern short _AEScontrol[5];
extern short _AESglobal[15];
extern short _AESintin[16];
extern short _AESintout[7];
extern void *_AESaddrin[3];
extern void *_AESaddrout[1];

/* Application library definitions */
typedef struct {
	long ap_event;
	long ap_value;
} EVNTREC;

int appl_init(void);
int appl_read(int,int,void *);
int appl_write(int,int,void *);
int appl_find(const char *);
int appl_tplay(const void *,int,int);
int appl_trecord(void *,int);
int appl_search(int,char *,short *,short *);
int appl_exit(void);
int appl_getinfo(int, short *, short *, short *, short *);

/* Event library definitions */
#define MU_KEYBD	0x0001
#define MU_BUTTON	0x0002
#define MU_M1		0x0004
#define MU_M2		0x0008
#define MU_MESAG	0x0010
#define MU_TIMER	0x0020

#define K_RSHIFT	0x0001
#define K_LSHIFT	0x0002
#define K_CTRL		0x0004
#define K_ALT		0x0008

#define MN_SELECTED	10

#define WM_REDRAW	20
#define WM_TOPPED	21
#define WM_CLOSED	22
#define WM_FULLED	23
#define WM_ARROWED	24
#define WM_HSLID	25
#define WM_VSLID	26
#define WM_SIZED	27
#define WM_MOVED	28
#define WM_NEWTOP	29
#define WM_UNTOPPED	30
#define WM_ONTOP	31
#define WM_BOTTOM	33
#define WM_ICONIFY	34
#define WM_UNICONIFY	35
#define WM_ALLICONIFY	36
#define WM_TOOLBAR	37
#define AC_OPEN		40
#define AC_CLOSE	41

#define	AP_TERM		50
#define AP_TFAIL	51
#define AP_RESCHG	57

#define SHUT_COMPLETED	60
#define RESCH_COMPLETED	61
#define AP_DRAGDROP	63

#define SH_WDRAW	72

#define CH_EXIT		80

#define CT_NEWTOP	52
#define CT_KEY		53

#define WA_UPPAGE	0
#define WA_DNPAGE	1
#define WA_UPLINE	2
#define WA_DNLINE	3
#define WA_LFPAGE	4
#define WA_RTPAGE	5
#define WA_LFLINE	6
#define WA_RTLINE	7

typedef struct moblk {
	short m_out;
	short m_x;
	short m_y;
	short m_w;
	short m_h;
} MOBLK;

int evnt_keybd(void);
int evnt_button(int,int,int,short *,short *,short *,short *);
int evnt_mouse(int,int,int,int,int,short *,short *,short *,short *);
int evnt_mesag(short *);
int evnt_timer(int,int);
int evnt_multi(int,int,int,int,int,int,int,int,int,int,int,int,int,int,short *,int,int,short *,short *,short *,short *,short *,short *);
int evnt_dclick(int,int);

/* Object library definitions */
#define ROOT		0
#define NIL			-1

#define MAX_DEPTH	8	/* max depth of search or draw */

#define IP_HOLLOW	0	/* inside patterns */
#define IP_1PATT	1
#define IP_2PATT	2
#define IP_3PATT	3
#define IP_4PATT	4
#define IP_5PATT	5
#define IP_6PATT	6
#define IP_SOLID	7

#define GDOS_PROP	0	/* font types */
#define GDOS_MONO	1
#define GDOS_BITM	2
#define IBM			3
#define SMALL		5

#define G_BOX		20	/* Graphic types of obs	*/
#define G_TEXT		21
#define G_BOXTEXT	22
#define G_IMAGE		23
#define G_PROGDEF	24
#define	G_USERDEF	G_PROGDEF
#define G_IBOX		25
#define G_BUTTON	26
#define G_BOXCHAR	27
#define G_STRING	28
#define G_FTEXT		29
#define G_FBOXTEXT	30
#define G_ICON		31
#define G_TITLE		32
#define G_CICON		33

#define NONE		0x0000	/* Object flags	*/
#define SELECTABLE	0x0001
#define DEFAULT		0x0002
#define EXIT		0x0004
#define EDITABLE	0x0008
#define RBUTTON		0x0010
#define LASTOB		0x0020
#define TOUCHEXIT	0x0040
#define HIDETREE	0x0080
#define INDIRECT	0x0100
#define FL3DMASK	0x0600
#define		FL3DNONE	0x0000
#define		FL3DIND		0x0200
#define		FL3DBAK		0x0400
#define		FL3DACT		0x0600
#define SUBMENU		0x0800

#define NORMAL		0x0000	/* Object states */
#define SELECTED	0x0001
#define CROSSED		0x0002
#define CHECKED		0x0004
#define DISABLED	0x0008
#define OUTLINED	0x0010
#define SHADOWED	0x0020
#define SPECIAL		0x0040	/* user defined object state */

#define WHITE		0	/* Object colors */
#define BLACK		1
#define RED			2
#define GREEN		3
#define BLUE		4
#define CYAN		5
#define YELLOW		6
#define MAGENTA		7
#define LWHITE		8
#define LBLACK		9
#define LRED		10
#define LGREEN		11
#define LBLUE		12
#define LCYAN		13
#define LYELLOW		14
#define LMAGENTA	15

#define TE_LEFT		0	/* editable text justification */
#define TE_RIGHT	1
#define TE_CNTR		2

typedef struct text_edinfo {
	char *te_ptext;
	char *te_ptmplt;
	char *te_pvalid;
	short te_font;
	short te_fontid;
	short te_just;
#ifdef __TE_COLOR
	__TE_COLOR te_color;
#else
	short te_color;
#endif
	short te_fontsize;
	short te_thickness;
	short te_txtlen;
	short te_tmplen;
} TEDINFO;

typedef struct icon_block {
	short *ib_pmask;
	short *ib_pdata;
	char *ib_ptext;
	short ib_char;
	short ib_xchar;
	short ib_ychar;
	short ib_xicon;
	short ib_yicon;
	short ib_wicon;
	short ib_hicon;
	short ib_xtext;
	short ib_ytext;
	short ib_wtext;
	short ib_htext;
} ICONBLK;

typedef struct cicon_data {
	short num_planes;
	short *col_data;
	short *col_mask;
	short *sel_data;
	short *sel_mask;
    struct cicon_data *next_res;
} CICON;

typedef struct cicon_blk {
	ICONBLK monoblk;
	CICON *mainlist;
} CICONBLK;

typedef struct bit_block {
	short *bi_pdata;
	short bi_wb;
	short bi_hl;
	short bi_x;
	short bi_y;
	short bi_color;
} BITBLK;

typedef struct parm_blk {
	struct object *pb_tree;
	short pb_obj;
	short pb_prevstate;
	short pb_currstate;
	short pb_x;
	short pb_y;
	short pb_w;
	short pb_h;
	short pb_xc;
	short pb_yc;
	short pb_wc;
	short pb_hc;
	long pb_parm;
} PARMBLK;

/* Lattice 3 flavour */
typedef struct user_blk {
	int (*__stdargs ub_code)(PARMBLK *);
	long ub_parm;
} USERBLK;

/* Rest of the world flavour */
typedef struct appl_blk {
	int (*__stdargs ab_code)(PARMBLK *);
	long ab_parm;
} APPLBLK;

typedef struct object {
	short ob_next;
	short ob_head;
	short ob_tail;
	unsigned short ob_type;
	unsigned short ob_flags;
	unsigned short ob_state;
#ifdef __OB_SPEC
	__OB_SPEC ob_spec;
#else
	void *ob_spec;
#endif
	short ob_x;
	short ob_y;
	short ob_width;
	short ob_height;
} OBJECT;

typedef struct grect {
	short g_x;
	short g_y;
	short g_w;
	short g_h;
} GRECT;

#define LK3DIND	1
#define LK3DACT	2
#define INDBUTCOL	3
#define ACTBUTCOL	4
#define BACKGRCOL	5
#define AD3DVALUE	6

int objc_add(OBJECT *,int,int);
int objc_delete(OBJECT *,int);
int objc_draw(OBJECT *,int,int,int,int,int,int);
int objc_find(OBJECT *,int,int,int,int);
int objc_offset(OBJECT *,int,short *,short *);
int objc_order(OBJECT *,int,int);
int objc_edit(OBJECT *,int,int,short *,int);
int objc_change(OBJECT *,int,int,int,int,int,int,int,int);
int objc_sysvar(int,int,int,int,short *,short *);

/* HiSoft extras */
int objc_xywh(OBJECT *,int,GRECT *);
void objc_walk(OBJECT *,int,int,int,int (*)(OBJECT *,int));

/* Menu library definitions */
typedef struct _menu {
	OBJECT *mn_tree;	/* the object tree of the menu */
	short mn_menu;		/* the parent object of the menu items */
	short mn_item;		/* the starting menu item */
	short mn_scroll;	/* the scroll field status of the menu */
	short mn_keystate;	/* CTRL, ALT, SHIFT */
} MENU;

typedef struct _mn_set {
	long Display;	/* the submenu display delay */
	long Drag;	/* the submenu drag delay */
	long Delay;	/* the single-click scroll delay */
	long Speed;	/* the continuous scroll delay */
	long Height;	/* the menu scroll height */
} MN_SET;

#define ME_INQUIRE	0
#define ME_ATTACH	1
#define ME_REMOVE	2

int menu_bar(OBJECT *, int);
int menu_icheck(OBJECT *,int,int);
int menu_ienable(OBJECT *,int,int);
int menu_tnormal(OBJECT *,int,int);
int menu_text(OBJECT *,int,const char *);
int menu_register(int,const char *);
int menu_popup(MENU *,int,int,MENU *);
int menu_attach(int,OBJECT *,int,MENU *);
int menu_istart(int,OBJECT *,int,int);
int menu_settings(int, MN_SET *);

/* Form library definitions */
#define ED_START	0	/* editable text field definitions */
#define EDSTART	ED_START
#define ED_INIT	1
#define EDINIT	ED_INIT
#define ED_CHAR	2
#define EDCHAR	ED_CHAR
#define ED_END	3
#define EDEND	ED_END

#define FMD_START	0
#define FMD_GROW	1
#define FMD_SHRINK	2
#define FMD_FINISH	3

int form_do(OBJECT *,int);
int form_dial(int,int,int,int,int,int,int,int,int);
int form_alert(int,const char *);
int form_error(int);
int form_center(OBJECT *,short *,short *,short *,short *);
int form_keybd(OBJECT *,int,int,int,short *,short *);
int form_button(OBJECT *,int,int,short *);

/* Graphics library definitions */
#define	ARROW		0
#define	TEXT_CRSR	1
#define	HOURGLASS	2
#define	BUSYBEE	HOURGLASS
#define	BUSY_BEE	HOURGLASS
#define	POINT_HAND	3
#define	FLAT_HAND	4
#define	THIN_CROSS	5
#define	THICK_CROSS	6
#define	OUTLN_CROSS	7
#define	USER_DEF	255
#define M_OFF		256
#define M_ON		257
#define M_SAVE		258
#define M_RESTORE	259
#define M_PREVIOUS	260

#define VERTICAL	0
#define HORIZONTAL	1

int graf_rubberbox(int,int,int,int,short *,short *);
int graf_dragbox(int,int,int,int,int,int,int,int,short *,short *);
int graf_movebox(int,int,int,int,int,int);
int graf_growbox(int,int,int,int,int,int,int,int);
int graf_shrinkbox(int,int,int,int,int,int,int,int);
int graf_watchbox(OBJECT *,int,int,int);
int graf_slidebox(OBJECT *,int,int,int);
int graf_handle(short *,short *,short *,short *);
int graf_mouse(int,const void *);
int graf_mkstate(short *,short *,short *,short *);

/* Scrap library definitions */
int scrp_read(char *);
int scrp_write(const char *);

/* File selector library definitions */
int fsel_input(char *,char *,short *);
int fsel_exinput(char *,char *,short *,const char *);

/* Window library definitions */
#define NAME		0x0001
#define CLOSE		0x0002
#define CLOSER		CLOSE
#define FULL		0x0004
#define FULLER		FULL
#define MOVE		0x0008
#define MOVER		MOVE
#define INFO		0x0010
#define SIZE		0x0020
#define SIZER		SIZE
#define UPARROW		0x0040
#define DNARROW		0x0080
#define VSLIDE		0x0100
#define LFARROW		0x0200
#define RTARROW		0x0400
#define HSLIDE		0x0800
#define SMALLER		0x4000

#define WF_KIND		1
#define WF_NAME 	2
#define WF_INFO 	3
#define WF_WXYWH	4
#define WF_WORKXYWH	WF_WXYWH
#define WF_CXYWH	5
#define WF_CURRXYWH	WF_CXYWH
#define WF_PXYWH	6
#define WF_PREVXYWH	WF_PXYWH
#define WF_FXYWH	7
#define WF_FULLXYWH	WF_FXYWH
#define WF_HSLIDE	8
#define WF_VSLIDE	9
#define WF_TOP		10
#define WF_FIRSTXYWH	11
#define WF_NEXTXYWH	12
#define WF_IGNORE	13
#define WF_NEWDESK	14
#define WF_HSLSIZE	15
#define WF_HSLSIZ	WF_HSLSIZE
#define WF_VSLSIZE	16
#define	WF_VSLSIZ	WF_VSLSIZE
#define WF_SCREEN	17
#define WF_COLOR	18
#define WF_DCOLOR	19
#define WF_OWNER	20
#define WF_BEVENT	24
#define WF_BOTTOM	25
#define WF_ICONIFY	26
#define WF_UNICONIFY	27
#define WF_UNICONIFYXYWH	28
#define WF_TOOLBAR	30
#define WF_FTOOLBAR	31
#define WF_NTOOLBAR	32

#define	END_UPDATE	0
#define	BEG_UPDATE	1
#define	END_MCTRL	2
#define	BEG_MCTRL	3

#define WC_BORDER	0
#define WC_WORK		1

#define DESK	0

#define	W_BOX		0
#define W_TITLE		1
#define W_CLOSER	2
#define W_NAME		3
#define W_FULLER	4
#define W_INFO		5
#define W_DATA		6
#define W_WORK		7
#define W_SIZER		8
#define W_VBAR		9
#define W_UPARROW	10
#define W_DNARROW	11
#define W_VSLIDE	12
#define W_VELEV		13
#define W_HBAR		14
#define W_LFARROW	15
#define W_RTARROW	16
#define W_HSLIDE	17
#define W_HELEV		18

int wind_create(int,int,int,int,int);
int wind_open(int,int,int,int,int);
int wind_close(int);
int wind_delete(int);
int wind_get(int,int,short *,short *,short *,short *);
int wind_set(int,int,...);
int wind_find(int,int);
int wind_update(int);
int wind_calc(int,int,int,int,int,int,short *,short *,short *,short *);
int wind_new(void);

/*  HiSoft extras */
int wind_newdesk(OBJECT *,int);
int wind_info(int,const char *);
int wind_title(int,const char *);
int wind_redraw(int,GRECT *,int (*)(int,GRECT *));

/* Resource library definitions */
#define R_TREE		0
#define R_OBJECT	1
#define R_TEDINFO	2
#define R_ICONBLK	3
#define R_BITBLK	4
#define R_STRING	5	/* gets pointer to free strings */
#define R_IMAGEDATA	6	/* gets pointer to free images */
#define R_OBSPEC	7
#define R_TEPTEXT	8	/* sub ptrs in TEDINFO */
#define R_TEPTMPLT	9
#define R_TEPVALID	10
#define R_IBPMASK	11	/* sub ptrs in ICONBLK */
#define R_IBPDATA	12
#define R_IBPTEXT	13
#define R_BIPDATA	14	/* sub ptrs in BITBLK */
#define R_FRSTR		15	/* gets addr of ptr to free strings */
#define R_FRIMG		16	/* gets addr of ptr to free images */

typedef struct {
	short rsh_vrsn;			/* RCS version no. */
	unsigned short rsh_object;	/* offset to object[] */
	unsigned short rsh_tedinfo;	/* offset to tedinfo[] */
	unsigned short rsh_iconblk;	/* offset to iconblk[] */
	unsigned short rsh_bitblk;	/* offset to bitblk[] */
	unsigned short rsh_frstr;		/* offset to free string index */
	unsigned short rsh_string;	/* offset to first string */
	unsigned short rsh_imdata;	/* offset to image data */
	unsigned short rsh_frimg;		/* offset to free image index */
	unsigned short rsh_trindex;	/* offset to object tree index */
	unsigned short rsh_nobs;			/* number of objects */
	unsigned short rsh_ntree;			/* number of trees */
	unsigned short rsh_nted;			/* number of tedinfos */
	unsigned short rsh_nib;			/* number of icon blocks */
	unsigned short rsh_nbb;			/* number of blt blocks */
	unsigned short rsh_nstring;		/* number of free strings */
	unsigned short rsh_nimages;		/* number of free images */
	unsigned short rsh_rssize;	/* total bytes in resource */
} RSHDR;

int rsrc_load(const char *);
int rsrc_free(void);
int rsrc_gaddr(int,int,void *);
int rsrc_saddr(int,int,void *);
int rsrc_obfix(OBJECT *,int);
int rsrc_rcfix(RSHDR *rc_header);

/* Shell library definitions */
int shel_read(char *,char *);
int shel_write(int,int,int,const char *,const char *);
int shel_get(char *,int);
int shel_put(const char *,int);
int shel_find(char *);
int shel_envrn(char **, const char *);

/* Rectangle manipulation definitions */
void rc_constrain(const GRECT *,GRECT *);
void rc_copy(const GRECT *,GRECT *);
int rc_equal(const GRECT *,const GRECT *);
int __builtin_memcmp(const void *, const void *, unsigned long);
#define rc_equal(p1,p2)	(!__builtin_memcmp(p1,p2,sizeof(GRECT)))
int rc_intersect(const GRECT *,GRECT *);
int rc_inside(int,int,const GRECT *);
void rc_union(const GRECT *,GRECT *);
void rc_center(const GRECT *,GRECT *);

/* Macro for Lattice 3 compatability */
#ifdef _SHORTINT
#define ADDR(a) (a)
#else
#define	ADDR(a)	((long)(a)>>16),((long)(a))
#endif

/* Useful AES functions */
#ifndef min
int min(int,int);
int __builtin_min(int,int);
#define min(a,b)	__builtin_min(a,b)
#endif

#ifndef max
int max(int,int);
int __builtin_max(int,int);
#define max(a,b)	__builtin_max(a,b)
#endif

unsigned short umul_div(unsigned int,unsigned int,unsigned int);
#pragma inline d0=umul_div(d0,d1,d2)	{"c0c180c2";}

/* macros to pass parameters */
#ifndef PTRS
#define PTRS(r) (r)->g_x, (r)->g_y, (r)->g_w, (r)->g_h
#endif

#ifndef ELTS
#define ELTS(r) (r).g_x, (r).g_y, (r).g_w, (r).g_h
#endif

#ifndef NULL
#define NULL	__NULL
#endif
#endif
