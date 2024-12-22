/*
    main() - This is the main function for the example VDI functions.
*/
#include <gemdefs.h>

/*
    Define VDI Global Variables
*/
int contrl[12];
int intin[256],  ptsin[256];
int intout[256], ptsout[256];

main()
{
    MFDB    theMFDB;    /*  Screen definition structure  */
    int     handle;     /*  Virtual Workstation Handle   */

	/*
		Start up ROM.
	*/
	appl_init();
	handle = open_workstation(&theMFDB);

    /*
        Call example function here.
    */
    sample_function();

    /*
        Wait for a Carriage Return.
    */
    wait(handle);

	/*
        Close the virtual workstation, and shutdown application.
	*/
	v_clsvwk(handle);
	appl_exit();
}
