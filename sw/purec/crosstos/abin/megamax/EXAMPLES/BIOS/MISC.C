
#include <osbind.h>

main()
{
	pticks();
	pversion();

	printf("\nPress RETURN to continue ...\n");
	Bconin(2);
}

pticks()
{
	long old_stack;
	
	old_stack = Super(0L);

	printf("Now in supervisor mode...\n");
	printf("\tThe number of ticks since power up is %ld\n", *(long *)0x4ba);

	Super(old_stack);

	printf("Now in user mode...\n");
}


pversion()
{
	printf("\nThe version of GEM is %04x\n", Sversion());
}
