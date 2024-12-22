
#include <gemdefs.h>
#include <obdefs.h>

#include "resource.h"  	/* contains definition of menubar */
#include "globals.h"	/* header file created by RCP     */

/*
	do_menu - determines which menu was selected and calls the
		appropriate routine to handle the item selected.
*/
do_menu(message)
	int *message;
{
	int menuid, itemid;

	menuid = message[3];
	itemid = message[4];

	switch(menuid) {
		case DESK:
			handle_desk(itemid);
		break;

		case FILE:
			handle_file(itemid);
		break;

		case EDIT:
			handle_edit(itemid);
		break;
	}

	menu_tnormal(menubar, menuid, 1);
}


/*
	handle_desk - performs the appropriate action for the menu item selected.
*/
handle_desk(itemid)
	int itemid;
{
	switch(itemid) {
		case ABOUT:
			form_alert(1, "[0][ A Sample Application | | rpt. ][ Ok ]");
		break;
	}
}


/*
	handle_file - performs the appropriate action for the menu item selected.
*/
handle_file(itemid)
	int itemid;
{
	int  button;

	switch(itemid) {
		case FILENEW:
			new_window(SIZER | MOVER | FULLER | CLOSER | NAME);
		break;

		case FILECLOS:
			{
				windowptr thewin = frontwindow();

				if (thewin)
					dispose_window(thewin);
			}
		break;

		case FILEQUIT:
			button = form_alert(2, "[3][ Are you sure? ][ Yes | No ]");

			if (button == 1)
				shutdown(0);
		break;
	}
}


/*
	handle_edit - performs the appropriate action for the menu item selected.
*/
handle_edit(itemid)
	int itemid;
{
	char string[80];

	switch(itemid) {
		case UNDO:
			sprintf(string, "Edit.  Undo.  itemid == %d.", itemid);
		break;

		case CUT:
			sprintf(string, "Edit.  Cut.  itemid == %d.", itemid);
		break;

		case COPY:
			sprintf(string, "Edit.  Copy.  itemid == %d.", itemid);
		break;

		case PASTE:
			sprintf(string, "Edit.  Paste.  itemid == %d.", itemid);
		break;

		case CLEAR:
			sprintf(string, "Edit.  Clear.  itemid == %d.", itemid);
		break;
	}

	paramdlog(string);
}
