/*-------------------------------------------------------------------------------
 * NOVA generic vga driver
 * (c)2025 Anders Granlund
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
#include "x86.h"

#if DRV_INCLUDE_S3

typedef enum {
    UNKNOWN = 0, S86C911, S86C924, S86C801, S86C928
} chipset_e;

static const char* chipset_strings[] = {
    "unknown", "86C911", "86C924", "86C801", "86C928"
};

#define s3_support_linear()     0 /*(chipset >= S86C801)*/
#define s3_support_blitter()    0 /*(chipset >= S86C801)*/

static uint16_t vram = 0;
static chipset_e chipset = 0;

/*-------------------------------------------------------------------------------
 * driver
 *-----------------------------------------------------------------------------*/

static bool identify(void) {
    uint8_t temp[1];

    chipset = 0; vram = 0;
    temp[0] = vga_ReadReg(0x3d4, 0x38);
    vga_WriteReg(0x3d4, 0x38, 0x00); /* s3 register lock */
    if (!vga_TestReg(0x3d4, 0x35, 0xf)) {
        vga_WriteReg(0x3d4, 0x38, 0x48); /* s3 register unlock */
        if (vga_TestReg(0x3d4, 0x35, 0xf)) {
            switch (vga_ReadReg(0x3d4, 0x30)) {
                case 0x81: chipset = S86C911; break;
                case 0x82: chipset = S86C924; break;    /* or 911A */
                case 0x90: chipset = S86C928; break;    /* rev.c */
                case 0x91: chipset = S86C928; break;    /* rev.d */
                case 0x94: chipset = S86C928; break;    /* rev.e */
                case 0x95: chipset = S86C928; break;    /* rev.e */
                case 0xA0: chipset = S86C801; break;    /* or C805, rev.a or b */
                case 0xA2: chipset = S86C801; break;    /* or C805, rev.c */
                case 0xA3: chipset = S86C801; break;    /* or C805, rev.c */
                case 0xA4: chipset = S86C801; break;    /* or C805, rev.c */
                case 0xA5: chipset = S86C801; break;    /* or C805, rev.d */
                case 0xA6: chipset = S86C801; break;    /* or C805, rev.p */
                case 0xA8: chipset = S86C801; break;    /* or C805, rev.i */
            };
        }
    }

    /* todo... */
    vram = 1024;

    /* leave card unlocked if supported */
    if (chipset && vram) {
        vga_WriteReg(0x3d4, 0x39, 0xa5); /* s3 register unlock 2 */
        vga_ModifyReg(0x3d4, 0x40, 0x01, 0x01); /* 8514a registers unlock */
        return true;
    }

    /* restore what we modified if this wasn't a supported card */
    vga_WriteReg(0x3d4, 0x38, temp[0]);
    return false;
}

static void configure_framebuffer(mode_t* mode) {
   
    if (s3_support_linear()) {
        uint8_t reg58;
        uint16_t crtc = vga_GetBaseReg(4);

        /* disable banked mode */
        vga_ModifyReg(crtc, 0x31, 0x01, 0x00);
        vga_ModifyReg(crtc, 0x31, 0x02, 0x00);
        vga_ModifyReg(crtc, 0x31, 0x08, 0x01);

        /* set linear aperture address */
        /*vga_WriteReg(crtc, 0x59, 0x20);*/
        vga_WritePort(crtc, 0x59);
        vga_WritePortW(crtc+1, 0x0020);

        /* fast write buffer enable */
        /*vga_ModifyReg(crtc, 0x40, 0x0a, 0x0a);
        /* enable linear adressing */
        reg58 = (vram > 1024) ? 0x12 : 0x11;
        vga_ModifyReg(crtc, 0x58, 0x13, reg58);
    }
}

static void setaddr(uint32_t addr) {
    uint16_t crtc = vga_GetBaseReg(4);
    vga_WritePortWLE(crtc, 0x3848);
    vga_ModifyReg(crtc, 0x31, 0x30, (addr >> 14) & 0xff);
    vga_WritePortWLE(crtc, 0x3800);
    vga_WritePortWLE(crtc, 0x0D00 | ((addr >>  2) & 0xff));
    vga_WritePortWLE(crtc, 0x0C00 | ((addr >> 10) & 0xff));
}

static bool setmode(mode_t* mode) {
    if (vga_setmode(mode->code)) {
        configure_framebuffer(mode);
        return true;
    }
    return false;
}

static bool init(card_t* card, addmode_f addmode) {

    if (!identify()) {
        return false;
    }

    /* provide card info and callbacks */
    card->name = chipset_strings[chipset];
    card->vram_size = 1024UL * vram;
    card->setmode = setmode;
    if (s3_support_linear()) {
        card->bank_addr = 0x200000UL;/* 0x0A0000UL;*/
        card->bank_size = card->vram_size;
    }

    /* vesa modes */
    addmode( 800, 600, 4, 0, 0x102);
    addmode(1024, 768, 4, 0, 0x104);
    addmode( 640, 480, 8, 0, 0x101);
    addmode( 800, 600, 8, 0, 0x103);
    addmode(1024, 768, 8, 0, 0x205);

    /* configure linear or banked operation */
    /*configure_framebuffer();*/
    return true;
}

driver_t drv_s3 = { "s3", init };

#endif /* DRV_INCLUDE_S3 */
