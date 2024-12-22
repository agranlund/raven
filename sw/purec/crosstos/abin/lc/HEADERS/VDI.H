/**
 * $Id: vdi.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * VDI interface header file
 *
 * (c) Copyright 1989, 1993 HiSoft
**/

#ifndef _VDI_H
#define _VDI_H

extern short *_VDIpb[5];

extern short _VDIcontrl[];	/* default 11 */
extern short _VDIintin[];	/* default 256 */
extern short _VDIintout[];	/* default 128 */
extern short _VDIptsin[];	/* default 8 */
extern short _VDIptsout[];	/* default 10 */

/**
 * Speedo type definitions
**/
typedef signed char fix7;
typedef unsigned char ufix8;
typedef short fix15;
typedef unsigned short ufix16;
typedef long fix31;
typedef unsigned long ufix32;

typedef struct fdbstr {
	void *fd_addr;
	short fd_w;
	short fd_h;
	short fd_wdwidth;
	short fd_stand;
	short fd_nplanes;
	short fd_r1;
	short fd_r2;
	short fd_r3;
} MFDB, FDB;

/* Control library definitions */
void v_opnwk(short *,short *,short *);
void v_clswk(int);
void v_opnvwk(short *,short *,short *);
void v_clsvwk(int);
void v_clrwk(int);
void v_updwk(int);
int vst_load_fonts(int,int);
void vst_unload_fonts(int,int);
void vs_clip(int,int,short *);
int v_bez_con(int,int);
void v_bez_qual(int,int,short *);
void v_set_app_buff(void *,int);
int v_flushcache(int);
int v_loadcache(int,const char *,int);
int v_savecache(int,const char *);
void v_getoutline(int, int, short *, char *, int, short *);
void vst_scratch(int,int);
void vst_error(int,int,short *);
int v_bez_on(int);
int v_bez_off(int);
void v_xopnwk(short *, short *, short *, int, int, void **);
void v_xupdwk(int, void **, int, short *);
void v_opnbm(short *, MFDB *, short *, short *);
void v_clsbm(int);

/* Output library definitions */
void v_pline(int,int,short *);
void v_pmarker(int,int,short *);
void v_gtext(int,int,int,const char *);
void v_ftext(int,int,int,const char *);
void v_fillarea(int,int,short *);
void v_cellarray(int,short *,int,int,int,int,short *);
void v_contourfill(int,int,int,int);
void vr_recfl(int,short *);
void v_bar(int,short *);
void v_arc(int,int,int,int,int,int);
void v_pieslice(int,int,int,int,int,int);
void v_circle(int,int,int,int);
void v_ellarc(int,int,int,int,int,int,int);
void v_ellpie(int,int,int,int,int,int,int);
void v_ellipse(int,int,int,int,int);
void v_rbox(int,short *);
void v_rfbox(int,short *);
void v_justified(int,int,int,const char *,int,int,int);
void v_bez(int,int,const short *,const char *,short *,short *,short *);
void v_bez_fill(int,int,const short *,const char *,short *,short *,short *);
void v_ftext_offset(int, int, int, const char *, const short *);
void v_wc_ftext(int, int, int, const short *);
void v_wc_ftext_offset(int, int, int, const short *, const short *);
void v_wc_gtext(int, int, int, const short *);
void v_wc_justified(int, int, int, const short *, int, int, int);
void v_ftext16(int, int, int, const short *);
void v_ftext_offset16(int, int, int, const short *, const short *);
void v_gtext16(int, int, int, const short *);
void v_justified16(int, int, int, const short *, int, int, int);

/* Attribute library definitions */
#define MD_REPLACE	1
#define MD_TRANS	2
#define MD_XOR	3
#define MD_ERASE	4

#define FIS_HOLLOW	0
#define HOLLOW	FIS_HOLLOW
#define FIS_SOLID	1
#define SOLID	FIS_SOLID
#define FIS_PATTERN	2
#define PATTERN	FIS_PATTERN
#define FIS_HATCH	3
#define HATCH	FIS_HATCH
#define FIS_USER	4
#define UDFILLSTYLE	FIS_USER

