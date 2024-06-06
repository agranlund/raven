#include <stdlib.h>
#include <limits.h>

extern unsigned long _seed;

void srand(unsigned int seed)
{
	_seed = seed;
}
