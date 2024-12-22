

#include <obdefs.h>

rect_union(r1x, r1y, r1w, r1h, r2x, r2y, r2w, r2h, theunion)
	register int r1x, r1y, r2x, r2y;
	int r1w, r1h, r2w, r2h;
	register GRECT *theunion;
{
	int bottom1, bottom2, right1, right2; 

	theunion -> g_x = ((r1x > r2x) ? r2x : r1x);
	theunion -> g_y = ((r1y > r2y) ? r2y : r1y);

	bottom1 = r1w + r1x;
	bottom2 = r2w + r2x;
	theunion -> g_w = ((bottom1 > bottom2) ? bottom1 : bottom2) - theunion->g_x;

	right1 = r1h + r1y;
	right2 = r2h + r2y;
	theunion -> g_h = ((right1 > right2) ? right1 : right2)  - theunion -> g_y;
}
