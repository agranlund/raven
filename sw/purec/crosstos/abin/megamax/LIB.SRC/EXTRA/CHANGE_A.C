#include <obdefs.h>
#include <define.h>
#include <gemdefs.h>
#include <osbind.h>

change_aux(tree, root, mask, value, cx, cy, cw, ch)
OBJECT *tree;
int root;
int cx, cy, cw, ch;
{
	int i;
	GRECT r1, r2;

	r1.g_x = cx; r1.g_y = cy; r1.g_w = cw; r1.g_h = ch;
	i = tree[root].ob_state;
	if (!mask || (i & mask) ^ value) {
		if (mask)
			tree[root].ob_state = (i & ~mask) | value;
		objc_offset(tree, root, &r2.g_x, &r2.g_y);
		r2.g_w = tree[root].ob_width;
		r2.g_h = tree[root].ob_height;
		if (rc_intersect(&r1, &r2))
			objc_draw(tree, root, 1, r2.g_x, r2.g_y, r2.g_w, r2.g_h);
	}

	i = tree[root].ob_head;
	while (i != -1 && i != root) {
		change_aux(tree, i, mask, value, cx, cy, cw, ch);
		i = tree[i].ob_next;
	}
}
