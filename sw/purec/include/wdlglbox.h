#ifndef __LBOX_H__
#define __LBOX_H__

#include <portaes.h>
#include <wdlgevnt.h>
#include <wdlgwdlg.h>

typedef struct _list_box { int dummy; } LIST_BOX;

typedef struct _lbox_item LBOX_ITEM;

struct _lbox_item {
	LBOX_ITEM *next;                    /* Pointer to the next entry in the list */
	_WORD selected;                     /* Specifies if the object is selected */
	_WORD data1;                        /* Data for the program... */
	void *data2;
	void *data3;
};


/** parameters for SLCT_ITEM callback function */
struct SLCT_ITEM_args
{
	LIST_BOX *box;
	OBJECT *tree;
	LBOX_ITEM *item;
	void *user_data;
	_WORD obj_index;
	_WORD last_state;
};

/** parameters for SET_ITEM callback function */
struct SET_ITEM_args
{
	LIST_BOX *box;
	OBJECT *tree;
	LBOX_ITEM *item;
	_WORD obj_index;
	void *user_data;
	GRECT *rect;
	_WORD first;
};

/* note: the callback needs arguments on stack;
   but since we pass the whole structure, the
   arguments will be pushed on the stack anyway */
typedef void _CDECL (*SLCT_ITEM)(struct SLCT_ITEM_args);
typedef _WORD _CDECL (*SET_ITEM)(struct SET_ITEM_args);

/*
 * flags for lbox_create
 */
#define LBOX_VERT     1                 /* Listbox with vertical slider */
#define LBOX_AUTO     2                 /* Auto-scrolling */
#define LBOX_AUTOSLCT 4                 /* Automatic display during auto-scrolling */
#define LBOX_REAL     8                 /* Real-time slider */
#define LBOX_SNGL    16                 /* Only one selectable entry */
#define LBOX_SHFT    32                 /* Multi-selection with Shift */
#define LBOX_TOGGLE  64                 /* Toggle status of an entry at selection */
#define LBOX_2SLDRS 128                 /* Listbox has a horiz. and a vertical slider */

/* #defines for listboxes with only one slider */
#define lbox_get_first(a) lbox_get_afirst(a)
#define lbox_scroll_to(a,b,c,d) lbox_ascroll_to(a,b,c,d)
#define lbox_get_avis(a) lbox_get_visible(a)
#define lbox_set_slider(a,b,c) lbox_set_asldr(a,b,c)

void lbox_ascroll_to(LIST_BOX *box, _WORD first, GRECT *box_rect, GRECT *slider_rect);
void lbox_bscroll_to(LIST_BOX *box, _WORD first, GRECT *box_rect, GRECT *slider_rect);
_WORD lbox_cnt_items(LIST_BOX *box);
LIST_BOX *lbox_create(OBJECT *tree,
	SLCT_ITEM slct, SET_ITEM set, LBOX_ITEM *items,
	_WORD visible_a, _WORD first_a, const _WORD *ctrl_objs, const _WORD *objs,
	_WORD flags, _WORD pause_a, void *user_data, DIALOG *dialog,
	_WORD visible_b, _WORD first_b,
	_WORD entries_b, _WORD pause_b);
_WORD lbox_delete(LIST_BOX *box);
_WORD lbox_do(LIST_BOX *box, _WORD obj);
void lbox_free_items(LIST_BOX *box);
void lbox_free_list(LBOX_ITEM *list);
_WORD lbox_get_afirst(LIST_BOX *box);
_WORD lbox_get_avis(LIST_BOX *box);
_WORD lbox_get_bentries(LIST_BOX *box);
_WORD lbox_get_bfirst(LIST_BOX *box);
_WORD lbox_get_bvis(LIST_BOX *box);
_WORD lbox_get_idx(LBOX_ITEM *items, LBOX_ITEM *search);
LBOX_ITEM *lbox_get_item(LIST_BOX *box, _WORD n);
LBOX_ITEM *lbox_get_items(LIST_BOX *box);
_WORD lbox_get_slct_idx(LIST_BOX *box);
LBOX_ITEM *lbox_get_slct_item(LIST_BOX *box);
OBJECT *lbox_get_tree(LIST_BOX *box);
void *lbox_get_udata(LIST_BOX *box);
void lbox_set_asldr(LIST_BOX *box, _WORD first, GRECT *rect);
void lbox_set_bentries(LIST_BOX *box, _WORD entries);
void lbox_set_bsldr(LIST_BOX *box, _WORD first, GRECT *rect);
void lbox_set_items(LIST_BOX *box, LBOX_ITEM *items);
void lbox_update(LIST_BOX *box, GRECT *rect);

#endif /* __LBOX_H__ */
