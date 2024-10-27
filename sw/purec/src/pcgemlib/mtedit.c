#include "gem_aesP.h"
#include "mt_gemx.h"

XEDITINFO *mt_edit_create(_WORD *global_aes)
{
	AES_PARAMS(210,0,0,0,1);

	aes_addrout[0] = 0;
	AES_TRAP(aes_params);

	return (XEDITINFO *) aes_addrout[0];
}


_WORD mt_edit_open(OBJECT *tree, _WORD obj, _WORD *global_aes)
{
	AES_PARAMS(211,1,1,1,0);

	aes_intin[0]  = obj;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

	return aes_intout[0];
}


void mt_edit_close(OBJECT *tree, _WORD obj, _WORD *global_aes)
{
	AES_PARAMS(212,1,0,1,0);

	aes_intin[0]  = obj;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);
}


void mt_edit_delete(XEDITINFO *xi, _WORD *global_aes)
{
	AES_PARAMS(213,0,0,1,0);

	aes_addrin[0] = xi;

	AES_TRAP(aes_params);
}


_WORD mt_edit_cursor(OBJECT *tree, _WORD obj, _WORD whdl, _WORD show, _WORD *global_aes)
{
	AES_PARAMS(214,3,1,1,0);
	
	aes_intin[0]  = obj;
	aes_intin[1]  = whdl;
	aes_intin[2]  = show;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

	return aes_intout[0];
}


_WORD mt_edit_evnt(OBJECT *tree, _WORD obj, _WORD whdl,
				EVNT *ev, _LONG *errc, _WORD *global_aes)
{
	AES_PARAMS(215,2,3,2,0);

	aes_intin[0]  = obj;
	aes_intin[1]  = whdl;
	aes_addrin[0] = tree;
	aes_addrin[1] = ev;
	*((_LONG *) (aes_intout+1)) = 0L;	/* For old SLB */

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (errc)
#endif
	*errc = *((_LONG *) (aes_intout+1));

	return aes_intout[0];
}


_WORD mt_edit_get_buf(OBJECT *tree, _WORD obj, char **buf, _LONG *buflen, _LONG *txtlen, _WORD *global_aes)
{
	AES_PARAMS(216,2,5,1,1);

	aes_intin[0]  = obj;
	aes_intin[1]  = 0;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (buf)
#endif
	*buf = (char *)aes_addrout[0];
#if CHECK_NULLPTR
	if (buflen)
#endif
	*buflen = *((_LONG *) (aes_intout+1));
#if CHECK_NULLPTR
	if (txtlen)
#endif
	*txtlen = *((_LONG *) (aes_intout+3));

	return aes_intout[0];
}
_WORD edit_get_buf(OBJECT *tree, _WORD obj, char **buf, _LONG *buflen, _LONG *txtlen)
{
	return mt_edit_get_buf(tree, obj, buf, buflen, txtlen, aes_global);
}


_WORD mt_edit_get_format(OBJECT *tree, _WORD obj, _WORD *tabwidth, _WORD *autowrap, _WORD *global_aes)
{
	AES_PARAMS(216,2,3,1,0);

	aes_intin[0]  = obj;
	aes_intin[1]  = 1;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (tabwidth)
#endif
	*tabwidth = aes_intout[1];
#if CHECK_NULLPTR
	if (autowrap)
#endif
	*autowrap = aes_intout[2];

	return aes_intout[0];
}
_WORD edit_get_format(OBJECT *tree, _WORD obj, _WORD *tabwidth, _WORD *autowrap)
{
	return mt_edit_get_format(tree, obj, tabwidth, autowrap, aes_global);
}


_WORD mt_edit_get_color(OBJECT *tree, _WORD obj, _WORD *tcolor, _WORD *bcolor, _WORD *global_aes)
{
	AES_PARAMS(216,2,3,1,0);

	aes_intin[0]  = obj;
	aes_intin[1]  = 2;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (tcolor)
#endif
	*tcolor = aes_intout[1];
#if CHECK_NULLPTR
	if (bcolor)
#endif
	*bcolor = aes_intout[2];

	return aes_intout[0];
}
_WORD edit_get_color(OBJECT *tree, _WORD obj, _WORD *tcolor, _WORD *bcolor)
{
	return mt_edit_get_color(tree, obj, tcolor, bcolor, aes_global);
}


