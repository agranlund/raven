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
#include <string.h>


/*-------------------------------------------------------------------------------
  driver core
-------------------------------------------------------------------------------*/

card_t* card = 0;
driver_t* driver = 0;
static card_t nvcard;


#define MAX_MODES 32
static int nummodes = 0;
static mode_t modes[MAX_MODES];

uint32_t nv_dummy_page;
static uint8_t nv_dummy_page_data[PMMU_PAGESIZE + PMMU_PAGEALIGN];

uint32_t nv_fillpatterns[8] = {
    0xffffffffUL, 0xeeffbbffUL, 0xaaffaaffUL, 0xaaddaa77UL,
    0xaa55aa55UL, 0x88552255UL, 0x00550055UL, 0x00440011UL
};

/*-------------------------------------------------------------------------------
 * nv_init_vram:
 *  sets up the logical vram after a mode change
 *-----------------------------------------------------------------------------*/
void nv_init_vram(uint32_t base, uint32_t size, uint16_t count) {
    static uint32_t cur_phys = 0;
    static uint32_t cur_size = 0;
    static uint16_t cur_count = 0;

    uint32_t log = VADDR_MEM;
    uint32_t phys = (card->isa_mem + base) & 0xFFFFFFFFUL;
    uint32_t flag = PMMU_CM_PRECISE;

    /* early out if no change */
    if ((phys == cur_phys) && (size == cur_size) && (count == cur_count)) {
        return;
    }

    /* keep current settings locally */
    cur_phys = phys;
    cur_size = size;
    cur_count = count;

    /* map logical framebuffer */
    dprintf(("nv_init_vram %08lx : %08lx x %d\n", cur_phys, size, count));
    flag = (count > 1) ? PAGE_INVALID : PAGE_READWRITE;
    while (count) {
        cpu_map(log, cur_phys, cur_size, flag);
        log += size; count--;
    }

    /* unmap remaining logical memory */
    while (log < (VADDR_MEM + VSIZE_MEM)) {
        cpu_map(log, nv_dummy_page, PMMU_PAGESIZE, PAGE_READWRITE);
        log += PMMU_PAGESIZE;
    }

    /* and flush the atc cache */
    cpu_flush_atc();
}


/*-------------------------------------------------------------------------------
 * nv_setmode:
 *  called by nova vdi
 *-----------------------------------------------------------------------------*/
bool nv_setmode(uint16_t width, uint16_t height, uint16_t bpp) {
    int i;
    int modeidx = -1;
    bool result = false;
    dprintf(("nv_setmode %dx%dx%dx\n", width, height, bpp));
    /* match against known modes */
    for (i = 0; i < nummodes && !result; i++) {
        mode_t* mode = &modes[i];
        if (mode && mode->code && (mode->width == width) && (mode->height == height) && (mode->bpp == bpp)) {
            /* call device driver */
            result = card->setmode(mode);
        }
    }
    if (result) {
        /* reconfigure logical vram as needed */
        nv_init_vram(card->bank_addr, card->bank_size, card->bank_count);
    }

    /* update bankswitcher */
    nv_banksw_prepare(width, height, bpp);

    return result;
}


/*-------------------------------------------------------------------------------
 * nv_addmode:
 *  add or modify a graphics mode, called by drivers during initialization
 *-----------------------------------------------------------------------------*/
void nv_addmode(uint16_t width, uint16_t height, uint8_t bpp, uint8_t flags, uint16_t code) {
    int i;
    for (i = 0; i < nummodes; i++) {
        if ((modes[i].width == width) && (modes[i].height == height) && (modes[i].bpp == bpp)) {
            modes[i].flags = flags;
            modes[i].code = code;
            return;
        }
    }
    if (nummodes < MAX_MODES) {
        modes[nummodes].width = width;
        modes[nummodes].height = height;
        modes[nummodes].bpp = bpp;
        modes[nummodes].flags = flags;
        modes[nummodes].code = code;
        nummodes++;
    }
}


extern driver_t drv_vga;
extern driver_t drv_cirrus;
extern driver_t drv_oak;
extern driver_t drv_s3;
extern driver_t drv_wdc;
static driver_t* drivers[] = {
#if DRV_INCLUDE_CIRRUS
    &drv_cirrus,
#endif
#if DRV_INCLUDE_OAK
    &drv_oak,
#endif
#if DRV_INCLUDE_S3
    &drv_s3,
#endif
#if DRV_INCLUDE_WDC
    &drv_wdc,
#endif    
    &drv_vga,
};


/*-------------------------------------------------------------------------------
 * nv_validate_modes:
 *  validate and discard modes which cannot work for various reasons
 *-----------------------------------------------------------------------------*/
