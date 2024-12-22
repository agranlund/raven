
#include <stdio.h>
#include <osbind.h>

extern mykbdinter(), globals(), init_key(), restore_key();

long oldkbdvec;


main()
{
	int x;

	/*
		Set up the interupt key handler.  

		Note:  Must be in supervisor mode.
	*/
	Supexec(init_key);

	/*
		Type in 10 characters and watch how the interrupt affects the keys
		pressed.
	*/
	puts("Type in 10 characters.");

	for (x=0; x<10; x++)
		Bconout(2, (int)Bconin(2)&127);

	/*
		Restore the previous state of the keyboard interrupt handler.
	*/
	Supexec(restore_key);
}


asm {
	mykbdinter:
			movem.l	A0/D0, -(A7)

			/*
				In order for the stack to remain set up properly it is necessary
				to jump to the old keyboard vector so that it can perform it's
				own chores.  However, we push our own return address to ensure
				that we have control returned back to us.
			*/
										;Do vector to kbd interupt.
			pea		retrn(PC)			;place to come back to.
			move.w	SR, -(A7)			;old kbd expects status register.
			move.l	oldkbdvec, A0		;go to the normal vector.
			jmp		(A0)
	retrn:
			move.b	0xfffffc02, D0		;D0 is keycode from kbd.

			/***********

				Here is where you can have fun with the key pressed.

				Note that the key press has already been handled
				by the old keyboard vector.

			***********/

			movem.l	(A7)+, A0/D0
			rte
}


/*
	init_key  - Set up our keyboard interrupt handler, making sure to save the
				old one for restoration later.
*/
init_key()
{
	asm {
			move.l	0x118, oldkbdvec		/* save original kbd vector. */
	}
	*((long *) 0x118) = (long) mykbdinter;
}


/*
	restore_key  -  restore the old keyboard interrupt handler.
*/
restore_key()
{
	*( (long *) 0x118) = oldkbdvec;
}
