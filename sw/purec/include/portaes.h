/*****************************************************************************
 * PORTAES.H
 *****************************************************************************/

#ifndef __PORTAES_H__
#define __PORTAES_H__

#ifndef __PORTAB_H__
#  include <portab.h>
#endif
#ifndef __GRECT_H__
#  include <grect.h>
#endif

/*
 * do indicate certain difference to original PCGEMLIB
 */
#define _GEMLIB_COMPATIBLE 1

EXTERN_C_BEG

extern short _app;

/* Keyboard states */

#define K_RSHIFT        0x0001
#define K_LSHIFT        0x0002
#define K_SHIFT			(K_LSHIFT|K_RSHIFT)
#define K_CTRL          0x0004
#define K_ALT           0x0008
#define K_CAPSLOCK		0x0010

/* mouse buttons */

#define MOB_LEFT		0x01
#define MOB_RIGHT		0x02
#define MOB_MIDDLE		0x04
#define MOB_BUTTON4		0x08
#define MOB_BUTTON5		0x10

/* Object structures */

#define OBTYPEMASK    0x00ff
#define OBEXTTYPEMASK 0xff00


/*
 * Macros to manipulate a TEDINFO color word
 */

#define COLSPEC_GET_FRAMECOL(color)    (((color) >> 12) & 0x0f)
#define COLSPEC_GET_TEXTCOL(color)     (((color) >>  8) & 0x0f)
#define COLSPEC_GET_TEXTMODE(color)    (((color) >>  7) & 0x01)
#define COLSPEC_GET_FILLPATTERN(color) (((color) >>  4) & 0x07)
#define COLSPEC_GET_INTERIORCOL(color) (((color)      ) & 0x0f)

#define COLSPEC_SET_FRAMECOL(color, framecol)       color = ( ((color) & 0x0fff) | (((framecol)    & 0x0f) << 12) )
#define COLSPEC_SET_TEXTCOL(color, textcol)         color = ( ((color) & 0xf0ff) | (((textcol)     & 0x0f) <<  8) )
#define COLSPEC_SET_TEXTMODE(color, textmode)       color = ( ((color) & 0xff7f) | (((textmode)    & 0x01) <<  7) )
#define COLSPEC_SET_FILLPATTERN(color, fillpattern) color = ( ((color) & 0xff8f) | (((fillpattern) & 0x07) <<  4) )
#define COLSPEC_SET_INTERIORCOL(color, interiorcol) color = ( ((color) & 0xfff0) | (((interiorcol) & 0x0f)      ) )

/* bfobspec.textmode/bfcolspec.textmode: */

#define TEXT_TRANSPARENT    0
#define TEXT_OPAQUE     1

#define COLSPEC_MAKE(framecol, textcol, textmode, fillpattern, interiorcol) \
	((((framecol)    & 0x0f) << 12) | \
	 (((textcol)     & 0x0f) <<  8) | \
	 (((textmode)    & 0x01) <<  7) | \
	 (((fillpattern) & 0x07) <<  4) | \
	 (((interiorcol) & 0x0f)      ))

#ifndef _MT_GEMLIB_H_
typedef struct _tedinfo
{
	char	*te_ptext;		/* ptr to text (must be 1st)    */
	char	*te_ptmplt;		/* ptr to template              */
	char	*te_pvalid;		/* ptr to validation            */
	_WORD	te_font;		/* font                         */
	_WORD	te_fontid;		/* font id                      */
	_WORD	te_just;		/* justification: left, right...*/
	_UWORD	te_color;		/* color information            */
	_WORD	te_fontsize;	/* junk word                    */
	_WORD	te_thickness;	/* border thickness             */
	_WORD	te_txtlen;		/* text string length           */
	_WORD	te_tmplen;		/* template string length       */
} TEDINFO;
#endif
#define te_junk1 te_fontid
#define te_junk2 te_fontsize

/*
 * Macros to manipulate a ICONBLK color word
 */
#define ICOLSPEC_GET_DATACOL(color)   ( ((color) >> 12) & 0x0f )
#define ICOLSPEC_GET_MASKCOL(color)   ( ((color) >>  8) & 0x0f )
#define ICOLSPEC_GET_CHARACTER(color) ( ((color)      ) & 0xff )

#define ICOLSPEC_SET_DATACOL(color, datacol) color = ( ((color) & 0x0fff) | (((datacol) & 0x0f) << 12) )
#define ICOLSPEC_SET_MASKCOL(color, maskcol) color = ( ((color) & 0xf0ff) | (((maskcol) & 0x0f) <<  8) )
#define ICOLSPEC_SET_CHARACTER(color, ch)    color = ( ((color) & 0xff00) | (((ch)      & 0xff)      ) )

#define ICOLSPEC_MAKE(datacol, maskcol, ch) \
	((((datacol)    & 0x0f) << 12) | \
	 (((maskcol)    & 0x0f) <<  8) | \
	 (((ch)         & 0xff)      ))

#ifndef _MT_GEMLIB_H_
typedef struct _iconblk
{
	_WORD	*ib_pmask;
	_WORD	*ib_pdata;
	char	*ib_ptext;
	_WORD	ib_char;
	_WORD	ib_xchar;
	_WORD	ib_ychar;
	_WORD	ib_xicon;
	_WORD	ib_yicon;
	_WORD	ib_wicon;
	_WORD	ib_hicon;
	_WORD	ib_xtext;
	_WORD	ib_ytext;
	_WORD	ib_wtext;
	_WORD	ib_htext;
} ICONBLK;
#endif


#ifndef _MT_GEMLIB_H_
typedef struct _bitblk
{
	_WORD	*bi_pdata;			/* ptr to bit forms data        */
	_WORD	bi_wb;				/* width of form in bytes       */
	_WORD	bi_hl;				/* height in lines              */
	_WORD	bi_x;				/* source x in bit form         */
	_WORD	bi_y;				/* source y in bit form         */
	_WORD	bi_color;			/* foreground color             */
} BITBLK;
#endif


#ifndef _MT_GEMLIB_H_
typedef struct _cicon {
	_WORD	num_planes;			/* number of planes in the following data */
	_WORD	*col_data;			/* pointer to color bitmap in standard form */
	_WORD	*col_mask;			/* pointer to single plane mask of col_data */
	_WORD	*sel_data;			/* pointer to color bitmap of selected icon */
	_WORD	*sel_mask;			/* pointer to single plane mask of selected icon */
	struct _cicon *next_res;	/* pointer to next icon for a different resolution */
} CICON; /* AES >= 3.3 */

typedef struct _ciconblk {
	ICONBLK monoblk;			/* default monochrome icon */
	CICON *mainlist;			/* list of color icons for different resolutions */
} CICONBLK; /* AES >= 3.3 */
#endif

#define CICON_STR_SIZE 12

struct parm_block;

typedef _WORD _CDECL (*PARMBLKFUNC)(struct parm_block *pb);
#ifndef _MT_GEMLIB_H_
typedef struct
{
	PARMBLKFUNC ub_code;
	_LONG_PTR ub_parm;
} USERBLK;
#endif

#ifndef _MT_GEMLIB_H_
typedef struct
{
	unsigned character   :  8;
	signed   framesize   :  8;
	unsigned framecol    :  4;
	unsigned textcol     :  4;
	unsigned textmode    :  1;
	unsigned fillpattern :  3;
	unsigned interiorcol :  4;
} bfobspec;
typedef bfobspec BFOBSPEC;
#endif

#ifndef _MT_GEMLIB_H_
typedef struct objc_colorword
{
	unsigned	borderc : 4;
	unsigned	textc   : 4;
	unsigned	opaque  : 1;
	unsigned	pattern : 3;
	unsigned	fillc   : 4;
} OBJC_COLORWORD;
#endif

/*
 * Macros to manipulate a OBSPEC info
 */
#define OBSPEC_GET_CHARACTER(obspec)   ((unsigned char) ( (((obspec).index) >> 24) & 0xff ))
#define OBSPEC_GET_FRAMESIZE(obspec)   ((signed char)   ( (((obspec).index) >> 16) & 0xff ))
#define OBSPEC_GET_FRAMECOL(obspec)    ((unsigned char) ( (((obspec).index) >> 12) & 0x0f ))
#define OBSPEC_GET_TEXTCOL(obspec)     ((unsigned char) ( (((obspec).index) >>  8) & 0x0f ))
#define OBSPEC_GET_TEXTMODE(obspec)    ((unsigned char) ( (((obspec).index) >>  7) & 0x01 ))
#define OBSPEC_GET_FILLPATTERN(obspec) ((unsigned char) ( (((obspec).index) >>  4) & 0x07 ))
#define OBSPEC_GET_INTERIORCOL(obspec) ((unsigned char) ( (((obspec).index)      ) & 0x0f ))

#define OBSPEC_SET_CHARACTER(obspec, ch)            (obspec).index = ( (((obspec).index) & 0x00ffffffl) | ((((_ULONG)((ch)          & 0xff)) << 24)) )
#define OBSPEC_SET_FRAMESIZE(obspec, framesize)     (obspec).index = ( (((obspec).index) & 0xff00ffffl) | ((((_ULONG)((framesize)   & 0xff)) << 16)) )
#define OBSPEC_SET_FRAMECOL(obspec, framecol)       (obspec).index = ( (((obspec).index) & 0xffff0fffl) | ((((_ULONG)((framecol)    & 0x0f)) << 12)) )
#define OBSPEC_SET_TEXTCOL(obspec, textcol)         (obspec).index = ( (((obspec).index) & 0xfffff0ffl) | ((((_ULONG)((textcol)     & 0x0f)) <<  8)) )
#define OBSPEC_SET_TEXTMODE(obspec, textmode)       (obspec).index = ( (((obspec).index) & 0xffffff7fl) | ((((_ULONG)((textmode)    & 0x01)) <<  7)) )
#define OBSPEC_SET_FILLPATTERN(obspec, fillpattern) (obspec).index = ( (((obspec).index) & 0xffffff8fl) | ((((_ULONG)((fillpattern) & 0x07)) <<  4)) )
#define OBSPEC_SET_INTERIORCOL(obspec, interiorcol) (obspec).index = ( (((obspec).index) & 0xfffffff0l) | ((((_ULONG)((interiorcol) & 0x0f))      )) )

#define OBSPEC_MAKE(ch, framesize, framecol, textcol, textmode, fillpattern, interiorcol) \
	   ( \
		((((_ULONG)((ch)          & 0xff)) << 24)) | \
		((((_ULONG)((framesize)   & 0xff)) << 16)) | \
		((((_ULONG)((framecol)    & 0x0f)) << 12)) | \
		((((_ULONG)((textcol)     & 0x0f)) <<  8)) | \
		((((_ULONG)((textmode)    & 0x01)) <<  7)) | \
		((((_ULONG)((fillpattern) & 0x07)) <<  4)) | \
		((((_ULONG)((interiorcol) & 0x0f))      )) )

#define OBSPEC_SET_OBSPEC(obspec, ch, framesize, framecol, textcol, textmode, fillpattern, interiorcol) \
	obspec.index = OBSPEC_MAKE(ch, framesize, framecol, textcol, textmode, fillpattern, interiorcol)


#ifndef _SWINFO
#define _SWINFO
typedef struct {
	char	*string;					/* etwa "TOS|KAOS|MAG!X" */
	_WORD	num;						/* Nr. der aktuellen Zeichenkette */
	_WORD	maxnum;						/* maximal erlaubtes <num> */
} SWINFO;
#endif /* SWINFO */

#ifndef _POPINFO
#define _POPINFO
typedef struct {
	struct	_object *tree;				/* Popup- Menue */
	_WORD	obnum;						/* aktuelles Objekt von <tree> */
} POPINFO;
#endif

#ifndef _MT_GEMLIB_H_
typedef union obspecptr
{
	_LONG_PTR	index;
	union obspecptr *indirect;
	bfobspec	obspec;
	TEDINFO		*tedinfo;
	ICONBLK		*iconblk;
	BITBLK		*bitblk;
	USERBLK		*userblk;
	CICONBLK	*ciconblk;
	char		*free_string;
} OBSPEC;
#endif


#ifndef _MT_GEMLIB_H_
typedef struct _object
{
	_WORD	ob_next;					/* -> object's next sibling */
	_WORD	ob_head;					/* -> head of object's children */
	_WORD	ob_tail;					/* -> tail of object's children */
	_UWORD	ob_type;					/* object type: BOX, CHAR,... */
	_UWORD	ob_flags;					/* object flags */
	_UWORD	ob_state;					/* state: OS_SELECTED, OPEN, ... */
	OBSPEC	ob_spec;					/* "out": -> anything else */
	_WORD	ob_x;						/* upper left corner of object */
	_WORD	ob_y;						/* upper left corner of object */
	_WORD	ob_width;					/* object width */
	_WORD	ob_height;					/* object height */
} OBJECT;
#endif


#ifndef _MT_GEMLIB_H_
typedef struct parm_block
{
	OBJECT	*pb_tree;
	_WORD	pb_obj;
	_UWORD	pb_prevstate;
	_UWORD	pb_currstate;
	_WORD	pb_x, pb_y, pb_w, pb_h;
	_WORD	pb_xc, pb_yc, pb_wc, pb_hc;
	_LONG_PTR	pb_parm;
} PARMBLK;
#endif

