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

/*-------------------------------------------------------------------------------
 * shared vga functionality
 *-----------------------------------------------------------------------------*/

void vga_vsync(void) {
    do { cpu_nop(); } while (vga_ReadPort(0x3DA) & 8);
    do { cpu_nop(); } while (!(vga_ReadPort(0x3DA) & 8));
}

void vga_delay(uint32_t ms) {
    /* a bit of a kludge and not accurate at all
     * assumes a vertical refresh of 60hz which may not be true
     * but close enough for our intended use */
    uint32_t vsyncs = (ms / 17);
    vsyncs = vsyncs ? vsyncs : 1;
    for (; vsyncs; vsyncs--) {
        vga_vsync();
    }
}

bool vga_screen_on(bool on) {
    bool ret = vga_ReadPort(0x3c6) == 0 ? false : true;
    vga_WritePort(0x3c6, on ? 0xff : 0x00);
    return ret;
}

void vga_clear(void) {
    uint32_t col = 0x00000000UL;
    /* write to all planes */
    uint8_t mask = vga_ReadReg(0x3c4, 0x02);
    vga_WriteReg(0x3c4, 0x02, 0x0f);
    /* clear all banks */
    if (card) {
        uint16_t bank; uint16_t banks = card->bank_count ? card->bank_count : 1;
        for (bank = 0; bank < banks; bank++) {
            uint32_t* dst  = (uint32_t*)(RV_PADDR_ISA_RAM16 + card->bank_addr);
            uint32_t  siz  = card->bank_size >> 2;
            card->setbank(bank);
            for (; siz; siz--) {
                *dst++ = col;
            }
        }
    } else {
        uint32_t* dst = (uint32_t*)RV_PADDR_ISA_RAM16;
        uint32_t  siz = 1024UL * 64;
        for (; siz; siz--) {
            *dst++ = col;
        }
    }
    /* restore write mask */
    vga_WriteReg(0x3c4, 0x02, mask);
}

bool vga_setmode(uint16_t code) {
    x86_regs_t r;
    /* we clear the screen ourselves instead of slowly inside x86 emulation */
    if (!(code & 0x80)) {
        code |= 0x80;
        vga_vsync();
        vga_clear();
    }
    vga_vsync();
    r.x.ax = code;
    int86(0x10, &r, &r);
    /* delay here because some cards can lock up if accessed too soon after mode change */
    /* the value used here is based on absolutely nothing */
    vga_delay(500);
    vga_vsync();
    return true;
}

void vga_setcolors(uint16_t index, uint16_t count, uint8_t* colors) {
    while (count) {
        vga_WritePort(0x3c8, index++);
        vga_WritePort(0x3c9, (*colors++)>>2);
        vga_WritePort(0x3c9, (*colors++)>>2);
        vga_WritePort(0x3c9, (*colors++)>>2);
        count--;
    }
}

void vga_getcolors(uint16_t index, uint16_t count, uint8_t* colors) {
    while (count) {
        vga_WritePort(0x3c8, index++);
        *colors++ = vga_ReadPort(0x3c9);
        *colors++ = vga_ReadPort(0x3c9);
        *colors++ = vga_ReadPort(0x3c9);
        count--;
    }
}


/*-------------------------------------------------------------------------------
 * generic base vga driver
 *-----------------------------------------------------------------------------*/

static void setbank(uint16_t num) {
    /* dummy function intentionally left blank */
}

static bool setmode(mode_t* mode) {
    return vga_setmode(mode->code);
}

static bool init(card_t* card, addmode_f addmode) {

    card->name = "unknown";
    card->bank_addr = 0xA0000UL;
    card->vram_size = 1024UL * 256;
    card->bank_size = 1024UL * 64;
    card->bank_step = 1024UL * 4;
    card->bank_count = 1;

    card->setmode = setmode;
    card->setbank = setbank;
    card->setcolors = vga_setcolors;
    card->getcolors = vga_getcolors;
    card->vsync = vga_vsync;
    card->clear = vga_clear;

    addmode(640, 350, 1, 0, 0x10);
    addmode(640, 480, 1, 0, 0x12);
    addmode(640, 350, 4, 0, 0x10);
    addmode(640, 480, 4, 0, 0x12);
    addmode(320, 200, 8, 0, 0x13);
    return true;
}

driver_t drv_vga = { "vga", init };
