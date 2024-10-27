#ifndef __MTAES__
#define __MTAES__

#include <aes.h>
#include <wdlgevnt.h>
#include <wdlgpdlg.h>
#include <wdlgfslx.h>
#include <wdlglbox.h>
#include <wdlgedit.h>

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

/** structure comprising the most of the input arguments of mt_evnt_multi()
 */
#if !defined(_MT_GEMLIB_H_) && !defined(__PORTAES_H__)
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

typedef struct {
	_WORD emo_events;	/* the bitfield of events occured (also a return value of mt_evnt_multi_fast() */
	PXY   emo_mouse;
	_WORD emo_mbutton;
	_WORD emo_kmeta;
	_WORD emo_kreturn;
	_WORD emo_mclicks;
} EVMULT_OUT;

typedef void __CDECL (*FSEL_CALLBACK)( _WORD *msg);

/** parameters for the init callback function (7th parameter of xfrm_popup() )
 */
struct POPUP_INIT_args
{
	OBJECT *tree;
	_WORD scrollpos;
	_WORD nlines;
	void *param;
};

#endif

/* Low level interface */

typedef struct
{
	_WORD	control[AES_CTRLMAX];
	_WORD	intin[AES_INTINMAX];
	_WORD	intout[AES_INTOUTMAX];
	void	*addrin[AES_ADDRINMAX];
	void	*addrout[AES_ADDROUTMAX];
} MX_PARMDATA;

extern _WORD _mt_aes_alt(MX_PARMDATA *pb, const _WORD *contrl, _WORD *global_aes);

#define	mt_AESversion(aes_global)   (((AES_GLOBAL *)aes_global)->ap_version)
#define	mt_AESnumapps(aes_global)   (((AES_GLOBAL *)aes_global)->ap_count)
#define	mt_AESapid(aes_global)      (((AES_GLOBAL *)aes_global)->ap_id)
#define	mt_AESappglobal(aes_global) ((_LONG)(((AES_GLOBAL *)aes_global)->ap_private))
#define	mt_AESrscfile(aes_global)   (((AES_GLOBAL *)aes_global)->ap_ptree)
#define	mt_AESrscmem(aes_global)    (((AES_GLOBAL *)aes_global)->ap_rscmem)
#define	mt_AESrsclen(aes_global)    (((AES_GLOBAL *)aes_global)->ap_rsclen)
#define	mt_AESmaxchar(aes_global)   (((AES_GLOBAL *)aes_global)->ap_bvdisk)
#define	mt_AESminchar(aes_global)   (((AES_GLOBAL *)aes_global)->ap_bvhard)

/****** Application definitions *****************************************/

WORD mt_appl_bvset(WORD bvdisk, WORD bvhard, WORD *global_aes);
WORD mt_appl_control (WORD ap_cid, WORD ap_cwhat, void *ap_cout, WORD *global_aes);
extern WORD mt_appl_init( WORD *global_aes );
extern WORD mt_appl_read( WORD ap_rid, WORD ap_rlength, void *ap_rpbuff, WORD *global_aes );
extern WORD mt_appl_write( WORD ap_wid, WORD ap_wlength, const void *ap_wpbuff, WORD *global_aes);
extern WORD mt_appl_find( const char *ap_fpname, WORD *global_aes);
extern WORD mt_appl_tplay( void *ap_tpmem, WORD ap_tpnum, WORD ap_tpscale, WORD *global_aes );
extern WORD mt_appl_trecord( void *ap_trmem, WORD ap_trcount, WORD *global_aes );
extern WORD mt_appl_exit( WORD *global_aes );
extern WORD mt_appl_search( WORD ap_smode, char *ap_sname, WORD *ap_stype, WORD *ap_sid, WORD *global_aes );
extern WORD mt_appl_yield( WORD *global_aes );	/* GEM 2.x */
extern WORD mt_appl_getinfo( WORD ap_gtype, WORD *ap_gout1, WORD *ap_gout2, WORD *ap_gout3, WORD *ap_gout4, WORD *global_aes );
extern _WORD mt_appl_getinfo_str(_WORD type, char *out1, char *out2, char *out3, char *out4, WORD *global_aes);


/****** Event definitions ***********************************************/

extern WORD mt_evnt_keybd( WORD *global_aes );
extern WORD mt_evnt_button( WORD nclicks, WORD bmask, WORD bstate, WORD *Mx, WORD *My, WORD *ButtonState, WORD *KeyState, WORD *global_aes );
WORD mt_evnt_button_evnt(WORD nclicks, WORD mask, WORD state, EVNTDATA *ev, WORD *global_aes);
extern WORD mt_evnt_mouse( WORD EnterExit, WORD InX, WORD InY, WORD InW, WORD InH,
            WORD *OutX, WORD *OutY, WORD *ButtonState, WORD *KeyState, WORD *global_aes );
