

#include <obdefs.h>

pt_add(pt1, pt2)
	GPOINT pt1, *pt2;
{
	pt2 -> p_x += pt1.p_x;
	pt2 -> p_y += pt1.p_y;
}
