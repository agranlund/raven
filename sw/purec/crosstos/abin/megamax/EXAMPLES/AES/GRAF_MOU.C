
#include <osbind.h>
#include <gemdefs.h>
#include "linea.h"


main()
{
	mouse themouse;

	appl_init();

	/*
		Hide the mouse;
	*/
	graf_mouse(256, 0L);

	/*
		Change the mouse
	*/
	makemouse(&themouse);

	graf_mouse(0xff, &themouse);

	/*
		Show the new mouse
	*/
	graf_mouse(257, 0L);

	/*
		Wait for keypress to end.
	*/
	while(!(Bconstat(2)))
		;

	appl_exit();
}


makemouse(thesprite)
	sprite *thesprite;
{
	int x;

	thesprite -> x         = 1;
	thesprite -> y         = 1;
	thesprite -> format    = 1;
	thesprite -> forecolor = 0;
	thesprite -> backcolor = 1;

	for (x=0; x<15; x++)
		stuffbits(&thesprite -> image[x],   "1111111111111111");

	stuffbits(&thesprite -> image[16],   "1111111111111111");
	for (x=17; x<31; x++)
		stuffbits(&thesprite -> image[x],   "1000000000000001");
	stuffbits(&thesprite -> image[31],   "1111111111111111");

}