WORD mt_evnt_mouse_event(WORD flg_leave, GRECT *g, EVNTDATA *ev, WORD *global_aes);
extern WORD mt_evnt_mesag( _WORD *ev_mgpbuff,  WORD *global_aes );
extern WORD mt_evnt_timer( ULONG ms,  WORD *global_aes );

extern WORD mt_evnt_multi(
            WORD evtypes,
            WORD nclicks, WORD bmask, WORD bstate,
            WORD EnterExit1, WORD In1X, WORD In1Y, WORD In1W, WORD In1H,
            WORD EnterExit2, WORD In2X, WORD In2Y, WORD In2W, WORD In2H,
            WORD *msgbuf,
            ULONG ms,
            WORD *OutX, WORD *OutY,
            WORD *ButtonState, WORD *KeyState, WORD *Key, WORD *nbclicks,
            WORD *global_aes );

_WORD mt_evnt_multi_fast (const EVMULT_IN * em_i, _WORD MesagBuf[], EVMULT_OUT *em_o, _WORD *global_aes);

WORD mt_evnt_dclick( WORD ev_dnew, WORD ev_dgetset, WORD *global_aes );

void	MT_EVNT_multi( WORD evtypes, WORD nclicks, WORD bmask, WORD bstate,
							MOBLK *m1, MOBLK *m2, ULONG ms,
							EVNT *event, WORD *global_aes);

/****** Menu definitions ************************************************/

/* menu_bar modes */

extern WORD mt_menu_bar( OBJECT *me_btree, WORD me_bshow, WORD *global_aes );
extern WORD mt_menu_icheck( OBJECT *me_ctree, WORD me_citem, WORD me_ccheck, WORD *global_aes );
extern WORD mt_menu_ienable( OBJECT *me_etree, WORD me_eitem, WORD me_eenable, WORD *global_aes );
extern WORD mt_menu_tnormal( OBJECT *me_ntree, WORD me_ntitle, WORD me_nnormal, WORD *global_aes );
extern WORD mt_menu_text( OBJECT *tree, WORD objnr, const char *text, WORD *global_aes );
extern WORD mt_menu_register( WORD apid, const char *text, WORD *global_aes );
extern WORD mt_menu_popup( MENU *me_menu, WORD me_xpos, WORD me_ypos, MENU *me_mdata, WORD *global_aes );
extern WORD mt_menu_attach( WORD me_flag, OBJECT *me_tree, WORD me_item, MENU *me_mdata, WORD *global_aes );
extern WORD mt_menu_istart( WORD me_flag, OBJECT *me_tree, WORD me_imenu, WORD me_item, WORD *global_aes );
extern WORD mt_menu_settings( WORD flag, MN_SET *values, WORD *global_aes );
extern WORD mt_menu_unregister( WORD menuid, WORD *global_aes );
extern WORD mt_menu_click( WORD val, WORD setit, WORD *global_aes );  /* GEM 3.x     */



/* Object prototypes */

extern WORD mt_objc_add( OBJECT *ob_atree, WORD ob_aparent, WORD ob_achild, WORD *global_aes );
extern WORD mt_objc_delete( OBJECT *ob_dltree, WORD ob_dlobject, WORD *global_aes );
extern WORD mt_objc_draw( OBJECT *tree, WORD start, WORD depth, _WORD xc, _WORD yc, _WORD wc, _WORD hc, WORD *global_aes );
extern WORD	mt_objc_draw_grect(OBJECT *, WORD Start, WORD Depth, const GRECT *r, WORD *global_aes);
extern WORD mt_objc_find( OBJECT *ob_ftree, WORD ob_fstartob, WORD ob_fdepth, WORD ob_fmx, WORD ob_fmy, WORD *global_aes );
extern WORD mt_objc_xfind( OBJECT *ob_ftree, WORD ob_fstartob, WORD ob_fdepth, WORD ob_fmx, WORD ob_fmy, WORD *global_aes );
extern WORD mt_objc_offset( OBJECT *ob_oftree, WORD ob_ofobject, WORD *ob_ofxoff, WORD *ob_ofyoff, WORD *global_aes );
extern WORD mt_objc_order( OBJECT *ob_ortree, WORD ob_orobject, WORD ob_ornewpos, WORD *global_aes );
extern WORD mt_objc_edit( OBJECT *ob_edtree, WORD ob_edobject, WORD ob_edchar, WORD *ob_edidx, WORD ob_edkind, WORD *global_aes );
extern WORD mt_objc_xedit( OBJECT *tree, WORD objnr, WORD key, WORD *cursor_xpos, WORD subfn, GRECT *r, WORD *global_aes );
extern WORD mt_objc_change( OBJECT *tree, WORD objnr, WORD resvd, _WORD xc, _WORD yc, _WORD wc, _WORD hc, WORD newstate, WORD redraw, WORD *global_aes );
extern WORD mt_objc_change_grect( OBJECT *tree, WORD objnr, WORD resvd, const GRECT *r, WORD newstate, WORD redraw, WORD *global_aes );
extern WORD mt_objc_sysvar( WORD mode, WORD which, WORD ival1, WORD ival2, WORD *oval1, WORD *oval2, WORD *global_aes ); /* AES 4.0     */

