#include <linea.h>
#include <osbind.h>

#define CONSOLE 2

#define WHITE   0
#define RED		1
#define GREEN   2
#define BLACK	3

int top    = 15;
int left   = 10;
int bottom = 195;
int right  = 630;

int x, y;
int color;

main()
{
	lineaport *myport;

	myport = a_init();

	myport -> plane0 = BLACK;
	myport -> plane1 = BLACK;

	a_hidemouse();

	drawbox();

	bounce();

	a_showmouse();
}


bounce()
{
	int mx, my, sx ,sy;
	sprite     thesprite;
	spriteback theback;

	mx = left + 10;
	my = top  + 10;
	sx = 1;
	sy = 1;

	makesprite(&thesprite);

	while(!(Bconstat(CONSOLE))) {
		mx += sx;
		my += sy;
		
		if ((mx < left) | (mx > right-16))
			sx *= -1;

		if ((my < top+2) | (my > bottom-16))
			sy *= -1;

		a_putpixel(mx, my, RED);
		a_drawsprite(mx, my, &thesprite, theback); 
		Vsync();
		a_undrawsprite(theback);
	}
	Bconin(CONSOLE);
}


drawbox()
{
	a_line(left,  top,    right, top);
	a_line(right, top,    right, bottom);
	a_line(right, bottom, left,  bottom);
	a_line(left,  bottom, left,  top);
}


makesprite(thesprite)
	sprite *thesprite;
{
	int x;

	thesprite -> x         = 0;
	thesprite -> y         = 0;
	thesprite -> format    = 0;
	thesprite -> forecolor = WHITE;
	thesprite -> backcolor = BLACK;

	for (x=0; x<32; x+=2) {
		stuffbits(&thesprite -> image[x],   "1010101010101010");
		stuffbits(&thesprite -> image[x+1], "0101010101010101");
	}
}
