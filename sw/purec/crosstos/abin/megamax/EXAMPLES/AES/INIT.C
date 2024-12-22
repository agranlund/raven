
#include <osbind.h>
#include <gemdefs.h>
#include <obdefs.h>

#include "resource.h"	/* header file created by RCP     */
#include "globals.h"  	/* contains definition of menubar */


/*
	init_resources - attempts to load the applications resources into 
		memory.  Note that if the file is not found in the current
		directory the ROM will search the A:\ drive automatically.
*/
init_resources()
{
	if (!rsrc_load("resource.rsc")) {
		form_alert(1, "[0][Cannot find resource.rsc file|Terminating ...][OK]");
		exit(2);
	}
}


/*
	init_menu  -  find the address of the menubar and draw it.
*/
init_menu()
{
	rsrc_gaddr(0, MENUBAR, &menubar);

	menu_bar(menubar, 1);
}


/*
	shutdown - is the code that closes down the application.  This routine
		is called when errors occurs and guarantees that all window's will
		be closed properly before exiting.
*/
shutdown(code)
	int code;
{
	/*
		Clean up memory.
	*/
	cleanup();

	/*
		Shut down the application.
	*/
	appl_exit();

	/*
		bye ...
	*/
	exit(code);
}


/*
	cleanup - releases the memory used by the application.
*/
cleanup()
{
	windowptr	thewin;

	/*
		Close down the windows.
	*/
	for (thewin = firstwindow; thewin; thewin = thewin -> next)
		dispose_window(thewin);

	/*
		Free memory used by resource.
	*/
	rsrc_free();
}


/*
	open_vwork - Open a virtual workstation.

	Note: a virtual workstation is associated with each window created.
		This means that each window's graphic attributes are independent
		of the other's.
*/
int open_vwork(form)
	register MFDB	*form;
{
	register int x;
	int 	 work_in[11];
	int		 handle, d;
	int		 work_out[57];

	/*
		Initialize workstation variables.
	*/
	for(x=0; x<10; x++)
		work_in[x] = 1;

	work_in[10] = 2;

	handle = graf_handle(&d, &d, &d, &d);
	v_opnvwk(work_in, &handle, work_out);

	form -> fd_addr 	= Logbase();
	form -> fd_w		= work_out[0] + 1;
	form -> fd_h		= work_out[1] + 1;
	form -> fd_wdwidth	= form -> fd_w / 16;
	form -> fd_stand	= 0;

	switch(work_out[13]) {
		case 16: form -> fd_nplanes = 4; break;
		case 08: form -> fd_nplanes = 3; break;
		case 04: form -> fd_nplanes = 2; break;
		default: form -> fd_nplanes = 1; break;
	}

	return handle;
}
