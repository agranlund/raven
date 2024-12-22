#include <obdefs.h>
#include <define.h>
#include <gemdefs.h>
#include <osbind.h>

change_item(tree, item, mask, value)
OBJECT *tree;
int item, mask, value;
{
	int x, y;
	OBJECT *ob;

	ob = &tree[item];
	objc_offset(tree, item, &x, &y);
	change_aux(tree, item, mask, value, x, y, ob->ob_width, ob->ob_height);
}
