
#include <osbind.h>
#include <stdio.h>

main()
{
	appl_init();

	puts("Turn your speaker volume up and press return");
	getchar();

	do_crash();

	puts("Boom!");
	getchar();

	appl_exit();
}


do_crash()
{
	/*
		Sound definition
	*/
	static char crash[] = {
			  0x06, 0x1f,   /* Noise Period				*/
			  0x07, 0x2f,   /* Mixer					*/
			  0x09, 0x10,   /* Channel B volume			*/
			  0x0c, 0x20,   /* Duration Course tune		*/
			  0x0d, 0x00,   /* Envelope Shape			*/
			  0x82, 0x00,   /* Sustain Tone				*/
			  0xff, 0x00	/* End Tone					*/
	};

   Dosound(crash);
}
