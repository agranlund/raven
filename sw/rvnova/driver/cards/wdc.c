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

/* todo: find out how capabilities differs between card versions.
 * this code has only been tested on WDC90C31
 */

#if DRV_INCLUDE_WDC

typedef enum {
    UNKNOWN = 0, PVGA1, WD90C00, WD90C10, WD90C11, WD90C20, WD90C22, WD90C24, WD90C26, WD90C30, WD90C31, WD90C33,
} chipset_e;


static const char* chipset_strings[] = {
    "unknown", "PVGA1", "WD90C00", "WDC90C10", "WDC90C11", "WD90C20", "WD90C22", "WD90C24", "WD90C26", "WD90C30", "WD90C31", "WD90C33"
};

static chipset_e chipset;
static uint16_t vram;

static bool identify(void) {
    uint16_t rcrtc;
    uint8_t old_3c4_06;
    uint8_t old_3ce_0f;
    uint8_t old_3d4_29;

    /* look for known string in vgabios */
    if (('V' != *((volatile char*)(RV_PADDR_ISA_RAM + 0xC007dUL + 0x00))) ||
        ('G' != *((volatile char*)(RV_PADDR_ISA_RAM + 0xC007dUL + 0x01))) ||
        ('A' != *((volatile char*)(RV_PADDR_ISA_RAM + 0xC007dUL + 0x02))) ||
        ('=' != *((volatile char*)(RV_PADDR_ISA_RAM + 0xC007dUL + 0x03))))
    {
        return false;
    }

    /* test lock PR0-PR4 */
    old_3ce_0f = vga_ReadReg(0x3ce, 0x0f);
    vga_WriteReg(0x3ce, 0x0f, 0x17 | old_3ce_0f);
    if (vga_TestReg(0x3ce, 0x09, 0x7f)) {
        vga_WriteReg(0x3ce, 0x0f, old_3ce_0f);
        return false;
    }

    /* test unlock PR0-PR4 */
    vga_WriteReg(0x3ce, 0x0f, 0x05);
    if (!vga_TestReg(0x3ce, 0x09, 0x7f)) {
        vga_WriteReg(0x3ce, 0x0f, old_3ce_0f);
        return false;
    }

    /* we have some kind of wdc chip, figure out which one */
    chipset = PVGA1;
    rcrtc = vga_GetBaseReg(4);
    old_3d4_29 = vga_ReadReg(rcrtc, 0x29);
    vga_ModifyReg(rcrtc, 0x29, 0x8f, 0x85);  /* test unlock PR11-PR14 */
    if (vga_TestReg(rcrtc, 0x2b, 0xff)) {
        old_3c4_06 = vga_ReadReg(0x3c4, 0x06);
        vga_WriteReg(0x3c4, 0x06, 0x48);    /* unlock extended sequencer registers */
        if (!vga_TestReg(0x3c4, 0x07, 0xf0)) {
            chipset = WD90C00;
        } else if (!vga_TestReg(0x3c4, 0x10, 0xff)) {
            if (vga_TestReg(rcrtc, 0x31, 0x68)) {
                chipset = WD90C22;
            } else {
                chipset = WD90C20;
            }
        } else if (vga_TestReg(0x3c4, 0x14, 0x0f)) {
            uint8_t id36 = vga_ReadReg(rcrtc, 0x36);
            uint8_t id37 = vga_ReadReg(rcrtc, 0x37);
            if ((id36 == 0x32) && (id37 == 0x34)) {
                chipset = WD90C24;
            } else if ((id36 == 0x32) && (id37 == 0x36)) {
                chipset = WD90C26;
            } else if ((id36 == 0x33) && (id37 == 0x30)) {
                chipset = WD90C30;
            } else if ((id36 == 0x33) && (id37 == 0x31)) {
                chipset = WD90C31;
            } else if ((id36 == 0x33) && (id37 == 0x33)) {
                chipset = WD90C33;
            } else {
                /*chipset = 0;*/
            }
        } else if (!vga_TestReg(0x3c4, 0x10, 0x04)) {
            chipset = WD90C10;
        } else {
            chipset = WD90C11;
        }
    }

    /* find out how much vram we have */
    vram = 256;
    switch (vga_ReadReg(0x3ce, 0x0b) >> 6) {
        case 2:
            vram = 512;
            break;
        case 3:
            vram = 1024;
            if (chipset == WD90C33) {
                if (vga_ReadReg(rcrtc, 0x3e) & 0x80) {
                    vram = 2048;
                }
            }
            break;
        default:
            break;
    }

    /* todo: reject WDC cards that the driver doesn't support */
    /* at the moment, that would be all cards without linear support */

    if (chipset && vram) {
        /* unlock crtc registers */
        vga_ModifyReg(rcrtc, 0x11, 0x80, 0x00);
        /* unlock extended sequencer registers */
        if (chipset != WD90C33) {
            vga_WriteReg(0x3c4, 0x06, 0x48 | (old_3c4_06 & 0xef));
        }
        return true;
    }

    /* restore what we modified */
    vga_WriteReg(rcrtc, 0x29, old_3d4_29);
    vga_WriteReg(0x3ce, 0x0f, old_3ce_0f);
    return false;
}

