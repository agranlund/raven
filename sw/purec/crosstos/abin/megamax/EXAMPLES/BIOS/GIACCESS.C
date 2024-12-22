
#include <osbind.h>

#define WRITE 0x80

/*
	Define register set for each tone.
*/
char tone1[] = { 0x1b, 0x01, 0x1c, 0x01, 0x1d, 0x01, 0x00, 
				 0x38, 0x10, 0x10, 0x10, 0x00, 0x30, 0x03
};

char tone2[] = { 0xa7, 0x00, 0xab, 0x00, 0xa9, 0x00, 0x00,
				 0x38, 0x10, 0x10, 0x10, 0x00, 0x30, 0x03
};

char tone3[] = { 0xd3, 0x00, 0xd4, 0x00, 0xd5, 0x00, 0x00,
				 0x38, 0x10, 0x10, 0x10, 0x00, 0x30, 0x03
};

char tone4[] = { 0xa8, 0x01, 0xa9, 0x01, 0xaa, 0x01, 0x00, 
				 0x38, 0x10, 0x10, 0x10, 0x00, 0x30, 0x03
};

char *song[] = { tone1, tone2, tone3, tone4, tone1 };

main()
{
	int x;
	int reg7;		/*	bits 7 and 6 are used by the OS	*/

	puts("Phone home?");

	/*
		Save bits 7 and 6 of register 7
	*/
	reg7 = Giaccess(0, 7);

	/*
		Play each tone.
	*/
	for(x=0; x<(sizeof(song) / sizeof(char *)); x++) {
		/*
			Play tone.
		*/
		do_tone(song[x], reg7 & 0xc0);

		/*
			Wait for tone to finish.
		*/
		wait60(30);
	}

	Bconin(2);
}


/*
	do_tone -  setup the sound chips registers.
		As the registers change the tone is produced.
*/
do_tone(thetone, mask)
	char *thetone;
	int mask;
{
	int x;

	for(x=0; x<0x0e; x++)
		if ( x == 7 )
			Giaccess((unsigned)thetone[x]|mask, x|WRITE);
		else
			Giaccess((unsigned)thetone[x], x|WRITE);
}


/*
	wait60 - wait for delay 1/60th seconds
*/
wait60(delay)
	int delay;
{
	while(delay--)
		Vsync();
}
