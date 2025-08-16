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
#ifndef DRV_INCLUDE_WDC
#define DRV_INCLUDE_WDC         0
#endif
#ifndef DRV_INCLUDE_OAK
#define DRV_INCLUDE_OAK         1
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
#define PADDR_IO        RV_PADDR_ISA_IO
#define PADDR_MEM       RV_PADDR_ISA_RAM16

#define VADDR_MEM       0x44000000UL    /* logical mem space */
#define VADDR_IO        0x44F00000UL    /* logical io space */

#define VSIZE_MEM       0x00400000UL    /*  4Mb virtual mem space */
#define VSIZE_IO        0x00010000UL    /* 64kb virtual io space  */

/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------
 * driver
 *-----------------------------------------------------------------------------*/

typedef void(*addmode_f)(uint16_t, uint16_t, uint8_t, uint8_t, uint16_t);

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
    uint32_t    vram_size;
    uint32_t    bank_count;
    uint32_t    bank_addr;
    uint32_t    bank_size;
    uint32_t    bank_step;
    bool        (*setmode)(mode_t* mode);
    void        (*setbank)(uint16_t num);
    void        (*setcolors)(uint16_t index, uint16_t count, uint8_t* colors);
    void        (*getcolors)(uint16_t index, uint16_t count, uint8_t* colors);
    void        (*vsync)(void);
    void        (*clear)(void);
} card_t;

typedef struct
{
    const char* name;
    bool        (*init)(card_t* card, addmode_f addmode);
} driver_t;

/*-------------------------------------------------------------------------------
 * core
 *-----------------------------------------------------------------------------*/
extern nova_xcb_t nova;
extern driver_t* driver;
extern card_t* card;

/* initialise system and driver */
extern bool nv_init(void);

/* set graphics mode */
extern bool nv_setmode(uint16_t w, uint16_t h, uint16_t b);

/* initialize vram and banks, if any */
extern void nv_init_vram(uint32_t phys, uint32_t size, uint16_t count);


/* standard vga functionality */
extern void vga_vsync(void);
extern bool vga_screen_on(bool on);
extern bool vga_setmode(uint16_t code);
extern void vga_setcolors(uint16_t index, uint16_t count, uint8_t* colors);
extern void vga_getcolors(uint16_t index, uint16_t count, uint8_t* colors);

#endif /* _EMULATOR_H_ */
