

#include <obdefs.h>

rect_offset(therect, horiz, vert)
	GRECT *therect;
	int   horiz, vert;
{
	therect -> g_x += vert;
	therect -> g_y += horiz;
}
