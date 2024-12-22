#include <stdio.h>
#include <osbind.h>

#define NTIMES 10
#define NUMBER 24

long *ptr;

gettime()
{
	*ptr = *(long *)0x462;
}

main()
{
    int i;
    long t, t2;
    unsigned value, fib();

    printf("%d iterations: \n", NTIMES);

	ptr = &t;
	Supexec(gettime);
    for (i = 1; i <= NTIMES; i++)
	value = fib(NUMBER);
	ptr = &t2;
    Supexec(gettime);

    printf("fibonacci(%d) = %u.\n", NUMBER, value);
	printf("Took %0.2f seconds\n", (float)(t2-t)/(Getrez() == 2 ? 70 : 60));

	puts("Press RETURN to continue");
	getchar();
}

unsigned fib(x)
int x;
{
    if (x > 2)
	return (fib(x - 1) + fib(x - 2));
    else
	return (1);
}
