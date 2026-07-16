/*-------------------------------------------------------------------------------
 * dsptest
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
#include <mint/falcon.h>
#include "raven.h"
#include "sysutil.h"

static uint8_t tempbuf[1024UL * 128 * 3];
extern void readbuf(uint8_t* buf, uint32_t num);


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

int loadprog(char* filename) {
	uint32_t fsize = 0;
	uint8_t* p = loadfile(filename, &fsize);
	if (p) {
		uint32_t len = fsize / 3;
		printf("Loaded prog %s : %ld words\n", filename, len);
		Dsp_ExecProg(p, len, 0);
		Mfree(p);
		return 1;
	}
	return 0;
}

long test_sram(void)
{
	uint32_t i,j;
	long result = 0;

	printf("load\n");
	loadprog("testram.p56");

	printf("read\n");
	for (i=0; i<62; i++) {
		uint32_t addr = 1024UL * (2 + i);
		readbuf(tempbuf, 1024UL);
		for (j=0; j<1024; j++) {
			uint8_t* ptr = &tempbuf[j * 3];
			uint32_t r = 0x550000UL | ((1024UL * (2 + i)) + j);
			uint32_t v = 0;
			v |= ptr[0]; v <<= 8;
			v |= ptr[1]; v <<= 8;
			v |= ptr[2];
			if (j == 0) {
				printf("y: %06lx : %06lx\n", addr, v);
			}
			if (r != v) {
				printf("** err: %06lx / %06lx\n", v, r);
				result = -1;
				break;
			}
		}
	}

	for (i=0; i<61; i++) {
		uint32_t addr = 1024UL * (2 + i);
		readbuf(tempbuf, 1024UL);
		for (j=0; j<1024; j++) {
			uint8_t* ptr = &tempbuf[j * 3];
			uint32_t r = 0xaa0000UL | ((1024UL * (2 + i)) + j);
			uint32_t v = 0;
			v |= ptr[0]; v <<= 8;
			v |= ptr[1]; v <<= 8;
			v |= ptr[2];
			if (j == 0) {
				printf("x: %06lx : %06lx\n", addr, v);
			}
			if (r != v) {
				printf("** err: %06lx / %06lx\n", v, r);
				result = -2;
				break;
			}
		}
	}
	return result;
}


long supermain(int args, char** argv) {
	int wordsize = Dsp_GetWordSize();
	printf("wordsize = %d\n", wordsize);
	test_sram();
	return 0;
}

int main(int args, char** argv) {
    return (int) Supmain(args, argv, supermain);
}