/****** Object definitions **********************************************/

/* graphic types of obs */
#define G_BOX           20
#define G_TEXT          21
#define G_BOXTEXT       22
#define G_IMAGE         23
#define G_USERDEF       24
#define G_PROGDEF       G_USERDEF
#define G_IBOX          25
#define G_BUTTON        26
#define G_BOXCHAR       27
#define G_STRING        28
#define G_FTEXT         29
#define G_FBOXTEXT      30
#define G_ICON          31
#define G_TITLE         32
#define G_CICON         33              /* AES >= 3.3 */
#define G_CLRICN		G_CICON			/* From ViewMAX beta. Incompatible with Atari colour icons. */
#define G_SWBUTTON      34              /* MAG!X */
#define G_DTMFDB		34				/* ViewMax: for internal AES use only: desktop image */
#define G_POPUP         35              /* MAG!X */
#define G_RESVD1        36              /* MagiC 3.1 */
#define G_WINTITLE		36				/* MagiC internal window title */
#define G_EDIT			37				/* MagiC extended edit object */
#define G_SHORTCUT		38				/* MagiC 6 menu entry with shortcut */
#define G_WORDCUT       G_SHORTCUT
#define G_SLIST			39				/* XaAES scrolling list */
#define G_EXTBOX		40				/* XaAES */
#define G_OBLINK		41				/* XaAES */


/* Object flags */
#define OF_NONE            0x0000
#define OF_SELECTABLE      0x0001
#define OF_DEFAULT         0x0002
#define OF_EXIT            0x0004
#define OF_EDITABLE        0x0008
#define OF_RBUTTON         0x0010
#define OF_LASTOB          0x0020
#define OF_TOUCHEXIT       0x0040
#define OF_HIDETREE        0x0080
#define OF_INDIRECT        0x0100
/* 3D objects AES 3.4	*/
#undef OF_FL3DMASK
#define OF_FL3DMASK        0x0600
#define OF_FL3DNONE        0x0000
#define OF_FL3DIND		   0x0200
#define OF_FL3DBAK		   0x0400
#define OF_FL3DACT		   0x0600
#define OF_SUBMENU         0x0800         /* falcon aes hierarchical menus */
#define OF_FLAG11		   OF_SUBMENU
#define OF_FLAG12		   0x1000
#define OF_FLAG13		   0x2000
#define OF_FLAG14		   0x4000
#define OF_FLAG15		   0x8000
/* ViewMAX */
#define OF_ESCCANCEL		0x0200
#define OF_BITBUTTON		0x0400
#define OF_SCROLLER 		0x0800
#define OF_FLAG3D			0x1000
#define OF_USECOLORCAT		0x2000


/* Object states */
#define OS_NORMAL		0x0000
#define OS_SELECTED		0x0001
#define OS_CROSSED		0x0002
#define OS_CHECKED		0x0004
#define OS_DISABLED		0x0008
#define OS_OUTLINED		0x0010
#define OS_SHADOWED		0x0020
#define OS_WHITEBAK		0x0040
#define OS_DRAW3D		0x0080
#define OS_STATE08		0x0100
#define OS_STATE09		0x0200
#define OS_STATE10		0x0400
#define OS_STATE11		0x0800
#define OS_STATE12		0x1000
#define OS_STATE13		0x2000
#define OS_STATE14		0x4000
#define OS_STATE15		0x8000
/* ViewMAX */
#define OS_HIGHLIGHTED	0x0100
#define OS_UNHIGHLIGHTED 0x0200

/* Object colors - default pall. */
#define G_WHITE			0
#define G_BLACK			1
#define G_RED			2
#define G_GREEN			3
#define G_BLUE			4
#define G_CYAN			5
#define G_YELLOW		6
#define G_MAGENTA		7
#define G_LWHITE		8
#define G_LBLACK		9
#define G_LRED			10
#define G_LGREEN		11
#define G_LBLUE			12
#define G_LCYAN			13
#define G_LYELLOW		14
#define G_LMAGENTA		15


#if !defined(__USE_GEMLIB) || defined(__GEMLIB_OLDNAMES)

/* object flags */
#define NONE		 	OF_NONE
#define SELECTABLE		OF_SELECTABLE
#define DEFAULT			OF_DEFAULT
#define EXIT			OF_EXIT
#define EDITABLE		OF_EDITABLE
#define RBUTTON			OF_RBUTTON
#define LASTOB			OF_LASTOB
#define TOUCHEXIT		OF_TOUCHEXIT
#define HIDETREE		OF_HIDETREE
#define INDIRECT		OF_INDIRECT
/* 3D objects AES 3.4	*/
#define FL3DMASK        OF_FL3DMASK
#define FL3DNONE        OF_FL3DNONE
#define FL3DIND         OF_FL3DIND         /* 3D Indicator      AES 4.0 */
#define FL3DBAK         OF_FL3DBAK         /* 3D Background     AES 4.0 */
#define FL3DACT         OF_FL3DACT         /* 3D Activator      AES 4.0 */
#define SUBMENU			OF_SUBMENU	/* bit 11 */
#define FLAG11			OF_FLAG11
#define FLAG12			OF_FLAG12
#define FLAG13			OF_FLAG13
#define FLAG14			OF_FLAG14
#define FLAG15			OF_FLAG15

/* ViewMAX */
#define ESCCANCEL		OF_ESCCANCEL
#define BITBUTTON		OF_BITBUTTON
#define SCROLLER 		OF_SCROLLER
#define FLAG3D			OF_FLAG3D
#define USECOLORCAT		OF_USECOLORCAT

/* Object states */
#define NORMAL          OS_NORMAL
#define SELECTED        OS_SELECTED
#define CROSSED         OS_CROSSED
#define CHECKED         OS_CHECKED
#define DISABLED        OS_DISABLED
#define OUTLINED        OS_OUTLINED
#define SHADOWED        OS_SHADOWED
#define WHITEBAK        OS_WHITEBAK            /* TOS         */
#define DRAW3D          OS_DRAW3D            /* GEM 2.x     */
/* ViewMAX */
#define HIGHLIGHTED		OS_HIGHLIGHTED
#define UNHIGHLIGHTED	OS_UNHIGHLIGHTED

/* Object colors */
#if !defined(__COLORS)
/*
 * using AES-colors and BGI-colors
 * is not possible
 */
#define __COLORS
#define WHITE            0
#define BLACK            1
#define RED              2
#define GREEN            3
#define BLUE             4
#define CYAN             5
#define YELLOW           6
#define MAGENTA          7
#define LWHITE           8
#define LBLACK           9
#define LRED            10
#define LGREEN          11
#define LBLUE           12
#define LCYAN           13
#define LYELLOW         14
#define LMAGENTA        15
/* ViewMAX */
#define DWHITE			LWHITE
#define DBLACK			LBLACK
#define DRED			LRED
#define DGREEN			LGREEN
#define DBLUE			LBLUE
#define DCYAN			LCYAN
#define DYELLOW			LYELLOW
#define DMAGENTA		LMAGENTA
#endif /* __COLORS */

#endif

#ifndef NIL
# define NIL (-1)
#endif
#ifndef DESK
#define DESKTOP_HANDLE		0
#define DESK			 	DESKTOP_HANDLE
#endif
#define ROOT             0
#define MAX_LEN         81              /* max string length */
#define MAX_DEPTH        8              /* max depth of search or draw */


/* font types */
#define GDOS_PROP        0 /* Speedo GDOS font */
#define GDOS_MONO        1 /* Speedo GDOS font, force monospace output */
#define GDOS_BITM        2 /* GDOS bit map font */
#define IBM              3
#define SMALL            5
#define TE_FONT_MASK     7


/* editable text field definitions */
#define ED_START        0
#define ED_INIT         1
#define ED_CHAR         2
#define ED_END          3
#define ED_CRSR         100            /* MAG!X */
#define ED_DRAW         103            /* MAG!X 2.00 */

#define EDSTART			ED_START	/* alias */
#define EDINIT			ED_INIT		/* alias */
#define EDCHAR			ED_CHAR		/* alias */
#define EDEND 			ED_END		/* alias */


/* editable text justification */
#define TE_LEFT         0
#define TE_RIGHT        1
#define TE_CNTR         2
#define TE_JUST_MASK    3

/* inside patterns */
#define IP_HOLLOW		0
#define IP_1PATT		1
#define IP_2PATT		2
#define IP_3PATT		3
#define IP_4PATT		4
#define IP_5PATT		5
#define IP_6PATT		6
#define IP_SOLID		7


/* data structure types */
#define R_TREE           0
#define R_OBJECT         1
#define R_TEDINFO        2
#define R_ICONBLK        3
#define R_BITBLK         4
#define R_STRING         5              /* gets pointer to free strings */
#define R_IMAGEDATA      6              /* gets pointer to free images */
#define R_OBSPEC         7
#define R_TEPTEXT        8              /* sub-pointers in TEDINFO */
#define R_TEPTMPLT       9
#define R_TEPVALID      10
#define R_IBPMASK       11              /* sub-pointers in ICONBLK */
#define R_IBPDATA       12
#define R_IBPTEXT       13
#define R_BIPDATA       14              /* sub-pointers in BITBLK */
#define R_FRSTR         15              /* gets addr of pointer to free strings */
#define R_FRIMG         16              /* gets addr of pointer to free images  */

#ifndef _MT_GEMLIB_H_
typedef struct rshdr
{
	_UWORD	rsh_vrsn;
	_UWORD	rsh_object;
	_UWORD	rsh_tedinfo;
	_UWORD	rsh_iconblk;	/* list of ICONBLKS */
	_UWORD	rsh_bitblk;
	_UWORD	rsh_frstr;
	_UWORD	rsh_string;
	_UWORD	rsh_imdata;		/* image data */
	_UWORD	rsh_frimg;
	_UWORD	rsh_trindex;
	_UWORD	rsh_nobs;		/* counts of various structs */
	_UWORD	rsh_ntree;
	_UWORD	rsh_nted;
	_UWORD	rsh_nib;
	_UWORD	rsh_nbb;
	_UWORD	rsh_nstring;
	_UWORD	rsh_nimages;
	_UWORD	rsh_rssize;		/* total bytes in resource */
} RSHDR;
#endif


/* wind calc flags */
#define WC_BORDER 0
#define WC_WORK   1


#ifndef _AES_GLOBAL_defined
#define _AES_GLOBAL_defined
/*
 * used by DESKTOP.APP of PC-GEM to store the color
 * spec of the desktop background window
 */
typedef union
{
	void *spec;			/* PC_GEM */
	_LONG_PTR l;
	short pi[2];
} aes_private;

/* At last give in to the fact that it is a struct, NOT an array */
typedef struct _aes_global {
	_WORD ap_version;
	_WORD ap_count;
	_WORD ap_id;
	aes_private ap_private;
	OBJECT **ap_ptree;
	void *ap_rscmem; /* RSHDR or RSXHDR */
	_UWORD ap_rsclen; /* note: short only; unusable with resource >64k */
	_WORD ap_planes;
	void *ap_3resv;                  /* ptr to AES global area D (struct THEGLO) */
	_WORD ap_bvdisk;
	_WORD ap_bvhard;
} AES_GLOBAL;
#endif

#define	_AESversion   (((AES_GLOBAL *)aes_global)->ap_version)
#define	_AESnumapps   (((AES_GLOBAL *)aes_global)->ap_count)
#define	_AESapid      (((AES_GLOBAL *)aes_global)->ap_id)
#define	_AESappglobal (((AES_GLOBAL *)aes_global)->ap_private.l)
#define	_AESrscfile   (((AES_GLOBAL *)aes_global)->ap_ptree)
#define	_AESrscmem    (((AES_GLOBAL *)aes_global)->ap_rscmem)
#define	_AESrsclen    (((AES_GLOBAL *)aes_global)->ap_rsclen)
#define	_AESmaxchar   (((AES_GLOBAL *)aes_global)->ap_bvdisk)
#define	_AESminchar   (((AES_GLOBAL *)aes_global)->ap_bvhard)

/* Mouse form definition block */

#ifndef _MT_GEMLIB_H_
typedef struct mfstr
{
	_WORD	mf_xhot;
	_WORD	mf_yhot;
	_WORD	mf_nplanes;
	_WORD	mf_fg;
	_WORD	mf_bg;
	_WORD	mf_mask[16];
	_WORD	mf_data[16];
} MFORM;
#endif


/************************************************************************/
/* end of portable definitions											*/
/************************************************************************/

#if defined(__TOS__) || defined(__atarist__)

#ifdef __PUREC__
#include <wdlgevnt.h>
#endif

/****** GEMparams *******************************************************/

#ifndef _MT_GEMLIB_H_
/** size of the aes_control[] array */
#define AES_CTRLMAX		6		/* actually 5; use 6 to make it long aligned */
/** size of the aes_global[] array */
#define AES_GLOBMAX		16
/** size of the aes_intin[] array */
#define AES_INTINMAX 		16
/** size of the aes_intout[] array */
#define AES_INTOUTMAX		16
/** size of the aes_addrin[] array */
#define AES_ADDRINMAX		16
/** size of the aes_addrout[] array */
#define AES_ADDROUTMAX		16

