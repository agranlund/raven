
#include <gemdefs.h>
#include <obdefs.h>

#include "resource.h"


paramdlog(string)
	char *string;
{
	OBJECT	*dialog;

	/*
		Get address of dialog definition in memory.
	*/
	rsrc_gaddr(0, PARMDLOG, &dialog);

	dialog[PARMTEXT].ob_spec = string;
	do_dialog(dialog);

	/*
		De-select the OK button
	*/
	dialog[OK].ob_state &= ~SELECTED;
}


do_dialog(dialog)
	OBJECT *dialog;
{
	int		x, y, w, h;
	int		itemhit;

	/*
		Center the dialog box.
	*/
	form_center(dialog, &x, &y, &w, &h);

	/*
		Reserve screen memory for dialog.
	*/
	form_dial(FMD_START, 0, 0, 0, 0, x, y, w, h);

	/*
		Draw dialog
	*/
	objc_draw(dialog, 0, 10, x, y, w, h);

	/*
		Handle Dialog Event.
	*/
	itemhit = form_do(dialog, 0);

	/*
		Release reserved screen memory.
	*/
	form_dial(FMD_FINISH, 0, 0, 0, 0, x, y, w, h);

	return itemhit;
}