_WORD mt_edit_get_font(OBJECT *tree, _WORD obj, _WORD *font_id, _WORD *font_h, _WORD *font_pix, _WORD *mono, _WORD *global_aes)
{
	AES_PARAMS(216,2,5,1,0);
	
	aes_intin[0]  = obj;
	aes_intin[1]  = 3;
	aes_addrin[0] = tree;
	aes_intout[4] = 0;	/* For old libraries */

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (font_id)
#endif
	*font_id  = aes_intout[1];
#if CHECK_NULLPTR
	if (font_h)
#endif
	*font_h   = aes_intout[2];
#if CHECK_NULLPTR
	if (font_pix)
#endif
	*font_pix = aes_intout[4];
#if CHECK_NULLPTR
	if (mono)
#endif
	*mono     = aes_intout[3];
	
	return aes_intout[0];
}
_WORD edit_get_font(OBJECT *tree, _WORD obj, _WORD *font_id, _WORD *font_h, _WORD *font_pix, _WORD *mono)
{
	return mt_edit_get_font(tree, obj, font_id, font_h, font_pix, mono, aes_global);
}


_WORD mt_edit_get_cursor(OBJECT *tree, _WORD obj, char **cursorpos, _WORD *global_aes)
{
	AES_PARAMS(216,2,1,1,1);

	aes_intin[0]  = obj;
	aes_intin[1]  = 4;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (cursorpos)
#endif
	*cursorpos = (char *)aes_addrout[0];
	
	return aes_intout[0];
}
_WORD edit_get_cursor(OBJECT *tree, _WORD obj, char **cursorpos)
{
	return mt_edit_get_cursor(tree, obj, cursorpos, aes_global);
}


void mt_edit_get_pos(OBJECT *tree, _WORD obj, _WORD *xscroll, _LONG *yscroll, char **cyscroll, char **cursorpos, _WORD *cx, _WORD *cy, _WORD *global_aes)
{
	AES_PARAMS(216,2,6,1,2);

	aes_intin[0]  = obj;
	aes_intin[1]  = 5;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (xscroll)
#endif
	*xscroll = aes_intout[1];
#if CHECK_NULLPTR
	if (yscroll)
#endif
	*yscroll = *((_LONG *) (aes_intout+2));
#if CHECK_NULLPTR
	if (cyscroll)
#endif
	*cyscroll = (char *)aes_addrout[0];
#if CHECK_NULLPTR
	if (cx)
#endif
	*cx = aes_intout[4];
#if CHECK_NULLPTR
	if (cy)
#endif
	*cy = aes_intout[5];
#if CHECK_NULLPTR
	if (cursorpos)
#endif
	*cursorpos = (char *)aes_addrout[1];
}
void edit_get_pos(OBJECT *tree, _WORD obj, _WORD *xscroll, _LONG *yscroll, char **cyscroll, char **cursorpos, _WORD *cx, _WORD *cy)
{
	mt_edit_get_pos(tree, obj, xscroll, yscroll, cyscroll, cursorpos, cx, cy, aes_global);
}


_WORD mt_edit_get_dirty(OBJECT *tree, _WORD obj, _WORD *global_aes)
{
	AES_PARAMS(216,2,1,1,0);

	aes_intin[0]  = obj;
	aes_intin[1]  = 7;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

	return aes_intout[0];
}
_WORD edit_get_dirty(OBJECT *tree, _WORD obj)
{
	return mt_edit_get_dirty(tree, obj, aes_global);
}


