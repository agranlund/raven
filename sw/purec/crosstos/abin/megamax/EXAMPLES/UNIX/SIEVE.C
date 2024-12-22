/*#define register*/	/* remove comment for no register variables */
#include <osbind.h>

#define true 1
#define false 0
#define size 8190

long *ptr;

gettime()
{
	*ptr = *(long *)0x462;
}

char flags[size+1];
main()
{
	register int i,k;
	register int prime,count,iter;
	long t, t2;

	printf("10 interations\n");
	ptr = &t;
	Supexec(gettime);
	for (iter = 1; iter <= 10; iter++) {
		count = 0;
		for (i = 0; i <= size; i++) 
			flags[i] = true;
		for (i = 0; i <= size; i++) {
			if (flags[i]) {
				prime = i + i + 3;
				for (k = i+prime; k <= size; k+=prime)
					flags[k] = false;
				count++;
			}
		}
	}
	ptr = &t2;
	Supexec(gettime);
	printf("Took %0.2f seconds\n", (double)(t2-t)/(Getrez() == 2 ? 70 : 60));
	puts("Press RETURN to exit");
	Cconin();
}
