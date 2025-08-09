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
#define PMMU_INVALID            (0 << 0)
#define PMMU_VALID              (3 << 0)
#define PMMU_INDIRECT           (1 << 1)
#define PMMU_WRITEPROTECT       (1 << 2)
#define PMMU_USED               (1 << 3)
#define PMMU_SUPER              (1 << 7)
#define PMMU_CM_WRITETHROUGH    (0 << 5)
#define PMMU_CM_COPYBACK        (1 << 5)
#define PMMU_CM_PRECISE         (2 << 5)
#define PMMU_CM_IMPRECISE       (3 << 5)

#define PMMU_READONLY           (PMMU_VALID | PMMU_USED | PMMU_WRITEPROTECT)
#define PMMU_READWRITE          (PMMU_VALID | PMMU_USED)

extern uint16_t cpu_di(void);
extern uint16_t cpu_ei(uint16_t sr);
static void     cpu_nop(void) 0x4E71;
static void     cpu_map(uint32_t log, uint32_t phys, uint32_t size, uint32_t flags) { raven()->mmu_Map(log, phys, size, flags); }
static void     cpu_flush_atc() { raven()->mmu_Flush(); }
static void     cpu_flush_cache() { raven()->cache_Flush(); }

/*-----------------------------------------------------------------------------*/
#define PADDR_IO        RV_PADDR_ISA_IO
#define PADDR_MEM       RV_PADDR_ISA_RAM16

#define VADDR_MEM       0x44000000UL    /* logical mem space */
#define VADDR_IO        0x44F00000UL    /* logical io space */

#define VSIZE_MEM       0x00400000UL    /*  4Mb virtual mem space */
#define VSIZE_IO        0x00010000UL    /* 64kb virtual io space  */

/*-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
extern nova_xcb_t nova;

/* initialise system and driver */
extern bool nv_init(void);

/* set graphics mode */
extern bool nv_setmode(uint16_t w, uint16_t h, uint16_t b);

/* initialize vram and banks, if any */
extern void nv_init_vram(uint32_t phys, uint32_t size, uint32_t count);




/* standard vga or vesa functionality */
extern void nv_vesa_vsync(void);
extern bool nv_vesa_setmode(uint16_t mode);
extern void nv_vesa_setcolors(uint16_t index, uint16_t count, uint8_t* colors);
extern void nv_vesa_getcolors(uint16_t index, uint16_t count, uint8_t* colors);

#endif /* _EMULATOR_H_ */
