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

extern void InstallTrap14(void);


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

	printf("%02x %02x %02x\n", p[0], p[1], p[2]);
	printf("%02x %02x %02x\n", p[3], p[4], p[5]);
	printf("%02x %02x %02x\n", p[6], p[7], p[8]);

	if (p) {
		uint32_t len = ((((uint32_t)p[6]) << 16) | (((uint32_t)p[7]) << 8) | (((uint32_t)p[8]) << 0));
		printf("boot %ld words\n", len);
		printf("hstat = %02x, hf2 = %02x, hf3 = %02x\n", (uint8_t)Dsp_HStat(), (uint8_t)Dsp_Hf2(), (uint8_t)Dsp_Hf3());
		Dsp_ExecBoot(&p[9], len, 0);
		printf("hstat = %02x, hf2 = %02x, hf3 = %02x\n", (uint8_t)Dsp_HStat(), (uint8_t)Dsp_Hf2(), (uint8_t)Dsp_Hf3());
		return;
	}

	printf("fail load %s\n", filename);
}

static uint8_t tempbuf[1024UL * 128 * 3];

void loadprog(char* filename) {
	uint32_t fsize = 0;
	uint8_t* p = loadfile(filename, &fsize);
	if (p) {
		uint32_t len = fsize / 3;
		printf("prog %ld words\n", len);
		printf("hstat = %02x, hf2 = %02x, hf3 = %02x\n", (uint8_t)Dsp_HStat(), (uint8_t)Dsp_Hf2(), (uint8_t)Dsp_Hf3());
		Dsp_ExecProg(p, len, 0);
		printf("hstat = %02x, hf2 = %02x, hf3 = %02x\n", (uint8_t)Dsp_HStat(), (uint8_t)Dsp_Hf2(), (uint8_t)Dsp_Hf3());
		return;
	}
	printf("fail load %s\n", filename);
}

	extern void readbuf(uint8_t* buf, uint32_t num);

long supermain(int args, char** argv) {
	int test;

	/*raven()->snd_speaker(1);*/
	raven()->snd_volume(0);

	InstallTrap14();

	test = Dsp_GetWordSize();
	printf("wordsize = %d\n", test);

#if 0
	{
		uint32_t i,j;
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
					break;
				}
			}
		}
	}
#elif 1
	{
		raven()->snd_volume(0);
		raven()->snd_speaker(1);
		loadboot("test3.p56");
		#if 0
		while(1) {
			int32_t i;
			uint32_t v = 0;
			uint8_t* ptr = tempbuf;
			readbuf(tempbuf, 3);
			v |= ptr[0]; v <<= 8;
			v |= ptr[1]; v <<= 8;
			v |= ptr[2];
			for (i=23; i>=0; i--) {
				if (v & (1UL << i)) { Cconout('#'); }
				else { Cconout('.'); }
			}
			Cconout('\r');
/*			printf("%06lx\n", v);*/
		}
		#endif
	}

#elif 0
	loadboot("boot.p56");
#else
	loadprog("test.p56");
#endif	
	return 0;
}

int main(int args, char** argv) {
    Supmain(args, argv, supermain);
	Ptermres(_PgmSize + 1024, 0);
	return 0;
}
