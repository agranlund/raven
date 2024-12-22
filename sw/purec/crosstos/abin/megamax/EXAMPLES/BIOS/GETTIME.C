
#include <stdio.h>
#include <osbind.h>

/*
	This structure is a bit field that represents the different components of
	the date and time words.   A union structure was used so that a long 
	could be used for the assignment from the gettime() function and the
	bit-field structure could be used to easily decode the long word.

	Note:  This data structure was designed to work with Megamax C.  Not all
			compilers allocate bit-fields in the same manner.  rpt

    Note : To set the time just assign the ``part'' fields of the
        structure and then pass Settime() the  real datetime. Ex:

        mytime.part.day  = 10;
        mytime.part.year = 7;

        Settime(mytime.realtime);
*/
typedef union {
	struct {
		unsigned day	: 5;
		unsigned month	: 4;
		unsigned year	: 7;
		unsigned seconds : 5;
		unsigned minutes : 6;
		unsigned hours	 : 5;
	} part;
	long realtime;
} time;


/*
	Example of how to get information  from the Gettime Xbios functions.
*/

main()
{
	time mytime;

	/*
		Get the date and time with the long word of the time data structure.
	*/
	mytime.realtime = Gettime();

	/*
		Send it off to be printed.
	*/
	showtime(mytime);
	puts( "Press return" );
	getchar();
}


/*
	Print the date and time based on the time data structure.
*/
showtime(mytime)
	time mytime;
{
	/*
		Print the date.

		Note:  The years are represented from 1980.
	*/
	printf("\t\t date \n Day: %d \t Month: %d \t Year: %d\n",
			mytime.part.day,
			mytime.part.month,
			mytime.part.year + 80
	);

	/*
		Print the time.

		Note:  The seconds are represented in multiples of 2.
	*/
	printf("\t\t time \n Hour: %d \t Minute: %d \t Seconds: %d\n",
			mytime.part.hours,
			mytime.part.minutes,
			mytime.part.seconds * 2
	);
}
