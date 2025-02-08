/*  CPX DATA STRUCTURES
 *==========================================================================
 *  XCPB structure is passed TO the CPX
 *  CPXINFO structure pointer is returned FROM the CPX
 *
 *  xcpb structure is initialized in XCONTROL.C
 */

#ifndef __foobar_defined
#define __foobar_defined 1
struct foobar
{
	_WORD dummy;
	_WORD *image;
};
#endif

typedef struct
{
	short handle;
	short booting;
	short reserved;
	short SkipRshFix;

	void *__CDECL (*Get_Head_Node) (void);	/* ON distribution disk...         */
	_WORD __CDECL (*Save_Header) (void *ptr);	/* These 2 would be void *reserved */

	void __CDECL (*rsh_fix) (_WORD num_obs, _WORD num_frstr, _WORD num_frimg,
						  _WORD num_tree, OBJECT *rs_object,
						  TEDINFO *rs_tedinfo, char *rs_strings[],
						  ICONBLK *rs_iconblk, BITBLK *rs_bitblk,
						  long *rs_frstr, long *rs_frimg, long *rs_trindex, struct foobar *rs_imdope);

	void __CDECL (*rsh_obfix) (OBJECT *tree, _WORD curob);

	_WORD __CDECL (*Popup) (const char *const *items, _WORD num_items, _WORD default_item, _WORD font_size, GRECT *button, GRECT *world);

	void __CDECL (*Sl_size) (OBJECT *tree, _WORD base, _WORD slider, _WORD num_items, _WORD visible, _WORD direction, _WORD min_size);
#define VERTICAL	0
#define HORIZONTAL	1

	void __CDECL (*Sl_x) (OBJECT *tree, _WORD base, _WORD slider, _WORD value, _WORD num_min, _WORD num_max, void __CDECL (*foo) (void));

	void __CDECL (*Sl_y) (OBJECT *tree, _WORD base, _WORD slider, _WORD value, _WORD num_min, _WORD num_max, void __CDECL (*foo) (void));

	void __CDECL (*Sl_arrow) (OBJECT *tree, _WORD base, _WORD slider, _WORD obj,
						   _WORD inc, _WORD min, _WORD max, _WORD *numvar, _WORD direction, void __CDECL (*foo) (void));

	void __CDECL (*Sl_dragx) (OBJECT *tree, _WORD base, _WORD slider, _WORD min, _WORD max, _WORD *numvar, void __CDECL (*foo) (void));

	void __CDECL (*Sl_dragy) (OBJECT *tree, _WORD base, _WORD slider, _WORD min, _WORD max, _WORD *numvar, void __CDECL (*foo) (void));

	_WORD __CDECL (*Xform_do) (OBJECT *tree, _WORD start_field, _WORD puntmsg[]);
	GRECT *__CDECL (*GetFirstRect) (GRECT *prect);
	GRECT *__CDECL (*GetNextRect) (void);

	void __CDECL (*Set_Evnt_Mask) (_WORD mask, MOBLK *m1, MOBLK *m2, long time);
	_WORD __CDECL (*XGen_Alert) (_WORD id);
#define SAVE_DEFAULTS	0
#define MEM_ERR			1
#define FILE_ERR		2
#define FILE_NOT_FOUND	3

	_WORD __CDECL (*CPX_Save) (void *ptr, long num);
	void *__CDECL (*Get_Buffer) (void);

	_WORD __CDECL (*getcookie) (long cookie, long *p_value);

	_WORD Country_Code;

	void __CDECL (*MFsave) (_WORD saveit, MFORM *mf);
#define MFSAVE 1
#define MFRESTORE 0
} XCPB;


/* MRETS struct for mouse parameters returned by
 * Evnt_button(), Evnt_mouse(), Evnt_multi(), Graf_mkstate()
 */
typedef struct
{
	_WORD x;
	_WORD y;
	_WORD buttons;
	_WORD kstate;
} MRETS;


typedef struct
{
	_WORD __CDECL (*cpx_call) (GRECT *work);

	void __CDECL (*cpx_draw) (GRECT *clip);
	void __CDECL (*cpx_wmove) (GRECT *work);

	void __CDECL (*cpx_timer) (_WORD *event);
	void __CDECL (*cpx_key) (_WORD kstate, _WORD key, _WORD *event);
	void __CDECL (*cpx_button) (MRETS *mrets, _WORD nclicks, _WORD *event);
	void __CDECL (*cpx_m1) (MRETS *mrets, _WORD *event);
	void __CDECL (*cpx_m2) (MRETS *mrets, _WORD *event);
	_WORD __CDECL (*cpx_hook) (_WORD event, _WORD *msg, MRETS *mrets, _WORD *key, _WORD *nclicks);

	void __CDECL (*cpx_close) (_WORD flag);
} CPXINFO;

#ifndef GNU_ASM_NAME
#ifdef __GNUC__
#define GNU_ASM_NAME(x) __asm__(x)
#else
#define GNU_ASM_NAME(x)
#endif
#endif

CPXINFO *__CDECL cpx_init(XCPB *Xcpb) GNU_ASM_NAME("cpx_init");