/* Array sizes in vdi control block */
#define VDI_CNTRLMAX     16		/* max size of vdi_control[] ; actually 15; use 16 to make it long aligned */
#define VDI_INTINMAX   1024		/* max size of vdi_intin[] */
#define VDI_INTOUTMAX   256		/* max size of vdi_intout[] */
#define VDI_PTSINMAX    256		/* max size of vdi_ptsin[] */
#define VDI_PTSOUTMAX   256		/* max size of vdi_ptsout[] */

typedef struct
{
	_WORD	contrl[VDI_CNTRLMAX];
	_WORD	global[AES_GLOBMAX];
	_WORD	intin[VDI_INTINMAX];
	_WORD	intout[VDI_INTOUTMAX];
	_WORD	ptsout[VDI_PTSOUTMAX];
	void	*addrin[AES_ADDRINMAX];
	void	*addrout[AES_ADDROUTMAX];
	_WORD	ptsin[VDI_PTSINMAX];
	_WORD	acontrl[AES_CTRLMAX];
	_WORD	aintin[AES_INTINMAX];
	_WORD	aintout[AES_INTOUTMAX];
} GEMPARBLK;

typedef struct _aes_control {
	_WORD opcode;
	_WORD nintin;
	_WORD nintout;
	_WORD naddrin;
	_WORD naddrout;
} AES_CONTROL;

typedef struct
{
	_WORD *control;
	_WORD *global;
	_WORD *intin;
	_WORD *intout;
	void **addrin;
	void **addrout;
} AESPARBLK;

typedef AESPARBLK AESPB; /* MagiC name */

extern AESPARBLK _AesParBlk;
extern GEMPARBLK _GemParBlk;
#endif

extern _WORD gl_apid;
extern _WORD gl_ap_version;
/** global AES array */
extern _WORD aes_global[];
#ifdef __AHCC__
#define aes_global _GemParBlk.global
#endif

int _AesCall( _LONG c0to3); /* c4=0 */ /* MO */
int _AesXCall( _LONG c0to3, _WORD c4);  /* MO */
short vq_aes(void);
void _crystal(AESPB *aespb);
_WORD _aes(_WORD dummy, _LONG code);
#ifndef _MT_GEMLIB_H_
_WORD aes(AESPB *pb);
#endif
_WORD _mt_aes(AESPB *pb, _LONG code);



/* maybe also declared in OS headers */
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
	long      __CDECL (*etv_critic)(short err);         /* etv_critic of GEMDOS      */
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
	long	date;							/* Creation date ddmmyyyy          */
    void    (*chgres)(short res, short txt);    /* Change resolution               */
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


/****** Application definitions *****************************************/

/* appl_read modes */
#define APR_NOWAIT			-1	/* Do not wait for message -- see mt_appl_read() */

/* appl_search modes */
#define APP_FIRST 0	/* MO */
#define APP_NEXT  1	/* MO */
#define APP_DESK  2	/* MO */

#define X_APS_CHILD0    0x7100 /* Geneva */
#define X_APS_CHILD     0x7101 /* Geneva */
#define X_APS_CHEXIT    -1 /* Geneva */

/* alternative names for appl_search modes: */
#define APS_FIRST APP_FIRST
#define APS_NEXT  APP_NEXT
#define APS_SHEL  APP_DESK

/* application type (appl_search return values) */
#undef APP_SYSTEM
#define APP_SYSTEM			0x001
#undef APP_APPLICATION
#define APP_APPLICATION		0x002
#undef APP_ACCESSORY
#define APP_ACCESSORY		0x004
#undef APP_SHELL
#define APP_SHELL 			0x008
#undef APP_AESSYS
#define APP_AESSYS			0x010
#undef APP_AESTHREAD
#define APP_AESTHREAD		0x020
#undef APP_TASKINFO
#define APP_TASKINFO        0x100 /* XaAES extension for taskbar applications. */
#undef APP_HIDDEN
#define APP_HIDDEN          0x100 /* Task is disabled; XaAES only for APP_TASKINFO */
#undef APP_FOCUS
#define APP_FOCUS 	        0x200 /* Active application; XaAES only for APP_TASKINFO */

#define APK_SYS APP_SYSTEM
#define APK_APP APP_APPLICATION
#define APK_ACC APP_ACCESSORY


#define APPL_AESFIND(mintid) \
	appl_find( (const char *)(0xFFFF0000L | (_ULONG)mintid))
#define APPL_MINTFIND(aesid) \
	appl_find( (const char *)(0xFFFE0000L | (_ULONG)aesid))
#define APPL_CURRFIND() appl_find(NULL)

/* appl_getinfo modes */
#undef AES_LARGEFONT
#define AES_LARGEFONT		APG_FONT
#define APG_FONT    0   /* Get AES regular font information
                         * o1: font height, o2: font id,
                         * o3: font type: 0 - system font
                         *     1 - fsm font
                         *     2 and on to be defined in the future */

#undef AES_SMALLFONT
#define AES_SMALLFONT		APG_SMLFONT
#define APG_SMLFONT 1   /* Get AES small font information. see above */

#undef AES_SYSTEM
#define AES_SYSTEM			APG_REZ
#define APG_REZ     2   /* Get AES current resolution number and the number
                         * of color is being supported by the object library
                         * o1: resolution number
                         * o2: number of color supported by AES object library
                         * o3: color icons: 0 - Not supported 1 - supported
                         */
#undef AES_LANGUAGE
#define AES_LANGUAGE 		APG_LANG
#define APG_LANG    3   /* o1: currently used language (see below) */

/* appl_getinfo return values (AES_LANGUAGE/APG_LANG) */
#define L_ENGLISH    0
#define L_GERMAN     1
#define L_FRENCH     2
#define L_SPANISH    4
#define L_ITALIAN    5
#define L_SWEDISH    6

#undef AES_PROCESS
#define AES_PROCESS 		APG_GLOBAL1
#define APG_GLOBAL1    4
        /* Get general AES environment info #1
           ap_gout1 - 0 - non-pre-emptive multitasking
                      1 - pre-emptive multitasking
           ap_gout2 - 0 - appl_find cannot convert from MiNT to AES ids
                  1 - extended appl_find modes supported
           ap_gout3 - 0 - appl_search not implemented
                  1 - appl_search implemented
           ap_gout4 - 0 - rsrc_rcfix not implemented
                  1 - rsrc_rcfix implemented
        */

#undef AES_PCGEM
#define AES_PCGEM			APG_GLOBAL2
#define APG_GLOBAL2    5
        /*
        5 - General AES environment info #2

           ap_gout1 - 0 - objc_xfind not implemented
                      1 - objc_xfind implemented
           ap_gout2 - 0 - reserved, always 0
           ap_gout3 - 0 - GEM/3 menu_click not implemented
                  1 - menu_click implemented
           ap_gout4 - 0 - GEM/3 shel_r/wdef not implemented
                  1 - shel_r/wdef implemented
        */

#undef AES_INQUIRE
#define AES_INQUIRE 		APG_GLOBAL3
#define APG_GLOBAL3    6
        /*
        6 - General AES environment info #3

           ap_gout1 - 0 - appl_read(-1) not implemented
                      1 - appl_read(-1) implemented
           ap_gout2 - 0 - shel_get(-1) not implemented
                  1 - shel_get(-1) implemented
           ap_gout3 - 0 - menu_bar(-1) not implemented
                  1 - menu_bar(-1) implemented
           ap_gout4 - 0 - menu_bar(MENU_INSTL) not implemented
                  1 - menu_bar(MENU_INSTL) implemented
        */

#undef AES_WDIALOG
#define AES_WDIALOG APG_RESVD
#define APG_RESVD  7

        /*
        7 - Reserved for OS extensions. MultiTOS sets
            ap_gout1,2,3,4 to 0.
        */

#undef AES_MOUSE
#define AES_MOUSE			APG_MOUSE
#define APG_MOUSE  8

        /*
        8 - Mouse support

           ap_gout1 - 0 - graf_mouse modes 258-260 not supported
                      1 - graf_mouse modes 258-260 supported
           ap_gout2 - 0 - application must maintain mouse form
                  1 - mouse form maintained by OS on a per-application
                  basis
        */

#undef AES_MENU
#define AES_MENU			APG_MENUS
#define APG_MENUS  9

        /*
        9 - Menu support

           ap_gout1 - 0 - submenus not supported
                      1 - MultiTOS style submenus
           ap_gout2 - 0 - popup menus not supported
                  1 - MultiTOS style popup menus
           ap_gout3 - 0 - scrollable menus not supported
                  1 - MultiTOS style scrollable menus
           ap_gout4 - 0 - extended MN_SELECTED not supported
                  1 - words 5/6/7 in MN_SELECTED message give extra info
        */

#undef AES_SHELL
#define AES_SHELL			APG_SHELLW
#define APG_SHELLW     10

        /*
        10 - shel_write info

           ap_gout1 - shel_write modes supported:
                bit 0-7: indicates highest legal value for
                    (sh_wdoex & 0x00ff)
                bit 8-15: indicates which bits in (sh_wdoex & 0xFF00)
                     are supported as in MultiTOS
           ap_gout2 - 0 - shel_write(0) launches an application
                  1 - shel_write(0) cancels previous shel_write
           ap_gout3 - 0 - shel_write(1) launches an application immediately
                  1 - shel_write(1) takes effect after current application
                  exits (like TOS 1.4)
           ap_gout4 - 0 - ARGV parameter passing not possible
                  1 - sh_wiscr controls ARGV parameter passing
        */

#undef AES_WINDOW
#define AES_WINDOW			APG_WINDOWS
#define APG_WINDOWS    11

        /*
        11 - window support

           ap_gout1 - extended WF_ functions available in wind_get/set
                  (0=not available, 1=available)
                bit 0: WF_TOP returns window below current one
                bit 1: wind_get(WF_NEWDESK) supported
                bit 2: WF_COLOR get/set supported
                bit 3: WF_DCOLOR get/set supported
                bit 4: WF_OWNER supported in wind_get
                bit 5: WF_BEVENT get/set supported
                bit 6: WF_BOTTOM supported
                bit 7: WF_ICONIFY supported
                bit 8: WF_UNICONIFY supported
                bits 9-15 reserved, 0
           ap_gout2 - reserved, 0
           ap_gout3 - new gadgets supported:
                  (0=supported, 1=not supported)
                    bit 0: iconifier
                bit 1: explicit "bottomer" gadget
                bit 2: shift+click to send window to bottom
                bit 3: "hot" close box
                all other bits: reserved, 0
           ap_gout4 - 0 - wind_update check and set not allowed
                  1 - wind_update check and set allowed
        */

#undef AES_MESSAGE
#define AES_MESSAGE			APG_MESSAGES
#define APG_MESSAGES   12

        /*
        12 - messages sent to applications

           ap_gout1 - bit field of extra messages supported (1)
                  (0=no,1=yes)
                bit 0: WM_NEWTOP message meaningful
                bit 1: WM_UNTOPPED message sent
                bit 2: WM_ONTOP message sent
                bit 3: AP_TERM message sent
                bit 4: MultiTOS shutdown and resolution change
                       messages supported
                bit 5: AES sends CH_EXIT
                bit 6: WM_BOTTOM message sent
                bit 7: WM_ICONIFY message sent
                bit 8: WM_UNICONIFY message sent
                bit 9: WM_ALLICONIFY message sent
           ap_gout2 - bit field of extra messages supported (2)
                (currently all bits are reserved and 0)
           ap_gout3 - message behaviour
                bit 0: WM_ICONIFY message gives coordinates (0=no,1=yes)
        */

#undef AES_OBJECT
#define AES_OBJECT			APG_OBJECTS
#define APG_OBJECTS    13

        /*
        13 - object information

           ap_gout1 - 0 - no 3D objects
                      1 - 3D objects supported via objc_flags
           ap_gout2 - 0 - objc_sysvar supported
                      1 - MultiTOS 1.01 objc_sysvar
                      2 - extended objc_sysvar
           ap_gout3 - 0 - only system font for TEDINFO structures
                      1 - SPEEDO and GDOS fonts allowed in TEDINFO
           ap_gout4 - reserved for OS extensions (MultiTOS always
                      sets this to 0)
                      suggestion: use this to indicate presence
                      of new object types like radio buttons
        */

#undef AES_FORM
#define AES_FORM			APG_FORMS
#define APG_FORMS      14

        /*
        14 - form library information

           ap_gout1 - 0 - no flying dialogs
                      1 - flying dialogs supported
           ap_gout2 - 0 - keyboard tables not supported
                      1 - Mag!X style keyboard tables
           ap_gout3 - 0 - last cursor position not returned
                      1 - last cursor position returned
           ap_gout4 - reserved, 0
         */

#define AES_EXTENDED		64
#define AES_NAES			65
#define AES_VERSION         96
#define AES_WOPTS           97
#define AES_WFORM			98
#undef AES_APPL_OPTION
#define AES_APPL_OPTION		99
#define AES_WINX		 22360			/* AES WINX information */