#define ALL_WHITE	0	/* bit blt rules */
#define S_AND_D	1
#define S_AND_NOTD	2
#define S_ONLY	3
#define NOTS_AND_D	4
#define D_ONLY	5
#define S_XOR_D	6
#define S_OR_D	7
#define NOT_SORD	8
#define NOT_SXORD	9
#define NOT_D	10
#define S_OR_NOTD	11
#define NOT_S	12
#define NOTS_OR_D	13
#define NOT_SANDD	14
#define ALL_BLACK	15

/*#define SOLID	1*/	/* polyline line styles */
#define LDASHED	2
#define LONGDASH	LDASHED
#define DOTTED	3
#define DOT	DOTTED
#define DASHDOT	4
#define DASHED	5
#define DASH	DASHED
#define DASHDOTDOT	6
#define DASH2DOT	DASHDOTDOT
#define USERLINE	7

#define SQUARE	0
#define SQUARED	SQUARE
#define ARROWED	1
#define ROUND	2
#define ROUNDED	ROUND

#define DOTS	3	/* fill patterns */
#define GRID	6
#define BRICKS	9
#define WEAVE	16

#define THICKENED	0x0001	/* text effects */
#define SHADED	0x0002
#define SKEWED	0x0004
#define UNDERLINED	0x0008
#define OUTLINE	0x0010
#define SHADOW	0x0020

int vswr_mode(int,int);
void vs_color(int,int,const short *);
int vsl_type(int,int);
void vsl_udsty(int,int);
int vsl_width(int,int);
int vsl_color(int,int);
void vsl_ends(int,int,int);
int vsm_type(int,int);
int vsm_height(int,int);
int vsm_color(int,int);
void vst_height(int,int,short *,short *,short *,short *);
int vst_point(int,int,short *,short *,short *,short *);
int vst_rotation(int,int);
int vst_font(int,int);
int vst_color(int,int);
int vst_effects(int,int);
void vst_alignment(int,int,int,short *,short *);
int vsf_interior(int,int);
int vsf_style(int,int);
int vsf_color(int,int);
int vsf_perimeter(int,int);
void vsf_udpat(int,const void *,int);
int vst_arbpt(int,int,short *,short *,short *,short *);
int vst_setsize(int,int,short *,short *,short *,short *);
int vst_skew(int,int);
fix31 vst_arbpt32(int, fix31, short *, short *, short *, short *);
fix31 vst_setsize32(int, fix31, short *, short *, short *, short *);
void vst_charmap(int, int);
void vst_kern(int, int, int, short *, short *);

/* Raster library definitions */
void vro_cpyfm(int,int,short *,MFDB *,MFDB *);
void vrt_cpyfm(int,int,short *,MFDB *,MFDB *,short *);
void vr_trnfm(int,MFDB *,MFDB *);
void v_get_pixel(int,int,int,short *,short *);

/* Input library definitions */
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

int vsin_mode(int,int,int);
void vrq_locator(int,int,int,short *,short *,short *);
int vsm_locator(int,int,int,short *,short *,short *);
void vrq_valuator(int,int,short *,short *);
void vsm_valuator(int,int,short *,short *,short *);
void vrq_choice(int,int,short *);
int vsm_choice(int,short *);
void vrq_string(int,int,int,short *,char *);
int vsm_string(int,int,int,short *,char *);
void vsc_form(int,MFORM *);
void vex_timv(int,int (*)(void),int (**)(void),short *);
void v_show_c(int,int);
void v_hide_c(int);
void vq_mouse(int,short *,short *,short *);
void vex_butv(int,int (*__regargs)(short),int (**__regargs)(short));
void vex_motv(int,int (*__regargs)(short,short),int (**__regargs)(short,short));
void vex_curv(int,int (*__regargs)(short,short),int (**__regargs)(short,short));
void vq_key_s(int,short *);

