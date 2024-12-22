#include <obdefs.h>
#include <define.h>
#include <gemdefs.h>
#include <osbind.h>

clear_tree(dialog, cx, cy, cw, ch)
OBJECT *dialog;
int cx, cy, cw, ch;
/*
	Reset flags on all objects in a dialog tree to NORMAL
*/
{
	register int i;
	int x,y,x2,y2;

	if (!dialog)
		return;

	change_aux(dialog, 0, SELECTED, 0, cx, cy, cw, ch);
}