extern void mt_objc_wdraw( OBJECT *tree, WORD object, WORD depth, GRECT *clip,
                    WORD windowhandle, WORD *global_aes );
extern void mt_objc_wchange( OBJECT *tree, WORD object, WORD newstate,
                    GRECT *clip, WORD windowhandle,
                    WORD *global_aes);
extern WORD mt_objc_wedit( OBJECT *tree, WORD object, WORD edchar,
                    WORD *didx, WORD kind, WORD windowhandle,
                    WORD *global_aes);

/****** Form definitions ************************************************/

extern WORD mt_form_do( OBJECT *tree, WORD startob, WORD *global_aes );
extern WORD mt_form_xdo( OBJECT *tree, WORD startob, WORD *lastcrsr, XDO_INF *tabs, void *flydial, WORD *global_aes );   /* MAGIC       */
extern WORD mt_form_dial( WORD fo_diflag, WORD fo_dilittlx,
               WORD fo_dilittly, WORD fo_dilittlw,
               WORD fo_dilittlh, WORD fo_dibigx,
               WORD fo_dibigy, WORD fo_dibigw, WORD fo_dibigh, WORD *global_aes );
extern WORD mt_form_dial_grect( WORD subfn, const GRECT *lg, const GRECT *bg, WORD *global_aes );
WORD mt_form_xdial( WORD fo_diflag, WORD fo_dilittlx,
               WORD fo_dilittly, WORD fo_dilittlw,
               WORD fo_dilittlh, WORD fo_dibigx,
               WORD fo_dibigy, WORD fo_dibigw, WORD fo_dibigh,
               void **flydial, WORD *global_aes );
extern WORD mt_form_xdial_grect( WORD fo_diflag, const GRECT *little, const GRECT *big, void **flydial, WORD *global_aes );
extern WORD mt_form_alert( WORD fo_adefbttn, const char *fo_astring, WORD *global_aes );
extern WORD mt_form_error( WORD fo_enum, WORD *global_aes );
extern WORD mt_form_center (OBJECT *, WORD *Cx, WORD *Cy, WORD *Cw, WORD *Ch, WORD *global_aes);
extern WORD mt_form_center_grect (OBJECT *, GRECT *r, WORD  *global_aes);
extern WORD mt_form_keybd( OBJECT *fo_ktree, WORD fo_kobject,
                    WORD fo_kobnext, WORD fo_kchar,
                    WORD *fo_knxtobject, WORD *fo_knxtchar, WORD *global_aes );
extern WORD mt_form_button( OBJECT *fo_btree, WORD fo_bobject, WORD fo_bclicks, WORD *fo_bnxtobj, WORD *global_aes );
extern WORD mt_form_popup( OBJECT *tree, WORD x, WORD y, WORD *global_aes );    /* MAG!X */
/* note: the init callback needs arguments on stack;
   but since we pass the whole structure, the
   arguments will be pushed on the stack anyway */
extern _WORD mt_xfrm_popup(
                OBJECT *tree, _WORD x, _WORD y,
                _WORD firstscrlob, _WORD lastscrlob,
                _WORD nlines,
                void /* _CDECL */ (*init)(struct POPUP_INIT_args),
                void *param, _WORD *lastscrlpos, _WORD *global_aes );     /* MagiC 5.03 */
extern WORD mt_form_xerr( long errcode, char *errfile, WORD *global_aes );      /* MAG!X */
extern WORD mt_form_wbutton( OBJECT *tree, WORD object, WORD nclicks,
                    WORD *nextob, WORD windowhandle,
                    WORD *global_aes);
extern WORD mt_form_wkeybd( OBJECT *tree, WORD object, WORD nextob,
                    WORD ichar, WORD *onextob, WORD *ochar,
                    WORD windowhandle,
                    WORD *global_aes);

/****** Graph definitions ************************************************/


extern WORD mt_graf_rubberbox( WORD gr_rx, WORD gr_ry, WORD gr_minwidth,
                    WORD gr_minheight, WORD *gr_rlastwidth,
                    WORD *gr_rlastheight, WORD *global_aes );
