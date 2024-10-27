#include "gem_aesP.h"

#undef min
#undef max
#undef abs
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define abs(x) ((x)<0?(-(x)):(x))


#undef rc_copy
void rc_copy(const GRECT *src, GRECT *dst)
{
	*dst = *src;
}

#undef rc_equal
_WORD rc_equal(CONST GRECT *p1, CONST GRECT *p2)
{
	return p1->g_x == p2->g_x && p1->g_y == p2->g_y &&
	       p1->g_w == p2->g_w && p1->g_h == p2->g_h;
}

_WORD rc_intersect(const GRECT *r1, GRECT *r2)
{
	_WORD tx, ty, tw, th;
	_WORD ret;

	tx = max(r2->g_x, r1->g_x);
	tw = min(r2->g_x + r2->g_w, r1->g_x + r1->g_w) - tx;
	
	ret = tw > 0;
	if (ret)
	{
		ty = max(r2->g_y, r1->g_y);
		th = min(r2->g_y + r2->g_h, r1->g_y + r1->g_h) - ty;
		
		ret = th > 0;
		if (ret)
		{
			r2->g_x = tx;
			r2->g_y = ty;
			r2->g_w = tw;
			r2->g_h = th;
		}
	}
	
	return ret;
}


GRECT *array_to_grect(const _WORD *array, GRECT *area)
{
	area->g_x = array[0];
	area->g_y = array[1];
	area->g_w = array[2] - array[0] + 1;
	area->g_h = array[3] - array[1] + 1;
	
	return area;
}


_WORD *grect_to_array(const GRECT *area, _WORD *array)
{
	array[0] = area->g_x;
	array[1] = area->g_y;
	array[2] = area->g_x + area->g_w - 1;
	array[3] = area->g_y + area->g_h - 1;

	return array;
}
