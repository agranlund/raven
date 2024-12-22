
#include <osbind.h>

#define ESC 27

main()
{
	char c;

	while (c != ESC) {
		/*
			Display characters that come accross the serial port.
		*/
		if (Cauxis())
			Cconout((int)Cauxin() & 0x7f);

		/*
			Check for keyboard data
		*/
		if (Cconis()) {
			/* 
				Get keyboard data
			*/
			c = Cconin();

			/*
				Wait for OK to send char to RS-232 
			*/
			while(!Cauxos())
				;

			/*
				Send character to serial port.
			*/
			Cauxout(c);
		}
	}
}