WORD mt_graf_dragbox(WORD Sw, WORD Sh, WORD Sx, WORD Sy, WORD Bx, WORD By, WORD Bw, WORD Bh, WORD *Fw, WORD *Fh, WORD *global_aes);
extern WORD mt_graf_movebox( WORD gr_mwidth, WORD gr_mheight, WORD gr_msourcex, WORD gr_msourcey, WORD gr_mdestx, WORD gr_mdesty, WORD *global_aes );
extern WORD mt_graf_growbox( _WORD gr_gstx, _WORD gr_gsty, _WORD gr_gstwidth, _WORD gr_gstheight, _WORD gr_gfinx, _WORD gr_gfiny, _WORD gr_gfinwidth, _WORD gr_gfinheight, WORD *global_aes );
extern WORD mt_graf_growbox_grect(const GRECT *in, const GRECT *out, _WORD *global_aes);
WORD mt_graf_shrinkbox( WORD gr_sfinx, WORD gr_sfiny, WORD gr_sfinwidth, WORD gr_sfinheight, WORD gr_sstx, WORD gr_ssty, WORD gr_sstwidth, WORD gr_sstheight, WORD *global_aes );
extern WORD mt_graf_shrinkbox_grect( const GRECT *endg, const GRECT *startg, WORD *global_aes );
extern WORD mt_graf_watchbox( OBJECT *tree, WORD obj, WORD instate, WORD outstate, WORD *global_aes );
extern WORD mt_graf_slidebox( OBJECT *gr_slptree, WORD gr_slparent, WORD gr_slobject, WORD gr_slvh, WORD *global_aes );
extern WORD mt_graf_handle( WORD *gr_hwchar, WORD *gr_hhchar, WORD *gr_hwbox, WORD *gr_hhbox, WORD *global_aes );
extern WORD mt_graf_xhandle( WORD *wchar, WORD *hchar, WORD *wbox, WORD *hbox, WORD *dev, WORD *global_aes );    /* KAOS 1.4    */
extern WORD mt_graf_mouse( WORD gr_monumber, const MFORM *gr_mofaddr, WORD *global_aes );
extern WORD mt_graf_mkstate(WORD *Mx, WORD *My, WORD *ButtonState, WORD *KeyState, WORD *global_aes);
extern WORD mt_graf_multirubber (WORD bx, WORD by, WORD mw, WORD mh, GRECT *rec, WORD *rw, WORD *rh, WORD *global_aes);
extern WORD mt_graf_wwatchbox( OBJECT *tree, WORD object, WORD instate,
                    WORD outstate, WORD windowhandle,
                    WORD *global_aes);
extern WORD mt_graf_mkstate_event(EVNTDATA *data, WORD *global_aes);

/****** Scrap definitions ***********************************************/

WORD    mt_scrp_read( char *sc_rpscrap, WORD *global_aes );
WORD    mt_scrp_write( const char *sc_wpscrap, WORD *global_aes );
WORD    mt_scrp_clear( WORD *global_aes );                  /* GEM 2.x  */

/****** File selector definitions ***************************************/


extern WORD mt_fsel_input( char *path, char *name, WORD *button, WORD *global_aes );
extern WORD mt_fsel_exinput( char *path, char *name, WORD *button, const char *label, WORD *global_aes );
extern WORD mt_fsel_boxinput(char *path, char *file, WORD *exit_button, const char *title, FSEL_CALLBACK callback, WORD *global);

/****** Window definitions **********************************************/

extern WORD mt_wind_create( _WORD wi_crkind, _WORD wi_crwx, _WORD wi_crwy, _WORD wi_crww, _WORD wi_crwh, WORD *global_aes );
_WORD mt_wind_calc( _WORD wi_ctype, _WORD wi_ckind, _WORD wi_cinx, _WORD wi_ciny, _WORD wi_cinw, _WORD wi_cinh, _WORD *coutx, _WORD *couty, _WORD *coutw,
               _WORD *couth, WORD *global_aes );
extern WORD mt_wind_close( WORD whdl, WORD *global_aes );
extern WORD mt_wind_delete( WORD whdl, WORD *global_aes );
extern WORD mt_wind_get( WORD whdl, WORD subfn, WORD *g1, WORD *g2, WORD *g3, WORD *g4, WORD *global_aes );
extern WORD mt_wind_get_grect( WORD whdl, WORD subfn, GRECT *g, WORD *global_aes );
_WORD mt_wind_xget ( _WORD wi_ghandle, _WORD wi_gfield, 
                    _WORD *wi_sw1, _WORD *wi_sw2, _WORD *wi_sw3, _WORD *wi_sw4,
                    _WORD *wo_gw1, _WORD *wo_gw2, _WORD *wo_gw3, _WORD *wo_gw4, WORD *global_aes );
