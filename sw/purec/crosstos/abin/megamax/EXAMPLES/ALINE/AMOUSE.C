#include "linea.h"
#include <osbind.h>

#define CONSOLE 2

int top    = 15;
int left   = 10;
int bottom = 195;
int right  = 630;

int x,y;
int color;

main()
{
	lineaport *myport;
	mouse      themouse;

	myport = a_init();

	a_hidemouse();

	drawbox();

	makemouse(&themouse);

	a_transformmouse(&themouse);

	a_showmouse();

	while(!(Bconstat(CONSOLE)))
		;
}


drawbox()
{
	a_line(left,  top,    right, top);
	a_line(right, top,    right, bottom);
	a_line(right, bottom, left,  bottom);
	a_line(left,  bottom, left,  top);
}


makemouse(thesprite)
	sprite *thesprite;
{
	int x;

	thesprite -> x         = 1;
	thesprite -> y         = 1;
	thesprite -> format    = 1;
	thesprite -> forecolor = 2;
	thesprite -> backcolor = 3;

	stuffbits(&thesprite -> image[0],   "0000000000000000");
	for (x=1; x<15; x++)
		stuffbits(&thesprite -> image[x],   "0111111111111110");
	stuffbits(&thesprite -> image[15],   "0000000000000000");

	stuffbits(&thesprite -> image[16],   "1111111111111111");
	for (x=17; x<32; x++)
		stuffbits(&thesprite -> image[x],   "1000000000000001");
	stuffbits(&thesprite -> image[32],   "1111111111111111");
}
