/*
  v_arc()
  v_bar()
  v_circle()
  v_ellarc()
  v_ellipse()
  v_ellpie()
  v_pieslice()
  v_pline()
  v_pmarker()
  v_rbox()
  v_rfbox()
  vr_recfl()
  vsl_udsty()
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
	draw_arcs(handle);
	draw_bars(handle);
	draw_circles(handle);
	draw_ellarc(handle);
	draw_ellipse(handle);
	draw_ellpie(handle);
	draw_pieslice(handle);
	set_line_pattern(handle);
	vsl_type(handle, 7);
	do_polyline(handle);
	do_polymarker(handle);

	wait(handle);				/* Clear screen and continue */
	v_clrwk(handle);

	draw_boxes(handle);
	draw_filled_bars(handle);
	draw_recfl(handle);

	/*
		Wait & Close the virtual workstation.
	*/
	wait(handle);
	v_clsvwk(handle);
	appl_exit();
}

/*
    draw_arcs - show how to use the v_arc() vdi function.  
        The handle that is passed as a parameter is the 
        vdi workstation handle.  For further information 
        refer to the vdi function v_opnvwk().
*/
draw_arcs(handle)
    int handle;
{
    int px          = 60;
    int py          = 70;
    int start_angle = 0;
    int end_angle   = 3600 - 900;
    int radius;

    for (radius = 10; radius < 40; radius += 10) {
        /*
            draw an arc.
        */
        v_arc(handle, px, py, radius, start_angle, end_angle);

        /*
            Make the arc larger while moving the start angle.
        */
        end_angle   += 300;
        start_angle += 300;
    }
}

/*
    draw_bars  -  An example of how to use the v_rbox() function to
        draw hollow rounded edge boxes.
*/
draw_bars(handle)
    int handle;
{
    int rect[4];
    int px = 200, py = 100;
    int x       , y  = 90;

    for (x=0; x < 100; x += 25, px += 25, y -= 10) {
        rect_set(rect, px, py, px+20, py-y);
        v_rbox(handle, rect);
    }
}

/*
    draw_circles  -  An example of how to use the v_circle() 
        function to draw circles.  In this case a circle 
        within a circle, ...  The parameter handle is the 
        vdi workstation handle that is returned from the 
        function v_opnvwk().
*/
draw_circles(handle)
    int handle;
{
    int radius;
    int px = 150;
    int py = 70;

    for (radius = 10; radius < 40; radius += 10)
        v_circle(handle, px, py, radius);
}

/*
    draw_ellarc  -  An example of how to use the v_ellarc() 
        function to draw elliptical hollow arcs.

    Note: circular drawing functions use tenth's of degress 
        for angles.
*/
draw_ellarc(handle)
    int handle;
{
    int x           = 50;
    int y           = 130;
    int xradius     = 10;
    int yradius     = 30;
    int start_angle = 0;
    int end_angle   = 3600;

    /*
        Draw the elliptical arc.
    */
    v_ellarc(handle, x, y, xradius, yradius, start_angle, end_angle);
}

/*
    draw_ellipse - An example of how to use the function v_ellipse().
        This example will draw a solid ellipse at the point x, y.
*/
draw_ellipse(handle)
    int handle;
{
    int x       = 180;
    int y       = 130;
    int xradius = 40;
    int yradius = 10;

    v_ellipse(handle, x, y, xradius, yradius);
}

/*
    draw_ellpie  -  An example of how to use the function v_ellpie() to 
        draw and elliptical pie slice.  This function will use the
        current fill attributes when drawing the slice of pie.
*/
draw_ellpie(handle)
    int handle;
{
    int x           = 260;
    int y           = 130;
    int xradius     = 30;
    int yradius     = 10;
    int start_angle = 0;
    int end_angle   = 1200;

    v_ellpie(handle, x, y, xradius, yradius, start_angle, end_angle);
}

/*
    draw_pieslice  -  This is an example of how to use the vdi
        function v_pieslice().  
*/
draw_pieslice(handle)
    int handle;
{
    int x           = 320;
    int y           = 130;
    int radius      = 30;
    int start_angle = 0;
    int end_angle   = 1200;

    v_pieslice(handle, x, y, radius, start_angle, end_angle);
}

do_polyline(handle)
{
    int cx      = 400;
    int cy      = 70;
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
        Now draw the diamond.
    */
    v_pline(handle, count, points);
}

/*
    do_polymarker  -  This function draws a diamond with a marker at
        each of the points of the diamond.
*/
do_polymarker(handle)
{
    int cx      = 400;
    int cy      = 130;
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
        Now draw the diamond.
    */
	vsm_type(handle, 3);		/* Asterisks */
	vsm_height(handle, 30);		/* Make that BIG asterisks */
    v_pmarker(handle, count, points);
}

/*
    draw_rbox  -  An example of how to use the v_rbox() function to
        draw hollow rounded edge boxes.
*/
draw_boxes(handle)
    int handle;
{
    int rect[4];
    int px = 100, py = 100;
    int x       , y  = 90;

    for (x=0; x < 100; x += 25, px += 25, y -= 10) {
        rect_set(rect, px, py, px+20, py-y);
        v_rbox(handle, rect);
    }
}

/*
    draw_filled_bars  -  An example of how to use the v_rfbox() 
	    function to draw filled rounded edge boxes.
*/
draw_filled_bars(handle)
    int handle;
{
    int rect[4];
    int px = 200, py = 100;
    int x       , y  = 90;

    for (x=0; x < 100; x += 25, px += 25, y -= 10) {
        rect_set(rect, px, py, px+20, py-y);
        v_rfbox(handle, rect);
    }
}

/*
    draw_recfl  -  An example of how to use the vr_recfl() function to
        draw filled rectangles.
*/
draw_recfl(handle)
    int handle;
{
    int rect[4];
    int px = 300, py = 100;
    int x       , y  = 90;

    for (x=0; x < 100; x += 25, px += 25, y -= 10) {
        rect_set(rect, px, py, px+20, py-y);
        vr_recfl(handle, rect);
    }
}

set_line_pattern(handle)
    int handle;
{
    int pattern;

    /*
        Define the line pattern.  (16 bits wide)
    */  
    stuffbits(&pattern, "0101010101010101"); 

    /*
        Set the pattern to the user defined line drawing pattern.
    */
    vsl_udsty(handle, pattern);
}
