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

#if DRV_INCLUDE_OAK

typedef enum { UNKNOWN = 0, OTI067, OTI077, OTI087 } chipset_e;
static const char* chipset_strings[] = { "unknown", "OTI067", "OTI077", "OTI087" };
static chipset_e chipset;
static uint16_t vram;

static bool identify(void) {
    uint16_t rcrtc;
    uint8_t temp[2];

    /* oak unlock */
    chipset = 0; vram = 0;
    rcrtc = vga_GetBaseReg(4);
    temp[1] = vga_ReadReg(rcrtc, 0x11);
    vga_WriteReg(rcrtc, 0x11, temp[1] & 0x7f);
    temp[0] = vga_ReadReg(0x3de, 0x0c);
    vga_WriteReg(0x3de, 0x0c, (temp[0] & 0xf0));

    /* check if we can read and write the segment register */
    if (vga_TestReg(0x3de, 0x11, 0xff)) {
        /* figure out which oak card we have */
        switch (vga_ReadPort(0x3de) & 0xE0) {
            case 0xE0: /*chipset = OTI057;*/ break;
            case 0x40: chipset = OTI067; break;
            case 0xA0: chipset = OTI077; break;
            default: {
                if (vga_ReadReg(0x3de, 0x00) & 1) {
                    chipset = OTI087;
                }
                break;
            }
        }
        /* figure out how much ram we have */
        if (chipset != 0) {
            vram = 256;
            if (chipset == OTI087) {
                switch (vga_ReadReg(0x3de, 0x02) & 6) {
                    case 0: vram =  256; break;
                    case 2: vram =  512; break;
                    case 4: vram = 1024; break;
                    case 6: vram = 2048; break;
                }
            } else {
                switch (vga_ReadReg(0x3de, 0x0d) & 0xc0) {
                    case 0xc0: vram = 1024; break;
                    case 0x80: vram =  512; break;
                }
            }
        }
    }

    /* restore what we poked at if unsupported card */
    if (!chipset || !vram) {
        vga_WriteReg(0x3de, 0x0c, temp[0]);
        vga_WriteReg(rcrtc, 0x11, temp[1]);
    }
    return (chipset != 0);
}

static void configure_framebuffer(void) {
    if (chipset >= OTI087) {
        vga_WriteReg(0x3de, 0x05, 
            0x01 |                  /* enable linear framebuffer */
            0x20 |                  /* starting at ISA:2MB mark */
            ((vram >> 6) & 0x0C)    /* expose all of the memory */
        );
    }
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

    /* provide card info and callbacks */
    card->name = chipset_strings[chipset];
    card->vram_size = 1024UL * vram;
    card->setmode = setmode;
    if (chipset >= OTI087) {
        /* linear framebuffer */
        card->bank_addr = 0x200000UL;
        card->bank_size = card->vram_size;
    }

    /* this mode will always work */
    addmode( 800, 600, 4, 0, 0x52);

    /*
     * OAK cards can only do 64k segment granularity, and our
     * bankswitcher requires granularity higher than bank size.
     * Thus, we enable the larger SVGA modes for OTI087 only
     * 
     * todo: check if OAK cards can do 128Kb banks.
     * If they can then we can enable svga on OTI67/77 as well.
     * 
     */
     if (chipset >= OTI087) {
        addmode(1024, 768, 4, 0, 0x56);
        addmode(1280,1024, 4, 0, 0x58);
        addmode( 640, 400, 8, 0, 0x61);
        addmode( 640, 480, 8, 0, 0x53);
        addmode( 800, 600, 8, 0, 0x54);
        addmode(1024, 768, 8, 0, 0x59);
        addmode(1280,1024, 8, 0, 0x5E);
    }

    configure_framebuffer();
    return true;
}

driver_t drv_oak = { "oak", init };

#endif /* DRV_INCLUDE_OAK */
