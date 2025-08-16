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

static uint32_t nv_dummy_page;
static uint8_t nv_dummy_page_data[PMMU_PAGESIZE + PMMU_PAGEALIGN];


/*-------------------------------------------------------------------------------
 * bankswitcher:
 *  for older cards that do not have support for linear framebuffer
 * 
 *  this provides a (virtual) linear framebuffer that VDI requires.
 *  it uses the pmmu to map in physical gfxcard banks if the driver
 *  has implemented a chipset specific setbank() callback.
 * 
 *  different cards have different bankswitch capabilities so this here aims
 *  for the lowest common denominator which is single-bank only.
 * 
 *  in order to handle single-instruction read+write across different
 *  banks we implement a software read-only mini bank.
 *  thus a memory copy from one bank to another will read from the
 *  software buffer and write to the gfxcard.
 *  this software buffer is filled on read-violations.
 * 
 *  because of this, it cannot work with planar modes so 4bpp resolutions
 *  bigger than 800x600 are automatically rejected during initialization.
 *  8bit or higher modes have no such restrictions.
 *
 *  some gfxcards do provide two separate banks but these are in the majority
 *  of cases provided as one read-only and another write-only bank.
 *  we cannot really use those since the pmmu can only tag pages
 *  as read-only or read+write. it does not support a write-only concept.
 *-----------------------------------------------------------------------------*/
#if NV_SUPPORT_BANKS

#define DEBUG_BANKS 0

static uint32_t banksw_readpage;
static uint32_t banksw_readpage_addr;
static uint8_t banksw_readpage_data[(PMMU_PAGESIZE*2)+PMMU_PAGEALIGN];

extern void nv_bankvec_install(void);
void nv_install_bankswitcher(void) {
    banksw_readpage = (uint32_t)banksw_readpage_data;
    banksw_readpage = ((banksw_readpage + (PMMU_PAGEALIGN - 1)) & ~(PMMU_PAGEALIGN - 1));
    banksw_readpage_addr = 0;
    nv_bankvec_install();
}

typedef struct {
/*  0 */ uint16_t sp;
/*  2 */ uint32_t pc;
/*  6 */ uint16_t vec;
/*  8 */ uint32_t addr;
/* 12 */ uint32_t fslw;
} exception_frame04_t;

