/*-------------------------------------------------------------------------------
 * RVGA Tseng driver
 * (c)2026 Anders Granlund
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
#include "emulator.h"
#include "raven.h"
#include "vga.h"
#include "ini.h"

#define support_blitter() (chipset >= ET4000W32)
#define support_linear()  (chipset >= ET4000AX)

#if DRV_INCLUDE_TSENG

typedef enum {
    UNKNOWN = 0, ET4000AX, ET4000W32, ET4000W32I, ET4000W32P
} chipset_e;

static const char* chipset_strings[] = {
    "unknown", "ET4000AX", "ET4000/W32", "ET4000/W32i", "ET4000/W32p"
};

static chipset_e chipset;
static uint16_t vram;
static uint32_t pclks[16];

static bool identify(void) {
    uint8_t old3bf, old3d8;

    chipset = 0; vram = 0;

    /* enbable tseng extensions */
    old3bf = vga_ReadPort(0x3bf);
    old3d8 = vga_ReadPort(0x3d8);
    vga_WritePort(0x3bf, 3);
    vga_WritePort(0x3d8, 0xa0);

    /* and see if we've got one */
    if (vga_TestPort(0x3cd, 0x3f)) {
        if (vga_TestReg(0x3d4, 0x33, 0x0f)) {
            chipset = ET4000AX;
            if (vga_TestPort(0x3cb, 0x33)) {
                switch (vga_ReadReg(0x217a, 0xec) >> 4) {
                    case 0: chipset = ET4000W32; break;
                    case 2: chipset = ET4000W32P; break;
                    case 3: chipset = ET4000W32I; break;
                }
            }
        }
    }

    if (chipset != 0) {
        /* todo: find vram size */
        vram = 1024;
        return true;
    }

    /* restore and fail */
    vga_WritePort(0x3d8, old3d8);
    vga_WritePort(0x3bf, old3bf);
    return false;
}

static void configure_framebuffer(gfxmode_t* mode) {
    if (support_linear()) {
        vga_ModifyReg(0x3ce, 0x06, 0x0c, 0x00);         /* 128kb mode */
        vga_WriteReg(0x3ce, 0x08, 0x0ff);               /* all bits enable */
        vga_WritePort(0x3cd, 0x00);                     /* segment 0 */
        if (chipset >= ET4000W32) {

            /* todo: work out the correct setting for vram @4MB
               regardless of card. look at dram config regs or vram size?
            */
#if 1
            /* this appears right for the 1mb card */
            vga_WriteReg(0x3d4, 0x30, (1<<4) | 0x02);   /* vram at 4mb mark */
#else
            /* and this works for the 2mb card */
            vga_WriteReg(0x3d4, 0x30, (1<<4) | 0x01);   /* vram at 4mb mark */
#endif            
            vga_ModifyReg(0x3d4, 0x36, 0x18, 0x10);     /* linear on */
            /* reset screen offsets */
            vga_WriteReg(0x3d4, 0x0c, 0x00);            /* addr start 15..8 */
            vga_WriteReg(0x3d4, 0x0d, 0x00);            /* addr start  7..0 */
            vga_ModifyReg(0x3d4, 0x33, 0x13, 0x00);     /* bit 0,1,4 = bit 16,17,18 */
            vga_ModifyReg(0x3d4, 0x35, 0x0f, 0x00);     /* another extended display start */
        } else {
            vga_ModifyReg(0x3d4, 0x36, (3<<4), (3<<4)); /* linear adressing enable */
        }
    } else if (vram >= 512) {
        /* single 128kb bank */
        vga_ModifyReg(0x3ce, 0x06, 0x0c, 0x00);
    }

    /* blitter configuration */
    if (support_blitter() && mode) {
    }
}

