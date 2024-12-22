#include <gemdefs.h>
#include <obdefs.h>

#include "globals.h"

extern char	*malloc();

/*
	External references for window update procs.
*/
extern nullproc();
extern laserproc();

/*
	do_window - determines the type of window event and then calls
		the appropriate function to handle the event.
*/
do_window(message)
	int *message;
{
	int handle;

	handle = message[3];

	graf_mouse(M_OFF, 0L);
	wind_update(BEG_UPDATE);

	switch (message[0]) {
		case WM_REDRAW:
			do_update(message);
		break;

		case WM_NEWTOP:
		case WM_TOPPED:
			make_frontwin(findwindowptr(handle));
		break;

		case WM_MOVED:
		case WM_SIZED:
			do_resize(message);
		break;

		case WM_FULLED:
			do_fullsize(handle);
		break;

		case WM_CLOSED:
			dispose_window(findwindowptr(handle));
		break;
	}

	wind_update(END_UPDATE);
	graf_mouse(M_ON, 0L);

}


/*
	do_resize - redraws the window at it's new postion and updates all
		of the window's position records.
*/
do_resize(message)
	int *message;
{
	int x, y, w, h;
	int handle;

	handle	= message[3];
	x		= message[4];
	y		= message[5];
	w		= message[6];
	h		= message[7];

	/*
		Make sure that the window doesn't become too small.
	*/
	if (w < 80) w = 80;
	if (h < 80) h = 80;

	/*
		Redraw the window at it's new size.
	*/
	wind_set(handle, WF_CURRXYWH, x, y, w, h);
	wind_get(handle, WF_WORKXYWH, &x, &y, &w, &h);

	{
		/*
			Set the Window record data.
		*/
		windowptr	thewin;

		thewin = findwindowptr(handle);

		rect_set(&thewin -> work, x, y, w, h);
		rect_set(&thewin -> box, x, y, w, h);

		thewin -> fullsize = FALSE;
	}
}


/*
	do_fullsize - draws the window at it's fully defined size.  If the window
		is at it's full size then this routines restores the window to it's
		previous size.
*/
do_fullsize(handle)
	int handle;
{
	register windowptr	thewin;

	int x, y, w, h;
	int d;

	thewin = findwindowptr(handle);

	if (thewin -> fullsize) {
		/*
			Back to normal size
		*/
		wind_calc(WC_WORK, thewin -> kind, thewin -> box,
			&thewin -> work.g_x, &thewin -> work.g_y,
			&thewin -> work.g_w, &thewin -> work.g_h);

		wind_set(handle, WF_CURRXYWH, thewin -> box);
		thewin -> fullsize = FALSE;
	} else {
		/*
			Draw window at full size;
		*/
		wind_get(handle, WF_FULLXYWH, &x, &y, &w, &h);
		wind_set(handle, WF_CURRXYWH, x, y, w, h);
		wind_calc(WC_WORK, thewin -> kind, x, y, w, h,
			&thewin -> work.g_x, &thewin -> work.g_y,
			&thewin -> work.g_w, &thewin -> work.g_h);

		thewin -> fullsize = TRUE;
	}
}


/*
	do_update - Update all of the rectangles affected by the update event.
*/
do_update(message)
	int *message;
{
	int	 thewindow;
	GRECT r1, therect;

	thewindow = message[3];

	rect_set(&therect, message[4], message[5], message[6], message[7]);

	wind_get(thewindow, WF_FIRSTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);

	while (r1.g_w && r1.g_h) {
		if (rect_intersect(therect, r1, &r1)) {
			setclip(thewindow, &r1);
			update_window(thewindow);
		}

		wind_get(thewindow, WF_NEXTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);
	}

	{
		int x, y, w, h;

		/*
			Restore clip rectangle to desktop rectangle.
		*/
		wind_get(0, WF_WORKXYWH, &x, &y, &w, &h);
		rect_set(&r1, x, y, x+w, y+h);
		vs_clip(phys_handle, 1, &r1);
	}
}


/*
	update_window  -  execute the update procedure associated with the window.
*/
update_window(windhandle)
	int			windhandle;
{
	windowptr	thewin;

	thewin = findwindowptr(windhandle);

	(*thewin -> updateproc)(thewin);
}


/*
	setclip  -  set the windows clipping rectangle.
*/
setclip(thewindow, r1)
	int   thewindow;
	GRECT *r1;
{
	GRECT cliprect;
	int  grafhandle;

	grafhandle = findwindowptr(thewindow) -> graf.handle;

	rect_set(&cliprect, r1->g_x, r1->g_y, r1->g_x+r1->g_w-1, r1->g_y+r1->g_h-1);

	vs_clip(grafhandle, 1, &cliprect);
}