/* Inquire library definitions */
void vq_extnd(int,int,short *);
void vq_color(int,int,int,short *);
void vql_attributes(int,short *);
void vqm_attributes(int,short *);
void vqf_attributes(int,short *);
void vqt_attributes(int,short *);
void vqt_extent(int,const char *,short[8]);
void vqt_f_extent(int, const char *, short[8]);
int vqt_width(int,int,short *,short *,short *);
int vqt_name(int,int,char *);
int vqt_f_name(int,int,char *,short *);
void vq_cellarray(int,short *,int,int,short *,short *,short *,short *);
void vqin_mode(int,int,short *);
void vqt_fontinfo(int,short *,short *,short *,short *,short *);
void vqt_advance(int,int,short *,short *,short *,short *);
void vqt_cachesize(int,int,long *);
void vqt_devinfo(int,int,short *,char *);
void vqt_get_table(int,short **);
void v_getbitmap_info(int, int, fix31 *, fix31 *, fix31 *, fix31 *, short *, short *, short **);
void vqt_advance32(int, int, fix31 *, fix31 *);
void vqt_fontheader(int, void *, char *);
void vqt_pairkern(int, int, int, fix31 *, fix31 *);
void vqt_trackkern(int, fix31 *, fix31 *);
void vqt_wc_extent(int, const short *, short *);
void vqt_wc_f_extent(int, const short *, short *);
void vqt_extent16(int, const short *, short *);
void vqt_f_extent16(int, const short *, short *);
void vq_xextnd(int, int, short *, int, int);
void vq_scrninfo(int, short *);

/* Escape library definitions */
void vq_chcells(int,short *,short *);
void v_exit_cur(int);
void v_enter_cur(int);
void v_curup(int);
void v_curdown(int);
void v_curright(int);
void v_curleft(int);
void v_curhome(int);
void v_eeos(int);
void v_eeol(int);
void vs_curaddress(int,int,int);
void v_curtext(int,const char *);
void v_rvon(int);
void v_rvoff(int);
void vq_curaddress(int,short *,short *);
int vq_tabstatus(int);
void v_hardcopy(int);
void v_dspcur(int,int,int);
void v_rmcur(int);
void v_form_adv(int);
void v_output_window(int,short *);
void v_clear_disp_list(int);
void v_bit_image(int,const char *,int,int,int,int,int,short *);
void vq_scan(int,short *,short *,short *,short *,short *);
void v_alpha_text(int,const char *);
int vs_palette(int,int);
void vqp_films(int,char *);
void vqp_state(int,short *,short *,short *,short *,short *,short *);
void vsp_state(int,int,int,int,int,int,const short *);
void vsp_save(int);
void vsp_message(int);
int vqp_error(int);
void v_meta_extents(int,int,int,int,int);
void v_write_meta(int,int,short *,int,short *);
void vm_filename(int,const char *);
void v_pgcount(int,int);

/* Metafile sub-opcodes for GEM output */
void vm_pagesize(int,int,int);
void vm_coords(int,int,int,int,int);

/* Atari specific escape library defintions */
void v_offset(int,int);
void v_font(int,void *);

/* GDOS detection functions */
int vq_gdos(void);

#define GDOS_NONE	0xfffffffe	/* no GDOS */
#define GDOS_FSM	0x5f46534d	/* _FSM */
#define GDOS_FNT	0x5f464e54	/* _FNT */
unsigned long vq_vgdos(void);

#ifndef __NO_VDI_DEFINES
#pragma inline d0=vq_vgdos()	{register d2,a2; "70fe4e42";}
#ifdef _SHORTINT
#pragma inline d0=vq_gdos()	{register d2,a2; "70fe4e42548056c0";}
#else
#pragma inline d0=vq_gdos()	{register d2,a2; "70fe4e425480";}
#endif
#endif
#endif
