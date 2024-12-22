/*
*/

#include <gemdefs.h>

int contrl[12];
int intin[256],  ptsin[256];
int intout[256], ptsout[256];

#define ON 1

main()
{
	MFDB	source;
	int		handle;

	/*
		Start up ROM.
	*/
	appl_init();
	handle = open_workstation(&source);

	/*
		Do neat things.
	*/
	check_pixel(handle);
	set_mouse(handle);

	/*
		Wait & Close the virtual workstation.
	*/
	wait(handle);
	v_clsvwk(handle);
	appl_exit();
}

check_pixel(handle)
    int handle;
{
    int x = 100;
    int y = 100;
    int state;
    int color;

    v_get_pixel(handle, x, y, &state, &color);

    printf("The Pixel at (%d, %d) is ", x, y);

    if (state == ON)
        puts("on");
    else
        puts("off");
}

/*
    set_mouse - redefine the current mouse cursor using vsc_form().

    Note: stuffbits() is a routine that converts ascii 0's & 1's to
          real binary and stores the result at the location pointed to
          by the first parameter.  It is defined in grafstuf.c
*/
set_mouse(handle)
    int handle;
{
    unsigned form[37];
    int      x;

    /*
        Define the mouses ``Hot Spot''
    */
    pt_set(&form[0], 5, 2);

    /*
        Setup array information
    */
    form[2] = 1;    /*  reserved by Atari  */
    form[3] = 2;    /*  Background color   */
    form[4] = 3;    /*  Foreground color   */

    /*
        Define Background mouse form
    */
    stuffbits(&form[5], "0000000000000000");
    stuffbits(&form[6], "0000011111100000");
    stuffbits(&form[7], "0000001111100000");
    stuffbits(&form[8], "0000000001100000");
    stuffbits(&form[9], "0000000000000000");

    for (x=10; x<21; x++)
        stuffbits(&form[x], "0000000000000000");

    /*
        Define Foreground mouse form
    */
    stuffbits(&form[21], "0011111111110000");
    stuffbits(&form[22], "0011100000010000");
    stuffbits(&form[23], "0011110000010000");
    stuffbits(&form[24], "0011111110010000");
    stuffbits(&form[25], "0011111111110000");

    for (x=26; x<37; x++)
        stuffbits(&form[x], "0000000000000000");

    vsc_form(handle, form);
}
