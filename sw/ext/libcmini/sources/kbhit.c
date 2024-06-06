/*
 * kbhit.c
 *
 *  Created on: 25.07.2019
 *      Author: og
 */

#include <ext.h>
#include <osbind.h>


int
kbhit()
{
	return Bconstat(DEV_CONSOLE);
}
