#ifndef __WDLGEDIT_H__
#define __WDLGEDIT_H__

#ifndef __EDITOBJC_IMPLEMENTATION
typedef struct _xeditinfo { int dummy; } XEDITINFO;

extern XEDITINFO *edit_create(void);
extern _WORD edit_open(OBJECT *tree, _WORD obj);
extern void edit_close(OBJECT *tree, _WORD obj);
extern void edit_delete(XEDITINFO *xi);
extern _WORD edit_cursor(OBJECT *tree, _WORD obj, _WORD whdl, _WORD show);
extern _WORD edit_evnt(OBJECT *tree, _WORD obj, _WORD whdl, EVNT *ev, _LONG *errc);
extern _WORD edit_get_buf(OBJECT *tree, _WORD obj, char **buf, _LONG *buflen, _LONG *txtlen);
extern _WORD edit_get_format(OBJECT *tree, _WORD obj, _WORD *tabwidth, _WORD *autowrap);
extern _WORD edit_get_color(OBJECT *tree, _WORD obj, _WORD *tcolour, _WORD *bcolour);
extern _WORD edit_get_font(OBJECT *tree, _WORD obj, _WORD *fontID, _WORD *fontH, _WORD *fontPix, _WORD *mono);
extern _WORD edit_get_cursor(OBJECT *tree, _WORD obj, char **cursorpos);
extern void edit_get_pos(OBJECT *tree, _WORD obj, _WORD *xscroll, _LONG *yscroll, char **cyscroll, char **cursorpos, _WORD *cx, _WORD *cy);
extern _WORD edit_get_dirty(OBJECT *tree, _WORD obj);
extern void edit_get_sel(OBJECT *tree, _WORD obj, char **bsel, char **esel);
extern void edit_get_scrollinfo(OBJECT *tree, _WORD obj, _LONG *nlines, _LONG *yscroll, _WORD *yvis, _WORD *yval, _WORD *ncols, _WORD *xscroll, _WORD *xvis);
extern void edit_set_buf(OBJECT *tree, _WORD obj, char *buf, _LONG buflen);
extern void edit_set_format(OBJECT *tree, _WORD obj, _WORD tabwidth, _WORD autowrap);
extern void edit_set_color(OBJECT *tree, _WORD obj, _WORD tcolor, _WORD bcolor);
extern void edit_set_font(OBJECT *tree, _WORD obj, _WORD fontID, _WORD fontH, _WORD fontPix, _WORD mono);
extern void edit_set_cursor(OBJECT *tree, _WORD obj, char *cursorpos);
extern void edit_set_pos(OBJECT *tree, _WORD obj, _WORD xscroll, _LONG yscroll, char *cyscroll, char *cursorpos, _WORD cx, _WORD cy);
extern _WORD edit_resized(OBJECT *tree, _WORD obj, _WORD *oldrh, _WORD *newrh);
extern void edit_set_dirty(OBJECT *tree, _WORD obj, _WORD dirty);
extern _WORD edit_scroll(OBJECT *tree, _WORD obj, _WORD whdl, _LONG yscroll, _WORD xscroll);

#endif

#endif /* __WDLGEDIT_H__ */
