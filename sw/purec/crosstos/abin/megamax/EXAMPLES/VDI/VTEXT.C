/*
  v_gtext()
  v_justified()
  vst_effects()
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

	drawtext(handle);
	justtext(handle);
	laserproc(handle);

	/*
		Wait & Close the virtual workstation.
	*/
	wait(handle);
	v_clsvwk(handle);
	appl_exit();
}

drawtext(handle)
    int handle;
{
    int x = 100;
    int y = 100;
    char *text = "Hello, World ...";

    v_gtext(handle, x, y, text);
}

justtext(handle)
    int handle;
{
    int x = 100;
    int y = 150;
    char *text = "Hello, World";

    v_justified(handle, x, y, text, 150, 0, 1);
}

#define BOLD        0x001
#define PLAIN       0x002
#define ITALICS     0x004
#define UNDERLINE   0x008
#define OUTLINE     0x010
#define SHADOW      0x020

laserproc(grafhandle)
    int grafhandle;
{
    int  dummy, cw, ch;
    char *text = "Laser C";

    /*
        Set text size 25 pts &
        set slant mode       &
        write mode = transparent.
    */
    vst_height(grafhandle, 25, &dummy, &dummy, &cw, &ch);
    vst_effects(grafhandle, OUTLINE | ITALICS | UNDERLINE);
    vswr_mode(grafhandle, 2);

    v_gtext(grafhandle, 40, 80, text);
}
