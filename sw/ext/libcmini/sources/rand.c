#include <stdlib.h>
#include <limits.h>

extern unsigned long _seed;

int rand(void)
{
	_seed = (_seed >> 1) ^ (-(_seed & 1u) & 0x80200003u);
	return (_seed - 1) & RAND_MAX;
}
