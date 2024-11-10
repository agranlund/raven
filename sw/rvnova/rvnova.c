/*-------------------------------------------------------------------------------
 * NOVA driver loader
 * (c)2024 Anders Granlund
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
#include <string.h>
#include <mint/osbind.h>
#include "rvnova.h"


int rvnova_loadinf(rvnova_menuinf_t* inf, char* fname)
{
	int32_t ok = Fopen(fname, 0);
	memset((void*)inf, 0, sizeof(rvnova_menuinf_t));
	if (ok >= 0) {
		int16_t fp = (short)ok;
		Fread(fp, sizeof(rvnova_menuinf_t), (void*)inf);
		Fclose(fp);
		return 1;
	}
	return 0;
}


int rvnova_saveinf(rvnova_menuinf_t* inf, char* fname)
{
	/* only write if different from existing data */
	int32_t ok;
	rvnova_menuinf_t olddata;
	if (rvnova_loadinf(&olddata, fname)) {
		if (memcmp((void*)inf, (void*)&olddata, sizeof(rvnova_menuinf_t)) == 0) {
			return 1;
		}
	}

	/* write file */
	ok = Fcreate(fname, 0);
	if (ok >= 0) {
		int16_t fp = (short)ok;
		Fwrite(fp, sizeof(rvnova_menuinf_t), (void*)inf);
		Fclose(fp);
		return 1;
	}
	return 0;
}

void rvnova_freebib(bib_t* bib)
{
	bib->num = 0;
	if (bib->res) {
		free(bib->res);
		bib->res = 0;
	}
}


int rvnova_loadbib(bib_t* bib, char* fname)
{
	int32_t ok = Fopen(fname, 0);
	bib->num = 0;
	bib->res = NULL;
	if (ok >= 0) {
		int16_t fp = (int16_t)ok;
		int32_t fs = Fseek(0, fp, 2);
		Fseek(0, fp, 0);
		bib->res = (bibres_t*)malloc(fs);
		Fread(fp, fs, (void*)bib->res);
		Fclose(fp);
		bib->num = fs / sizeof(bibres_t);
	}
	return bib->num;
}

int rvnova_savebib(bib_t* bib, char* fname)
{
	int32_t ok;
	bib_t oldbib;

	if (rvnova_loadbib(&oldbib, fname)) {
		if (oldbib.num == bib->num) {
			if (memcmp((void*)oldbib.res, (void*)bib->res, bib->num * sizeof(bibres_t)) == 0) {
				rvnova_freebib(&oldbib);
				return 1;
			}
		}
		rvnova_freebib(&oldbib);
	}

	ok = Fcreate(fname, 0);
	if (ok >= 0) {
		int16_t fp = (int16_t)ok;
		int32_t fs = bib->num * sizeof(bibres_t);
		Fwrite(fp, fs, (void*)bib->res);
		Fclose(fp);
		return 1;
	}
	return 0;
}

void rvnova_copybib(bib_t* dst, int dsti, bib_t* src, int srci)
{
	memcpy((void*)&dst->res[dsti], (void*)&src->res[srci], sizeof(bibres_t));
}

int rvnova_findres(bib_t* bib, res_t* res)
{
	int i; int best_i = -1;
	uint16_t diff_w = 0xffff;
	uint16_t diff_h = 0xffff;
	for (i = 0; i < bib->num; i++) {
		uint8_t planes = bib->res[i].planes;
		if ((planes == 16) && (bib->res[i].colors < 64000U)) {
			planes = 15;
		}
		if (planes == res->b) {
			uint16_t bw = bib->res[i].real_x + 1;
			uint16_t bh = bib->res[i].real_y + 1;
			uint16_t dw = (res->w >= bw) ? (res->w - bw) : (bw - res->w);
			uint16_t dh = (res->h >= bh) ? (res->h - bh) : (bh - res->h);
			if ((best_i < 0) || (dw < diff_w) || ((dw == diff_w) && (dh < diff_h))) {
				diff_w = dw;
				diff_h = dh;
				best_i = i;
			}
		}
	}
	if (best_i >= 0) {
		res->i = (uint8_t)best_i;
		return 1;
	}
	return 0;
}

