/*
  vq_key_s()
*/

#include <gemdefs.h>

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

	v_enter_cur(handle);

	do {
		v_eeos(handle);
		v_curhome(handle);
		check_key_status(handle);
	} while	(!check_mouse(handle));

	v_exit_cur(handle);

	/*
        Close the virtual workstation, and shutdown application.
	*/
    wait(handle);
	v_clsvwk(handle);
	appl_exit();
}

#define LSHIFT  0x001
#define RSHIFT  0x002
#define CTRL    0x004
#define ALT     0x008

check_key_status(handle)
    int handle;
{
    int status;

    vq_key_s(handle, &status);

    if (status & RSHIFT)
        puts("The Left Shift key is down.   ");

    if (status & LSHIFT)
        puts("The Right Shift key is down.  ");

    if (status & CTRL)
        puts("The Control key is down.      ");

    if (status & ALT)
        puts("The Alternate key is down.    ");
}

#define LBUTTON 0x1
#define RBUTTON 0x2

int check_mouse(handle)
    int handle;
{
    int status, x, y;

    vq_mouse(handle, &status, &x, &y);

    printf("The mouse is at (%d, %d) and \n", x, y);

    if (status & LBUTTON)
        printf("the left button is down.\n");
    
    if (status & RBUTTON)
        printf("the right button is down.\n");

    if (!status)
        printf("no buttons are down.\n");

	return status;
}