#undef AESLANG_ENGLISH
#define AESLANG_ENGLISH		L_ENGLISH
#undef AESLANG_GERMAN
#define AESLANG_GERMAN		L_GERMAN
#undef AESLANG_FRENCH
#define AESLANG_FRENCH		L_FRENCH
#undef AESLANG_SPANISH
#define AESLANG_SPANISH 	L_SPANISH
#undef AESLANG_ITALIAN
#define AESLANG_ITALIAN 	L_ITALIAN
#undef AESLANG_SWEDISH
#define AESLANG_SWEDISH 	L_SWEDISH

/* appl_getinfo return values (AES_LARGEFONT, AES_SMALLFONT) */
#define SYSTEM_FONT			0	/* see  mt_appl_getinfo() */
#define OUTLINE_FONT 		1	/* see  mt_appl_getinfo() */


/* appl_control[ap_cwhat]: */
#define APC_TOPNEXT         0   /* OAESis internal mode */
#define APC_KILL            1   /* OAESis internal mode */
#define APC_SYSTEM          2   /* XaAES internal mode */
#define APC_HIDE            10  /* Hide application */
#define APC_SHOW            11  /* Show application */
#define APC_TOP             12  /* Bring application to front */
#define APC_HIDENOT         13  /* Hide all applications except the one referred to by ap_cid */
#define APC_INFO            14  /* Get the application parameter */
#define APC_MENU            15  /* The last used menu tree is returned */
#define APC_WIDGETS         16  /* Inquires or sets the 'default' positions of the window widgets */
#define APC_APP_CONFIG      17  /* Change some way to manage application by AES most of them can be already set in configuration file -- see mt_appl_control() */
#define APC_INFORM_MESAG    18  /* Request/Remove the sent an user Unix Signal to application when AES message is available -- see mt_appl_control() */

/* APC_INFO bits */
#define APCI_HIDDEN			0x01  /* the application is hidden -- subopcode for #APC_INFO */
#define APCI_HASMBAR		0x02  /* the application has a menu bar -- subopcode for #APC_INFO */
#define APCI_HASDESK		0x04  /* the application has a own desk -- subopcode for #APC_INFO */

/* appl_trecord types */
#define APPEVNT_TIMER	 	0	/* see struct pEvntrec */
#define APPEVNT_BUTTON	 	1	/* see struct pEvntrec */
#define APPEVNT_MOUSE	 	2	/* see struct pEvntrec */
#define APPEVNT_KEYBOARD 	3	/* see struct pEvntrec */

/** struct used by mt_appl_trecord() and mt_appl_tplay()
 *
 * \a ap_event defines the required interpretation of \a ap_value
 *  as follows:
 *  <table>
 *  <tr><td>\a ap_event <td> \a ap_value
 *  <tr><td> #APPEVNT_TIMER (0) <td> Elapsed Time (in milliseconds)
 *  <tr><td> #APPEVNT_BUTTON (1) <td> low word  = state (1 = down), high word = # of clicks
 *  <tr><td> #APPEVNT_MOUSE (2) <td> low word  = X pos, high word = Y pos
 *  <tr><td> #APPEVNT_KEYBOARD (3) <td> bits 0-7 = ASCII code, bits 8-15 = scan code, bits 16-31 = shift key
 *
 *  Please read documentation of mt_appl_trecord() and mt_appl_tplay() for more details and
 *  known bugs related to this structure.
 */
#ifndef _MT_GEMLIB_H_
typedef struct pEvntrec
{
	long ap_event;		/* one of the APPEVNT_XXX constant */
	long ap_value;		/* kind of data depends on \a ap_event */
} EVNTREC;
#endif


/* extended appl_write structure */

typedef struct
{
	_WORD	dst_apid;
	_WORD	unique_flg;
	void *	attached_mem;
	_WORD *	msgbuf;
} XAESMSG;

_WORD appl_init( void );
_WORD appl_read( _WORD ap_rid, _WORD ap_rlength, void *ap_rpbuff );
_WORD appl_write( _WORD ap_wid, _WORD ap_wlength, const void *ap_wpbuff );
_WORD appl_find( const char *ap_fpname );
_WORD appl_tplay( void *ap_tpmem, _WORD ap_tpnum, _WORD ap_tpscale );
_WORD appl_trecord( void *ap_trmem, _WORD ap_trcount );
_WORD appl_exit( void );
_WORD appl_search( _WORD ap_smode, char *ap_sname, _WORD *ap_stype, _WORD *ap_sid ); /* AES 4.0 */
_WORD appl_getinfo( _WORD ap_gtype, _WORD *ap_gout1, _WORD *ap_gout2, _WORD *ap_gout3, _WORD *ap_gout4); /* AES 4.0 */
_WORD appl_getinfo_str(_WORD type, char *out1, char *out2, char *out3, char *out4);
_WORD appl_xgetinfo( _WORD ap_gtype, _WORD *ap_gout1, _WORD *ap_gout2, _WORD *ap_gout3, _WORD *ap_gout4); /* AES 4.0 */
_WORD appl_control(_WORD ap_cid, _WORD ap_cwhat, void *ap_cout);
_WORD appl_yield(void);
_WORD appl_bvset(_WORD bvdisks, _WORD bvharddisks);
_WORD appl_xbvget(_ULONG *bvdisk, _ULONG *bvhard);
_WORD appl_xbvset(_ULONG bvdisk, _ULONG bvhard);
void _appl_yield(void);


/****** Event definitions ***********************************************/

#define MU_KEYBD         0x0001
#define MU_BUTTON        0x0002
#define MU_M1            0x0004
#define MU_M2            0x0008
#define MU_MESAG         0x0010
#define MU_TIMER         0x0020
#define MU_WHEEL         0x0040		 /* AES 4.09 & XaAES */
#define MU_MX            0x0080      /* XaAES */
#define MU_NORM_KEYBD    0x0100      /*   "   */
#define MU_DYNAMIC_KEYBD 0x0200      /*                      keybd as a bunch of buttons, includes release of key */

/* evnt_button flags */
#define LEFT_BUTTON		0x0001	/* mask for left mouse button */
#define RIGHT_BUTTON 	0x0002	/* mask for right mouse button */
#define MIDDLE_BUTTON	0x0004	/* mask for middle mouse button */

#define MN_SELECTED     10
#define WM_REDRAW       20
#define WM_TOPPED       21
#define WM_CLOSED       22
#define WM_FULLED       23
#define WM_ARROWED      24
#define WM_HSLID        25
#define WM_VSLID        26
#define WM_SIZED        27
#define WM_MOVED        28
#define WM_NEWTOP       29
#define WM_UNTOPPED     30          /* GEM 2.x, AES 4.0 */
#define WM_ONTOP        31          /* AES 4.0, GEM/3 */
#define WM_BACKDROPPED  31          /* Kaos 1.4.2 */
#define WM_OFFTOP       32          /* MultiGEM, GEM/3 */
#define PR_FINISH       33          /* GEM/3 */
#define WM_BOTTOM		33
#define WM_BOTTOMED		WM_BOTTOM   /* AES 4.1 */
#define WM_BACK         WM_BOTTOM	/* WINX */
#define WM_ICONIFY      34          /* AES 4.1 */
#define WM_UNICONIFY    35          /* AES 4.1 */
#define WM_ALLICONIFY   36          /* AES 4.1 */
#define WM_TOOLBAR      37
#define WM_REPOSED      38          /* XaAES */

#define AC_OPEN         40
#define AC_CLOSE        41
#define WM_ISTOP        43          /* MultiGEM */
#define CT_UPDATE       50
#define CT_MOVE         51
#define CT_NEWTOP       52
#define CT_KEY          53
#define CT_SWITCH       53
#define AP_TERM         50          /* AES 4.0 */
#define AP_TFAIL        51          /* AES 4.0 */
#define AP_RESCHG       57          /* AES 4.0 */
#define SHUT_COMPLETED  60          /* AES 4.0 */
#define RESCHG_COMPLETED 61
#define RESCH_COMPLETED RESCHG_COMPLETED          /* AES 4.0 */
#define AP_DRAGDROP     63          /* AES 4.0 */
#define SH_EXIT         68          /* AES 4.0 */
#define SH_START        69          /* AES 4.0 */
#define TDI_Question    70          /* TDI Modula */
#define TDI_Answer      71          /* TDI Modula */
#define SH_WDRAW        72          /* AES 4.0 */
#define SC_CHANGED      80
#define PRN_CHANGED     82          /* NVDI */
#define FNT_CHANGED     83          /* NVDI */
#define COLORS_CHANGED  84          /* NVDI */
#define THR_EXIT        88          /* MagiC 4.5 */
#define PA_EXIT         89          /* MagiC 3 */
#define CH_EXIT         90          /* AES 4.0 */
#define WM_M_BDROPPED   100         /* KAOS 1.4 */
#define WM_BACKDROP		WM_M_BDROPPED
#define SM_M_SPECIAL    101         /* MAG!X */
#define SM_M_RES2       102         /* MAG!X */
#define SM_M_RES3       103         /* MAG!X */
#define SM_M_RES4       104         /* MAG!X */
#define SM_M_RES5       105         /* MAG!X */
#define SM_M_RES6       106         /* MAG!X */
#define SM_M_RES7       107         /* MAG!X */
#define SM_M_RES8       108         /* MAG!X */
#define SM_M_RES9       109         /* MAG!X */
#define WM_WHEEL        345         /* XaAES */
#define WM_MOUSEWHEEL   2352
#define WM_SHADED       22360       /* WiNX */
#define WM_UNSHADED     22361       /* WinX */

/* SM_M_SPECIAL codes */
#define SMC_TIDY_UP     0           /* MagiC 2  */
#define SMC_TERMINATE   1           /* MagiC 2  */
#define SMC_SWITCH      2           /* MagiC 2  */
#define SMC_FREEZE      3           /* MagiC 2  */
#define SMC_UNFREEZE    4           /* MagiC 2  */
#define SMC_TASKSWITCH  5           /* MagiC 2  */
#define SMC_UNHIDEALL   6           /* MagiC 3.1 */
#define SMC_HIDEOTHERS  7           /* MagiC 3.1 */
#define SMC_HIDEACT     8           /* MagiC 3.1 */

/* evnt_mouse modes */
#define MO_ENTER		0	/* Wait for mouse to enter rectangle, see mt_evnt_mouse() */
#define MO_LEAVE		1	/* Wait for mouse to leave rectangle, see mt_evnt_mouse() */

/* AP_DRAGDROP return codes */
#define DD_OK        0
#define DD_NAK       1
#define DD_EXT       2
#define DD_LEN       3
#define DD_TRASH     4
#define DD_PRINTER   5
#define DD_CLIPBOARD 6

#define DD_TIMEOUT	4000		/* Timeout in ms */

#define DD_NUMEXTS	8			/* Number of formats */
#define DD_EXTLEN   4
#define DD_EXTSIZE	(DD_NUMEXTS * DD_EXTLEN)
#define DD_NAMEMAX	128
#define DD_TIMEOUT	4000

#define DD_FNAME	"U:\\PIPE\\DRAGDROP.AA"
#define DD_NAMEMAX	128			/* Maximum length of a format name */
#define DD_HDRMIN	9			/* Minimum length of Drag&Drop headers */
#define DD_HDRMAX	( 8 + DD_NAMEMAX )	/* Maximum length */


/* SC_CHANGED formats */
#define SCF_INDEF       0		/* nothing specified */
#define SCF_DATABASE    0x0001	/* data to be loaded into a database (".DBF", ".CSV", ...) */
#define SCF_DBASE       SCF_DATABASE
#define SCF_TEXT        0x0002	/* text files (".TXT", ".ASC", ".RTF", ".DOC", ...) */
#define SCF_VECTOR      0x0004	/* vector graphics (".GEM", ".EPS", ".CVG", ".DXF", ...) */
#define SCF_RASTER      0x0008	/* bitmap graphics (".IMG", ".TIF", ".GIF", ".PCX", ".IFF", ...) */
#define SCF_SPREADSHEET 0x0010	/* spreadsheet data (".DIF", ".WKS", ...) */
#define SCF_SAMPLE      0x0020	/* samples, MIDI files, sound, ... (".MOD", ".SND", ...) */
#define SCF_SOUND       SCF_SAMPLE
#define SCF_ARCHIVE     0x0040	/* archives (".ZIP", ".LZH", ...) */
#define SCF_SYSTEM      0x8000	/* system files */


typedef struct _mevent
{
	_UWORD	e_flags;
	_UWORD	e_bclk;
	_UWORD	e_bmsk;
	_UWORD	e_bst;
	_UWORD	e_m1flags;
	GRECT	e_m1;
	_UWORD	e_m2flags;
	GRECT	e_m2;
	_WORD	*e_mepbuf;
	_ULONG	e_time;
	_WORD	e_mx;
	_WORD	e_my;
	_UWORD	e_mb;
	_UWORD	e_ks;
	_UWORD	e_kr;
	_UWORD	e_br;
	_UWORD	e_m3flags;
	GRECT	e_m3;
	_WORD	e_xtra0;
	_WORD	*e_smepbuf;
	_ULONG	e_xtra1;
	_ULONG	e_xtra2;
} MEVENT;

#ifndef _MT_GEMLIB_H_
typedef struct mouse_event_type
{
	_WORD *x;
	_WORD *y;
	_WORD *b;
	_WORD *k;
} MOUSE_EVENT;
#endif

#ifndef __PXY
# define __PXY
typedef struct point_coord
{
	_WORD p_x;
	_WORD p_y;
} PXY;
#endif