void nv_validate_modes(void) {
    int i;

    for (i = (int)nummodes - 1; i>=0; i--) {
        uint32_t mem_required;
        uint32_t mem_available;

        uint16_t pages = 1;
        uint16_t usable_banks = card->bank_count;
        if ((modes[i].bpp > 1) && (modes[i].bpp < 8)) {
            /* we cannot bankswitch multiplane modes */
            usable_banks = 1;
            /* each bank contains X pages */
            pages = modes[i].bpp;
        }

        /* work out banksize required */
        mem_required = (uint32_t)modes[i].width;
        mem_required *= modes[i].height;
        mem_required *= modes[i].bpp;
        mem_required /= pages;
        mem_required /= 8;

        /* work out usable bank size */
        mem_available = card->bank_size * usable_banks;

        if (mem_required > mem_available) {
            static first = true;
            if (first) {
                first = false;
                dprintf((" ignored modes:\n"));
            }
            dprintf(("  %4d,%4d,%2d : %02x : %04x\n", modes[i].width, modes[i].height, modes[i].bpp, modes[i].flags, modes[i].code));
            if (i < (nummodes - 1)) {
                memcpy(&modes[i], &modes[nummodes-1], sizeof(mode_t));
            }
            nummodes--;
        }
    }
}

/*-------------------------------------------------------------------------------
 * nv_validate_modes:
 *  validate and correct card details given by driver
 *-----------------------------------------------------------------------------*/
void nv_validate_card(void) {
    if(!card->name) {
        card->name = driver->name;
    }

#if NV_SUPPORT_BANKS
    /* make sure bankswitcher can work with the capabilities of the card */
    if (card->bank_step < PMMU_PAGESIZE) {
        card->bank_step = PMMU_PAGESIZE;
        card->bank_count = 1;
    }
    if (card->bank_step >= card->bank_size) {
        card->bank_step = card->bank_size;
        card->bank_count = 1;
    }
#else
    card->bank_count = 1;
#endif
    if (card->bank_count < 2) {
        card->bank_count = 1;
        card->bank_step = card->bank_size;
    }
/*
    if (card->fill && !card->hlines) {
        card->hlines = nv_accel_hlines;
    }
*/        
}

/*-------------------------------------------------------------------------------
 * nv_init:
 *  initialize the whole subsystem
 *-----------------------------------------------------------------------------*/
bool nv_init(void) {
    int i;

    card = 0;
    nummodes = 0;
    memset((void*)modes, 0, sizeof(modes));
    memset((void*)&nvcard, 0, sizeof(card_t));

    /* dummy page for mapping parts of virtual vram to void */
    nv_dummy_page = (uint32_t)nv_dummy_page_data;
    nv_dummy_page = ((nv_dummy_page + (PMMU_PAGEALIGN - 1)) & ~(PMMU_PAGEALIGN - 1));

    /* initialize default vga */
    dprintf(("vga init\n"));
    drv_vga.init(&nvcard, nv_addmode);

    /* initialize svga driver */
    for (i = 0; i < (sizeof(drivers) / sizeof(driver_t*)) && !card; i++) {
        dprintf(("svga init %d\n", i));
        if (drivers[i] && drivers[i]->init(&nvcard, nv_addmode)) {
            driver = drivers[i];
            card = &nvcard;
        }
    }
    if (driver && card) {
        uint32_t la;

        nv_validate_card();
        dprintf(("nv_init: %s : %s (%ldKb)\n",
            driver->name,
            card->name,
            card->vram_size / 1024));
        dprintf((" banks: %08lx : %ldKb * %ld (%ldKb * %ld)\n",
            card->bank_addr,
            card->bank_size / 1024, card->bank_count,
            card->bank_step / 1024, card->bank_count * (card->bank_size / card->bank_step)));

        nv_validate_modes();
        dprintf((" valid modes:\n"));
        for (i = 0; i < nummodes; i++) {
            dprintf(("  %4d,%4d,%2d : %02x : %04x\n", modes[i].width, modes[i].height, modes[i].bpp, modes[i].flags, modes[i].code));
        }

        /* prepare logical framebuffer */
        nv_init_vram(card->bank_addr, card->bank_size, card->bank_count);
        
        /* prepare logical ioregs */
        cpu_map(VADDR_IO, card->isa_io, VSIZE_IO, PAGE_READWRITE);

        /* install bankswitcher */
        nv_banksw_install();
        
        /* finalize */
        cpu_flush_atc();
        return true;
    }

    dprintf(("nv_init failed\n"));
    return false;
}
