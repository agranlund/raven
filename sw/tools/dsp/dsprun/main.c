/*-------------------------------------------------------------------------------
 * dsp test prog
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

uint8_t* loadfile(char* filename, uint32_t* size) {
	FILE* f = fopen(filename, "rb");
	if (f) {
		uint8_t* p = 0;
		uint32_t s = 0;
		fseek(f, 0, SEEK_END);
		s = ftell(f);
		fseek(f, 0, SEEK_SET);
		if (s > 0) {
			p = (uint8_t*)Mxalloc(s+3, 0);
			if (p) {
				fread(p, s, 1, f);
				fclose(f);
				*size = s;
				p[s+0] = 0;
				p[s+1] = 0;
				p[s+2] = 3;
				return p;
			}
		}
		fclose(f);
	}
	return 0;
}

static uint32_t get32(uint8_t* p) { return ((((uint32_t)p[0]) << 16) | (((uint32_t)p[1])<<8) | (((uint32_t)p[2])<<0)); }

void infoprog(uint8_t* p)
{
#if 1	
	while (1)
	{
		char type;
		uint32_t hdr_space = get32(p+0);
		uint32_t hdr_offset = get32(p+3);
		uint32_t hdr_size = get32(p+6);
		/*printf("%06lx %06lx %06lx\n", hdr_space, hdr_offset, hdr_size);*/
		if (hdr_space == 0) { type = 'P'; }
		else if (hdr_space == 1) { type = 'X'; }
		else if (hdr_space == 2) { type = 'Y'; }
		else { break; }
		printf("%c : %06lx : %06lx\n", type, hdr_offset, hdr_size);
		p += ((hdr_size + 3) * 3);
	}
#endif	
}

int loadprog(char* filename) {
	uint32_t fsize = 0;
	uint8_t* p = loadfile(filename, &fsize);
	if (p) {
		uint32_t len = fsize / 3;
		printf("Loaded prog %s : %ld words\n", filename, len);
		infoprog(p);
		Dsp_ExecProg(p, len, 0);
		Mfree(p);
		return 1;
	}
	return 0;
}

long supermain(int args, char** argv) {

	if (args < 2) {
		printf("dsprun <file.p56>\n");
		return 0;
	}

	/* shouldn't be hardcoded here but for now it is */
	/* in case a test program wants to route YM sound through the DSP */
	raven()->snd_volume(0);

	if (!loadprog(argv[1])) {
		printf("fail loading %s\n");
		return -1;
	}

	return 0;
}

int main(int args, char** argv) {
    return (int) Supmain(args, argv, supermain);
}
