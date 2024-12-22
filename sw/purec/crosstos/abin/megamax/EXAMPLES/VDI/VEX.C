/*
  vex_butv()
  vex_curv()
  vex_motv()
  vex_timv()
*/

#include <gemdefs.h>
#include <linea.h>

int contrl[12];
int intin[256],  ptsin[256];
int intout[256], ptsout[256];

int     leftbutton, rightbutton;
unsigned mousex, mousey;
long    tickcount;
long    oldmouse, old_mouse_draw, old_mousexy, old_timer;

main()
{
	MFDB	source;
	int		handle;
	
	char 	outline[100];

	/*
		Start up ROM.
	*/
	appl_init();
	handle = open_workstation(&source);

	a_init();

	/*
	    Exchange the vectors.
	*/
	set_mymouse(handle);
	set_mouse_draw(handle);
	set_mousexy(handle);
	set_timer(handle);

	while (!rightbutton) { 		/* Wait for right button */
		sprintf(outline, "x=%u y=%u, left button=%d, tickcount=%ld", 
			mousex, mousey, leftbutton, tickcount);
		v_gtext(handle, 10, 100, outline);
	}

	/*
	    Restore the vectors with the original values
	*/
	restore_mouse(handle);
	restore_mouse_draw(handle);
	restore_mousexy(handle);
	restore_timer(handle);

	/*
		Wait & Close the virtual workstation.
	*/
	wait(handle);
	v_clsvwk(handle);
	appl_exit();
}

set_mymouse(handle)
    int handle;
{
    extern mymouse();

    vex_butv(handle, mymouse, &oldmouse);
}

restore_mouse(handle)
	int handle;
{
	long dummy;

	vex_butv(handle, oldmouse, &dummy);
}

mymouse()
{
    unsigned buttonstate;

    /*
        Save registers used by compiler and 
        move button state into local var.
    */
    asm {
            movem.l A0-A1/D1-D2, -(A7)
            move    D0, buttonstate(A6)
    }

    /*  
        Handle the button event
    */
    leftbutton = buttonstate & 1;
	rightbutton = buttonstate & 2;

    /*
        Restore the registers used and put the
        new button state into D0.
    */
    asm {
            movem.l (A7)+, A0-A1/D1-D2
            move    buttonstate(A6), D0
    }
}


set_mouse_draw(handle)
    int handle;
{
    extern my_mouse_draw();

    vex_curv(handle, my_mouse_draw, &old_mouse_draw);
}

restore_mouse_draw(handle)
	int handle;
{
	long dummy;

	vex_curv(handle, old_mouse_draw, &dummy);
}

my_mouse_draw()
{
	unsigned 	mousex, mousey;

    /*
        Save registers used by compiler and move mouse position
        into local variables.
    */
    asm {
            movem.l A0-A1/D0-D2, -(A7)
            move    D0, mousex(A6)
            move    D1, mousey(A6)
    }

    /*  
        Draw the mouse cursor.
    */
	a_fillrect(mousex, mousey, mousex+16, mousey+16);
	
    /*
        Restore the registers used.
    */
    asm {
            movem.l (A7)+, A0-A1/D0-D2
    }
}


set_mousexy(handle)
    int handle;
{
    extern mousexy();

    vex_motv(handle, mousexy, &old_mousexy);
}

restore_mousexy(handle)
	int handle;
{
	long dummy;

    vex_motv(handle, old_mousexy, &dummy);
}

mousexy()
{
    /*
        Save registers used in interrupt function and set up local
        variables to use in function.
    */
    asm {
            movem.l A0-A1/D0-D2, -(A7)
            move    D0, mousex
            move    D1, mousey
    }

    /*
        Work with the new (x,y) position of the mouse.
    */
	if (mousex > 300)
		mousex = 300;
	if (mousey > 150)
		mousey = 150;

    /*
        Restore registers changed during interrupt and reset D0 & D1
        to contain the modified mouse (x, y) coordinates.
    */
    asm {
            movem.l (A7)+, A0-A1/D0-D2
            move    mousex, D0
            move    mousey, D1
    }
}


set_timer(handle)
    int handle;
{
    extern mytimer();
    int    mils_per_tick;

    vex_timv(handle, mytimer, &old_timer, &mils_per_tick);
}

restore_timer(handle)
	int handle;
{
	long dummy;

	vex_timv(handle, old_timer, &dummy, &dummy);
}

mytimer()
{
    /*
        Preserve register states
    */
    asm {
            movem.l A0-A1/D0-D2, -(A7)
    }

    /*
        Handle the tick event.
    */  
    tickcount++;

    /*
        Restore register states.
    */
    asm {
            movem.l (A7)+, A0-A1/D0-D2
    }   
}
