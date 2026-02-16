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
#include <mint/basepage.h>

#include <linea.h>
extern LINEA *Linea;
extern VDIESC *Vdiesc;
extern FONTS *Fonts;

#include "raven.h"
#include "rvnova.h"
#include "sysutil.h"
#include "vga.h"
#define INI_IMPL
#include "ini.h"

bool w32i_EnableInterleaveMode(void);

static const char* path_root     = "c:";
static const char* path_nova     = "c:\\nova";
static const char* path_auto     = "c:\\auto";
static const char* path_emulator = "auto\\emulator.prg";
static const char* path_vdi		 = "auto\\sta_vdi.prg";
static const char* path_vdibib 	 = "auto\\sta_vdi.bib";
static const char* path_emubib 	 = "auto\\emulator.bib";


static long cpu;
static rvnova_menuinf_t inf;
static ini_t ini;

/*----------------------------------------
	Set small font
----------------------------------------*/
static void font_small(void) {
    FONT_HDR* font;
    linea_init();
    font = Vdiesc->font_ring[1];
    Vdiesc->v_cel_ht = font->frm_hgt;
    Vdiesc->v_cel_wr = Linea->v_lin_wr * font->frm_hgt;
    Vdiesc->v_cel_mx = (Vdiesc->v_rez_hz / font->wcel_wdt) - 1;
    Vdiesc->v_cel_my = (Vdiesc->v_rez_vt / font->frm_hgt) - 1;
    Vdiesc->v_fnt_wd = font->frm_wdt;
    Vdiesc->v_fnt_st = font->ade_lo;
    Vdiesc->v_fnt_nd = font->ade_hi;
    Vdiesc->v_fnt_ad = font->fnt_dta;
    Vdiesc->v_off_ad = font->ch_ofst;
}

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
	(void)Cconws("\033E\r\n");              /* clear screen */
}


/*----------------------------------------
	Driver pre-load
----------------------------------------*/
void drv_preload(void) {
    raven_t* rv = 0;
    if ((Getcookie(C_RAVN, (long*)&rv) == C_FOUND) && rv) {
        /* Nova driver specific VME -> ISA remap */
        if (stricmp(inf.drvpath, "MACH32") == 0) {
            /* driver code references fea00000 */
            /* also fe908000 for register writes (adding 0x8000) to reg number */
            /* VME adapter is supposed to be on ST jumper setting which grounds A22 */
            rv->mmu_Redirect(0xFE900000UL, 0x83000000UL, 0x00100000UL); /* TT Nova Mach32 reg */
            rv->mmu_Redirect(0xFE800000UL, 0x82000000UL, 0x00100000UL); /* TT Nova Mach32 vga */
            rv->mmu_Redirect(0xFEA00000UL, 0x82200000UL, 0x00100000UL); /* TT Nova Mach32 mem0 -> isa2 */
            rv->mmu_Redirect(0xFEB00000UL, 0x82300000UL, 0x00100000UL); /* TT Nova Mach32 mem1 -> isa3 */
        } else if (stricmp(inf.drvpath, "MACH64") == 0) {
            /* --> this is based on guessing and has not been tested <-- */
            /* driver code references fec00000 and fee00000 */
            /* also fe908000 for register writes (adding 0x8000 to reg number )*/
            /* VME adapter is supposed to be on TT jumper setting and this let's A22 */
            /* pass onto the ISA bus instead of forced to GND */
            rv->mmu_Redirect(0xFE900000UL, 0x83000000UL, 0x00100000UL); /* TT Nova Mach64 reg  */
            rv->mmu_Redirect(0xFE800000UL, 0x82000000UL, 0x00100000UL); /* TT Nova Mach64 vga  */
            rv->mmu_Redirect(0xFEA00000UL, 0x82200000UL, 0x00100000UL); /* probably unused */
            rv->mmu_Redirect(0xFEB00000UL, 0x82300000UL, 0x00100000UL); /* probably unused */
            rv->mmu_Redirect(0xFEC00000UL, 0x82400000UL, 0x00100000UL); /* TT Nova Mach64 mem0 -> isa4 */  
            rv->mmu_Redirect(0xFED00000UL, 0x82500000UL, 0x00100000UL); /* TT Nova Mach64 mem1 -> isa5 */
            rv->mmu_Redirect(0xFEE00000UL, 0x82600000UL, 0x00100000UL); /* TT Nova Mach64 mem2 -> isa6 */
            rv->mmu_Redirect(0xFEF00000UL, 0x82700000UL, 0x00100000UL); /* TT Nova Mach64 mem3 -> isa7 */
        } else if (strnicmp(inf.drvpath, "ET4000", 6) == 0) {
            rv->mmu_Invalid( 0xFE900000UL, 0x00100000UL);               /* probably unnecessary */
            rv->mmu_Invalid( 0xFE800000UL, 0x00100000UL);               /* probably unnecessary */
            rv->mmu_Redirect(0xFED00000UL, 0x83000000UL, 0x00100000UL); /* TT Nova ET4000 reg */
            rv->mmu_Redirect(0xFEC00000UL, 0x82000000UL, 0x00100000UL); /* TT Nova ET4000 mem0 -> isa0 */
            rv->mmu_Redirect(0x00D00000UL, 0x83000000UL, 0x00100000UL); /* ST Nova ET4000 reg */
            rv->mmu_Redirect(0x00C00000UL, 0x82000000UL, 0x00100000UL); /* ST Nova ET4000 mem0 -> isa0 */
        }
    }
}

