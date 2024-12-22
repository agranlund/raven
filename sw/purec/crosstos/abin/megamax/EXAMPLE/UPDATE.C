
/*
			Window update functions
*/
#include <gemdefs.h>
#include <osbind.h>
#include <obdefs.h>

#include "globals.h"


/*
	Null udpate function
*/
nullproc(thewin)
	windowptr thewin;
{
	wind_blank(thewin);
}


/*
	Typical update function
*/
show_wlist(thewin)
	windowptr	thewin;
{
	int			tx, ty;
	int			grafhandle;
	windowptr	mywin;

	wind_blank(thewin);
	tx = thewin -> work.g_x + 5;
	ty = thewin -> work.g_y + 15;

	grafhandle = thewin -> graf.handle;

	/*
		Display the data.
	*/
	v_gtext(grafhandle, tx+75, ty, "The Window List");
	for (mywin = firstwindow; mywin; mywin = mywin -> next) {
		ty += 15;
		v_gtext(grafhandle, tx, ty, mywin -> title);
	}
}


/*
	wind_blank  -  clear the content region of the window.  

	Note:  This function should save and restore the current
		fill interior and style.
*/
wind_blank(thewin)
	windowptr thewin;
{
	int			grafhandle;
	GRECT		windrect;
	int			x, y, w, h;

	grafhandle = thewin -> graf.handle;

	wind_get(thewin -> handle, WF_WORKXYWH, &x, &y, &w, &h);

	rect_set(&windrect, x, y, x+w, y+h);

	vsf_interior(grafhandle, 1);  /* set for solid fill */
	vsf_color(grafhandle, 0);    /*  blank to white  */

	v_bar(grafhandle, &windrect);		/* blank the interior */
}


/*
	Text attributes
*/
#define BOLD		0x001
#define PLAIN		0x002
#define ITALICS		0x004
#define UNDERLINE	0x008
#define OUTLINE		0x010
#define SHADOW		0x020


laserproc(thewin)
	windowptr thewin;
{
	int tx, ty;
	int ch, cw, dummy;
	char *text = "Laser C";

	int grafhandle = thewin -> graf.handle;

	/*
		Function begins here.
	*/
	wind_blank(thewin);

	/*
		Set text size 25 pts & set slant mode & write mode = transparent.
	*/
	vst_height(grafhandle, 25, &dummy, &dummy, &cw, &ch);
	vst_effects(grafhandle, OUTLINE | ITALICS | UNDERLINE);
	vswr_mode(grafhandle, 2);

	tx = thewin -> work.g_x + 40;
	ty = thewin -> work.g_y + 80;
	v_gtext(grafhandle, tx, ty, text);
}
