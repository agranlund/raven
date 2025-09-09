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
#include "nova.h"
#include <string.h>
#include <tos.h>

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

#define BANKSW_BLITBUF_MAX  (1024UL * 512)

extern uint32_t nv_dummy_page;
static uint8_t  banksw_bank = 0;
static uint32_t banksw_map_log = 0;
static uint32_t banksw_map_phy = 0;
static uint32_t banksw_map_siz = 0;
static uint32_t banksw_readpage;
static uint32_t banksw_readpage_addr;
static uint8_t  banksw_readpage_data[(PMMU_PAGESIZE*2)+PMMU_PAGEALIGN];
static uint8_t* banksw_blitbuf;
static uint32_t banksw_blitbufsize;
static uint16_t banksw_ybanks[1024];
static uint32_t banksw_yaddr[1024];

static void nv_banksw_unmap_readpage(void) {
    if (banksw_readpage_addr >= VADDR_MEM) {
        if (((banksw_readpage_addr + PMMU_PAGESIZE) <= banksw_map_log) || (banksw_readpage_addr >= (banksw_map_log + banksw_map_siz))) {
            #if DEBUG_BANKS
            dprintf("readpage- %08lx\n", banksw_readpage_addr);
            #endif
            cpu_map(banksw_readpage_addr, banksw_readpage, PMMU_PAGESIZE, PAGE_INVALID);
        }
        banksw_readpage_addr += PMMU_PAGESIZE;
        if (((banksw_readpage_addr + PMMU_PAGESIZE) <= banksw_map_log) || (banksw_readpage_addr >= (banksw_map_log + banksw_map_siz))) {
            #if DEBUG_BANKS
            dprintf("readpage- %08lx\n", banksw_readpage_addr);
            #endif
            cpu_map(banksw_readpage_addr, PMMU_PAGESIZE + banksw_readpage, PMMU_PAGESIZE, PAGE_INVALID);
        }
    }
}

typedef struct {
/*  0 */ uint16_t sp;
/*  2 */ uint32_t pc;
/*  6 */ uint16_t vec;
/*  8 */ uint32_t addr;
/* 12 */ uint32_t fslw;
} exception_frame04_t;

uint8_t nv_banksw_handler(exception_frame04_t* frame) {

    /* 
     * todo:
     *  now that this actually works we greatly optimize it.
     *  move some of the early-out logic to our exception handler in asm.s
     *  precalculate shifts and masks
    */

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
            card->setbank(banksw_bank);
        }

        /* map new readpage */
        nv_banksw_unmap_readpage();
        banksw_readpage_addr = VADDR_MEM + page_aligned_offset + PMMU_PAGESIZE;
        if (((banksw_readpage_addr + PMMU_PAGESIZE) <= banksw_map_log) || (banksw_readpage_addr >= (banksw_map_log + banksw_map_siz))) {
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
    if ((banksw_map_log == new_map_log) && (banksw_map_siz == new_map_siz)) {
        /* this would be an issue... */
        #if DEBUG_BANKS
        bprintf("*** ignore **\n");
        dprintf("addr = %08lx\n", frame->addr);
        dprintf("offs = %08lx\n", offset);
        dprintf("bank = %d\n", bank);
        dprintf("oldl: %08lx\n", banksw_map_log);
        dprintf("oldp: %08lx\n", banksw_map_phy);
        dprintf("olds: %08lx\n", banksw_map_siz);
        dprintf("newl: %08lx\n", new_map_log);
        dprintf("newp: %08lx\n", new_map_phy);
        dprintf("news: %08lx\n", new_map_siz);
        #endif
        frame->fslw &= ~(1 << 9);
        return 0;
    }

    /* unmap virtual memory */
    if (banksw_map_siz) {
        if (banksw_map_log == nv_dummy_page) {
            uint32_t st = banksw_map_log;
            uint32_t en = banksw_map_log + banksw_map_siz;
            for (; st < en; st += PMMU_PAGESIZE) {
                cpu_map(st, nv_dummy_page, PMMU_PAGESIZE, PAGE_INVALID);
            }
        } else {
            cpu_map(banksw_map_log, banksw_map_phy, banksw_map_siz, PAGE_INVALID);
        }
        banksw_map_siz = 0;
    }

    /* map in new virtual memory */
    banksw_map_log = new_map_log;
    banksw_map_phy = new_map_phy;
    banksw_map_siz = new_map_siz;
    if (bankoffs < maxoffs) {
        /* map virtual bank to gfxcard */
        card->setbank(bankoffs);
        banksw_bank = bankoffs;
        cpu_map(new_map_log, new_map_phy, new_map_siz, PAGE_READWRITE);
    } else {
        /* map virtual bank to void */
        for(; new_map_log < (banksw_map_log + new_map_siz); new_map_log += PMMU_PAGESIZE) {
            cpu_map(new_map_log, nv_dummy_page, PMMU_PAGESIZE, PAGE_READWRITE);
        }
    }
    cpu_flush_atc();
    return 0;
}

