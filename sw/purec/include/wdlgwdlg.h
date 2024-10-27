#ifndef __WDLG_H__
#define __WDLG_H__

#include <aes.h>
#include <wdlgevnt.h>

/* MagiC XTED Struktur */
#ifndef __XTED
#define __XTED
typedef struct
{
	char *xte_ptmplt;
	char *xte_pvalid;
	_WORD xte_vislen;
	_WORD xte_scroll;
} XTED;
#endif

#ifndef __MTDIALOG
#define __MTDIALOG
typedef struct _dialog { int dummy; } DIALOG;
#endif

/** parameters of HNDL_OBJ callback functions */
struct HNDL_OBJ_args 
{
	DIALOG *dialog;
	EVNT *events;
	_WORD obj;
	_WORD clicks;
	void *data;
};

/** service routine that is called, among others, by mt_wdlg_evnt().
 *
 *  This function may be called if an exit or touchexit 
 *  object was clicked on (in that case \p obj is a positive object number) 
 *  or when an event has occurred that affects the dialog (in that case 
 *  \p obj is negative and contains a corresponding function number such as 
 *  HNDL_CLSD, for instance).
 *  
 *  If \p obj is an object number (>= 0), then \p events points to 
 *  the EVNT structure that was passed by mt_wdlg_evnt().
 *  Otherwise \p events is basically 0L and can  not be used for addressing.
 *  
 *  \p clicks contains then number of mouse clicks (if \p obj is an object number)
 *
 *  Here is a list of event (value given in the \p obj parameter):
 *  - HNDL_INIT (-1) : \n
 *	  \p data is the variable passed by wdlg_create.
 *	  If handle_exit() returns 0, mt_wdlg_create() does not
 *	  create a dialog structure (error).
 *	  The variable \p code is passed in \p clicks.
 *  - HNDL_OPEN (-5) : \n
 *    \p data is the variable passed by wdlg_open.
 *    The variable \p code is passed in \p clicks.
 *  - HNDL_CLSD (-3) : \n
 *    \p data is \p user_data. If handle_exit() returns 0,
 *    the dialog will be closed -- mt_wdlg_evnt() returns 0
 *    \p events points to the EVNT structure passed by
 *    mt_wdlg_evnt(). 
 *  - HNDL_MOVE (-9) : \n
 *    \p data is \p user_data. If handle_exit() returns 0,
 *    the dialog will be closed -- mt_wdlg_evnt() returns 0.
 *    \p events points to the EVNT structure passed by
 *    mt_wdlg_evnt().
 *  - HNDL_TOPW (-10) : \n
 *    \p data is \p user_data. If handle_exit() returns 0,
 *    the dialog will be closed -- mt_wdlg_evnt() returns 0.
 *    \p events points to the EVNT structure passed by 
 *    mt_wdlg_evnt().
 *  - HNDL_UNTP (-11) : \n
 *    \p data is \p user_data. If handle_exit() returns 0,
 *    the dialog will be closed -- mt_wdlg_evnt() returns 0.
 *    \p events points to the EVNT structure passed by 
 *    mt_wdlg_evnt().
 *  - HNDL_EDIT (-6) : \n
 *    \p data points to a word with the key code.
 *    If handle_exit() returns 1, the key press will be
 *    evaluated, if 0 ignored.
 *    \p events points to the EVNT structure passed by
 *    mt_wdlg_evnt().  
 *  - HNDL_EDDN (-7) : \n
 *    \p data points to a word with the key code.
 *    \p events points to the EVNT structure passed by
 *    mt_wdlg_evnt().
 *  - HNDL_EDCH (-8) : \n
 *    \p data points to a word with the object number of
 *    the new editable field.
 *  - HNDL_MESG (-2) : \n
 *    \p data is \p user_data. If handle_exit() returns 0,
 *    the dialog will be closed -- mt_wdlg_evnt() returns 0.
 *    \p events points to the EVNT structure passed by
 *    mt_wdlg_evnt().	\n
 *    HNDL_MESG is only passed if a message code between
 *    20 and 39 was received that is not handled by other
 *    opcodes.
 *    Is required for iconification, for instance.\n
 *    Warning: This opcode is only present from MagiC 4.5
 *    of 18.4.96 
 *  .
 *  Of these function numbers one only has to react to HNDL_CLSD. All other 
 *  events need only be paid attention to when needed.\n
 *  If handle_exit is called with an unknown function number in \p obj, or 
 *  one of the above function numbers is to be ignored, then 1 has to be 
 *  returned.
 * 
 *  The parameters are passed via the stack and the routine may alter 
 *  registers d0-d2/a0-a2.
 */
#if !defined(__HNDL_OBJ)
#define __HNDL_OBJ
typedef _WORD __CDECL (*HNDL_OBJ)(struct HNDL_OBJ_args);
#endif

/* Function numbers for <obj> with handle_exit(...) */
#define HNDL_INIT (-1)                  /* Initialise dialog */
#define HNDL_MESG (-2)                  /* Handle message */
#define HNDL_CLSD (-3)                  /* Dialog window was closed */
#define HNDL_OPEN (-5)                  /* End of dialog initialisation (second  call at end of wdlg_init) */
#define HNDL_EDIT (-6)                  /* Test characters for an edit-field */
#define HNDL_EDDN (-7)                  /* Character was entered in edit-field */
#define HNDL_EDCH (-8)                  /* Edit-field was changed */
#define HNDL_MOVE (-9)                  /* Dialog was moved */
#define HNDL_TOPW (-10)                 /* Dialog-window has been topped */
#define HNDL_UNTP (-11)                 /* Dialog-window is not active */

/*
 * Flags fuer wdlg_create()
 */
#define WDLG_BKGD 0x0001

_WORD wdlg_close(DIALOG *dialog, _WORD *x, _WORD *y);
DIALOG *wdlg_create(HNDL_OBJ handle_exit, OBJECT *tree, void *user_data, _WORD code, void *data, _WORD flags);
_WORD wdlg_delete(DIALOG *dialog);
_WORD wdlg_evnt(DIALOG *dialog, EVNT *events);
_WORD wdlg_get_edit(DIALOG *dialog, _WORD *cursor);
_WORD wdlg_get_handle(DIALOG *dialog);
_WORD wdlg_get_tree(DIALOG *dialog, OBJECT **tree, GRECT *r);
void *wdlg_get_udata(DIALOG *dialog);
_WORD wdlg_open(DIALOG *dialog, const char *title, _WORD kind, _WORD x, _WORD y, _WORD code, void *data);
void wdlg_redraw(DIALOG *dialog, GRECT *rect, _WORD obj, _WORD depth);
_WORD wdlg_set_edit(DIALOG *dialog, _WORD obj);
_WORD wdlg_set_iconify(DIALOG *dialog, GRECT *g, const char *title, OBJECT *tree, _WORD obj);
_WORD wdlg_set_size(DIALOG *dialog, GRECT *new_size);
_WORD wdlg_set_tree(DIALOG *dialog, OBJECT *new_tree);
_WORD wdlg_set_uniconify(DIALOG *dialog, GRECT *g, const char *title, OBJECT *tree);


#endif /* __WDLG_H__ */
