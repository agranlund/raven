

#include <obdefs.h>

rect_inset(therect, horiz, vert)
	register GRECT *therect;
	int   horiz, vert;
{
	therect -> g_x += vert;
	therect -> g_y += horiz;
	therect -> g_w -= vert << 1;
	therect -> g_h -= horiz << 1;
}