static bool setmode(gfxmode_t* mode) {
    if (vga_setmode(mode->code)) {
        /* unlock in case vgabios locked it */
        vga_WritePort(0x3bf, 3);
        vga_WritePort(0x3d8, 0xa0);

        /* todo: make sure vgabios didn't pick interlaced 1024x768 */
        if (mode->code == 0x38) {
            uint8_t misc = vga_ReadPort(0x3cc);
            if ((misc & (1 << 4)) == 0) {
                dprintf(("interlaced\n"));
                /* From ET4000AX Databook */
                vga_WritePort(0x3c2, misc | (1 << 4));
                vga_ModifyReg(0x3d4, 0x11, 0x80, 0x00);
                vga_WriteReg(0x3d4, 0x00, 0xa1);
                vga_WriteReg(0x3d4, 0x02, 0x80);
                vga_WriteReg(0x3d4, 0x03, 0x04);
                vga_WriteReg(0x3d4, 0x04, 0x88);
                vga_WriteReg(0x3d4, 0x05, 0x9e);
                vga_WriteReg(0x3d4, 0x06, 0x26);
                vga_WriteReg(0x3d4, 0x07, 0xfd);
                vga_WriteReg(0x3d4, 0x11, 0x0a);
                vga_WriteReg(0x3d4, 0x15, 0x04);
                vga_WriteReg(0x3d4, 0x16, 0x22);
                vga_WriteReg(0x3d4, 0x35, 0x00);
                vga_ModifyReg(0x3d4, 0x11, 0x80, 0x80);


                /* measure clocks */

                /* w32i
                    1340 : 80mhz
                    1700 : 63mhz
                    1430 : 75mhz
                    2390 : 44mhz
                */

                /* misc out register, cs0, cs1 */
                /* crtc reg 34, bit 1 = cs2 */
                #if 0
                {
                    uint16_t j,k;
                    uint8_t mout = vga_ReadPort(0x3cc) & 0xf3;

                    for (k = 0; k < 2; k++) {
                        for (j = 0; j < 4; j++) {
                            /* todo W32i and later: crtc31 also has CS2 as well as CS3 */
                            uint8_t crtc34 = vga_ReadReg(0x3d4, 0x34);
                            uint8_t ts01 = vga_ReadReg(0x3c4, 0x01);
                            vga_ModifyReg(0x3d4, 0x34, 0x02, (k << 1));
                            vga_WritePort(0x3c2, mout | (j << 2));
                            {
                                uint16_t i;
                                uint16_t sr;
                                uint32_t time = *((volatile uint32_t*)0x4ba);
                                sr = cpu_ei(0x2600);
                                for (i=0; i<100; i++) {
                                    while(!(vga_ReadPort(0x3DA) & 0x08));
                                    while(vga_ReadPort(0x3DA) & 0x08);
                                }
                                time = *((volatile uint32_t*)0x4ba) - time;
                                cpu_ei(sr);
                                dprintf(("time: %d:%d: CRTC34=%02x : TS01=%02x : %ld ms\n", k, j, crtc34, ts01, time * 5));
                            }
                        }
                    }
                }
                #endif
            }
        }

#if 0
        /* custom 1280x720 mode */
        if ((mode->width == 1280) && (mode->height == 720)) {
            /* cvt-rb is for 64Mhz */
            modeline_t* ml = &modeline_720p_cvtrb;
            /* update vga crtc registers */
            vga_modeline(ml);
        }
#endif            

        configure_framebuffer(mode);
        return true;
    }
    return false;
}

static void setaddr(uint32_t addr) {
#if 0    
    /* todo... */
    /* 3d4, 33 */
    uint16_t crtc = vga_GetBaseReg(4);
    vga_ModifyReg(0x3ce, 0x0D, 0x18, (addr >> 15));
    vga_WritePortWLE(crtc, 0x0D00 | ((addr >>  2) & 0xff));
    vga_WritePortWLE(crtc, 0x0C00 | ((addr >> 10) & 0xff));
#endif    
}

static void init_clocks(void) {
    int16_t i;
    /*  todo: measure the cards selection of clocks.
        assume we are in startup 640x480x4bpp mode,
        or perhaps set it now before we measure them.
    */
    for (i=0; i<16; i++) {
        pclks[i] = 0;
    }
}

static bool init(card_t* card, addmode_f addmode) {
    /* detect hardware */
    if (!identify()) {
        return false;
    }

    /* measure clocks */
    init_clocks();

    /* supply driver callbacks and card info */
    card->name = chipset_strings[chipset];
    card->vram_size = 1024UL * vram;
    card->setmode = setmode;
    card->setaddr = setaddr;
    if (support_linear()) {
        /* todo: should be 4MB aligned for w32 */
        if (chipset >= ET4000W32) {
            card->bank_addr = 0;
            card->bank_addr = 0x400000UL;
        } else {
            card->bank_addr = 0; /*0x100000UL;*/
        }
        card->bank_size = card->vram_size;
    } else if (vram >= 512) {
        card->bank_size = 1024UL * 128;
    }

    /*
    if (support_blitter()) {
        card->blit = blit;
    }
    */

    /* 4bpp modes */
    /*addmode( 640, 480,  4, 0, 0x25);*/
    addmode( 800, 600,  4, 0, 0x29);
    addmode(1024, 768,  4, 0, 0x37);
    
    /* 8bpp modes */
    addmode( 640, 400,  8, 0, 0x2F);
    addmode( 640, 480,  8, 0, 0x2E);
    addmode( 800, 600,  8, 0, 0x30);
    addmode(1024, 768,  8, 0, 0x38);

    /* enable 16bit mem access */
    vga_ModifyReg(0x3d4, 0x36, 0x40, 0x40);

    /* configure linear or banked framebuffer */
    configure_framebuffer(0);

    /* interleave mode */
    if (chipset == ET4000W32I) {
        ini_t ini_root, ini;
        ini_Load(&ini_root, "c:\\rvga.inf");
        ini_GetSection(&ini, &ini_root, "et4000.w32");
        if (ini_GetInt(&ini, "interleave", 0) != 0) {
            vga_ModifyReg(0x3d4, 0x37, 0x01, 0x01); /* membus width */
            vga_ModifyReg(0x3d4, 0x32, 0x83, 0x80); /* interleave enable */
        }
        ini_Unload(&ini_root);
    }
    return true;
}

driver_t drv_tseng = { "tseng", init };

#endif /* DRV_INCLUDE_TSENG */
