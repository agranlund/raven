
#include <osbind.h>
#include <stdio.h>

/*
	DTA	: Disk Transfer Address.  A buffer where directory information
		  is stored.
*/


main()
{
	ls("*.*");

	printf("Press return..." );
	getchar();
}


/*
	ls - list a disk directory using the path specification `pathspec'.
*/
ls(pathspec)
	char *pathspec;
{
	long	olddta;
	int		err;
	struct {
		char	reserved[21];
		char	fattr;
		int		ftime;
		int		fdate;
		long	fsize;
		char	fname[14];
	} newdta;

	olddta = Fgetdta();
	Fsetdta(&newdta);

	printf(" File               Size   Date      Time      Attributes\n");

	err = Fsfirst(pathspec, 0x003f);	/* find all files	*/
	while(!err) {
		printf("%-14.14s  ", newdta.fname );
		printf("%8ld  ", newdta.fsize );

		ls_date( newdta.ftime, newdta.fdate );

		printf("  0x%02x\n", newdta.fattr);
		err = Fsnext();	/* find next file	*/
	}

	Fsetdta(olddta);
}

/*
	Print time and date in human-readable form
*/
ls_date( date, time )
	int date, time;
{
	int	month = (date>>5)  & 0xf;
	int	day   = (date)	   & 0x1f;
	int	year  = ((date>>9) & 0x7f) + 80;
	int	hours = (time>>11) & 0x1f;
	int	min   = (time>>5)  & 0x3f;
	int	sec	  = ((time)	   & 0x1f) * 2;

	if (hours == 0)
		hours	= 12;

	printf( " %02d-%02d-%02d", month, day, year );
	printf( "  %02d:%02d:%02d", hours, min, sec );
}