_WORD mt_wind_xget_grect( _WORD whl, _WORD srt, const GRECT *clip, GRECT *g, WORD *global_aes);
extern WORD mt_wind_get_ptr( WORD whdl, WORD subfn, void **v, WORD *global_aes );
extern WORD mt_wind_get_int( WORD whdl, WORD subfn, WORD *g1, WORD *global_aes );
extern WORD mt_wind_set( WORD whdl, WORD subfn, WORD g1, WORD g2, WORD g3, WORD g4, WORD *global_aes );
extern WORD mt_wind_set_str( WORD whdl, WORD subfn, const char *s, WORD *global_aes );
extern WORD mt_wind_set_ptr( WORD whdl, WORD subfn, void *p1, void *p2, WORD *global_aes );
extern WORD mt_wind_set_grect( WORD whdl, WORD subfn, const GRECT *g, WORD *global_aes );
extern WORD mt_wind_set_ptr_int( WORD whdl, WORD subfn, void *s, WORD i, WORD *global_aes );
extern WORD mt_wind_set_int( WORD whdl, WORD subfn, WORD g1, WORD *global_aes );
_WORD mt_wind_xset ( _WORD wi_ghandle, _WORD wi_gfield, 
                    _WORD wi_sw1, _WORD wi_sw2, _WORD wi_sw3, _WORD wi_sw4,
                    _WORD *wo_gw1, _WORD *wo_gw2, _WORD *wo_gw3, _WORD *wo_gw4, WORD *global_aes );
_WORD mt_wind_xset_grect ( _WORD wi_ghandle, _WORD wi_gfield, const GRECT *s, GRECT *r, WORD *global_aes );
extern WORD mt_wind_find( WORD wi_fmx, WORD wi_fmy, WORD *global_aes );
_WORD mt_wind_open( _WORD wi_ohandle, _WORD wi_owx, _WORD wi_owy, _WORD wi_oww, _WORD wi_owh, WORD *global_aes );
extern WORD mt_wind_update( WORD wi_ubegend, WORD *global_aes );
extern WORD mt_wind_new( WORD *global_aes );

extern WORD mt_wind_create_grect( _WORD wi_crkind, const GRECT *gr, WORD *global_aes );
extern WORD mt_wind_calc_grect( WORD subfn, WORD kind, const GRECT *ing, GRECT *outg, WORD *global_aes );
_WORD mt_wind_xcreate_grect ( _WORD Parts, const GRECT *r, GRECT *ret, WORD *global_aes);
extern WORD mt_wind_open_grect( WORD whdl, const GRECT *g, WORD *global_aes );

/****** Resource definitions ********************************************/

extern WORD mt_rsrc_load( const char *filename, WORD *global_aes );
extern WORD mt_rsrc_free( WORD *global_aes );
extern WORD mt_rsrc_gaddr( WORD type, WORD index, void *addr, WORD *global_aes );
extern WORD mt_rsrc_saddr( WORD type, WORD index, void *o, WORD *global_aes );
extern WORD mt_rsrc_obfix( OBJECT *re_otree, WORD re_oobject, WORD *global_aes );
extern WORD mt_rsrc_rcfix( void /* RSHDR */ *rsh, WORD *global_aes );

/****** Shell definitions ***********************************************/

extern WORD mt_shel_read( char *sh_rpcmd, char *sh_rptail, WORD *global_aes );
extern WORD mt_shel_write( WORD sh_wdoex, WORD sh_wisgr, WORD sh_wiscr,
                const void *sh_wpcmd, const char *sh_wptail, WORD *global_aes );
extern WORD mt_shel_get( char *sh_gaddr, WORD sh_glen, WORD *global_aes );
extern WORD	mt_shel_help(WORD sh_hmode, const char *sh_hfile, const char *sh_hkey, WORD *global_aes);
extern WORD mt_shel_put( const char *sh_paddr, WORD sh_plen, WORD *global_aes );
extern WORD mt_shel_find( char *sh_fpbuff, WORD *global_aes );
extern WORD mt_shel_envrn( char **sh_epvalue, const char *sh_eparm, WORD *global_aes );
extern WORD mt_shel_rdef( char *cmd, char *dir, WORD *global_aes );    /* GEM 2.x     */
extern WORD mt_shel_wdef( const char *cmd, const char *dir, WORD *global_aes );    /* GEM 2.x     */

/****** dummies ***********************************************/

extern _WORD mt_xgrf_stepcalc(_WORD orgw, _WORD orgh, _WORD xc, _WORD yc, _WORD wc, _WORD hc,
    _WORD *pxc, _WORD *pyc, _WORD *pcnt, _WORD *pxstep, _WORD *pystep, WORD *global_aes);
extern _WORD mt_xgrf_2box(_WORD xc, _WORD yc, _WORD w, _WORD h, _WORD corners, _WORD cnt,
    _WORD xstep, _WORD ystep, _WORD doubled, WORD *global_aes);

/****** Wdialog definitions ***********************************************/

