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
#include "vga.h"

uint32_t vga_iobase = 0x81000000UL;
bool w32i_EnableInterleaveMode(void);


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

static void screen_clear(void) {
	(void)Cconws("\033E");              /* clear screen */
}


/*----------------------------------------
	Main
----------------------------------------*/

long supermain()
{
	nova_bib_t bib_emu;
	nova_bib_t bib_vdi;
	rvnova_menuinf_t inf;
	char fname[128];

	/* load settings */
	sprintf(fname, "%s\\%s", path_root, path_inf);
	if (!rvnova_loadinf(&inf, fname)) {
        rvnova_makeinf(&inf);
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

    /* Card specific hackery after the driver has loaded */

    /* ET4000/W32i interleaved memory mode */
    if (strcmp(inf.drvpath, "ET4000.W32") == 0) {
        if (inf.flags & FLG_W32I_INTERLEAVE) {
            screen_clear();
            w32i_EnableInterleaveMode();
            screen_clear();
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

unsigned long _StkSize = 4096;
int main()
{
	if (Supexec(supermain)) {
/*		Ptermres(_PgmSize, 0); */
	}
	return 0;
}



bool w32i_EnableInterleaveMode(void) {

    /* todo: it would be good if this code first identified that we actually
       have an ET4000/W32i card.
    */

    uint16_t port;
    uint8_t r32, r37;

    /*
    NOTE: The "KEY" must be set in order to write CRTC indices above 18, except indices 33 and
    35, (CRTC 35 is protected by bit 7 of CRTC 11). See Section 5.1.2, Input Status Register Zero for
    definition of "KEY
    */

    /* To set the KEY: write 03 to hercules compatibility registers */
    vga_WritePort(0x3bf, 3);
    /* then set bits 7 and 5 of mode control register to 1,1, e.g.A0 (other bits are don't care) */
    port = (vga_ReadPort(0x3cc) & 1) ? 0x3d8 : 0x3b8;
    vga_WritePort(port, 0xa0);

    /* now enable interleaved memory mode */
    r32 = vga_ReadReg(VGA_REG_CRTC, 0x32);            /* ras/cas conf */
    r37 = vga_ReadReg(VGA_REG_CRTC, 0x37);            /* vsconf2 */

    /* Databook page: 126
    CRTC:37 video system conf2
    bit0 : display memory data bus width
    */
    vga_WriteReg(VGA_REG_CRTC, 0x37, (r37 | 1));

    /* Databook page: 130
     CRTC:32 ras/cas config
     bit7 : memory interleave enable.
     When set to 1 (memory interleave enabled), the chip operates properly only if
     CSW<0> = 0, CSP<0> = 0, and CRTC Indexed Register 37 bit 0 = 1.
    */
    vga_WriteReg(VGA_REG_CRTC, 0x32, ((r32 | (1 << 7)) & 0xFC));
    return true;
}
