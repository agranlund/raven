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

extern void xcb_create(void);

static const char* path_root   		= "c:";
static const char* path_vdibib 		= "auto\\sta_vdi.bib";
static const char* path_emubib 		= "auto\\emulator.bib";
static const char* path_inf			= "auto\\xmenu.inf";
static const char* path_nova		= "nova";
static const char* path_xmenu		= "xmenu.prg";
static const char* path_emulator	= "auto\\emulator.prg";
static const char* path_vdi			= "auto\\sta_vdi.prg";


static void screen_off(void) {
#if 0
	char tmp[6];
	sprintf(tmp, "\033c%c", '0'+15);	/* bg color */
	(void)Cconws(tmp);
	(void)Cconws("\033E");              /* clear screen */
	sprintf(tmp, "\033b%c", '0'+15);	/* fg color */
	(void)Cconws(tmp);
#endif
}

static void screen_restore(void) {
#if 0
	char tmp[6];
	sprintf(tmp, "\033b%c", '0');		/* fg color */
	(void)Cconws("\033E");              /* clear screen */
	(void)Cconws(tmp);
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
	sprintf(fname, "%s\\%s", path_root, path_inf);
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

    /* Mach32 cookie hackery */
#if 0
    if (strncmp(inf.drvpath, "MACH", 4) == 0) {
        xcb_create();
    }
#endif

	return 1;
}


int main()
{
	if (Supexec(supermain)) {
/*		Ptermres(_PgmSize, 0); */
	}
	return 0;
}