void mt_edit_get_sel(OBJECT *tree, _WORD obj, char **bsel, char **esel, _WORD *global_aes)
{
	AES_PARAMS(216,2,0,1,2);

	aes_intin[0]  = obj;
	aes_intin[1]  = 8;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (bsel)
#endif
	*bsel = (char *)aes_addrout[0];
#if CHECK_NULLPTR
	if (esel)
#endif
	*esel = (char *)aes_addrout[1];
}
void edit_get_sel(OBJECT *tree, _WORD obj, char **bsel, char **esel)
{
	mt_edit_get_sel(tree, obj, bsel, esel, aes_global);
}


void mt_edit_get_scrollinfo(OBJECT *tree, _WORD obj, _LONG *nlines, _LONG *yscroll,
                        _WORD *yvis, _WORD *yval, _WORD *ncols, _WORD *xscroll, _WORD *xvis,
					    _WORD *global_aes)
{
	AES_PARAMS(216,2,10,1,0);

	aes_intin[0]  = obj;
	aes_intin[1]  = 9;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (nlines)
#endif
	*nlines = *((_LONG *) (aes_intout+1));
#if CHECK_NULLPTR
	if (yscroll)
#endif
	*yscroll = *((_LONG *) (aes_intout+3));
#if CHECK_NULLPTR
	if (yvis)
#endif
	*yvis = aes_intout[5];
#if CHECK_NULLPTR
	if (yval)
#endif
	*yval = aes_intout[6];
#if CHECK_NULLPTR
	if (ncols)
#endif
	*ncols = aes_intout[7];
#if CHECK_NULLPTR
	if (xscroll)
#endif
	*xscroll = aes_intout[8];
#if CHECK_NULLPTR
	if (xvis)
#endif
	*xvis = aes_intout[9];
}
void edit_get_scrollinfo(OBJECT *tree, _WORD obj, _LONG *nlines, _LONG *yscroll,
                        _WORD *yvis, _WORD *yval, _WORD *ncols, _WORD *xscroll, _WORD *xvis)
{
	mt_edit_get_scrollinfo(tree, obj, nlines, yscroll, yvis, yval, ncols, xscroll, xvis, aes_global);
}


void mt_edit_set_buf(OBJECT *tree, _WORD obj, char *buf, _LONG buflen, _WORD *global_aes)
{
	AES_PARAMS(217,4,0,2,0);

	aes_intin[0] = obj;
	aes_intin[1] = 0;
	*(_LONG *) (aes_intin+2) = buflen;
	aes_addrin[0] = tree;
	aes_addrin[1] = buf;

	AES_TRAP(aes_params);
}
void edit_set_buf(OBJECT *tree, _WORD obj, char *buf, _LONG buflen)
{
	mt_edit_set_buf(tree, obj, buf, buflen, aes_global);
}


void mt_edit_set_format(OBJECT *tree, _WORD obj, _WORD tabwidth, _WORD autowrap, _WORD *global_aes)
{
	AES_PARAMS(217,4,0,1,0);
	
	aes_intin[0]  = obj;
	aes_intin[1]  = 1;
	aes_intin[2]  = tabwidth;
	aes_intin[3]  = autowrap;
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);
}
void edit_set_format(OBJECT *tree, _WORD obj, _WORD tabwidth, _WORD autowrap)
{
	mt_edit_set_format(tree, obj, tabwidth, autowrap, aes_global);
}


void mt_edit_set_color(OBJECT *tree, _WORD obj, _WORD tcolor, _WORD bcolor, _WORD *global_aes)
{
	AES_PARAMS(217,4,0,1,0);
	
	aes_intin[0] = obj;
	aes_intin[1] = 2;
	aes_intin[2] = tcolor;
	aes_intin[3] = bcolor;
	
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);
}
void edit_set_color(OBJECT *tree, _WORD obj, _WORD tcolor, _WORD bcolor)
{
	mt_edit_set_color(tree, obj, tcolor, bcolor, aes_global);
}