/** structure comprising the most of the input arguments of mt_evnt_multi()
 */
#ifndef _MT_GEMLIB_H_
typedef struct {
	_WORD emi_flags;          /* the event mask to watch */
	_WORD emi_bclicks;		  /* see mt_evnt_multi() */
	_WORD emi_bmask;		  /* see mt_evnt_multi() */
	_WORD emi_bstate;		  /* see mt_evnt_multi() */
	_WORD emi_m1leave;
	GRECT emi_m1;             /* the first rectangle to watch */
	_WORD emi_m2leave;
	GRECT emi_m2;             /* the second rectangle to watch */
	_WORD emi_tlow;		  	  /* see mt_evnt_multi() */
	_WORD emi_thigh;          /* the timer 32-bit value of interval split into short type member */
} EVMULT_IN;
#endif

/** structure comprising the output arguments of mt_evnt_multi()
 *
 * @note For undocumented members consult the mt_evnt_multi() documentation.
 */
#ifndef _MT_GEMLIB_H_
typedef struct {
	_WORD emo_events;	/* the bitfield of events occured (also a return value of mt_evnt_multi_fast() */
	PXY   emo_mouse;
	_WORD emo_mbutton;
	_WORD emo_kmeta;
	_WORD emo_kreturn;
	_WORD emo_mclicks;
} EVMULT_OUT;
#endif


/* evnt_dclick flags */
#define EDC_INQUIRE		0	/* inquire double-clic rate, see mt_evnt_dclick() */
#define EDC_SET			1	/* set double-clic rate, see mt_evnt_dclick() */

_WORD evnt_button( _WORD ev_bclicks, _WORD ev_bmask, _WORD ev_bstate,
                 _WORD *ev_bmx, _WORD *ev_bmy, _WORD *ev_bbutton,
                 _WORD *ev_bkstate );
_WORD evnt_dclick( _WORD ev_dnew, _WORD ev_dgetset );
_WORD evnt_keybd( void );
_WORD evnt_mouse( _WORD ev_moflags, _WORD ev_mox, _WORD ev_moy,
                _WORD ev_mowidth, _WORD ev_moheight, _WORD *ev_momx,
                _WORD *ev_momy, _WORD *ev_mobutton,
                _WORD *ev_mokstate );
_WORD evnt_mesag( _WORD *ev_mgpbuff );
/*
 * note: these are incompatible with
 * Pure-C's original implementation
 */
_WORD evnt_timer( _ULONG interval );
_WORD evnt_multi( _WORD ev_mflags, _WORD ev_mbclicks, _WORD ev_mbmask,
                _WORD ev_mbstate, _WORD ev_mm1flags, _WORD ev_mm1x,
                _WORD ev_mm1y, _WORD ev_mm1width, _WORD ev_mm1height,
                _WORD ev_mm2flags, _WORD ev_mm2x, _WORD ev_mm2y,
                _WORD ev_mm2width, _WORD ev_mm2height,
                _WORD *ev_mmgpbuff,
                _ULONG ev_interval,
                _WORD *ev_mmox, _WORD *ev_mmoy,
                _WORD *ev_mmbutton, _WORD *ev_mmokstate,
                _WORD *ev_mkreturn, _WORD *ev_mbreturn );
_WORD evnt_event( MEVENT *mevent );


/* this is our special invention to increase evnt_multi performance */

#ifndef _EVENT
#define _EVENT 1
typedef struct /* Special type for EventMulti */
{
	/* input parameters */
	_WORD   ev_mflags, ev_mbclicks, ev_bmask, ev_mbstate,
			ev_mm1flags, ev_mm1x, ev_mm1y, ev_mm1width, ev_mm1height,
			ev_mm2flags, ev_mm2x, ev_mm2y, ev_mm2width, ev_mm2height,
			ev_mtlocount, ev_mthicount;
	/* output parameters */
	_WORD     ev_mwich, ev_mmox, ev_mmoy;
	_UWORD    ev_mmobutton, ev_mmokstate;
	_WORD     ev_mkreturn, ev_mbreturn;
	/* message buffer */
	_WORD     ev_mmgpbuf[8];
} EVENT;
#endif

_WORD EvntMulti( EVENT *evnt_struct );

/* another approach */
_WORD evnt_multi_fast(const EVMULT_IN *em_i, _WORD MesagBuf[], EVMULT_OUT *em_o);

/****** Menu definitions ************************************************/

/* menu_bar modes */
#undef MENU_INQUIRE
#define MENU_INQUIRE (-1)	/* MO */
#undef MENU_INQUIRY
#define MENU_INQUIRY   MENU_INQUIRE
#define MENU_REMOVE 0
#define MENU_ERASE   MENU_REMOVE    	/* MO */
#define MENU_HIDE   MENU_REMOVE
#define MENU_INSTALL 1
#define MENU_DISPLAY     MENU_INSTALL	/* MO */
#define MENU_SHOW    MENU_INSTALL
#define MENU_INSTL       100            /* MAG!X       */
#define MENU_GETMODE     3              /* Menueleistenmodus abfragen                */
#define MENU_SETMODE     4              /* Menueleistenmodus setzen (nicht implem.)  */
#define     MENU_HIDDEN     0x0001      /* Menueleiste nur bei Bedarf sichtbar       */
#define     MENU_PULLDOWN   0x0002      /* Pulldown-Menue (aufklappen bei Mausklick) */
#define     MENU_SHADOWED   0x0004      /* Menueleistenboxen mit Schatten            */
#define MENU_UPDATE      5              /* Update des Systemteils der Menueleiste    */

/* menu_icheck modes */
#define UNCHECK			0	/* remove the check mark of a menu item, see mt_menu_icheck() */
#define CHECK			1	/* set a check mark of a menu item, see mt_menu_icheck() */

/* menu_ienable modes */
#define DISABLE			0	/* disable a menu item, see mt_menu_ienable() */
#define ENABLE 			1	/* enable a menu item, see mt_menu_ienable() */

/* menu_istart modes */
#define MIS_GETALIGN 		0	/* get the alignment of a parent menu item with a sub-menu item, see mt_menu_istart() */
#define MIS_SETALIGN 		1	/* set the alignment of a parent menu item with a sub-menu item, see mt_menu_istart() */


#ifndef _MT_GEMLIB_H_
typedef struct
{
	OBJECT *mn_tree;		/*  - the object tree of the menu */
	_WORD	mn_menu;		/* - the parent object of the menu items */
	_WORD	mn_item;		/* - the starting menu item	*/
	_WORD	mn_scroll;		/* - the scroll field status of the menu	*/
					/* 0	- The menu will not scroll	*/
					/* !0 - it will scroll if the number of menu
					 *	 items exceed the menu scroll height. The
					 * NOTE: If the scroll field status is !0, the menu
					 *	 items must consist entirely of G_STRINGS.
					 */
	_UWORD	mn_keystate;	/* - The CTRL, ALT, SHIFT Key state at the time the	*/
} MENU;
#endif


#ifndef _MT_GEMLIB_H_
typedef struct
{
	_LONG	display;
	_LONG	drag;
	_LONG	delay;
	_LONG	speed;
	_WORD	height;
} MN_SET;
#endif

/* menu_attach modes */
#define ME_INQUIRE		0	/* inquire information on a sub-menu attached, see mt_menu_attach() */
#define ME_ATTACH 		1	/* attach or change a sub-menu, see mt_menu_attach() */
#define ME_REMOVE 		2	/* remove a sub-menu. see mt_menu_attach() */

/* menu_attach attributes */
#define SCROLL_NO 		0	/* the menu will not scroll, see MENU::mn_scroll structure */
#define SCROLL_YES		1	/* menu may scroll if it is too high, see MENU::mn_scroll structure  */

/* menu_popup modes */
#define SCROLL_LISTBOX		-1	/* display a drop-down list (with slider) instead of popup menu, see MENU::mn_scroll */

/* menu_register modes */
#define REG_NEWNAME		-1	/* register your application with a new name, see mt_menu_register() */

/* menu_settings modes */
#define MN_INQUIRE      0  /* inquire the current menu settings, see mt_menu_settings() */
#define MN_CHANGE       1  /* set the menu settings, see mt_menu_settings() */

/* menu_tnormal modes */
#define HIGHLIGHT		0	/* display the title in reverse mode, see mt_menu_tnormal() */
#define UNHIGHLIGHT		1	/* display the title in normal mode, see mt_menu_tnormal() */


_WORD menu_bar( OBJECT *me_btree, _WORD me_bshow );
_WORD menu_icheck( OBJECT *me_ctree, _WORD me_citem, _WORD me_ccheck );
_WORD menu_ienable( OBJECT *me_etree, _WORD me_eitem, _WORD me_eenable );
_WORD menu_tnormal( OBJECT *me_ntree, _WORD me_ntitle, _WORD me_nnormal );
_WORD menu_text( OBJECT *me_ttree, _WORD me_titem, const char *me_ttext );
_WORD menu_register( _WORD me_rapid, const char *me_rpstring );
_WORD menu_unregister( _WORD me_rapid ); /* GEM 2.x */
_WORD menu_popup( MENU *me_menu, _WORD me_xpos, _WORD me_ypos, MENU *me_mdata ); /* AES 4.0 */
_WORD menu_attach( _WORD me_flag, OBJECT *me_tree, _WORD me_item, MENU *me_mdata ); /* AES 4.0 */
_WORD menu_istart( _WORD me_flag, OBJECT *me_tree, _WORD me_imenu, _WORD me_item ); /* AES 4.0 */
_WORD menu_settings( _WORD me_flag, MN_SET *me_values ); /* AES 4.0 */
_WORD menu_click ( _WORD val, _WORD setit ); /* GEM 3.x */


/****** Object prototypes ************************************************/

/* the objc_sysvar ob_swhich values */
#define LK3DIND      1                  /* AES 4.0     */
#define LK3DACT      2                  /* AES 4.0     */
#define INDBUTCOL    3                  /* AES 4.0     */
#define ACTBUTCOL    4                  /* AES 4.0     */
#define BACKGRCOL    5                  /* AES 4.0     */
#define AD3DVAL      6                  /* AES 4.0     */
#define AD3DVALUE    AD3DVAL
#define MX_ENABLE3D  10                 /* MagiC 3.0   */
#define MENUCOL		 11                 /* MagiC 6     */

#define OB_GETVAR 0
#define OB_SETVAR 1

/* objc_change modes */
#define NO_DRAW			0	/* object will not be redrawn, see mt_objc_change() */
#define REDRAW 			1	/* object will be redrawn, see mt_objc_change() */

/* objc_order modes */
#define OO_LAST			-1	/* make object the last child, see mt_objc_order() */
#define OO_FIRST		0	/* make object the first child, see mt_objc_order() */

/* objc_sysvar modes */
#define SV_INQUIRE		0	/* inquire sysvar data, see mt_objc_sysvar() */
#define SV_SET 			1	/* set sysvar data, see mt_objc_sysvar() */

_WORD objc_add( OBJECT *ob_atree, _WORD ob_aparent, _WORD ob_achild );
_WORD objc_delete( OBJECT *ob_dltree, _WORD ob_dlobject );
_WORD objc_draw( OBJECT *ob_drtree, _WORD ob_drstartob,
               _WORD ob_drdepth, _WORD ob_drxclip, _WORD ob_dryclip,
               _WORD ob_drwclip, _WORD ob_drhclip );
_WORD objc_draw_grect(OBJECT *, _WORD Start, _WORD Depth, const GRECT *r);
_WORD objc_find( OBJECT *ob_ftree, _WORD ob_fstartob, _WORD ob_fdepth,
               _WORD ob_fmx, _WORD ob_fmy );
_WORD objc_offset( OBJECT *ob_oftree, _WORD ob_ofobject,
                 _WORD *ob_ofxoff, _WORD *ob_ofyoff );
_WORD objc_order( OBJECT *ob_ortree, _WORD ob_orobject,
                _WORD ob_ornewpos );
_WORD objc_edit( OBJECT *ob_edtree, _WORD ob_edobject,
               _WORD ob_edchar, _WORD *ob_edidx, _WORD ob_edkind );
_WORD objc_change( OBJECT *ob_ctree, _WORD ob_cobject,
                 _WORD ob_cresvd, _WORD ob_cxclip, _WORD ob_cyclip,
                 _WORD ob_cwclip, _WORD ob_chclip,
                 _WORD ob_cnewstate, _WORD ob_credraw );
_WORD objc_change_grect( OBJECT *ob_ctree, _WORD ob_cobject, _WORD ob_cresvd, const GRECT *clip, _WORD ob_cnewstate, _WORD ob_credraw );
_WORD objc_sysvar( _WORD ob_svmode, _WORD ob_svwhich,  /* AES 4.0 */
                 _WORD ob_svinval1, _WORD ob_svinval2,
                 _WORD *ob_svoutval1, _WORD *ob_svoutval2); /* AES 4.0 */
_WORD objc_xfind( OBJECT *ob_ftree, _WORD ob_fstartob, _WORD ob_fdepth,
               _WORD ob_fmx, _WORD ob_fmy );
void objc_wchange( OBJECT *ob_ctree, _WORD ob_cobject,
                 _WORD ob_cnewstate, GRECT *clip, _WORD whandle);
