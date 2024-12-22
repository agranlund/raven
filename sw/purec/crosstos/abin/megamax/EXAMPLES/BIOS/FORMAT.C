
#include <osbind.h>
#include <stdio.h>


/*
	Program to format a floppy disk
*/

#define DRIVE		1
#define SECTORS		9
#define BUFSIZE		SECTORS*1024

char buf[BUFSIZE];

main()
{
	setbuf(stdout, NULL);

	printf("Floppy disk format program v1.0\n\n");

	format();

	create_boot_blocks();

	verify();

	printf("\nFormat Complete.");
	wait();
}


format()
{
	/*
		Variables for formating floppy.
	*/
	int		interleave		 = 1;
	long	filler			 = NULL;
	int		devno			 = DRIVE;
	int		sectors_pertrack = SECTORS;
	int		trackno;
	int		sideno			 = 0;
	long	magic			 = 0x87654321L;
	int		virgin			 = 0xe5e5;


	printf("Place disk to be formatted in drive B:");
	wait();

	puts("Formatting track:");
	for (trackno = 0; trackno < 80; trackno++) {
		printf("[%02d] ", trackno);

		if (!((trackno+1) % 10))
			printf("\n");

		if (Flopfmt(buf, filler, devno, sectors_pertrack, trackno, sideno,
					interleave, magic, virgin))
			printf("\nError on track %02d\n", trackno);
	}
}


verify()
{
	/*
		Variables required for disk verify.
	*/
	long	filler		= NULL;
	int		devno		= DRIVE;
	int		sectno		= 1;
	int		trackno;
	int		sideno		= 0;

	puts("Verifying track:");
	for (trackno = 0; trackno < 80; trackno++) {
		printf("[%02d] ", trackno);

		if (!((trackno+1) % 10))
			printf("\n");

		if (Flopver(buf, filler, devno, sectno, trackno, sideno, SECTORS))
			printf("\nError on track %02d\n", trackno);
	}
}


create_boot_blocks()
{
	/*
		Variables required for disk write.
	*/
	long	filler		= NULL;
	int		devno		= DRIVE;
	int		sectno		= 1;
	int		trackno;
	int		sideno		= 0;
	int		i;

	/*
		Variables for Building Boot Blocks.
	*/
	long	serialno		 = 0x01000000L;
	int		disktype		 = 2;
	int		execflag		 = 0;

	
	printf("\nCreating Boot Blocks.\n\n");

	/*
		Zero out buffer.
	*/
	for (i=0; i<BUFSIZE; i++)
		buf[i] = 0;

	/*
		Write out zero'd buffer to tracks one and two.
	*/
	for(trackno = 0; trackno < 1; trackno++)
		Flopwr(buf, filler, devno, sectno, trackno, sideno, SECTORS);

	/*
		Build prototype boot blocks.
	*/
	Protobt(buf, serialno, disktype, execflag);

	/*
		Write boot blocks to disk.
	*/
	trackno = 0;
	Flopwr(buf, filler, devno, sectno, trackno, sideno, 1);
}


/*
	A routine to keep the number of printf() & getchar()'s to a minimum.
*/
wait()
{
	printf("\nPress RETURN to continue.\n");
	getchar();
}
