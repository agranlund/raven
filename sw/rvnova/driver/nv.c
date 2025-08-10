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

driver_t* card = 0;


/*-------------------------------------------------------------------------------
  standard vga and vesa functionality
-------------------------------------------------------------------------------*/
void nv_vesa_vsync(void) {
    do { cpu_nop(); } while (vga_ReadPort(0x3DA) & 8);
    do { cpu_nop(); } while (!(vga_ReadPort(0x3DA) & 8));
}

bool nv_vesa_setmode(uint16_t mode) {
    /* todo: vesa support */
    /* todo: x86 api directly instead of this... */
    nv_vesa_vsync();
    raven()->vga_SetMode((uint32_t)mode);
    nv_vesa_vsync();
    return true;
}

void nv_vesa_setcolors(uint16_t index, uint16_t count, uint8_t* colors) {
    while (count) {
        vga_WritePort(0x3c8, index++);
        vga_WritePort(0x3c9, (*colors++)>>2);
        vga_WritePort(0x3c9, (*colors++)>>2);
        vga_WritePort(0x3c9, (*colors++)>>2);
        count--;
    }
}

void nv_vesa_getcolors(uint16_t index, uint16_t count, uint8_t* colors) {
    while (count) {
        vga_WritePort(0x3c8, index++);
        *colors++ = vga_ReadPort(0x3c9);
        *colors++ = vga_ReadPort(0x3c9);
        *colors++ = vga_ReadPort(0x3c9);
        count--;
    }
}

/*-------------------------------------------------------------------------------
  driver core
-------------------------------------------------------------------------------*/

static uint32_t vram_base = 0;
static uint32_t vram_size = 0;
static uint16_t vram_count = 0;

static uint8_t nv_dummy_page[4096];
void nv_init_vram(uint32_t base, uint32_t size, uint16_t count) {
    uint16_t pagesize = 4096;
    uint32_t log = VADDR_MEM;
    uint32_t phys = PADDR_MEM + base;

    /* early out if no change */
    if ((phys == vram_base) && (size == vram_size) && (count == vram_count)) {
        return;
    } else {
        vram_base = phys;
        vram_size = size;
        vram_count = count;
    }

    dprintf("nv_init_vram %08lx : %08lx x %ld\n", base, size, count);

    /* map logical framebuffer */
    while (count) {
        cpu_map(log, vram_base, vram_size, PMMU_CM_PRECISE | PMMU_READWRITE);
        log += size;
        count--;
    }

    /* unmap remaining logical mem */
    while (log < (VADDR_MEM + VSIZE_MEM)) {
        cpu_map(log, (uint32_t)nv_dummy_page, pagesize, PMMU_CM_PRECISE | PMMU_READWRITE);
        log += pagesize;
    }

    /* and flush the atc cache */
    cpu_flush_atc();
}

bool nv_setmode(uint16_t width, uint16_t height, uint16_t bpp) {
    int i;
    int modeidx = -1;
    bool result = false;
    dprintf("nv_setmode %dx%dx%dx\n", width, height, bpp);
    for (i = 0; i < card->num_modes && !result; i++) {
        mode_t* mode = card->getmode(i);
        if (mode && !(mode->flags & MODE_FLAG_INVALID) && (mode->width == width) && (mode->height == height) && (mode->bpp == bpp)) {
            result = card->setmode(i);
        }
    }
    if (result) {
        nv_init_vram(card->bank_addr, card->bank_size, card->num_banks);
    }
    return result;
}


extern driver_t drv_vga;
static driver_t* drivers[] = {
    &drv_vga,
};

bool nv_init(void) {
    int i;
    card = 0;
    for (i = 0; i < (sizeof(drivers) / sizeof(driver_t*)) && !card; i++) {
        if (drivers[i] && drivers[i]->init()) {
            card = drivers[i];
        }
    }

    if (card) {
        dprintf("nv_init: %s [%s]\n", card->driver_name, card->card_name ? card->card_name : "unknown");
        /* prepare logical framebuffer */
        nv_init_vram(card->bank_addr, card->bank_size, card->num_banks);
        /* prepare logical ioregs */
        cpu_map(VADDR_IO, PADDR_IO, VSIZE_IO, PMMU_CM_PRECISE | PMMU_READWRITE);
        cpu_flush_atc();
        return true;
    }

    dprintf("nv_init failed\n");
    return false;
}