static void setbank(uint16_t num) {
    /* todo: banked mode support */
}

static void configure_framebuffer(void) {

    /* todo: which cards and configs support this? */
    bool vram32bit = true;

    /* bank location @ isa:2MB */
    vga_WriteReg(0x3C4, 0x14, 0x02);
    
    /* memory settings */ 
    vga_WriteReg(0x3CE, 0x0b,
        (vram32bit ? 0xC0 : 0x80) |     /* ram size */
        (3 << 4) |                      /* map 1MB */
        (1 << 2) |                      /* 16bit isa memory access */
        (vga_ReadReg(0x3CE, 0x0b) & 0x0f)
    );
    
    /* disable vga adressing */
    vga_WriteReg(vga_GetBaseReg(4), 0x2f, 0x00);

    /* memory interface */
    vga_WriteReg(0x3C4, 0x10,
        0xC0 |                          /* 4 level write buffer */
        (vram32bit ? 0x00 : 0x20) |     /* vram data path width */
        0x00 |                          /* 8 level fifo */
        0x01                            /* display fifo request @ 2 levels*/
    );
}

static bool setmode(mode_t* mode) {
    if (vga_setmode(mode->code)) {
        configure_framebuffer();
        return true;
    }
    return false;
}

static bool init(card_t* card, addmode_f addmode) {

    /* detect hardware */
    if (!identify()) {
        return false;
    }

    /* supply driver callbacks and card info */
    card->name = chipset_strings[chipset];
    card->vram_size = 1024UL * vram;
    card->setmode = setmode;
    if (1) { /* linear mode */
        card->bank_addr = 0x200000UL;
        card->bank_size = card->vram_size;
    } else { /* todo: banked mode */
        card->setbank = setbank;
        card->bank_count = 1;
    }

    /* supply graphics modes */
    addmode( 800, 600,  4, 0, 0x58);    /* 4bpp */
    addmode(1024, 768,  4, 0, 0x5D);
    addmode(1280, 960,  4, 0, 0x6C);
    if (chipset >= WD90C31) {
    addmode(1280,1024,  4, 0, 0x64);
    }
    addmode( 640, 400,  8, 0, 0x5E);    /* 8bpp */
    addmode( 640, 480,  8, 0, 0x5F);
    if ((chipset >= WD90C30) || (chipset == WD90C11)) {
    addmode( 800, 600,  8, 0, 0x5C);
    }
    if (chipset >= WD90C30) {
    addmode(1024, 768,  8, 0, 0x60);
    }

    /* 
     * PR31: system interface control
     *  7 : enable read/write offset
     *  6 : enable turbo for blanked lines
     *  5 : enable turbo for text
     *  4 : cpu rdy release 1
     *  3 : cpu rdy release 0
     *  2 : enable write buffer
     *  1 : enable 16bit IO (ATC)
     *  0 : enable 16bit IO (CRTC, SEQ, GDC)
     */
    if (chipset >= WD90C10) {
        vga_WriteReg(0x3C4, 0x11,
            (1 << 6) |  /* enable turbo for blanked lines */
            (3 << 3) |  /* cpu rdy release */
            (1 << 2) |  /* enable write buffer */
            (1 << 1) |  /* enable 16bit io (atc) */
            (1 << 0)    /* enable 16bit io (crtc, seq, gdc) */
        );
    }

    /* 
     * configure zero-waitstate for:
     *  a) write buffer is ready + memory address decoded + MWR asserted
     *  b) IO write is occuring
     */
    if (chipset >= WD90C10) {
        vga_ModifyReg(0x3c4, 0x13, 0xC0, 0xC0);
    }

    /* configure linear or banked framebuffer */
    configure_framebuffer();

    return true;
}

driver_t drv_wdc = { "wdc", init };

#endif /* DRV_INCLUDE_WDC */