/*
				Window support routines.
*/


/*
	frontwindow  -  returns a pointer which points to the window record for
		the frontmost window.
*/
windowptr frontwindow()
{
	return thefrontwin;
}


/*
	findwindowptr  -  find the window record associated with the window
		handle and return a pointer to that window record.
*/
windowptr findwindowptr(handle)
	int handle;
{
	register windowptr thewin = firstwindow;

	for (thewin = firstwindow; thewin; thewin = thewin -> next)
		if (thewin -> handle == handle)
			break;

	if (!thewin) {
		paramdlog("Internal Error: No window found for handle");
		shutdown(2);
	}

	return thewin;
}


/*
	new_window  -  create & draw a new window.

	1.)  create the window.
	2.)  draw the window with the wind_open()
	3.)  create and setup the window record.
*/
windowptr new_window(thekind)
	int			thekind;
{
	int			handle;
	int			xdesk, ydesk, wdesk, hdesk;
	windowptr	thewin;
	static		window_count = 1;

	/*
		Get the desktop coordinates.
	*/
	wind_get(0, WF_WORKXYWH, &xdesk, &ydesk, &wdesk, &hdesk);

	/*
		Create the information for the window.  Max size is the desktop.
	*/
	handle = wind_create(thekind, xdesk, ydesk, wdesk, hdesk);

	/*
		Check for error.
	*/
	if (handle < 0) {
		paramdlog("Sorry! No more windows available.");
		return NULL;
	}

	/*
		Allocate space for window record.
	*/
	thewin				  = (windowptr) malloc(sizeof(windowrec));

	/*
		Set the title for the window.
	*/
	sprintf(thewin -> title, " Untitled %d ", window_count++);

	wind_set(handle, WF_NAME, thewin -> title, 0, 0);

	/*
		A little flim-flammery.
	*/
	graf_growbox(0, 0, 0, 0, xdesk, ydesk, wdesk/2, hdesk/2);

	/*
		Draw the window.
	*/
	wind_open(handle, xdesk, ydesk, wdesk/2, hdesk/2);

	/*
		Initialize window data structure.
	*/
	thewin -> next		  = NULL;
	thewin -> handle	  = handle;
	thewin -> kind		  = thekind;
	thewin -> fullsize	  = FALSE;
	thewin -> graf.handle = open_vwork(&thewin -> graf.mfdb);
	thewin -> updateproc  = nullproc;
	thewin -> updateproc  = laserproc;

	wind_get(handle, WF_WORKXYWH, &xdesk, &ydesk, &wdesk, &hdesk);
	rect_set(&thewin -> work, xdesk, ydesk, wdesk, hdesk);

	wind_get(handle, WF_CURRXYWH, &xdesk, &ydesk, &wdesk, &hdesk);
	rect_set(&thewin ->  box, xdesk, ydesk, wdesk, hdesk);


	/*
		Insert into windowlist.
	*/
	{
		register windowptr	winptr = (windowptr) &firstwindow;

		while(winptr -> next) 
			winptr = winptr -> next;
	
		winptr -> next = thewin;
	}

	make_frontwin(thewin);

	return thewin;
}


/*
	dispose_window - Closes the window and disposes the storage for
		the window record.
*/
dispose_window(thewin)
	windowptr	thewin;
{
	int x, y, w, h;
	int handle;

	handle = thewin -> handle;

	wind_close(handle);

	wind_get(handle, WF_CURRXYWH, &x, &y, &w, &h);

	graf_shrinkbox(0, 0, 0, 0, x, y, w, h);

	wind_delete(handle);

	{
		/*
			Remove window record from window list.
		*/
		register windowptr	winptr = (windowptr) &firstwindow;

		while(winptr -> next)
			if (winptr -> next == thewin)
				break;
			else
				winptr = winptr -> next;

		if (winptr -> next)
			winptr -> next = winptr -> next -> next;
		else {
			paramdlog("Internal Error: Window pointer not in list.");
			shutdown(2);
		}

		/*
			Update the front window pointer.
		*/
		if (!firstwindow)
			thefrontwin = NULL;
		else
			if (winptr == (windowptr) &firstwindow)
				make_frontwin(winptr -> next);
			else
				make_frontwin(winptr);

		/*
			Close workstation associated with window.
		*/
		v_clsvwk(thewin -> graf.handle);

		/*
			Release window storage.
		*/
		free(thewin);
	}
}


/*
	make_frontwin - Force a window to the front.
*/
make_frontwin(thewin)
	windowptr thewin;
{
	wind_set(thewin -> handle, WF_TOP, 0, 0, 0, 0);
	thefrontwin = thewin;
}
