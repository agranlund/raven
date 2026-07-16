/*-------------------------------------------------------------------------------
 * dsp xbios testing
 * (c)2026 Anders Granlund
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
#include <mint/falcon.h>
#include "raven.h"
#include "sysutil.h"

extern void InstallTrap14(void);
extern void readbuf(uint8_t* buf, uint32_t num);


int32_t xbc_dsp_lodtobin(char* filename, uint8_t* buffer)
{
	(void)filename;
	(void)buffer;
	return 0;
}

uint8_t* loadfile(char* filename, uint32_t* size) {
	FILE* f = fopen(filename, "rb");
	if (f) {
		uint8_t* p = 0;
		uint32_t s = 0;
		fseek(f, 0, SEEK_END);
		s = ftell(f);
		fseek(f, 0, SEEK_SET);
		if (s > 0) {
			p = (uint8_t*)Mxalloc(s, 0);
			if (p) {
				fread(p, s, 1, f);
				fclose(f);
				*size = s;
				return p;
			}
		}
		fclose(f);
	}
	return 0;
}

void loadboot(char* filename) {
	uint32_t fsize = 0;
	uint8_t* p = loadfile(filename, &fsize);
	if (p) {
		uint32_t len = ((((uint32_t)p[6]) << 16) | (((uint32_t)p[7]) << 8) | (((uint32_t)p[8]) << 0));
		printf("Loaded %s : %ld words\n", filename, len);
		Dsp_ExecBoot(&p[9], len, 0);
		Mfree(p);
	}
}

long supermain(int args, char** argv) {
	(void)args; (void)argv;
	InstallTrap14();
	return 0;
}

int main(int args, char** argv) {
	printf("dsp xbios\n");
    Supmain(args, argv, supermain);
	loadboot("boot.p56");
	Ptermres(_PgmSize + 1024, 0);
	return 0;
}
