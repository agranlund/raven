

#include <obdefs.h>

pt_2rect(point1, point2, therect)
	GPOINT	point1, point2;
	GRECT	*therect;
{
	therect -> g_x = point1.p_x;
	therect -> g_y = point1.p_y;
	therect -> g_w = point2.p_x - point1.p_x;
	therect -> g_h = point2.p_y - point1.p_y;
}