uint8_t nv_bankvec_handler(exception_frame04_t* frame) {

    /* 
     * todo:
     *  now that this actually works we greatly optimize it.
     *  move some of the early-out logic to our exception handler in asm.s
     *  precalculate shifts and masks
    */

    static uint8_t  old_bankoffs = 0;
    static uint32_t old_map_log = 0;
    static uint32_t old_map_phy = 0;
    static uint32_t old_map_siz = 0;
    static uint32_t new_map_log;
    static uint32_t new_map_phy;
    static uint32_t new_map_siz;
    static uint32_t offset;
    static uint16_t bankoffs;
    static uint16_t maxoffs;

    #if DEBUG_BANKS
    dprintf("i=%08lx a=%08lx f=%08lx\n", frame->pc, frame->addr, frame->fslw);
    #endif    

    /* invalid or write-protected page? */
    if ((frame->fslw & ((1<<7)|(1<<9))) == 0) {
        return -1;
    }
    /* inside logical vram space? */
    if (frame->addr < VADDR_MEM) {
        return -1;
    }
    offset = frame->addr - VADDR_MEM;
    if (offset >= card->vram_size) {
        return -1;
    }

    /* gfxcard bank offset */
    maxoffs = card->bank_count * (card->bank_size / card->bank_step);
    bankoffs = (offset / card->bank_step);
    /* adjust a bit upward to balance in case of drawing bottom->up */
    bankoffs -= (uint16_t)((card->bank_size / (uint16_t)card->bank_step) >> 2);
    if (bankoffs & 0xff00) { bankoffs = 0; }

    /* read fault? */
    /*if ((frame->fslw & (3 << 23)) == (2 << 23)) {*/
    if (((frame->fslw >> 23) & 3) == 2) {
        uint32_t page_aligned_offset = offset & ~(PMMU_PAGESIZE - 1);
        uint32_t bank_aligned_offset = page_aligned_offset - (card->bank_step * bankoffs);
        #if DEBUG_BANKS
        dprintf("read, aligned = %08lx\n", VADDR_MEM + page_aligned_offset);
        #endif
        /* copy contents to readpage */
        if (bankoffs < maxoffs) {
            uint32_t* src; uint32_t* dst; int i;
            card->setbank(bankoffs);
            src = (uint32_t*) (PADDR_MEM + card->bank_addr + bank_aligned_offset);
            dst = (uint32_t*) banksw_readpage;
            i = (int) ((PMMU_PAGESIZE * 2) / 16);
            #if DEBUG_BANKS
            dprintf("%08lx -> %08lx\n", (uint32_t)src, (uint32_t)dst);
            #endif
            for (; i; i--) {
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
            }
            card->setbank(old_bankoffs);
        }
        /* unmap old readpage */
        if (banksw_readpage_addr >= VADDR_MEM) {
            if (((banksw_readpage_addr + PMMU_PAGESIZE) <= old_map_log) || (banksw_readpage_addr >= (old_map_log + old_map_siz))) {
                #if DEBUG_BANKS
                dprintf("readpage- %08lx\n", banksw_readpage_addr);
                #endif
                cpu_map(banksw_readpage_addr, banksw_readpage, PMMU_PAGESIZE, PAGE_INVALID);
            }
            banksw_readpage_addr += PMMU_PAGESIZE;
            if (((banksw_readpage_addr + PMMU_PAGESIZE) <= old_map_log) || (banksw_readpage_addr >= (old_map_log + old_map_siz))) {
                #if DEBUG_BANKS
                dprintf("readpage- %08lx\n", banksw_readpage_addr);
                #endif
                cpu_map(banksw_readpage_addr, PMMU_PAGESIZE + banksw_readpage, PMMU_PAGESIZE, PAGE_INVALID);
            }
        }
        /* map new readpage */
        banksw_readpage_addr = VADDR_MEM + page_aligned_offset + PMMU_PAGESIZE;
        if (((banksw_readpage_addr + PMMU_PAGESIZE) <= old_map_log) || (banksw_readpage_addr >= (old_map_log + old_map_siz))) {
            #if DEBUG_BANKS
            dprintf("readpage+ %08lx\n", banksw_readpage_addr);
            #endif
            cpu_map(banksw_readpage_addr, PMMU_PAGESIZE + banksw_readpage, PMMU_PAGESIZE, PAGE_READONLY);
        }
        banksw_readpage_addr -= PMMU_PAGESIZE;
        #if DEBUG_BANKS
        dprintf("readpage+ %08lx\n", banksw_readpage_addr);
        #endif
        cpu_map(banksw_readpage_addr, banksw_readpage, PMMU_PAGESIZE, PAGE_READONLY);
        cpu_flush_atc();
        return 0;
    }

    new_map_siz = card->bank_size;
    new_map_phy = PADDR_MEM + (card->bank_addr);
    new_map_log = VADDR_MEM + (card->bank_step * bankoffs);
    #if DEBUG_BANKS
    bprintf("write %08lx -> %08lx\n", new_map_log, new_map_phy);
    #endif
    if ((old_map_log == new_map_log) && (old_map_siz == new_map_siz)) {
        /* this would be an issue... */
        #if DEBUG_BANKS
        bprintf("*** ignore **\n");
        dprintf("addr = %08lx\n", frame->addr);
        dprintf("offs = %08lx\n", offset);
        dprintf("bank = %d\n", bank);
        dprintf("oldl: %08lx\n", old_map_log);
        dprintf("oldp: %08lx\n", old_map_phy);
        dprintf("olds: %08lx\n", old_map_siz);
        dprintf("newl: %08lx\n", new_map_log);
        dprintf("newp: %08lx\n", new_map_phy);
        dprintf("news: %08lx\n", new_map_siz);
        #endif
        frame->fslw &= ~(1 << 9);
        return 0;
    }

    /* unmap virtual memory */
    if (old_map_siz) {
        if (old_map_log == nv_dummy_page) {
            uint32_t st = old_map_log;
            uint32_t en = old_map_log + old_map_siz;
            for (; st < en; st += PMMU_PAGESIZE) {
                cpu_map(st, nv_dummy_page, PMMU_PAGESIZE, PAGE_INVALID);
            }
        } else {
            cpu_map(old_map_log, old_map_phy, old_map_siz, PAGE_INVALID);
        }
        old_map_siz = 0;
    }

    /* map in new virtual memory */
    old_map_log = new_map_log;
    old_map_phy = new_map_phy;
    old_map_siz = new_map_siz;
    if (bankoffs < maxoffs) {
        /* map virtual bank to gfxcard */
        card->setbank(bankoffs);
        old_bankoffs = bankoffs;
        cpu_map(new_map_log, new_map_phy, new_map_siz, PAGE_READWRITE);
    } else {
        /* map virtual bank to void */
        for(; new_map_log < (old_map_log + new_map_siz); new_map_log += PMMU_PAGESIZE) {
            cpu_map(new_map_log, nv_dummy_page, PMMU_PAGESIZE, PAGE_READWRITE);
        }
    }
    cpu_flush_atc();
    return 0;
}
#else
void nv_install_bankswitcher(void) {}
uint8_t nv_bankvec_handler(void* frame) { return -1; }
#endif



