#include <stdio.h>
#include <osbind.h>

main()
	{
	show_iorec(0);
	getchar();
	}

show_iorec(device)
    int    device;
{
    iorec *therec;

    therec = (iorec *)Iorec(device);

    printf("\nThe device %d is:\n", device);
    printf("buffer      == %lx\n", therec -> ibuf); 
    printf("buffer size == %d\n",  therec -> ibufsiz);  
    printf("head index  == %d\n",  therec -> ibufhd);   
    printf("tail index  == %d\n",  therec -> ibuftl);   
    printf("low mark    == %d\n",  therec -> ibuflow);  
    printf("high mark   == %d\n",  therec -> ibufhigh);
}