void objc_wdraw( OBJECT *ob_drtree, _WORD ob_drstartob,
               _WORD ob_drdepth, GRECT *clip, _WORD whandle);
_WORD objc_wedit( OBJECT *ob_edtree, _WORD ob_edobject,
               _WORD ob_edchar, _WORD *ob_edidx, _WORD ob_edkind, _WORD whandle );
_WORD objc_xedit( OBJECT *ob_edtree, _WORD ob_edobject,
               _WORD ob_edchar, _WORD *ob_edidx, _WORD ob_edkind, GRECT *r );


/****** Form definitions ************************************************/

/* constants for form_alert */
#define FA_NOICON   "[0]"	/* display no icon */
#define FA_ERROR    "[1]"	/* display Exclamation icon */
#define FA_QUESTION "[2]"	/* display Question icon */
#define FA_STOP     "[3]"	/* display Stop icon */
#define FA_INFO     "[4]"	/* display Info icon */
#define FA_DISK     "[5]"	/* display Disk icon */

#define FMD_START       0
#define FMD_GROW        1
#define FMD_SHRINK      2
#define FMD_FINISH      3

/* form_error modes */
#define FERR_FILENOTFOUND	 2	/* File Not Found (GEMDOS error -33) */
#define FERR_PATHNOTFOUND	 3	/* Path Not Found (GEMDOS error -34) */
#define FERR_NOHANDLES		 4	/* No More File Handles (GEMDOS error -35) */
#define FERR_ACCESSDENIED	 5	/* Access Denied (GEMDOS error -36) */
#define FERR_LOWMEM			 8	/* Insufficient Memory (GEMDOS error -39) */
#define FERR_BADENVIRON 	10	/* Invalid Environment (GEMDOS error -41) */
#define FERR_BADFORMAT		11	/* Invalid Format (GEMDOS error -42) */
#define FERR_BADDRIVE		15	/* Invalid Drive Specification (GEMDOS error -46) */
#define FERR_DELETEDIR		16	/* Attempt To Delete Working Directory (GEMDOS error -47) */
#define FERR_NOFILES 		18	/* No More Files (GEMDOS error -49) */

_WORD form_do( OBJECT *fo_dotree, _WORD fo_dostartob );
_WORD form_dial( _WORD fo_diflag, _WORD fo_dilittlx,
               _WORD fo_dilittly, _WORD fo_dilittlw,
               _WORD fo_dilittlh, _WORD fo_dibigx,
               _WORD fo_dibigy, _WORD fo_dibigw, _WORD fo_dibigh );
_WORD form_alert( _WORD fo_adefbttn, const char *fo_astring );
_WORD form_error( _WORD fo_enum );
_WORD form_center( OBJECT *fo_ctree, _WORD *fo_cx, _WORD *fo_cy,
                 _WORD *fo_cw, _WORD *fo_ch );
_WORD form_center_grect (OBJECT *, GRECT *r);
_WORD form_keybd( OBJECT *fo_ktree, _WORD fo_kobject, _WORD fo_kobnext,
                _WORD fo_kchar, _WORD *fo_knxtobject, _WORD *fo_knxtchar );
_WORD form_wkeybd( OBJECT *fo_ktree, _WORD fo_kobject, _WORD fo_kobnext,
                _WORD fo_kchar, _WORD *fo_knxtobject, _WORD *fo_knxtchar, _WORD whandle );
_WORD form_button( OBJECT *fo_btree, _WORD fo_bobject, _WORD fo_bclicks,
                _WORD *fo_bnxtobj );
_WORD form_wbutton( OBJECT *fo_btree, _WORD fo_bobject, _WORD fo_bclicks,
                _WORD *fo_bnxtobj, _WORD whandle );
_WORD form_dial_grect( _WORD subfn, const GRECT *lg, const GRECT *bg );

/* MagiC */

#ifndef _SCANX
#define _SCANX
typedef struct
{
	unsigned char	scancode;
	unsigned char	nclicks;
	_WORD	objnr;
} SCANX;
#endif

#ifndef _XDO_INF
#define _XDO_INF
typedef struct
{
	SCANX	*unsh;
	SCANX	*shift;
	SCANX	*ctrl;
	SCANX	*alt;
	void	*resvd;
} XDO_INF;
#endif

/** parameters for the init callback function (7th parameter of xfrm_popup() )
 */
struct POPUP_INIT_args
{
	OBJECT *tree;
	_WORD scrollpos;
	_WORD nlines;
	void *param;
};

_WORD form_popup(OBJECT *tree, _WORD x, _WORD y);
_WORD form_xdial( _WORD fo_diflag, _WORD fo_dilittlx,
               _WORD fo_dilittly, _WORD fo_dilittlw,
               _WORD fo_dilittlh, _WORD fo_dibigx,
               _WORD fo_dibigy, _WORD fo_dibigw, _WORD fo_dibigh,
               void **flydial );
_WORD form_xdial_grect( _WORD fo_diflag, const GRECT *little, const GRECT *big, void **flydial );
_WORD form_xdo( OBJECT *tree, _WORD startob, _WORD *lastcrsr, XDO_INF *tabs, void *flydial);
_WORD form_xerr(_LONG errcode, const char *errfile);
_WORD xfrm_popup(
                OBJECT *tree, _WORD x, _WORD y,
                _WORD firstscrlob, _WORD lastscrlob,
                _WORD nlines,
                void __CDECL (*init)(struct POPUP_INIT_args),
                void *param, _WORD *lastscrlpos );     /* MagiC 5.03 */


/****** Graph definitions ************************************************/


/* Mouse forms */

#define ARROW             0
#define TEXT_CRSR         1
#define HOURGLASS         2
#undef BUSY_BEE
#define BUSY_BEE          HOURGLASS
#undef BUSYBEE
#define BUSYBEE           HOURGLASS
#define POINT_HAND        3
#define FLAT_HAND         4
#define THIN_CROSS        5
#define THICK_CROSS       6
#define OUTLN_CROSS       7
#define USER_DEF        255
#define M_OFF           256
#define M_ON            257
#define M_SAVE          258  /* MO */
#define M_RESTORE       259  /* MO */
#define M_LAST 			260
#define M_PREV          M_LAST
#define M_PREVIOUS      M_LAST
#define M_FORCE      0x8000  /* MO */
#define X_MRESET      1000	/* geneva */
#define X_MGET        1001	/* geneva */
#define X_MSET_SHAPE  1100	/* geneva */
#define XACRS_BUBBLE_DISC 270 /* The Data Uncertain logo (XaAES) */
#define XACRS_RESIZER	  271	/* The 'resize window' cursors (XaAES) */
#define XACRS_SE_SIZER  XACRS_RESIZER
#define XACRS_NE_SIZER    272
#define XACRS_MOVER		  273 /* The 'move window' cursor (XaAES) */
#define XACRS_VERTSIZER	  274 /* The 'resize vertically' cursor (XaAES) */
#define XACRS_HORSIZER	  275 /* The 'resize horizontally' cursor (XaAES) */
#define XACRS_POINTSLIDE  276 /* The 'two-arrows pointing inwards' cursor to pinpoint slider position (XaAES) */

/* 8: sizer in N.AES, horizontal arrows in Geneva */
#define X_LFTRT 9			/* Horizontal arrows (XaAES, N.AES); Vertical arrows (Geneva) */
#define X_UPDOWN 10			/* Vertical arrows (XaAES, N.AES) */


_WORD graf_rubberbox( _WORD gr_rx, _WORD gr_ry, _WORD gr_minwidth,
                    _WORD gr_minheight, _WORD *gr_rlastwidth,
                    _WORD *gr_rlastheight );
_WORD graf_rubbox( _WORD gr_rx, _WORD gr_ry, _WORD gr_minwidth,
                    _WORD gr_minheight, _WORD *gr_rlastwidth,
                    _WORD *gr_rlastheight );
_WORD graf_dragbox( _WORD gr_dwidth, _WORD gr_dheight,
                  _WORD gr_dstartx, _WORD gr_dstarty,
                  _WORD gr_dboundx, _WORD gr_dboundy,
                  _WORD gr_dboundw, _WORD gr_dboundh,
                  _WORD *gr_dfinishx, _WORD *gr_dfinishy );
_WORD graf_movebox( _WORD gr_mwidth, _WORD gr_mheight,
                  _WORD gr_msourcex, _WORD gr_msourcey,
                  _WORD gr_mdestx, _WORD gr_mdesty );
_WORD graf_mbox( _WORD gr_mwidth, _WORD gr_mheight,
                  _WORD gr_msourcex, _WORD gr_msourcey,
                  _WORD gr_mdestx, _WORD gr_mdesty );
_WORD graf_growbox( _WORD gr_gstx, _WORD gr_gsty,
                  _WORD gr_gstwidth, _WORD gr_gstheight,
                  _WORD gr_gfinx, _WORD gr_gfiny,
                  _WORD gr_gfinwidth, _WORD gr_gfinheight );
_WORD graf_growbox_grect(const GRECT *in, const GRECT *out);
_WORD graf_shrinkbox( _WORD gr_sfinx, _WORD gr_sfiny,
                    _WORD gr_sfinwidth, _WORD gr_sfinheight,
                    _WORD gr_sstx, _WORD gr_ssty,
                    _WORD gr_sstwidth, _WORD gr_sstheight );
_WORD graf_shrinkbox_grect(const GRECT *in, const GRECT *out);
_WORD graf_watchbox( OBJECT *gr_wptree, _WORD gr_wobject,
                   _WORD gr_winstate, _WORD gr_woutstate );
_WORD graf_wwatchbox( OBJECT *gr_wptree, _WORD gr_wobject,
                   _WORD gr_winstate, _WORD gr_woutstate, _WORD whandle );
_WORD graf_slidebox( OBJECT *gr_slptree, _WORD gr_slparent,
                   _WORD gr_slobject, _WORD gr_slvh );
_WORD graf_handle( _WORD *gr_hwchar, _WORD *gr_hhchar,
                 _WORD *gr_hwbox, _WORD *gr_hhbox );
_WORD graf_mouse( _WORD gr_monumber, const MFORM *gr_mofaddr );
_WORD graf_mkstate( _WORD *gr_mkmx, _WORD *gr_mkmy,
                  _WORD *gr_mkmstate, _WORD *gr_mkkstate );
_WORD graf_xhandle( _WORD *gr_hwchar, _WORD *gr_hhchar,
                 _WORD *gr_hwbox, _WORD *gr_hhbox, _WORD *dev );
_WORD graf_multirubber (_WORD bx, _WORD by, _WORD mw, _WORD mh, GRECT *rec, _WORD *rw, _WORD *rh);

/****** Scrap definitions ***********************************************/

/* scrp_read return values */
#define SCRAP_CSV       0x0001  /* clipboard has a scrap.csv file, see mt_scrap_read() */
#define SCRAP_TXT       0x0002  /* clipboard has a scrap.txt file, see mt_scrap_read() */
#define SCRAP_GEM       0x0004  /* clipboard has a scrap.gem file, see mt_scrap_read() */
#define SCRAP_IMG       0x0008  /* clipboard has a scrap.img file, see mt_scrap_read() */
#define SCRAP_DCA       0x0010  /* clipboard has a scrap.dca file, see mt_scrap_read() */
#define SCRAP_DIF       0x0020  /* clipboard has a scrap.dif file, see mt_scrap_read() */
#define SCRAP_USR       0x8000  /* clipboard has a scrap.usr file, see mt_scrap_read() */

/* alternate names used by PC-GEM */
#define SC_FTCSV SCRAP_CSV
#define SC_FTTXT SCRAP_TXT
#define SC_FTGEM SCRAP_GEM
#define SC_FTIMG SCRAP_IMG
#define SC_FTDCA SCRAP_DCA
#define SC_FTUSR SCRAP_USR

_WORD scrp_read( char *sc_rpscrap );
_WORD scrp_write( const char *sc_wpscrap );
_WORD scrp_clear( void );


/****** File selector definitions ***************************************/

/* fsel_(ex)input return values */
#define FSEL_CANCEL		 0	/* the fileselector has been closed by using the CANCEL button */
#define FSEL_OK			 1	/* the fileselector has been closed by using the OK button */

/** callback function used by BoxKite file selector. See mt_fsel_boxinput() */
typedef void __CDECL (*FSEL_CALLBACK)( _WORD *msg);

_WORD fsel_input( char *fs_iinpath, char *fs_iinsel, _WORD *fs_iexbutton );
_WORD fsel_exinput( char *fs_einpath, char *fs_einsel, _WORD *fs_eexbutton, const char *fs_elabel );
_WORD fsel_boxinput( char *fs_einpath, char *fs_einsel, _WORD *fs_eexbutton, const char *fs_elabel, FSEL_CALLBACK hndl_message );


/****** Window definitions **********************************************/

#define NAME             0x0001
#define CLOSER           0x0002
#define FULLER           0x0004
#define MOVER            0x0008
#define INFO             0x0010
#define SIZER            0x0020
#define UPARROW          0x0040
#define DNARROW          0x0080
#define VSLIDE           0x0100
#define LFARROW          0x0200
#define RTARROW          0x0400
#define HSLIDE           0x0800
#define HOTCLOSEBOX      0x1000         /* GEM 2.x     */
#define MENUBAR          0x1000			/* XaAES */
#define BACKDROP         0x2000         /* KAOS 1.4    */
#define SMALLER          0x4000         /* AES 4.1     */
#define ICONIFIER        SMALLER
#define BORDER           0x8000         /* border sizing */