/*----------------------------------------
	Driver post-load
----------------------------------------*/
void drv_postload(void) {
    if (stricmp(inf.drvpath, "ET4000.W32") == 0) {
        ini_t drv_ini;
        if (ini_GetSection(&drv_ini, &ini, "et4000.w32")) {
            if (ini_GetInt(&drv_ini, "interleave", 0)) {
                screen_clear();
                w32i_EnableInterleaveMode();
                screen_clear();
            }
        }
    }
}

/*----------------------------------------
	Patch vdi
----------------------------------------*/
const unsigned char vdi_patch_cache[] = {
    0x30, 0x28, 0x00, 0x88,                 /*      move.w   (0x88,a0),d0    <- rts */
    0xb0, 0x7c, 0x00, 0x14,                 /*      cmp.w    #0x14,d0               */
    0x6d, 0x16,                             /*      blt.b    .1                     */
    0x4e, 0x7a, 0x00, 0x02,                 /*      movec    cacr,d0                */
    0x22, 0x00,                             /*      move.l   d0,d1                  */
    0xc2, 0xbc, 0xff, 0xff, 0xfe, 0xfe,     /*      and.l    #-0x102,d1             */
    0x82, 0xbc, 0x00, 0x00, 0x08, 0x08,     /*      or.l     #0x0808,d1             */
    0x4e, 0x7b, 0x10, 0x02,                 /*      movec    d1,cacr                */
    0x4e, 0x75,                             /* 1:   rts                             */
    0x32, 0x28, 0x00, 0x88,                 /*      move.w   (0x88,a0),d1    <- rts */
    0xb2, 0x7c, 0x00, 0x14,                 /*      cmp.w    #0x14,d1               */
    0x6d, 0x04,                             /*      blt.b    .2,d1                  */
    0x4e, 0x7b, 0x00, 0x02,                 /*      movec    d0,cacr                */
    0x4e, 0x75                              /* 2:   rts                             */
};

