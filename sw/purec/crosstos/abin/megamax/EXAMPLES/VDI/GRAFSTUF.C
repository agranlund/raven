
/*
	These routines are here as support routines for common functions not
	supported by vdi.
*/

#include <gemdefs.h>
#include <osbind.h>

extern int contrl[12];
extern int intin[256],  ptsin[256];
extern int intout[256], ptsout[256];


/*
    open_workstation  -  Open a VDI virtual workstation.

    Note:
      information about the workstation is returned in the
      parameter `form'. appl_init() must be called previously.
*/
int open_workstation(form)
    register MFDB   *form;
{
    register int x;
    int      work_in[11];
    int      work_out[57];
    int      handle;
    int      dummy;
    int      GDOS = 0;

    /*
        Does GDOS exist?
    */
    asm {
            move.w  #-2, D0

            trap    #2

            cmp.w   #-2, D0
            beq     gdos_not_installed
            move.w  #1, GDOS(A6)
    gdos_not_installed:
    }

    /*
        Initialize workstation variables.
    */
    if (GDOS)
        work_in[0] = Getrez() + 2;
    else
        work_in[0] = 1;

    for(x=1; x<10; x++)
        work_in[x] = 1;

    /*
        Set for Raster Coordinate System.
    */
    work_in[10] = 2;

    /*
        Open Virtual Workstation
    */
    handle = graf_handle(&dummy, &dummy, &dummy, &dummy);
    v_opnvwk(work_in, &handle, work_out);

    /*
        Check for error.
    */
    if (!handle) {
        Cconws("\033E Error: Cannot open Virtual Device");
        Bconin(2);
        exit(1);
    }

    /*
        Set up the Memory Form Definition Block (MFDB).  This 
        structure is defined in <gemdefs.h>.
    */

    /*
        The Base address of the drawing screen.
    */
    form -> fd_addr = Logbase();

    /*
        The width of the screen in pixels.
    */
    form -> fd_w    = work_out[0] + 1;

    /*
        The height of the screen in pixels.
    */
    form -> fd_h    = work_out[1] + 1;

    /*
        The number of words in the width of the screen.
    */
    form -> fd_wdwidth  = form -> fd_w / 16;

    /*
        Working in a raster coordinate system.  
    */
    form -> fd_stand       = 0;

    /*
        The number of drawing planes.
    */
    switch(work_out[13]) {
        case 16: form -> fd_nplanes = 4; break;
        case 08: form -> fd_nplanes = 3; break;
        case 04: form -> fd_nplanes = 2; break;
        default: form -> fd_nplanes = 1; break;
    }

    /*
        Return the workstation handle.
    */
    return handle;
}

/*
	wait - Well? What do you think it does?
*/
wait(handle)
	int handle;
{
	int		x = 10;
	int		y = 195;

	v_gtext(handle, x, y, "Press RETURN to continue.");
	Cconin();
}
