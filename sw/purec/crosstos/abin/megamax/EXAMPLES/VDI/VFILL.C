/*
  v_contourfill()
  v_fillarea()
  vsf_udfil()
*/

#include <gemdefs.h>

int contrl[12];
int intin[256],  ptsin[256];
int intout[256], ptsout[256];

main()
{
	MFDB	source;
	int		handle;

	/*
		Start up ROM.
	*/
	appl_init();
	handle = open_workstation(&source);
	v_clrwk(handle);

	/*
		Do neat things.
	*/
	do_fillarea(handle);
	vsf_interior(handle, 4);
	set_fill_pattern(handle);
	do_fill(handle);

	/*
		Wait & Close the virtual workstation.
	*/
	wait(handle);
	v_clsvwk(handle);
	appl_exit();
}

do_fill(handle)
    int handle;
{
    int x     = 300;
    int y     = 70;
    int color = 1;

    /*
        Draw an empty circle 
    */
    v_arc(handle, x, y, 70, 0, 3600);

    /*
        Fill the circle
    */
    v_contourfill(handle, x, y, color);
}

do_fillarea(handle)
{
    int cx      = 100;
    int cy      = 100;
    int count   = 5;
    int points[5][2];

    /*
        Create a diamond.
    */
    pt_set(points[0], cx     , cy - 50);
    pt_set(points[1], cx + 50, cy);
    pt_set(points[2], cx     , cy + 50);
    pt_set(points[3], cx - 50, cy);
    pt_set(points[4], cx     , cy - 50);

    /*
        Now fill the diamond.
    */
    v_fillarea(handle, count, points);
}

set_fill_pattern(handle)
    int handle;
{
    unsigned x;
    unsigned fill_pattern[16];

    /*
        Create checker board fill pattern
    */
    for (x=0; x<16; x+= 2)
        stuffbits(&fill_pattern[x], "0101010101010101");

    for (x=1; x<16; x += 2)
        stuffbits(&fill_pattern[x], "1010101010101010");

    vsf_udpat(handle, fill_pattern, 1);
}