extern DIALOG *mt_wdlg_create( HNDL_OBJ handle_exit, OBJECT *tree, void *user_data, WORD code, void *data, WORD flags, WORD *global_aes );
extern WORD mt_wdlg_open( DIALOG *dialog, const char *title, WORD kind, WORD x, WORD y, WORD code, void *data, WORD *global_aes );
extern WORD mt_wdlg_close( DIALOG *dialog, WORD *x, WORD *y, WORD *global_aes );
extern WORD mt_wdlg_delete( DIALOG *dialog, WORD *global_aes );

extern WORD mt_wdlg_get_tree( DIALOG *dialog, OBJECT **tree, GRECT *r, WORD *global_aes );
extern WORD mt_wdlg_get_edit( DIALOG *dialog, WORD *cursor, WORD *global_aes );
extern void *mt_wdlg_get_udata( DIALOG *dialog, WORD *global_aes );
extern WORD mt_wdlg_get_handle( DIALOG *dialog, WORD *global_aes );

extern WORD mt_wdlg_set_edit( DIALOG *dialog, WORD obj, WORD *global_aes );
extern WORD mt_wdlg_set_tree( DIALOG *dialog, OBJECT *tree, WORD *global_aes );
extern WORD mt_wdlg_set_size( DIALOG *dialog, GRECT *size, WORD *global_aes );
extern WORD mt_wdlg_set_iconify( DIALOG *dialog, GRECT *g, const char *title, OBJECT *tree, WORD obj, WORD *global_aes );
extern WORD mt_wdlg_set_uniconify( DIALOG *dialog, GRECT *g, const char *title, OBJECT *tree, WORD *global_aes );

extern WORD mt_wdlg_evnt( DIALOG *dialog, EVNT *events, WORD *global_aes );
extern void mt_wdlg_redraw( DIALOG *dialog, GRECT *rect, WORD obj, WORD depth, WORD *global_aes );


/****** Listbox definitions ***********************************************/
extern LIST_BOX *mt_lbox_create( OBJECT *tree, SLCT_ITEM slct, SET_ITEM set, LBOX_ITEM *items, WORD visible_a, WORD first_a,
                          const WORD *ctrl_objs, const WORD *objs, WORD flags, WORD pause_a, void *user_data, DIALOG *dialog,
                          WORD visible_b, WORD first_b, WORD entries_b, WORD pause_b, WORD *global_aes );

extern void mt_lbox_update( LIST_BOX *box, GRECT *rect, WORD *global_aes );
extern WORD mt_lbox_do( LIST_BOX *box, WORD obj, WORD *global_aes );
extern WORD mt_lbox_delete( LIST_BOX *box, WORD *global_aes );

extern WORD mt_lbox_cnt_items( LIST_BOX *box, WORD *global_aes );
extern OBJECT *mt_lbox_get_tree( LIST_BOX *box, WORD *global_aes );
extern WORD mt_lbox_get_avis( LIST_BOX *box, WORD *global_aes );
extern WORD mt_lbox_get_visible( LIST_BOX *box, WORD *global_aes );  /* another name for mt_lbox_get_avis */
extern void *mt_lbox_get_udata( LIST_BOX *box, WORD *global_aes );
extern WORD mt_lbox_get_afirst( LIST_BOX *box, WORD *global_aes );
extern WORD mt_lbox_get_first( LIST_BOX *box, WORD *global_aes );	/* another name for mt_lbox_get_afirst */
extern WORD mt_lbox_get_slct_idx( LIST_BOX *box, WORD *global_aes );
extern LBOX_ITEM *mt_lbox_get_items( LIST_BOX *box, WORD *global_aes );
extern LBOX_ITEM *mt_lbox_get_item( LIST_BOX *box, WORD n, WORD *global_aes );
extern LBOX_ITEM *mt_lbox_get_slct_item( LIST_BOX *box, WORD *global_aes );
extern WORD mt_lbox_get_idx( LBOX_ITEM *items, LBOX_ITEM *search, WORD *global_aes );
extern WORD mt_lbox_get_bvis( LIST_BOX *box, WORD *global_aes );
extern WORD mt_lbox_get_bentries( LIST_BOX *box, WORD *global_aes );
extern WORD mt_lbox_get_bfirst( LIST_BOX *box, WORD *global_aes );

extern void mt_lbox_set_asldr( LIST_BOX *box, WORD first, GRECT *rect, WORD *global_aes );
extern void mt_lbox_set_slider( LIST_BOX *box, WORD first, GRECT *rect, WORD *global_aes );  /* another name for mt_lbox_set_asldr */
extern void mt_lbox_set_items( LIST_BOX *box, LBOX_ITEM *items, WORD *global_aes );
extern void mt_lbox_free_items( LIST_BOX *box, WORD *global_aes );
extern void mt_lbox_free_list( LBOX_ITEM *items, WORD *global_aes );
extern void mt_lbox_ascroll_to( LIST_BOX *box, WORD first, GRECT *box_rect, GRECT *slider_rect, WORD *global_aes );
extern void mt_lbox_scroll_to( LIST_BOX *box, WORD first, GRECT *box_rect, GRECT *slider_rect, WORD *global_aes );  /* another name for mt_lbox_ascroll_to */
extern void mt_lbox_set_bsldr( LIST_BOX *box, WORD first, GRECT *rect, WORD *global_aes );
extern void mt_lbox_set_bentries( LIST_BOX *box, WORD entries, WORD *global_aes );
extern void mt_lbox_bscroll_to( LIST_BOX *box, WORD first, GRECT *box_rect, GRECT *slider_rect, WORD *global_aes );

