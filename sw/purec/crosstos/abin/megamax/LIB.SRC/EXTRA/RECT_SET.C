

#include <obdefs.h>

rect_set(therect, x, y, w, h)
	GRECT	*therect;
	int		 x, y, w, h;
{
	therect -> g_x = x;
	therect -> g_y = y;
	therect -> g_w = w;
	therect -> g_h = h;
}