void vdi_patch(BASEPAGE* bp) {
    long i;

    /* patch cache controls */
    if (cpu >= 40) {
        for (i=0; i<bp->p_tlen - sizeof(vdi_patch_cache); i+=2) {
            if (memcmp(vdi_patch_cache, &bp->p_tbase[i], sizeof(vdi_patch_cache)) == 0) {
                bp->p_tbase[i +  0] = 0x4e; bp->p_tbase[i +  1] = 0x75; /* rts */
                bp->p_tbase[i + 34] = 0x4e; bp->p_tbase[i + 35] = 0x75; /* rts */
                break;
            }
        }
    }

    /* replace card name from infoline */
    if ((strnicmp(inf.drvpath, "MACH", 4) != 0) && (strnicmp(inf.drvpath, "ET4000", 6) != 0)) {
        /* todo: retrieve rvnova card info */
        /* for now, just get rid of the ET4000 string */
        for (i=0; (i<bp->p_tlen - 16); i++) {
            if (strncmp("NOVA-VDI  Versio", &bp->p_tbase[i], 16) == 0) {
                long slen = strlen(&bp->p_tbase[i]) - 22;
                if (slen > 2) {
                    sprintf(&bp->p_tbase[i+22], "\r\n");
                }
                break;
            }
        }
    }
}

/*----------------------------------------
	Main
----------------------------------------*/

long supermain()
{
    nova_bib_t bib_emu;
	nova_bib_t bib_vdi;
	char fname[128];

    /* todo:
        get boot drive letter and update
        - path_root
        - path_nova
        - path_auto
    */

    /* cookies */
    if (Getcookie(C__CPU, &cpu) != C_FOUND) {
        cpu = 0;
    }

    /* load rvga settings */
    sprintf(fname, "%s\\rvga.inf", path_root);
    if (ini_Load(&ini, fname)) {
        path_nova = ini_GetStr(&ini, "path", path_nova);
    }

	/* load xmenu settings */
	sprintf(fname, "%s\\xmenu.inf", path_auto);
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
	sprintf(fname, "%s\\%s\\%s", path_nova, inf.drvpath, path_vdibib);
	rvnova_loadbib(&bib_vdi, fname);
	sprintf(fname, "%s\\sta_vdi.bib", path_auto);
	rvnova_savebib(&bib_vdi, fname);
	if (rvnova_findres(&bib_vdi, &inf.vdi_res)) {
		inf.menuinf.resid = inf.vdi_res.i;
	}
	
	/* emulator.bib */
	sprintf(fname, "%s\\%s\\%s", path_nova, inf.drvpath, path_emubib);
	rvnova_loadbib(&bib_emu, fname);
	if (rvnova_findres(&bib_vdi, &inf.drv_res) ) {
		rvnova_copybib(&bib_emu, 0, &bib_vdi, inf.drv_res.i);
	}
	sprintf(fname, "%s\\emulator.bib", path_auto);
	rvnova_savebib(&bib_emu, fname);

	rvnova_freebib(&bib_emu);
	rvnova_freebib(&bib_vdi);

	/* xmenu.inf */
	sprintf(fname, "%s\\xmenu.inf", path_auto);
	if (!rvnova_saveinf(&inf, fname)) {
		return 0;
	}

	/* launch driver */
	if (inf.drv_enable) {
        /* Card specific hackery before driver has loaded */
        drv_preload();

        /* start driver */
        screen_off();
		sprintf(fname, "%s\\%s\\%s", path_nova, inf.drvpath, path_emulator);
		Pexec(PE_LOADGO, fname, "", 0L);

		if (inf.vdi_enable) {
            BASEPAGE* bp;

            /* start xmenu */
			sprintf(fname, "%s\\xmenu.prg", path_nova);
			Pexec(PE_LOADGO, fname, "", 0L);
			screen_restore();

            /* start vdi */
			sprintf(fname, "%s\\%s\\%s", path_nova, inf.drvpath, path_vdi);
			bp = (BASEPAGE*)Pexec(PE_LOAD, fname, "", 0L);
            vdi_patch(bp);
            FlushCache(cpu);
            Cconws("\33p");
            Pexec(PE_GO_FREE, 0, (const void*)bp, 0L);
            Cconws("\33q");

            /* use small console font */
            /*
            font_small();
            screen_clear();
            */
		} else {
			screen_restore();
		}

        /* Card specific hackery after the driver has loaded */
        drv_postload();
	}

    ini_Unload(&ini);
    return 1;
}

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