/****** Font selector definitions ***********************************************/

extern FNT_DIALOG   *mt_fnts_create( WORD vdi_handle, WORD no_fonts, WORD font_flags, WORD dialog_flags, const char *sample, const char *opt_button, WORD *global_aes );
extern WORD mt_fnts_delete( FNT_DIALOG *fnt_dialog, WORD vdi_handle, WORD *global_aes );
extern WORD mt_fnts_open( FNT_DIALOG *fnt_dialog, WORD button_flags, WORD x, WORD y, LONG id, LONG pt, LONG ratio, WORD *global_aes );
extern WORD mt_fnts_close( FNT_DIALOG *fnt_dialog, WORD *x, WORD *y, WORD *global_aes );

extern WORD mt_fnts_get_no_styles( FNT_DIALOG *fnt_dialog, LONG id, WORD *global_aes );
extern LONG mt_fnts_get_style( FNT_DIALOG *fnt_dialog, LONG id, WORD index, WORD *global_aes );
extern WORD mt_fnts_get_name( FNT_DIALOG *fnt_dialog, LONG id, char *full_name, char *family_name, char *style_name, WORD *global_aes );
extern WORD mt_fnts_get_info( FNT_DIALOG *fnt_dialog, LONG id, WORD *mono, WORD *outline, WORD *global_aes );

extern WORD mt_fnts_add( FNT_DIALOG *fnt_dialog, FNTS_ITEM *user_fonts, WORD *global_aes );
extern void mt_fnts_remove( FNT_DIALOG *fnt_dialog, WORD *global_aes );
extern WORD mt_fnts_update( FNT_DIALOG *fnt_dialog, WORD button_flags, LONG id, LONG pt, LONG ratio, WORD *global_aes );

extern WORD mt_fnts_evnt( FNT_DIALOG *fnt_dialog, EVNT *events, WORD *button, WORD *check_boxes, LONG *id, LONG *pt, LONG *ratio, WORD *global_aes );
extern WORD mt_fnts_do( FNT_DIALOG *fnt_dialog, WORD button_flags, LONG id_in, LONG pt_in, LONG ratio_in, WORD *check_boxes, LONG *id, LONG *pt, LONG *ratio, WORD *global_aes );


extern XFSL_DIALOG *mt_fslx_open(
            const char *title,
            WORD x, WORD y,
            WORD *handle,
            char *path, WORD pathlen,
            char *fname, WORD fnamelen,
            const char *patterns,
            XFSL_FILTER filter,
            char *paths,
            WORD sort_mode,
            WORD flags,
            WORD *global_aes);

extern WORD mt_fslx_evnt(
            XFSL_DIALOG *fsd,
            EVNT *events,
            char *path,
            char *fname,
            WORD *button,
            WORD *nfiles,
            WORD *sort_mode,
            char **pattern, WORD *global_aes );

extern XFSL_DIALOG * mt_fslx_do(
            const char *title,
            char *path, WORD pathlen,
            char *fname, WORD fnamelen,
            char *patterns,
            XFSL_FILTER filter,
            char *paths,
            WORD *sort_mode,
            WORD flags,
            WORD *button,
            WORD *nfiles,
            char **pattern, WORD *global_aes );

extern WORD mt_fslx_getnxtfile( XFSL_DIALOG *fsd, char *fname, WORD *global_aes );
extern WORD mt_fslx_close( XFSL_DIALOG *fsd, WORD *global_aes );
extern WORD mt_fslx_set_flags( WORD flags, WORD *oldval, WORD *global_aes );

/************************************************************************/

PRN_DIALOG  *mt_pdlg_create( WORD dialog_flags, WORD *global_aes );
WORD mt_pdlg_delete( PRN_DIALOG *prn_dialog, WORD *global_aes );
WORD mt_pdlg_open( PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, const char *document_name, WORD option_flags, WORD x, WORD y, WORD *global_aes );
WORD mt_pdlg_close( PRN_DIALOG *prn_dialog, WORD *x, WORD *y, WORD *global_aes );

LONG mt_pdlg_get_setsize( WORD *global_aes );

