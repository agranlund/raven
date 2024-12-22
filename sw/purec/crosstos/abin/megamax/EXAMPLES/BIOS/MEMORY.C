
#include <osbind.h>
#include <stdio.h>

main()
{
	show_freemem();
	puts("Press return..." );
	getchar();
}

show_freemem()
{
	mpb   my_mpb;
	md   *free,   *used;
	long  freemem = 0;
	long  usedmem = 0;

	/*
		Do supervisor mode.
	*/
	long        save_ssp = Super(0L);

	/*
		Get the memory parameter block
	*/
	Getmpb(&my_mpb);

	/*
		Let's count free memory chunks
	*/
	for (free = my_mpb.mp_mfl; free; free = free -> m_link)
		freemem += free -> m_length;

	/*
		How much have we used?
	*/
	for (used = my_mpb.mp_mal; used; used = used -> m_link)
		usedmem += used -> m_length;

	/*
		Restore to user mode.
	*/
	Super(save_ssp);

	/*
		Print the compiled statistics.
	*/
	printf("Free memory: %ld\n", freemem);
	printf("Used memory: %ld\n", usedmem);
}
