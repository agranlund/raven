/**
 * $Id: cpx.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * XControl extension definitions
 *
 * (c) Copyright 1991, 1993 HiSoft
**/

#ifndef _CPX_H
#define _CPX_H

#include <aes.h>

typedef struct {
	short x;
	short y;
	short buttons;
	short kstate;
} MRETS;

#ifndef _MFORM_T
#define _MFORM_T
typedef struct mfstr {
	short mf_xhot;
	short mf_yhot;
	short mf_nplanes;
	short mf_fg;
	short mf_bg;
	short mf_mask[16];
	short mf_data[16];
} MFORM;
#endif

typedef struct {
	short handle;
	short booting;
	short version;
	short SkipRshFix;

	char *reserve1;
	char *reserve2;

	void (* __stdargs rsh_fix)(short,short,short,short,OBJECT *,TEDINFO *,
	  char *,ICONBLK *,BITBLK *,long *,long *,long *,void *);
	void (* __stdargs rsh_obfix)(OBJECT *,short);
	short (* __stdargs Popup)(char *[],short,short,short,GRECT *, GRECT *);
	void (* __stdargs Sl_size)(OBJECT *,short,short,short,short,short,short);
	void (* __stdargs Sl_x)(OBJECT *,short,short,short,short,short,void (* __saveds)(void));
	void (* __stdargs Sl_y)(OBJECT *,short,short,short,short,short,void (* __saveds)(void));
	void (* __stdargs Sl_arrow)(OBJECT *,short,short,short,short,short,short,short *,short,void (* __saveds)(void));
	void (* __stdargs Sl_dragx)(OBJECT *,short,short,short,short,short *,void (* __saveds)(void));
	void (* __stdargs Sl_dragy)(OBJECT *,short,short,short,short,short *,void (* __saveds)(void));
	short (* __stdargs Xform_do)(OBJECT *,short,short *);
	GRECT *(* __stdargs GetFirstRect)(GRECT *);
	GRECT *(* __stdargs GetNextRext)(void);
	void (* __stdargs Set_Evnt_Mask)(short,MOBLK *,MOBLK *,long);
#define SAVE_DEFAULTS	0
#define MEM_ERR		1
#define FILE_ERR	2
#define FILE_NOT_FOUND	3
	short (* __stdargs XGen_Alert)(short);
	short (* __stdargs CPX_Save)(void *,long);
	void *(* __stdargs Get_Buffer)(void);
	short (* __stdargs getcookie)(long cookie,long *p_value);
	short Country_Code;
#define MFSAVE	1
#define MFRESTORE	0
	void (* __stdargs MFsave)(short saveit,MFORM *mf);
} XCPB;

typedef struct {
	short (* __saveds __stdargs cpx_call)(GRECT *);
	void (* __saveds __stdargs cpx_draw)(GRECT *);
	void (* __saveds __stdargs cpx_wmove)(GRECT *);
	void (* __saveds __stdargs cpx_timer)(short *);
	void (* __saveds __stdargs cpx_key)(short,short,short *);
	void (* __saveds __stdargs cpx_button)(MRETS *,short,short *);
	void (* __saveds __stdargs cpx_m1)(MRETS *,short *);
	void (* __saveds __stdargs cpx_m2)(MRETS *mrets,short *);
	short (* __saveds __stdargs cpx_hook)(short,short *,MRETS *,short *,short *);
	void (* __saveds __stdargs cpx_close)(short);
} CPXINFO;

/* CPX header */
typedef struct _cpxhead {
#define CPXMAGIC	100			/* magic number */
	unsigned short magic;
#define CPX_RESIDENT	4		/* RAM resident flag */
#define CPX_BOOTINIT	2		/* boot initialization flag */
#define CPX_SETONLY	1			/* set only CPX flag */
	short flags;
	long cpx_id;				/* CPX ID value */
	unsigned short cpx_version;	/* CPX version number */
	char i_text[14];			/* icon text */
	unsigned short sm_icon[48];	/* icon bitmap - 32x24 pixels */
	unsigned short i_color;		/* icon colour */

	char title_txt[18];			/* title for CPX entry */
	unsigned short t_color;		/* tedinfo field for colour */
	char buffer[64];			/* buffer for RAM storage */

	char reserved[306];			/* reserved for expansion */
} CPXHEAD;
#endif