/*-------------------------------------------------------------------------------
 * nv_init_vram:
 *  sets up the logical vram after a mode change
 *-----------------------------------------------------------------------------*/
void nv_init_vram(uint32_t base, uint32_t size, uint16_t count) {
    static uint32_t cur_phys = 0;
    static uint32_t cur_size = 0;
    static uint16_t cur_count = 0;

    uint32_t log = VADDR_MEM;
    uint32_t phys = (PADDR_MEM + base) & 0xFFFFFFFFUL;
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
    dprintf("nv_init_vram %08lx : %08lx x %d\n", cur_phys, size, count);
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
    dprintf("nv_setmode %dx%dx%dx\n", width, height, bpp);
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
extern driver_t drv_wdc;
static driver_t* drivers[] = {
#if DRV_INCLUDE_CIRRUS
    &drv_cirrus,
#endif
#if DRV_INCLUDE_OAK
    &drv_oak,
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
                dprintf(" ignored modes:\n");
            }
            dprintf("  %4d,%4d,%2d : %02x : %04x\n", modes[i].width, modes[i].height, modes[i].bpp, modes[i].flags, modes[i].code);
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
    drv_vga.init(&nvcard, nv_addmode);

    /* initialize svga driver */
    for (i = 0; i < (sizeof(drivers) / sizeof(driver_t*)) && !card; i++) {
        if (drivers[i] && drivers[i]->init(&nvcard, nv_addmode)) {
            driver = drivers[i];
            card = &nvcard;
        }
    }
    if (driver && card) {
        uint32_t la;

        nv_validate_card();
        dprintf("nv_init: %s : %s (%ldKb)\n",
            driver->name,
            card->name,
            card->vram_size / 1024);
        dprintf(" banks: %08lx : %ldKb * %ld (%ldKb * %ld)\n",
            card->bank_addr,
            card->bank_size / 1024, card->bank_count,
            card->bank_step / 1024, card->bank_count * (card->bank_size / card->bank_step));

        nv_validate_modes();
        dprintf(" valid modes:\n");
        for (i = 0; i < nummodes; i++) {
            dprintf("  %4d,%4d,%2d : %02x : %04x\n", modes[i].width, modes[i].height, modes[i].bpp, modes[i].flags, modes[i].code);
        }

        /* prepare logical framebuffer */
        nv_init_vram(card->bank_addr, card->bank_size, card->bank_count);
        
        /* prepare logical ioregs */
        cpu_map(VADDR_IO, PADDR_IO, VSIZE_IO, PAGE_READWRITE);

        /* prepare bank switching mechanism */
        if (card->bank_count > 1) {
            nv_install_bankswitcher();
        }
        
        /* finalize */
        cpu_flush_atc();
        return true;
    }

    dprintf("nv_init failed\n");
    return false;
}
