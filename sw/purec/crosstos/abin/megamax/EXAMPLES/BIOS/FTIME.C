
#include <stdio.h>
#include <osbind.h>


main()
{
	int  fd;
	int  datime[2];
	int	 err;
	char fname[80];

	printf("Enter name of file: "); fflush(stdout);
	gets(fname);

	fd = Fopen(fname, 0);

	if (fd < 0) 
		fatal("Error in opening file.");

	err = Fdatime(datime, fd, 0);

	if (err < 0)
		fatal("Error reading date and time.");

	Fclose(fd);

	showtime(datime[0], datime[1]);

	printf("\nPress RETURN to exit...\n");
	Bconin(2);
}


showtime(mytime, mydate)
	timeinfo mytime;
	dateinfo mydate;
{
	printf("\t\t date \n Day: %d \t Month: %d \t Year: %d\n",
			mydate.part.day,
			mydate.part.month,
			mydate.part.year + 80
	);

	printf("\t\t time \n Hour: %d \t Minute: %d \t Seconds: %d\n",
			mytime.part.hours,
			mytime.part.minutes,
			mytime.part.seconds * 2
	);
}


/*
	fatal - works like printf() except that it waits for a <CR> and then dies.
*/

fatal(args)
	char *args;
{
	_fprintf(stdout, args, &args+1);
	printf("\n");
	puts("Press RETURN to exit...");
	getchar();
	exit(1);
}
