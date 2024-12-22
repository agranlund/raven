
/*
	A test application for the Atari.
*/

#include <gemdefs.h>
#include <osbind.h>
#include <obdefs.h>

#define  extern
#include "globals.h"
#undef   extern


/*
	main - executes initialization code and the starts the application.
*/
main()
{
	int  dummy;

	/*
		Initiailize the ROMs.
	*/
	appl_init();

	/*
		Load resources.
	*/
	init_resources();

	/*
		Read menu resource and draw menu bar.
	*/
	init_menu();

	/*
		Get the Physical work station handle.
	*/
	phys_handle = graf_handle(&dummy, &dummy, &dummy, &dummy);


	/*
		Initialize the mouse.
	*/
	graf_mouse(ARROW, NULL);

	/*
		Create the Initial window.
	*/
	firstwindow = new_window(SIZER | MOVER | FULLER | CLOSER | NAME | INFO);

	/*
		Handle events for application.
	*/
	TaskMaster(); 

	/*
		bye...  Note: This will never be executed.
	*/
	shutdown(0);
}

