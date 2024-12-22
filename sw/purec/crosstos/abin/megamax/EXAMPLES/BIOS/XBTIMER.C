
#include <osbind.h>
#include <stdio.h>

/*
	xbtimer.c

 	Sample code that demonstrates the use of TIMER A on the 68901 MFP
 	Also demonstrates how to handle the process terminate interrupt.
 
 		The program begins by installing the address of the function `terminate'
	into the exception vector 0x102 (Process terminate exception), saving its 
	old pointer. It then starts up Timer A on the 68901 MFP, configured to 
	interrupt the function `dispatcher' at 48Hz. The main loop continuously
	displays the value of a counter, that the function ``ticker'' increments.  
	If CTRL-C is struck, the `terminate' function is called to handle the
	termination of the program. It stops timer, then restores the original
	process terminate vector and returns (to the default system Interrupt
	Service Routine).

		The timers on the 68901 MFP are controlled by a 2.4576 Mhz crystal.
	Using timer 'A' in the delay mode with a prescale set at 200 (ie. by 
	setting the timer 'A' control register to 7) gives you a 12288 Hz counter
	(2457600/200). Using a count of 256 (ie. by loading the timer 'A' data 
	register with 0) you get an interrupt frequency of 48 Hz (12288/256).  For 
	other values for the control & data registers see the 68901 manual available
	for free by calling Motorola.

		The timer interrupt is handled by the function 'dispatcher'. This 
	function calls a routine to increment the long counter, then clears Bit 5 
	of the ISRA (In Service Register A), and then returns from the exception 
	by doing an RTE.

Note:	In the ST the 68901 always operates in the Software End of Interrupt
	Mode (ie. bit 3 of the Vector Register VR is always set).  In this mode the
	ISR bit of the ISRA, the ISR bit correpsonding to the Timer A interrupt is
	bit 5, is automatically set when an interrupt occurs and the processor
	requests the interrupt vector.  As long as the bit is set, that interrupt
	and any other interrupts of lower priority cannot occur. Once the bit is
	cleared the same interrupt or any lower priority interrupts can once
	again occur. This is why it is important to clear bit 5 of the ISRA before 
	performing the RTE. The address of the ISRA register is 0xfffA0F
*/

#define MyApp	0  /*  my application  		   */
#define Control	7  /*  divide by 200 prescale  */
#define Data	0  /*  Countdown from		   */
				   /* 1 byte, 1, 2, ... 254, 255, 0 = 256 */
#define Off		0

#define MAXITER	100	/* Iterations				*/
#define MAXTICKS 10 /* Maximum timer count-down events	*/

extern  dispatcher();	/* labels in in-line assembly must be declared. */
extern  set_timer();
extern  unset_timer();


long ticks = MAXTICKS;	/* local tick counter.					*/
long oldvector;			/* storage for old terminate vector.	*/


/*
	This routine is called by the interrupt handler to increment the 
	local tick counter.
*/
ticker()
{
	ticks++;
}


main()
{
	int count = 0;

 	puts("Sample code demonstrating the use of TIMER A on the 68901 MFP");
	printf("Iterations                : %d\n", MAXITER);
	printf("Timer events per iteration: %d\n\n", MAXTICKS);

	set_timer();		/* turn on timer */
	/*
		Set the terminate vector so that the user can't leave without
		turning of the timer first.
	*/
	set_terminate();

	/*
		Keep on ticking...
	*/
	while(count < MAXITER) {
		if (ticks == MAXTICKS) {
			printf("count == %d\r", count++);
			fflush(stdout);
			ticks = 0L;
			}
		}

	unset_terminate();
	unset_timer();		/* turn off timer */

	puts("\nPress return...");
	getchar();
}


/*
	My terminate application function.
*/
terminate()
{
	/*
		Clear 68901 timer interrupt
	*/
	unset_timer();

	/*
		Restore the old process terminate vector
	*/
	Setexc(0x0102, oldvector);
}

/*
	Get the old terminate application vector and setup
	the local terminate function.
*/
set_terminate()
{
	long user_stack = Super(0L);

	oldvector = Setexc(0x0102, -1L);
	Setexc(0x0102, terminate);

	Super(user_stack);
}


unset_terminate()
{
	/*
		Restore the old process terminate vector
	*/
	Supexec(Setexc(0x0102, oldvector));
}


/* 
	This is the interrupt dispatcher routine.
*/
asm {
dispatcher:
			jsr		ticker		/* our function								*/

			bclr.b	#5,0xfffa0f /* Tell MFP the interrupt has been serviced	*/
			rte		 			/* return from exception					*/
}


/*
	This function is callled by the main() function to set up the
	application terminate function and the 68901 function timer.
*/
set_timer()
{
	register	char	*globals;


	/*
		Tell the timer chip to call the dispatcher routine for the interrupt.
	*/
	Xbtimer(MyApp, Control, Data, dispatcher);
}


/*
	Turn off the timer and reset the terminate vector.
*/
unset_timer()
{
	/*
		Turn off the application timer.
	*/
	Xbtimer(MyApp, Off, Off, NULL);
}
