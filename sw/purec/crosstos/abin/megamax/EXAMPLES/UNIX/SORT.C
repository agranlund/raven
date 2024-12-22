#include <stdio.h>

int test(a, b)
int *a, *b;
{
    return *a - *b;
}

main()
{
    int x[100], i;

    for (i=0; i<100; i++)       /* Create some random data */
        x[i] = rand();

    qsort(x, 100, sizeof(int), test);

    for (i=0; i<100; i++)       /* Display sorted result */
        printf("%d ", x[i]);

	puts("Press RETURN to continue");
	getchar();
}
