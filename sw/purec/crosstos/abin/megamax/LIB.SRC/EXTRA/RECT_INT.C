#include <obdefs.h>

rect_intersect(x1, y1, w1, h1, x2, y2, w2, h2, therect)
	register int	x1, y1, w1, h1;
	int				x2, y2, w2, h2;
	register GRECT	*therect;
	{
	int				a, b, c, d;

	a	= x1 + w1;
	b	= x2 + w2;
	c	= y1 + h1;
	d	= y2 + h2;
	if (a > b) a = b;
	if (c > d) c = d;
	if (x1 < x2) x1 = x2;
	if (y1 < y2) y1 = y2;
	w1 = a - x1;
	if (w1 <= 0 )
		return 0;
	h1 = c - y1;
	if (h1 <= 0 )
		return 0;
	therect->g_x	= x1;
	therect->g_y	= y1;
	therect->g_w	= w1;
	therect->g_h	= h1;
	return 1;
	}