/* AES wind_s/get() modes */

#define WF_KIND           1  /* G */
#define WF_NAME           2  /* G&S */
#define WF_INFO           3  /* G&S */
#define WF_WORKXYWH       4  /* G */
#define WF_CURRXYWH       5  /* G&S */
#define WF_PREVXYWH       6  /* G */
#define WF_FULLXYWH       7  /* G */
#define WF_HSLIDE         8  /* G&S */
#define WF_VSLIDE         9  /* G&S */
#define WF_TOP           10  /* G&S */
#define WF_FIRSTXYWH     11  /* G */
#define WF_NEXTXYWH      12  /* G */
#define WF_FIRSTAREAXYWH 13  /* G */
#define WF_RESVD         WF_FIRSTAREAXYWH
#define WF_NEWDESK       14  /* G&S */
#define WF_HSLSIZE       15  /* G&S */
#define WF_VSLSIZE       16  /* G&S */
#define WF_SCREEN        17  /* G */
#define WF_COLOR         18  /* G&S */
#define WF_TATTRB        18  /* PC-GEM */
#define WF_DCOLOR        19  /* G&S */
#define WF_SIZETOP       19
#define WF_OWNER         20  /* G  AES 4.0 */
#define WF_COTOP		 20  /* For ViewMAX */
#define WF_TOPAP         20  /* X/GEM */
#define WF_BEVENT        24  /* G&S  AES 4.0 */
#define WF_BOTTOM        25  /* G&S  AES 4.0 */
#define WF_ICONIFY       26  /* G&S  AES 4.1 */
#define WF_UNICONIFY     27  /* G&S  AES 4.1 */
#define WF_UNICONIFYXYWH 28  /*   S  AES 4.1 */
#define WF_TOOLBAR       30  /* G&S */
#define WF_FTOOLBAR      31  /* G */
#define WF_NTOOLBAR      32  /* G */
#define WF_MENU          33  /* XaAES */
#define WF_WIDGET        34  /* XaAES */
#define WF_WHEEL         40  /* S  XaAES */
#define WF_OPTS          41  /* G&S XaAES */
#define WF_CALCF2W       42
#define WF_CALCW2F       43
#define WF_CALCF2U       44
#define WF_CALCU2F       45
#define WF_MAXWORKXYWH   46
#define WF_M_BACKDROP   100             /* KAOS 1.4 */
#define WF_M_OWNER      101             /* KAOS 1.4 */
#define WF_M_WINDLIST   102             /* KAOS 1.4 */
#define WF_MINXYWH      103             /* MagiC 6 */
#define WF_INFOXYWH     104             /* MagiC 6.10  */
#define WF_WIDGETS      200             /* N.AES */
#define WF_USER_POINTER 230
#define WF_WIND_ATTACH  231
#define WF_TOPMOST      232             /* XaAES, MyAES */
#define WF_BITMAP		233				/* MyAES 0.96 get bitmap of the window */
#define WF_OPTIONS		234				/* MyAES 0.96 at this time use only to request automaticaly close if application lost focus and appear when focus is back */
#define WF_FULLSCREEN	235				/* MyAES 0.96 set window in fullscreen without widget */
#define WF_OBFLAG       1001            /* FreeGEM: Window tree: flag words */
#define WF_OBTYPE       1002            /* FreeGEM: Window tree: type words */
#define WF_OBSPEC       1003            /* FreeGEM: Window tree: spec dwords */
#undef X_WF_MENU
#define X_WF_MENU       0x1100          /* Geneva */
#undef X_WF_DIALOG
#define X_WF_DIALOG     0x1200          /* Geneva */
#undef X_WF_DIALWID
#define X_WF_DIALWID    0x1300          /* Geneva */
#undef X_WF_DIALHT
#define X_WF_DIALHT     0x1400          /* Geneva */
#undef X_WF_DFLTDESK
#define X_WF_DFLTDESK   0x1500          /* Geneva */
#undef X_WF_MINMAX
#define X_WF_MINMAX     0x1600          /* Geneva */
#undef X_WF_HSPLIT
#define X_WF_HSPLIT     0x1700          /* Geneva */
#undef X_WF_VSPLIT
#define X_WF_VSPLIT     0x1800          /* Geneva */
#undef X_WF_SPLMIN
#define X_WF_SPLMIN     0x1900          /* Geneva */
#undef X_WF_HSLIDE2
#define X_WF_HSLIDE2    0x1a00          /* Geneva */
#undef X_WF_VSLIDE2
#define X_WF_VSLIDE2    0x1b00          /* Geneva */
#undef X_WF_HSLSIZE2
#define X_WF_HSLSIZE2   0x1c00          /* Geneva */
#undef X_WF_VSLSIZE2
#define X_WF_VSLSIZE2   0x1d00          /* Geneva */
#undef X_WF_DIALFLGS
#define X_WF_DIALFLGS   0x1e00          /* Geneva */
  #define X_WD_ACTIVE   1       /* Mouse/keyboard events processed */
  #define X_WD_BLITSCRL 2       /* Use blit for realtime scroll */
#undef X_WF_OBJHAND
#define X_WF_OBJHAND    0x1f00          /* Geneva */
#undef X_WF_DIALEDIT
#define X_WF_DIALEDIT   0x2000          /* Geneva */
#undef X_WF_DCOLSTAT
#define X_WF_DCOLSTAT   0x2100          /* Geneva */
#undef WF_WINX
#define WF_WINX         0x5758          /* WINX 2.3 */
#undef WF_WINXCFG
#define WF_WINXCFG      0x5759          /* WINX 2.3 */
#undef WF_DDELAY
#define WF_DDELAY       0x575a          /* WINX 2.3 */
                     /* 0x575b reserved by WINX; used for appl_getinfo(11) */
                     /* 0x575c reserved by WINX; used for appl_getinfo(12) */
#undef WF_SHADE
#define WF_SHADE        0x575d          /* WINX 2.3 */
#undef WF_STACK
#define WF_STACK        0x575e          /* WINX 2.3 */
#undef WF_TOPALL
#define WF_TOPALL       0x575f          /* WINX 2.3 */
#undef WF_BOTTOMALL
#define WF_BOTTOMALL    0x5760          /* WINX 2.3 */
#undef WF_XAAES
#define WF_XAAES        0x5841          /* XaAES: 'XA' */

/* wind_set(WF_DCOLOR) */

#define W_BOX            0
#define W_TITLE          1
#define W_CLOSER         2
#define W_NAME           3
#define W_FULLER         4
#define W_INFO           5
#define W_DATA           6
#define W_WORK           7
#define W_SIZER          8
#define W_VBAR           9
#define W_UPARROW       10
#define W_DNARROW       11
#define W_VSLIDE        12
#define W_VELEV         13
#define W_HBAR          14
#define W_LFARROW       15
#define W_RTARROW       16
#define W_HSLIDE        17
#define W_HELEV         18
#define W_SMALLER       19              /* AES 4.1     */
#define W_BOTTOMER      20              /* MagiC 3     */
#define W_HIDER			30

/* wind_set(WF_BEVENT) */

#define BEVENT_WORK     0x0001          /* AES 4.0  */
#define BEVENT_INFO     0x0002          /* MagiC 6  */

/* wind_set(WF_OPTS) bitmask flags */
#define WO0_WHEEL		0x0001  /* see mt_wind_set() with #WF_OPTS mode */
#define WO0_FULLREDRAW	0x0002  /* see mt_wind_set() with #WF_OPTS mode */
#define WO0_NOBLITW		0x0004  /* see mt_wind_set() with #WF_OPTS mode */
#define WO0_NOBLITH		0x0008  /* see mt_wind_set() with #WF_OPTS mode */
#define WO0_SENDREPOS	0x0010  /* see mt_wind_set() with #WF_OPTS mode */
#define WO1_NONE        0x0000  /* see mt_wind_set() with #WF_OPTS mode */
#define WO2_NONE        0x0000  /* see mt_wind_set() with #WF_OPTS mode */

/* wind_set(WF_WHEEL) modes */
#define WHEEL_MESAG		0	/* AES will send #WM_WHEEL messages */
#define WHEEL_ARROWED	1   /* AES will send #WM_ARROWED messages */
#define WHEEL_SLIDER	2   /* AES will convert mouse wheel events to slider events */

/* Window messages */

#define WA_UPPAGE   0
#define WA_DNPAGE   1
#define WA_UPLINE   2
#define WA_DNLINE   3
#define WA_LFPAGE   4
#define WA_RTPAGE   5
#define WA_LFLINE   6
#define WA_RTLINE   7
#define WA_WHEEL    8

/* update flags */
#define END_UPDATE  0
#define BEG_UPDATE  1
#define END_MCTRL   2
#define BEG_MCTRL   3
#define BEG_CHECK   0x100   /* prevent the application from blocking */
#undef NO_BLOCK
#define NO_BLOCK BEG_CHECK

_WORD wind_create( _WORD wi_crkind, _WORD wi_crwx, _WORD wi_crwy, _WORD wi_crww, _WORD wi_crwh );
_WORD wind_open( _WORD wi_ohandle, _WORD wi_owx, _WORD wi_owy, _WORD wi_oww, _WORD wi_owh );
_WORD wind_close( _WORD wi_clhandle );
_WORD wind_delete( _WORD wi_dhandle );
_WORD wind_get( _WORD wi_ghandle, _WORD wi_gfield, _WORD *wo_gw1, _WORD *wo_gw2, _WORD *wo_gw3, _WORD *wo_gw4 );
_WORD wind_xget ( _WORD wi_ghandle, _WORD wi_gfield,
                    _WORD *wi_sw1, _WORD *wi_sw2, _WORD *wi_sw3, _WORD *wi_sw4,
                    _WORD *wo_gw1, _WORD *wo_gw2, _WORD *wo_gw3, _WORD *wo_gw4 );
_WORD wind_xget_grect ( _WORD wi_ghandle, _WORD wi_gfield, const GRECT *clip, GRECT *r);
_WORD wind_set( _WORD wi_shandle, _WORD wi_sfield, _WORD g1, _WORD g2, _WORD g3, _WORD g4 );
_WORD wind_find( _WORD wi_fmx, _WORD wi_fmy );
_WORD wind_update( _WORD wi_ubegend );
_WORD wind_calc( _WORD wi_ctype, _WORD wi_ckind, _WORD wi_cinx,
               _WORD wi_ciny, _WORD wi_cinw, _WORD wi_cinh,
               _WORD *coutx, _WORD *couty, _WORD *coutw,
               _WORD *couth );
_WORD wind_new( void );
_WORD wind_draw(_WORD wi_dhandle, _WORD wi_dstartob);

_WORD wind_calc_grect( _WORD Type, _WORD Parts, const GRECT *in, GRECT *out );
_WORD wind_create_grect ( _WORD Parts, const GRECT *r);
_WORD wind_xcreate_grect ( _WORD Parts, const GRECT *r, GRECT *ret);
_WORD wind_get_grect( _WORD whl, _WORD srt, GRECT *g);
_WORD wind_get_int( _WORD whl, _WORD srt, _WORD *g1);
_WORD wind_get_ptr( _WORD whl, _WORD srt, void **v);
_WORD wind_open_grect ( _WORD whl, const GRECT *r);
_WORD wind_set_grect( _WORD whl, _WORD srt, const GRECT *r);
_WORD wind_xset ( _WORD wi_ghandle, _WORD wi_gfield,
                    _WORD wi_sw1, _WORD wi_sw2, _WORD wi_sw3, _WORD wi_sw4,
                    _WORD *wo_gw1, _WORD *wo_gw2, _WORD *wo_gw3, _WORD *wo_gw4 );
_WORD wind_xset_grect( _WORD whl, _WORD srt, const GRECT *s, GRECT *r);
_WORD wind_set_int( _WORD whl, _WORD srt, _WORD i);
_WORD wind_set_str( _WORD whl, _WORD srt, const char *s);
_WORD wind_get_str( _WORD whl, _WORD srt, char *s);
_WORD wind_set_ptr(_WORD whl, _WORD srt, void *p1);
_WORD wind_set_ptr_int(_WORD whl, _WORD srt, void *s, _WORD g);


/****** Resource definitions ********************************************/

_WORD rsrc_load( const char *re_lpfname );
_WORD rsrc_free( void );
_WORD rsrc_gaddr( _WORD re_gtype, _WORD re_gindex, void *gaddr );
_WORD rsrc_saddr( _WORD re_stype, _WORD re_sindex, void *saddr );
_WORD rsrc_obfix( OBJECT *re_otree, _WORD re_oobject );
_WORD rsrc_rcfix( void /* RSHDR */ *rc_header ); /* AES 4.0 */
_BOOL rsrc_flip(void *hdr, _LONG size);


/****** Shell definitions ***********************************************/

/* tail for default shell */

