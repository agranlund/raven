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
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "raven.h"
#include "nova.h"

#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#define DEBUG           0
#define DEBUGPRINT_UART 1

#if defined(DEBUG) && DEBUG
    #if DEBUGPRINT_UART
        extern void dprintf_uart(char* s, ...);
        #define dprintf dprintf_uart
    #else
        #include <stdio.h>
        #define dprintf printf
    #endif
#else
    static void dprintf(char* s, ...) { }
#endif

/*-----------------------------------------------------------------------------*/
#define NV_SUPPORT_BANKS        1


/*-----------------------------------------------------------------------------*/
#ifndef DRV_INCLUDE_CIRRUS
#define DRV_INCLUDE_CIRRUS      1
#endif
#ifndef DRV_INCLUDE_OAK
#define DRV_INCLUDE_OAK         0
#endif
#ifndef DRV_INCLUDE_S3
#define DRV_INCLUDE_S3          0
#endif
#ifndef DRV_INCLUDE_WDC
#define DRV_INCLUDE_WDC         1
#endif


/*-----------------------------------------------------------------------------*/
#define PMMU_PAGESIZE           4096UL
#define PMMU_PAGEALIGN          PMMU_PAGESIZE

#define PMMU_INVALID            (0 << 0)
#define PMMU_VALID              (3 << 0)
#define PMMU_WRITEPROTECT       (1 << 2)
#define PMMU_USED               (1 << 3)
#define PMMU_CM_PRECISE         (2 << 5)

#define PAGE_INVALID            (PMMU_CM_PRECISE | PMMU_INVALID)
#define PAGE_READONLY           (PMMU_CM_PRECISE | PMMU_VALID | PMMU_USED | PMMU_WRITEPROTECT)
#define PAGE_READWRITE          (PMMU_CM_PRECISE | PMMU_VALID | PMMU_USED)

extern uint16_t cpu_di(void);
extern uint16_t cpu_ei(uint16_t sr);
static void     cpu_nop(void) 0x4E71;
static void     cpu_map(uint32_t log, uint32_t phys, uint32_t size, uint32_t flags) { raven()->mmu_Map(log, phys, size, flags); }
static void     cpu_flush_atc(void) { raven()->mmu_Flush(); }
static void     cpu_flush_cache(void) { raven()->cache_Flush(); }

/*-----------------------------------------------------------------------------*/
#define PADDR_IO        RV_PADDR_ISA_IO16
#define PADDR_MEM       RV_PADDR_ISA_RAM16
#define VADDR_MEM       0x44000000UL    /* logical mem space */
#define VADDR_IO        0x44F00000UL    /* logical io space */
#define VSIZE_MEM       0x00400000UL    /*  4Mb virtual mem space */
#define VSIZE_IO        0x00010000UL    /* 64kb virtual io space  */


/*-------------------------------------------------------------------------------
 * driver
 *-----------------------------------------------------------------------------*/


/* display mode caps */
#define NV_CAPS_BLIT            (1UL<<0)      /* blit operation */
#define NV_CAPS_FILL            (1UL<<1)      /* fill operation */
#define NV_CAPS_DITHER          (1UL<<2)      /* 8x8 dither patterns */
#define NV_CAPS_SRCKEY          (1UL<<3)      /* source colorkey transparency */
#define NV_CAPS_DSTKEY          (1UL<<4)      /* destination colorkey transparency */
#define NV_CAPS_AUTOMASK        (1UL<<5)      /* monoexpand from color source, for example WDC cards */
#define NV_CAPS_ASYNC           (1UL<<7)      /* asynchronous blits */

/* blit command */
#define BL_SHIFT_BG             24      /* xxxxxxxx ........ ........ ........ */
#define BL_MASK_BG              0xff
#define BL_SHIFT_FG             16      /* ........ xxxxxxxx ........ ........ */
#define BL_MASK_FG              0xff
#define BL_SHIFT_PAT            12      /* ........ ........ xxxx.... ........ */
#define BL_MASK_PAT             0xf
#define BL_SHIFT_ROP            8       /* ........ ........ ....xxxx ........ */
#define BL_MASK_ROP             0xf
#define BL_SHIFT_CMD            0       /* ........ ........ ........ xxxxxxxx */
#define BL_MASK_CMD             0xff

#define BL_GETBGCOL(x)          (((x)>>BL_SHIFT_BG)&BL_MASK_BG)
#define BL_GETFGCOL(x)          (((x)>>BL_SHIFT_FG)&BL_MASK_FG)
#define BL_GETPATTERN(x)        (((x)>>BL_SHIFT_PAT)&BL_MASK_PAT)
#define BL_GETROP(x)            (((x)>>BL_SHIFT_ROP)&BL_MASK_ROP)

