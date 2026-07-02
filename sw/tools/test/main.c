/*-------------------------------------------------------------------------------
 * rvtest
 * (c)2026 Anders Granlund
 *
 * simple harness for quick and dirty test code
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <mint/osbind.h>
#include "raven.h"
#include "sysutil.h"

/* ------------------------------------------------------------------------ */

int test_speaker(int args, char** argv) {
	int enable = atoi(argv[0]);
	printf("speaker %s\n", enable ? "on" : "off");
	raven()->snd_speaker(enable);
	return 0;
}

int test_volume(int args, char** argv) {
	int vol = atoi(argv[0]);
	if (vol < 0) { vol = 0; }
	if (vol > 255) { vol = 255; }
	printf("volume %d\n", vol);
	raven()->snd_volume(vol);
	return 0;
}

/* ------------------------------------------------------------------------ */
typedef struct { int args; const char* name; int(*func)(int args, char** argv); } ftable_t;
const ftable_t ftable[] = {
	{ 1, "snd_spkr", test_speaker 	},
	{ 1, "snd_vol",  test_volume	},
	{ 0, 0, 0 }
};

/* ------------------------------------------------------------------------ */
long supermain(int args, char** argv)
{
    int i;
	if (args > 1) {
		for (i=0; ftable[i].name; i++) {
			if (strcmp(argv[1], ftable[i].name) == 0) {
				if (args > (1 + ftable[i].args)) {
					int result = ftable[i].func(args-2, &argv[2]);
					return (long) result;
				}
			}
		}
		return 0;
	}
	printf("usage: rvtest {test}\n");
	for (i=0; ftable[i].name; i++) {
		printf(" %s (args=%d)\n", ftable[i].name, ftable[i].args);
	}
	return 0;
}

int main(int args, char** argv) {
    return (int)Supmain(args, argv, supermain);
}