void mt_edit_set_font(OBJECT *tree, _WORD obj, _WORD font_id, _WORD font_h, _WORD font_pix, _WORD mono, _WORD *global_aes)
{
	AES_PARAMS(217,6,0,1,0);

	aes_intin[0] = obj;
	aes_intin[1] = 3;
	aes_intin[2] = font_id;
	aes_intin[3] = font_h;
	aes_intin[4] = mono;
	aes_intin[5] = font_pix;
	
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);
}
void edit_set_font(OBJECT *tree, _WORD obj, _WORD font_id, _WORD font_h, _WORD font_pix, _WORD mono)
{
	mt_edit_set_font(tree, obj, font_id, font_h, font_pix, mono, aes_global);
}


void mt_edit_set_cursor(OBJECT *tree, _WORD obj, char *cursorpos, _WORD *global_aes)
{
	AES_PARAMS(217,2,0,2,0);
	
	aes_intin[0] = obj;
	aes_intin[1] = 4;
	
	aes_addrin[0] = tree;
	aes_addrin[1] = cursorpos;

	AES_TRAP(aes_params);
}
void edit_set_cursor(OBJECT *tree, _WORD obj, char *cursorpos)
{
	mt_edit_set_cursor(tree, obj, cursorpos, aes_global);
}


void mt_edit_set_pos(OBJECT *tree, _WORD obj, _WORD xscroll, _LONG yscroll,
				 char *cyscroll, char *cursorpos, _WORD cx, _WORD cy, _WORD *global_aes)
{
	AES_PARAMS(217,7,0,3,0);

	aes_intin[0] = obj;
	aes_intin[1] = 5;
	aes_intin[2] = xscroll;
	*(_LONG *) (aes_intin+3) = yscroll;
	aes_intin[5] = cx;
	aes_intin[6] = cy;
	
	aes_addrin[0] = tree;
	aes_addrin[1] = cyscroll;
	aes_addrin[2] = cursorpos;

	AES_TRAP(aes_params);
}
void edit_set_pos(OBJECT *tree, _WORD obj, _WORD xscroll, _LONG yscroll,
				 char *cyscroll, char *cursorpos, _WORD cx, _WORD cy)
{
	mt_edit_set_pos(tree, obj, xscroll, yscroll, cyscroll, cursorpos, cx, cy, aes_global);
}


_WORD mt_edit_resized(OBJECT *tree, _WORD obj, _WORD *oldrh, _WORD *newrh, _WORD *global_aes)
{
	AES_PARAMS(217,2,3,1,0);

	aes_intin[0] = obj;
	aes_intin[1] = 6;
	
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (oldrh)
#endif
	*oldrh = aes_intout[1];
#if CHECK_NULLPTR
	if (newrh)
#endif
	*newrh = aes_intout[2];
	
	return aes_intout[0];
}
_WORD edit_resized(OBJECT *tree, _WORD obj, _WORD *oldrh, _WORD *newrh)
{
	return mt_edit_resized(tree, obj, oldrh, newrh, aes_global);
}



void mt_edit_set_dirty(OBJECT *tree, _WORD obj, _WORD dirty, _WORD *global_aes)
{
	AES_PARAMS(217,3,0,1,0);

	aes_intin[0] = obj;
	aes_intin[1] = 7;
	aes_intin[2] = dirty;
	
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);
}
void edit_set_dirty(OBJECT *tree, _WORD obj, _WORD dirty)
{
	mt_edit_set_dirty(tree, obj, dirty, aes_global);
}


_WORD mt_edit_scroll(OBJECT *tree, _WORD obj, _WORD whdl, _LONG yscroll, _WORD xscroll, _WORD *global_aes)
{
	AES_PARAMS(217,6,1,1,0);

	aes_intin[0] = obj;
	aes_intin[1] = 9;
	aes_intin[2] = whdl;
	*(_LONG *) (aes_intin+3) = yscroll;
	aes_intin[5] = xscroll;
	
	aes_addrin[0] = tree;

	AES_TRAP(aes_params);

	return aes_intout[0];
}


_WORD edit_scroll(OBJECT *tree, _WORD obj, _WORD whdl, _LONG yscroll, _WORD xscroll)
{
	return mt_edit_scroll(tree, obj, whdl, yscroll, xscroll, aes_global);
}