/* blit flags */
#define BL_BLIT                 (0UL << (BL_SHIFT_CMD + 0))
#define BL_FILL                 (1UL << (BL_SHIFT_CMD + 0))
#define BL_ASYNC                (1UL << (BL_SHIFT_CMD + 1))
#define BL_TRANSPARENT          (1UL << (BL_SHIFT_CMD + 2))
#define BL_MONO                 (1UL << (BL_SHIFT_CMD + 3))

/* raster operations, same order as VDI */
#define BL_ROP_0                ( 0UL << BL_SHIFT_ROP)  /* ALL_WHITE */
#define BL_ROP_DSa              ( 1UL << BL_SHIFT_ROP)  /* S_AND_D */
#define BL_ROP_SDna             ( 2UL << BL_SHIFT_ROP)  /* S_AND_NOTD */
#define BL_ROP_S                ( 3UL << BL_SHIFT_ROP)  /* S_ONLY */
#define BL_ROP_DSna             ( 4UL << BL_SHIFT_ROP)  /* NOTS_AND_D */
#define BL_ROP_D                ( 5UL << BL_SHIFT_ROP)  /* D_ONLY */
#define BL_ROP_DSx              ( 6UL << BL_SHIFT_ROP)  /* S_XOR_D */
#define BL_ROP_DSo              ( 7UL << BL_SHIFT_ROP)  /* S_OR_D */
#define BL_ROP_DSon             ( 8UL << BL_SHIFT_ROP)  /* NOT_SORD */
#define BL_ROP_DSxn             ( 9UL << BL_SHIFT_ROP)  /* NOT_SXORD */
#define BL_ROP_Dn               (10UL << BL_SHIFT_ROP)  /* NOT_D */
#define BL_ROP_SDno             (11UL << BL_SHIFT_ROP)  /* S_OR_NOTD */
#define BL_ROP_Sn               (12UL << BL_SHIFT_ROP)  /* NOT_S */
#define BL_ROP_DSno             (13UL << BL_SHIFT_ROP)  /* NOTS_OR_D */
#define BL_ROP_DSan             (14UL << BL_SHIFT_ROP)  /* NOT_SANDD */
#define BL_ROP_1                (15UL << BL_SHIFT_ROP)  /* ALL_BLACK */


/* fill values */
#define BL_PATTERN(x)           (((uint32_t)((x)&BL_MASK_PAT))<<BL_SHIFT_PAT)
#define BL_FGCOL(x)             (((uint32_t)((x)&BL_MASK_FG))<<BL_SHIFT_FG)
#define BL_BGCOL(x)             (((uint32_t)((x)&BL_MASK_BG))<<BL_SHIFT_BG)


typedef void(*addmode_f)(uint16_t, uint16_t, uint8_t, uint8_t, uint16_t);

typedef struct {
    int16_t x;
    int16_t y;
} vec_t;

typedef struct {
    vec_t min;
    vec_t max;
} rect_t;

typedef struct
{
    uint16_t    width;
    uint16_t    height;
    uint8_t     bpp;
    uint8_t     flags;
    uint16_t    code;
} mode_t;

typedef struct
{
    const char* name;
    uint32_t    caps;
    uint32_t    vram_size;
    uint32_t    bank_count;
    uint32_t    bank_addr;
    uint32_t    bank_size;
    uint32_t    bank_step;
    bool        (*setmode)(mode_t* mode);
    void        (*setaddr)(uint32_t addr);
    void        (*setbank)(uint16_t num);
    void        (*setcolors)(uint16_t index, uint16_t count, uint8_t* colors);
    void        (*getcolors)(uint16_t index, uint16_t count, uint8_t* colors);
    void        (*vsync)(void);
    void        (*clear)(void);
    bool        (*blit)(uint32_t cmd, rect_t* src, vec_t* dst);
} card_t;

typedef struct
{
    const char* name;
    bool        (*init)(card_t* card, addmode_f addmode);
} driver_t;

/*-------------------------------------------------------------------------------
 * core
 *-----------------------------------------------------------------------------*/
extern uint32_t nv_fillpatterns[8];
extern nova_xcb_t* nova;
extern driver_t* driver;
extern card_t* card;

extern bool nv_init(void);
extern bool nv_setmode(uint16_t w, uint16_t h, uint16_t b);
extern void nv_init_vram(uint32_t phys, uint32_t size, uint16_t count);
extern void nv_banksw_install(void);
extern void nv_banksw_prepare(uint16_t width, uint16_t height, uint16_t bpp);
extern bool nv_accel_hlines(uint16_t flg, uint16_t col, int16_t num, int16_t ypos, int16_t* pts);

/* standard vga functionality */
extern void vga_vsync(void);
extern void vga_vblank_out(void);
extern void vga_vblank_int(void);
extern bool vga_screen_on(bool on);
extern bool vga_setmode(uint16_t code);
extern void vga_setaddr(uint32_t addr);
extern void vga_setcolors(uint16_t index, uint16_t count, uint8_t* colors);
extern void vga_getcolors(uint16_t index, uint16_t count, uint8_t* colors);
extern void vga_enable_fastclear(bool on);

#endif /* _EMULATOR_H_ */
