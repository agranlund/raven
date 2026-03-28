/*
 * sleep.c
 *
 *  Created on: 25.07.2019
 *      Author: og
 */

#include <unistd.h>
#include <time.h>


unsigned int
sleep(unsigned int seconds)
{
	clock_t cycles = seconds * CLOCKS_PER_SEC;
	clock_t start  = clock();

	do {} while (clock() - start < cycles);
	return 0;
}