WORD mt_pdlg_add_printers( PRN_DIALOG *prn_dialog, DRV_INFO *drv_info, WORD *global_aes );
WORD mt_pdlg_remove_printers( PRN_DIALOG *prn_dialog, WORD *global_aes );
WORD mt_pdlg_update( PRN_DIALOG *prn_dialog, const char *document_name, WORD *global_aes );
WORD mt_pdlg_add_sub_dialogs( PRN_DIALOG *prn_dialog, PDLG_SUB *sub_dialogs, WORD *global_aes );
WORD mt_pdlg_remove_sub_dialogs( PRN_DIALOG *prn_dialog, WORD *global_aes );
PRN_SETTINGS *mt_pdlg_new_settings( PRN_DIALOG *prn_dialog, WORD *global_aes );
WORD mt_pdlg_free_settings( PRN_SETTINGS *settings, WORD *global_aes );
WORD mt_pdlg_dflt_settings( PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, WORD *global_aes );
WORD mt_pdlg_validate_settings( PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, WORD *global_aes );
WORD mt_pdlg_use_settings( PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, WORD *global_aes );
WORD mt_pdlg_save_default_settings( PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, WORD *global_aes );

WORD mt_pdlg_evnt( PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, EVNT *events, WORD *button, WORD *global_aes );
WORD mt_pdlg_do( PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, const char *document_name, WORD option_flags, WORD *global_aes );


/************************************************************************/

extern XEDITINFO *mt_edit_create( _WORD *global_aes );
extern _WORD mt_edit_open(OBJECT *tree, _WORD obj, _WORD *global_aes);
extern void mt_edit_close(OBJECT *tree, _WORD obj, _WORD *global_aes);
extern void mt_edit_delete(XEDITINFO *xi, _WORD *global_aes);
extern _WORD mt_edit_cursor(OBJECT *tree, _WORD obj, _WORD whdl, _WORD show, _WORD *global_aes);
extern _WORD mt_edit_evnt(OBJECT *tree, _WORD obj, _WORD whdl,	EVNT *ev, LONG *errc, _WORD *global_aes);
extern _WORD mt_edit_get_buf( OBJECT *tree, _WORD obj, char **buf, LONG *buflen, LONG *txtlen, _WORD *global_aes );
extern _WORD mt_edit_get_format( OBJECT *tree, _WORD obj, _WORD *tabwidth, _WORD *autowrap, _WORD *global_aes );
extern _WORD mt_edit_get_color( OBJECT *tree, _WORD obj, _WORD *tcolour, _WORD *bcolour, _WORD *global_aes );
extern _WORD mt_edit_get_cursor( OBJECT *tree, _WORD obj, char **cursorpos, _WORD *global_aes );
extern _WORD mt_edit_get_font( OBJECT *tree, _WORD obj,	_WORD *fontID, _WORD *fontH, _WORD *fontPix, _WORD *mono, _WORD *global_aes );
extern void mt_edit_set_buf( OBJECT *tree, _WORD obj, char *buf, LONG buflen, _WORD *global_aes );
extern void mt_edit_set_format( OBJECT *tree, _WORD obj, _WORD tabwidth, _WORD autowrap, _WORD *global_aes );
extern void mt_edit_set_font( OBJECT *tree, _WORD obj, _WORD fontID, _WORD fontH, _WORD fontPix, _WORD mono, _WORD *global_aes );
extern void mt_edit_set_color( OBJECT *tree, _WORD obj, _WORD tcolor, _WORD bcolor, _WORD *global_aes );
extern void mt_edit_set_cursor( OBJECT *tree, _WORD obj, char *cursorpos, _WORD *global_aes );
extern _WORD mt_edit_resized( OBJECT *tree, _WORD obj, _WORD *oldrh, _WORD *newrh, _WORD *global_aes );
extern _WORD mt_edit_get_dirty( OBJECT *tree, _WORD obj,	_WORD *global_aes );
extern void mt_edit_set_dirty( OBJECT *tree, _WORD obj,	_WORD dirty, _WORD *global_aes );
extern void mt_edit_get_sel( OBJECT *tree, _WORD obj, char **bsel, char **esel, _WORD *global_aes );
extern void mt_edit_get_pos( OBJECT *tree, _WORD obj, _WORD *xscroll, LONG *yscroll, char **cyscroll, char **cursorpos, _WORD *cx, _WORD *cy, _WORD *global_aes );
extern void mt_edit_set_pos( OBJECT *tree, _WORD obj, _WORD xscroll, LONG yscroll, char *cyscroll, char *cursorpos, _WORD cx, _WORD cy, _WORD *global_aes );
extern void mt_edit_get_scrollinfo( OBJECT *tree, _WORD obj,	LONG *nlines, LONG *yscroll, _WORD *yvis, _WORD *yval, _WORD *ncols, _WORD *xscroll, _WORD *xvis, _WORD *global_aes );
extern _WORD mt_edit_scroll( OBJECT *tree, _WORD obj, _WORD whdl, LONG yscroll, _WORD xscroll, _WORD *global_aes );

#endif
