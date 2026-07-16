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

int main(int args, char** argv) {

	if (args < 2) {
		printf("dsprun <file.p56>\n");
		return 0;
	}

	if (!loadprog(argv[1])) {
		printf("fail loading %s\n");
		return -1;
	}

	return 0;
}
