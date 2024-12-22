
#include <osbind.h>

typedef union {
	struct {
		unsigned day	: 5;
		unsigned month	: 4;
		unsigned year	: 7;
	} part;
	unsigned realdate;
} date;

typedef union {
	struct {
		unsigned seconds : 5;
		unsigned minutes : 6;
		unsigned hours	 : 5;
	} part;
	unsigned realtime;
} time;


main()
{
	time	mytime;
	date	mydate;

	mytime.realtime = Tgettime();
	mydate.realdate = Tgetdate();

	showtime(mytime, mydate);

	printf("\nPress RETURN to exit...\n");
	Bconin(2);
}


/*
	This function will display date and time information using the Bios routines
		Tgettime() and Tgetdate().  In order to set the time just set the
		individual members of the structure and call Tset****() accordingly.
		This would go as follows to set the time:

				time.part.hours = 5;
				time.part.minutes = 34;
				time.part.seconds = 15 / 2;

				Tsettime(time.realtime);
*/
showtime(mytime, mydate)
	time mytime;
	date mydate;
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
