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
#include "nova.h"

#define FLG_W32I_INTERLEAVE     (1<<0)

typedef struct {
	uint16_t	w,h;
	uint8_t		b,i;
} res_t;

typedef struct {
	nova_menuinf_t	menuinf;
	uint8_t			drv_enable;
	uint8_t			vdi_enable;
	res_t			drv_res;
	res_t			vdi_res;
	char			drvpath[14];
    uint8_t         flags;
} rvnova_menuinf_t;


int  rvnova_makeinf(rvnova_menuinf_t* inf);
int  rvnova_loadinf(rvnova_menuinf_t* inf, char* fname);
int  rvnova_saveinf(rvnova_menuinf_t* inf, char* fname);

int  rvnova_loadbib(nova_bib_t* bib, char* fname);
int  rvnova_savebib(nova_bib_t* bib, char* fname);
void rvnova_freebib(nova_bib_t* bib);
void rvnova_copybib(nova_bib_t* dst, int dsti, nova_bib_t* src, int srci);
int  rvnova_findres(nova_bib_t* bib, res_t* res);

#endif /* _RVNOVA_H_ */
