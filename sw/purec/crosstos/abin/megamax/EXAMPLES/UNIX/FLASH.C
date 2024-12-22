
#include <osbind.h>

#define SIZE 8000

standard(scrbase)
	long *scrbase;
{
    long *x;
    int i,j;

    for (j=1; j<=50; j++) {
		x = scrbase;
		i = SIZE;

		do  {
			*x = ~*x;
			x++;
		} while (--i);
	}
}


regs(scrbase)
	long *scrbase;
{
    register long *x;
    register int i,j;

    for (j=1; j<=50; j++) {
		x = scrbase;
		i = SIZE;

		do  {
			*x = ~*x;
			x++;
		} while (--i);
	}
}


assembly(scrbase)
	long *scrbase;
{
    register int i,j;

    for (j = 1; j <= 50; j++)
		asm {
			move.l    scrbase(A6), A0
			move.w    #SIZE-1, D0
		lp: 
			not.l     (A0)+
			dbf       D0, lp
		}
}


main()
{
	long *scrbase;

    scrbase = (long *) Logbase();

    puts("Complement the screen 50 times\n");

    puts("Standard C"); 
    standard(scrbase);

    puts("With register variables");
    regs(scrbase);

    puts("In-line assembly");
    assembly(scrbase);
}