static bool nv_banksw_blit(uint32_t cmd, rect_t* src, vec_t* dst) {
    if ((src->min.y < dst->y) || ((src->min.y == dst->y) && (src->min.x < dst->x))) {
        if (nova->planes == 8) {
            int16_t width = src->max.x - src->min.x + 1;
            int16_t height = src->max.y - src->min.y + 1;
            uint32_t size = ((uint32_t)width) * height;
            if ((size <= banksw_blitbufsize) && ((dst->y + height) <= 1024)) {
                int16_t y;
                uint8_t* bufptr = banksw_blitbuf;
                for (y=src->min.y; y<(src->min.y + height); y++) {
                    card->setbank(banksw_ybanks[y]);
                    memcpy(bufptr, (void*) (banksw_yaddr[y] + src->min.x), width);
                    bufptr += width;
                }
                bufptr = banksw_blitbuf;
                for (y=dst->y; y<(dst->y + height); y++) {
                    card->setbank(banksw_ybanks[y]);
                    memcpy((void*)(banksw_yaddr[y] + dst->x), bufptr, width);
                    bufptr += width;
                }
                nv_banksw_unmap_readpage();
                card->setbank(banksw_bank);
                return true;
            }
        }
    }
    return false;
}

void nv_banksw_prepare(uint16_t width, uint16_t height, uint16_t bpp) {
    if (card->blit == nv_banksw_blit) {
        int16_t y;
        uint32_t pitch = (bpp < 8) ? (width >> 3) : (width * ((bpp+1) & ~7)) >> 3;
        for (y=0; y<height && y<1024; y++) {
            uint32_t addr = pitch * y;
            banksw_ybanks[y] = addr / card->bank_step;
            banksw_yaddr[y] = PADDR_MEM + card->bank_addr + (addr - (((uint32_t)banksw_ybanks[y]) * card->bank_step));
        }
        if (bpp == 8) {
            card->caps |= NV_CAPS_BLIT;
        } else {
            card->caps &= ~NV_CAPS_BLIT;
        }
    }
}

extern void nv_banksw_installvector(void);
void nv_banksw_install(void) {
    if (card->bank_count > 1) {
        /* prepare readpage */
        banksw_readpage = (uint32_t)banksw_readpage_data;
        banksw_readpage = ((banksw_readpage + (PMMU_PAGEALIGN - 1)) & ~(PMMU_PAGEALIGN - 1));
        banksw_readpage_addr = 0;
        /* use software blit optimised for banked framebuffers */
        if ((card->bank_count > 1) && (card->blit == 0)) {
            banksw_blitbufsize = card->bank_size * card->bank_count;
            banksw_blitbufsize = banksw_blitbufsize > BANKSW_BLITBUF_MAX ? BANKSW_BLITBUF_MAX : banksw_blitbufsize;
            banksw_blitbuf = (uint8_t*) Mxalloc(banksw_blitbufsize, 3);
            if (banksw_blitbuf) {
                card->blit = nv_banksw_blit;
            }
        }
        /* install exception vector */
        nv_banksw_installvector();
    }
}

#else
void nv_banksw_install(void) {}
void nv_banksw_prepare(uint16_t width, uint16_t height, uint16_t bpp) {}
uint8_t nv_banksw_handler(void* frame) { return -1; }
#endif
