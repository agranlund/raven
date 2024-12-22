/*
	Program to format a floppy disk
*/
#include <osbind.h>
#include <stdio.h>

main()
{
	static char buf[8*1024];
	char *tr[80], *big;
	int i, j, v;
	int failed;	/* 1 if initial read verify failed */
	long l;
	
	puts("The Megamax single sided disk duplication program.");
	puts("By Eric Parker\n\n");

	if ((big = (char *)Malloc(9*512*80L)) == NULL) {
		puts("*** Not enough memory.\nPress RETURN");
		fgetc(stdin);
	}

	for (i=0; i<80; i++)
		tr[i] = big + i*9*512L;

	do {
		puts("Insert SOURCE disk in drive A:");
		puts("Press RETURN when ready.");
		fgetc(stdin);

		puts("Reading SOURCE disk");
		for (i=0; i<80; i++)
			Floprd(tr[i], 0L, 0, 1, i, 0, 9);

		puts("Verifying SOURCE disk");
		failed = 0;
		for (i=0; i<80; i++) {
			Floprd(buf, 0L, 0, 1, i, 0, 9);
			for (j=0; j<9*512; j++)
				if (buf[j] != tr[i][j]) {
					puts(" ****Verify FAILED!");
					failed = 1;
					continue;
				}
		}
	} while(failed);

	v = 1;	/* interleave factor */
	puts("Type ^C to quit when asked to press RETURN");

	while (1) {
		puts("\007Insert DESTINATION disk in drive A:");
		puts("Press RETURN when ready.");
		fgetc(stdin);

		for (i=0; i<80; i++) {
			if (Flopfmt(buf, 0L, 0, 9, i, 0, v, 0x87654321L, 0xe5e5)) {
				puts("\007 ****Error formating track");
				break;
			}

			if (Flopwr(tr[i], 0L, 0, 1, i, 0, 9)) {
				puts("\007 ****Error writing track");
				break;
			}
		}
	}
}
