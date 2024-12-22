
#include <stdio.h>
#include <osbind.h>

/*
	Example showing the use of Vsync(), Physbase(), Setscreen().
*/
main() 
{
	register char *vis_screen, *back_screen, *temp;
	int rez;
	int count = 10;

	/* 
		Allocate memory for second screen.
	*/
	back_screen = malloc(32768 + 256);

	/*
		The screen address must be on a 256 byte boundary.
	*/
	if ((long) back_screen & 0xff)
		back_screen = back_screen + (0x100 - (long)back_screen & 0xff);

	/* 
		Get visible screen address.
	*/
	vis_screen	= (char *)Physbase();

	/*
		Get the current resolution
	*/
	rez	= Getrez();

	/*
		Reset VT52 cursor to top-left of screen
	*/
	printf( "\033Y%c%c", 0 + ' ', 0 + ' ' );
	fflush( stdout );

	puts("Example showing the use of Vsync(), Physbase(), Setscreen()");
	puts("Press return to start the pageflip...");
	getchar();

	/*
		Wait until the vertical blank interrupt and then swap the screens.

		The physical screen address is the screen image that is displayed. 
		The logical screen address is the back screen where everything is 
		drawn.
	*/

	while ( count-- ) {
		Setscreen(back_screen, vis_screen, -1);

		/*
			do your thing.
		*/
		draw(back_screen,count);

		/*
			swap screens.
		*/
		temp		= vis_screen;
		vis_screen	= back_screen;
		back_screen	= temp;
 	}

	Setscreen(vis_screen, vis_screen, -1);
	puts("Press return...");
	getchar();
}


draw(back,count)
	char *back;
{
	int i	= 60;

	while(i--)
		Vsync();
	/*
		Draw and undraw the animation here in the background screen.
	*/
	if ( count & 1 )
		puts( "This is the first screen" );
	else
		puts( "This is the second screen" );
}
