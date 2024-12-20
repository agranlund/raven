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
 * but WITHOUT ANY WARRANTY  whthout even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/
#include <tos.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <mint/cookie.h>
#include <mint/osbind.h>

#include "rvnova.h"

static const char* path_root   		= "c:";
static const char* path_vdibib 		= "auto\\sta_vdi.bib";
static const char* path_emubib 		= "auto\\emulator.bib";
static const char* path_inf			= "auto\\xmenu.inf";
static const char* path_nova		= "nova";
static const char* path_xmenu		= "xmenu.prg";
static const char* path_emulator	= "auto\\emulator.prg";
static const char* path_vdi			= "auto\\sta_vdi.prg";


static void screen_off(void) {
#if 1
	char tmp[6];
	sprintf(tmp, "\033c%c", '0'+15);	/* bg color */
	Cconws(tmp);
	Cconws("\033E");
	sprintf(tmp, "\033b%c", '0'+15);	/* fg color */
	Cconws(tmp);
#endif
}

static void screen_restore(void) {
#if 1
	char tmp[6];
	sprintf(tmp, "\033b%c", '0');		/* fg color */
	Cconws("\033E");
	Cconws(tmp);
#endif
}


/*----------------------------------------
	Main
----------------------------------------*/

long supermain()
{
	bib_t bib_emu;
	bib_t bib_vdi;
	rvnova_menuinf_t inf;
	char fname[128];

	/* load settings */
	sprintf(fname, "%s\\%s", path_root, path_inf);
	if (!rvnova_loadinf(&inf, fname)) {
		return 0;
	}

	/* force settings */
	inf.menuinf.guikey = 3;		/* gui disable unless keypress */
	inf.menuinf.maccel = 0;

	/* temp setup */
#if 0
	inf.drv_enable = 1;
	inf.vdi_enable = 1;
	strcpy(&inf.drvpath, "et4kw32i");

	inf.menuinf.output 	= 1;	/* vdi output */
	inf.menuinf.gdos	= 1;
	strcpy(&inf.menuinf.gdosfile, "NVDI");

	inf.drv_res.w = 1024;
	inf.drv_res.h = 768;
	inf.drv_res.b = 1;
	inf.drv_res.i = 0;
	inf.vdi_res.w = 1280;
	inf.vdi_res.h = 720;
	inf.vdi_res.b = 8;
	inf.vdi_res.i = 0;
#endif

	/* early out if disabled */
	if ((inf.drv_enable == 0) || (inf.menuinf.output != 1) || (inf.drvpath[0] == 0)) {
		return 0;
	}

	/* sta_vdi.bib */
	sprintf(fname, "%s\\%s\\%s\\%s", path_root, path_nova, inf.drvpath, path_vdibib);
	rvnova_loadbib(&bib_vdi, fname);
	sprintf(fname, "%s\\%s", path_root, path_vdibib);
	rvnova_savebib(&bib_vdi, fname);
	if (rvnova_findres(&bib_vdi, &inf.vdi_res)) {
		inf.menuinf.resid = inf.vdi_res.i;
	}
	
	/* emulator.bib */
	sprintf(fname, "%s\\%s\\%s\\%s", path_root, path_nova, inf.drvpath, path_emubib);
	rvnova_loadbib(&bib_emu, fname);
	if (rvnova_findres(&bib_vdi, &inf.drv_res) ) {
		rvnova_copybib(&bib_emu, 0, &bib_vdi, inf.drv_res.i);
	}
	sprintf(fname, "%s\\%s", path_root, path_emubib);
	rvnova_savebib(&bib_emu, fname);

	rvnova_freebib(&bib_emu);
	rvnova_freebib(&bib_vdi);

	/* xmenu.inf */
	sprintf(&fname, "%s\\%s", path_root, path_inf);
	if (!rvnova_saveinf(&inf, fname)) {
		return 0;
	}

	/* launch driver */
	if (inf.drv_enable) {
		screen_off();
		sprintf(fname, "%s\\%s\\%s\\%s", path_root, path_nova, inf.drvpath, path_emulator);
		Pexec(0, fname, "", 0L);

		if (inf.vdi_enable) {
			sprintf(fname, "%s\\%s\\%s", path_root, path_nova, path_xmenu);
			Pexec(0, fname, "", 0L);

			screen_restore();

			sprintf(fname, "%s\\%s\\%s\\%s", path_root, path_nova, inf.drvpath, path_vdi);
			Pexec(0, fname, "", 0L);
		} else {
			screen_restore();
		}
	}

	return 1;
}


long main()
{
	if (Supexec(supermain)) {
/*		Ptermres(_PgmSize, 0); */
	}
	return 0;
}

