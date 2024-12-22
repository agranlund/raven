
#include <osbind.h>

#define ESC		27
#define AUX		1
#define CONSOLE 2

/*
	The dumb terminal Handler using Bcon's
*/
main()
{
	char c;

	while (c != ESC) {
		/*
			Display characters that come accross the serial port.
		*/
		if (Bconstat(AUX))
			Bconout(CONSOLE, (int)Bconin(AUX) & 0xff);

		/*
			Check for keyboard data
		*/
		if (Bconstat(CONSOLE)) {
			/* 
				Get keyboard data
			*/
			c = Bconin(CONSOLE);

			/*
				Wait for OK to send char to RS-232 
				Note: Bcostat() not Bconstat().
			*/
			while(!Bcostat(AUX))
				;

			/*
				Send character to serial port.
			*/
			Bconout(AUX, c);
		}
	}
}