#ifndef _MT_GEMLIB_H_
#ifndef _SHELTAIL
#define _SHELTAIL
typedef struct _sheltail {
	_WORD	dummy;                   /* ein Nullwort               */
	_LONG	magic;                   /* 'SHEL', wenn ist Shell     */
	_WORD	isfirst;                 /* erster Aufruf der Shell    */
	_LONG	lasterr;                 /* letzter Fehler             */
	_WORD	wasgr;                   /* Programm war Grafikapp.    */
} SHELTAIL;
#endif
#endif

/* shel_write modes for parameter "isover" */

#define SHW_IMMED        0              /* PC-GEM 2.x */
#define SHW_CHAIN        1              /* TOS */
#define SHW_DOS          2              /* PC-GEM 2.x */
#define SHW_PARALLEL   100              /* MAG!X */
#define SHW_SINGLE     101              /* MAG!X */

/* shel_write modes for parameter "doex" */

#define SWM_LAUNCH       0              /* MO,PC-GEM 2.x */
#define SWM_LAUNCHNOW    1              /* MO */
#define SWM_LAUNCHACC    3              /* AES 3.3 */
#define SWM_SHUTDOWN     4              /* AES 3.3 */
#define SWM_REZCHANGE    5              /* AES 3.3 */
#define SWM_BROADCAST    7              /* AES 4.0 */
#define SWM_ENVIRON      8              /* AES 4.0 */
#define SWM_NEWMSG       9              /* AES 4.0 */
#define SWM_AESMSG      10              /* AES 4.0 */
#define SWM_THRCREATE   20              /* MagiC 4.5 */
#define SWM_THREXIT     21              /* MagiC 4.5 */
#define SWM_THRKILL     22              /* MagiC 4.5 */

/* other names for shel_write modes */
#define SHW_NOEXEC			SWM_LAUNCH		/* alias */
#define SHW_EXEC			SWM_LAUNCHNOW	/* alias */
#define SHW_EXEC_ACC		SWM_LAUNCHACC	/* alias */
#define SHW_ACCEXEC         SWM_LAUNCHACC
#define SHW_SHUTDOWN		SWM_SHUTDOWN	/* alias (Geneva) */
#define SHW_RESCHNG			SWM_REZCHANGE	/* alias */
#define SHW_RESCHG          SWM_REZCHANGE
#define SHW_BROADCAST		SWM_BROADCAST	/* alias (Geneva) */
#define SHW_GLOBMSG         SWM_BROADCAST
#define SHW_SETENV
#define SHW_INFRECGN		SWM_NEWMSG		/* alias */
#define SHW_MSGREC          SWM_NEWMSG
#define SHW_AESSEND			SWM_AESMSG		/* alias */
#define SHW_SYSMSG          SWM_AESMSG
#define SHW_THR_CREATE		SWM_THRCREATE	/* alias */
#define SHW_THR_EXIT		SWM_THREXIT		/* alias */
#define SHW_THR_KILL		SWM_THRKILL		/* alias */
#define SHW_THR_TERM        SWM_THRKILL

#define SHW_RUNANY	SWM_LAUNCH	/* Run and let AES decide mode (Geneva) */
#define SHW_RUNAPP	SWM_LAUNCHNOW	/* Run an application (Geneva) */
#define SHW_RUNACC	SWM_LAUNCHACC	/* Run a desk accessory (Geneva) */
#define SHW_NEWREZ	SWM_REZCHANGE	/* Change resolution (Geneva) */
#define SHW_ENVIRON SWM_ENVIRON		/* Modify environment (Geneva) */
#define SHW_MSGTYPE	SWM_NEWMSG	/* What kind of message app can understand (Geneva) */
#define SHW_SENDTOAES	SWM_AESMSG	/* Send AES a message (Geneva) */

/* shel_write sh_wdoex parameter flags in MSB */
#define SHD_PSETLIM	(1<<8)	/* MiNT memory allocation limit */
#define SHD_PRENICE	(1<<9)	/* MiNT Prenice (priority) level */
#define SHD_DFLTDIR	(1<<10)	/* Default directory string */
#define SHD_ENVIRON	(1<<11)	/* Environment string */
#define SHD_UID		(1<<12)	/* set user id */
#define SHD_GID		(1<<13)	/* set group id */

/* shel_write, parameter wisgr */
#define TOSAPP				0  /* application launched as TOS application, see mt_shel_write() */
#define GEMAPP				1  /* application launched as GEM application, see mt_shel_write() */

/* command line parser (shel_write: parameter "wiscr") */
#define CL_NORMAL		0	/* command line passed normaly, see mt_shel_write() */
#define CL_PARSE		1	/* command line passed in ARGV environment string, see mt_shel_write() */

/* shutdown action (shel_write: mode SWM_SHUTDOWN, parameter "wiscr") */
#define SD_ABORT		0		/* Abort shutdown mode, see mt_shel_write() */
#define SD_PARTIAL		1		/* Partial shutdown mode, see mt_shel_write() */
#define SD_COMPLETE		2		/* Complete shutdown mode, see mt_shel_write() */

/* shel_write: mode SWM_ENVIRON, parameter 'wisgr' */
#define ENVIRON_SIZE	0	/* returns the current size of the environment string, see mt_shel_write() */
#define ENVIRON_CHANGE	1	/* modify an environment variable, see mt_shel_write() */
#define ENVIRON_COPY	2	/* copy the evironment string in a buffer, see mt_shel_write() */

/* shel_write: mode SWM_NEWMSG, parameter 'wisgr' */
#define NM_APTERM		0x0001	/* the application understands #AP_TERM messages, see mt_shel_write() and #SWM_NEWMSG */
#define NM_INHIBIT_HIDE	0x0002	/* the application won't be hidden, see mt_shel_write() and #SWM_NEWMSG */

/* Werte fr Modus SWM_AESMSG (fr shel_write) */
#define AP_AESTERM		52     /* Mode 10: N.AES komplett terminieren. */

/* shel_write extended mode flags */
/* extended shel_write() modes */
#define SW_PSETLIMIT	0x0100	/* Initial Psetlimit() , see SHELW::psetlimit */
#define SW_PRENICE		0x0200	/* Initial Prenice() , see SHELW::prenice */
#define SW_DEFDIR 		0x0400	/* Default Directory , see SHELW::defdir */
#define SW_ENVIRON		0x0800	/* Environment , see SHELW::env */

/* XaAES extensions for shel_write() extended modes*/
#define SW_UID			0x1000	/* Set user id of launched child, see SHELW::uid */
#define	SW_GID			0x2000	/* Set group id of launched child, see SHELW::gid */

/* MagiC 6 extensions for shel_write() extended modes*/
#define SHW_XMDFLAGS	0x1000	/* magiC 6 extension, see XSHW_COMMAND::flags*/

/* other names... */
#define SHW_XMDLIMIT	SW_PSETLIMIT	/* alias */
#define SHW_XMDNICE		SW_PRENICE		/* alias */
#define SHW_XMDDEFDIR	SW_DEFDIR		/* alias */
#define SHW_XMDENV		SW_ENVIRON		/* alias */

#define SHWF_LIMIT SW_PSETLIMIT  /* MO */
#define SHWF_NICE  SW_PRENICE    /* MO */
#define SHWF_DIR   SW_DEFDIR     /* MO */
#define SHWF_ENV   SW_ENVIRON    /* MO */
#define SHWF_FLAGS SW_UID		 /* MO */

#ifndef _MT_GEMLIB_H_
/* shel_write alternative structure for sh_wpcmd parameter */
typedef struct _shwparblk {
	char	*prgname;
	_LONG	psetlimit;
	_LONG	prenice;
	char	*directory;
	void	*environment;
    _LONG   flags;                      /* From MagiC 6 on */
} SHWPARBLK;
#endif

#ifndef _MT_GEMLIB_H_
typedef struct
{
	char *newcmd;
	long psetlimit;
	long prenice;
	char *defdir;
	char *env;
	_WORD uid;
	_WORD gid;
} SHELW;
#endif

#ifndef _MT_GEMLIB_H_
/** similar to ::SHELW, with MagiC 6 only specificity, and without XaAES extensions */
typedef struct
{
	char	*command;	/* see SHELW::newcmd */
	long	limit;		/* see SHELW::psetlimit */
	long	nice;		/* see SHELW::prenice */
	char	*defdir;	/* see SHELW::defdir */
	char	*env;		/* see SHELW::env */
	long	flags;		/* since MagiC 6.  only used if the extended mode #SHW_XMDFLAGS is set*/
} XSHW_COMMAND;
#endif

#ifndef _MT_GEMLIB_H_
#undef environ
/* Geneva */
typedef struct
{
	char *name;
	_LONG psetlimit;
	_LONG prenice;
	char *dflt_dir;
	char *environ;
} SHWRCMD;
#endif

#ifndef _MT_GEMLIB_H_
typedef struct {
	_LONG __CDECL (*proc)(void *par);
	void	*user_stack;
	_ULONG	stacksize;
	_WORD	mode;                       /* Always set to 0! */
	_LONG	res1;                       /* Always set to 0! */
} THREADINFO;
#endif

/* shel_get modes */
#define SHEL_BUFSIZE (-1)	/* return the size of AES shell buffer, see mt_shel_read() */

/* shel_help mode */
#define SHP_HELP 0      /* see mt_shel_help() */

_WORD shel_read( char *sh_rpcmd, char *sh_rptail );
_WORD shel_write( _WORD sh_wdoex, _WORD sh_wisgr, _WORD sh_wiscr,
                const void *sh_wpcmd, const char *sh_wptail );
_WORD shel_get( char *sh_gaddr, _WORD sh_glen );
_WORD shel_put( const char *sh_paddr, _WORD sh_plen );
_WORD shel_find( char *sh_fpbuff );
_WORD shel_envrn( char **sh_epvalue, const char *sh_eparm );
void shel_rdef ( char *cmd, char *dir ); /* GEM 2.x */
void shel_wdef ( const char *cmd, const char *dir ); /* GEM 2.x */

_WORD shel_help (_WORD sh_hmode, const char *sh_hfile, const char *sh_hkey);


/****** Xgrf definitions ***********************************************/

_WORD xgrf_stepcalc(_WORD orgw, _WORD orgh, _WORD xc, _WORD yc, _WORD wc, _WORD hc,
    _WORD *pxc, _WORD *pyc, _WORD *pcnt, _WORD *pxstep, _WORD *pystep);
_WORD xgrf_2box(_WORD xc, _WORD yc, _WORD w, _WORD h, _WORD corners, _WORD cnt,
    _WORD xstep, _WORD ystep, _WORD doubled);
void	xgrf_rbox(const GRECT *clip, const GRECT *box);

/* JCE 8 Jan 1998 ViewMAX UI elements (for the xgrf_colour command)
 *
 * These have been worked out by executing xgrf_colour() with different
 * parameters and seeing what changed colour.
 *
 */
#define VMAX_ATITLE	 8	/* Active titlebar */
#define	VMAX_SLIDER	 9	/* Slider (scrollbar) */
#define VMAX_DESK	10	/* Desktop */
#define	VMAX_BUTTON	11	/* Buttons */
#define	VMAX_INFO	12	/* Info bar (giving free space on drive) */
#define VMAX_ALERT  13  /* Alert box */
#define	VMAX_ITITLE	14	/* Inactive titlebar */

/* [JCE 6 May 1999] Official ViewMAX names for the above */

#define	CC_NAME		8
#define	CC_SLIDER	9
#define	CC_DESKTOP	10
#define	CC_BUTTON	11
#define	CC_INFO		12
#define	CC_ALERT	13
#define	CC_SLCTDNAME	14

/* Desktop Image Modes */
#define DT_CENTER	1
#define DT_TILE		2

/****** Proc_ definitions (GEM/XM) *************************************/

_WORD proc_create(void *ibegaddr, _LONG isize, _WORD isswap, _WORD isgem,
		_WORD *onum);
_WORD proc_run(_WORD proc_num, _WORD isgraf, _WORD isover, char *pcmd,
		char *ptail);
_WORD proc_delete(_WORD proc_num);
_WORD proc_info(_WORD num, _WORD *oisswap, _WORD *oisgem, 
		void **obegaddr, _LONG *ocsize, void **oendmem, 
		_LONG *ossize, void **ointtbl);
void *proc_malloc(_LONG size, _LONG *ret_size);
_WORD proc_switch(_WORD pid);
_WORD proc_shrink(_WORD pid);

/****** FreeGEM property definitions) **********************************/

_WORD prop_put(char *program, char *section, char *buf, _WORD options);
_WORD prop_del(char *program, char *section, _WORD options);
_WORD prop_gui_get(_WORD propnum);
_WORD prop_gui_set(_WORD propnum, _WORD value);

/* JCE 4-12-1999 "Get/set shell" calls */

_WORD xshl_getshell(char *program);
_WORD xshl_setshell(char *program);

/***********************************************************************/

#endif /* __TOS__ */

/* Utilities */
void rc_copy(const GRECT *src, GRECT *dst);
_WORD rc_equal(const GRECT *r1, const GRECT *r2);
_WORD rc_intersect(const GRECT *src, GRECT *dst);
void rc_union(const GRECT *p1, GRECT *p2);
GRECT *array_to_grect (const _WORD *array, GRECT *area);
_WORD *grect_to_array (const GRECT *area, _WORD *array);

EXTERN_C_END

#endif /* __PORTAES_H__ */
