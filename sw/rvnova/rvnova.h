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
#ifndef _RVNOVA_H_
#define _RVNOVA_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	uint16_t	w,h;
	uint8_t		b,i;
} res_t;

typedef struct
{
	uint8_t		maccel;
	uint8_t		guikey;
	uint8_t		u02;
	uint8_t		u03;
	uint8_t		resid;
	uint8_t		output;
	uint8_t		u06;
	uint8_t		gdos;
	char		gdosfile[48];
} nova_menuinf_t;

typedef struct
{
	nova_menuinf_t	menuinf;
	uint8_t			drv_enable;
	uint8_t			vdi_enable;
	res_t			drv_res;
	res_t			vdi_res;
	char			drvpath[14];
} rvnova_menuinf_t;

typedef struct
{
	char		name[33];
	char		dummy;
	uint16_t	mode;
	uint16_t	pitch;
	uint16_t	planes;
	uint16_t	colors;
	uint16_t	hcmode;
	uint16_t	max_x, max_y;
	uint16_t	real_x, real_y;
	uint16_t	freq;
	uint8_t		freq2;
	uint8_t		low_res;
	uint8_t		r_3c2;
	uint8_t		r_3d4[25];
	uint8_t		extended[3];
	uint8_t		dummy2;
} bibres_t;

typedef struct
{
	uint16_t	num;
	bibres_t*	res;
} bib_t;


int  rvnova_loadinf(rvnova_menuinf_t* inf, char* fname);
int  rvnova_saveinf(rvnova_menuinf_t* inf, char* fname);

int  rvnova_loadbib(bib_t* bib, char* fname);
int  rvnova_savebib(bib_t* bib, char* fname);
void rvnova_freebib(bib_t* bib);
void rvnova_copybib(bib_t* dst, int dsti, bib_t* src, int srci);
int  rvnova_findres(bib_t* bib, res_t* res);

#endif /* _RVNOVA_H_ */
