/*-------------------------------------------------------------------------------
 * flash
 * (c)2025 Anders Granlund
 *
 * ROM flasher
 *
 *-------------------------------------------------------------------------------
 *
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *-----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <mint/osbind.h>
#include "raven.h"

static long tIDA;      /* id access and exit */
static long tSE;       /* sector erase */
static long tSCE;      /* chip erase */
static long tBP;       /* program time */

static void nop(void) 0x4E71;
extern uint16_t setsr(uint16_t sr);
extern void restart(void);

static uint32_t flash_addr;
static uint32_t flash_size;
static uint32_t flash_desc;
static uint16_t flash_bits;

static uint32_t loops_per_125ms;
static void fDelay(uint32_t us) {
    uint32_t i, loops;
    if (us > 10000) {
        loops = 1 + ((loops_per_125ms * (us / 1000)) / 125);
    } else {
        loops = 1 + ((loops_per_125ms * us ) / (125 * 1000UL));
    }
    for (i=0; i<=loops; i++) {
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
    }
    return;
}

static void fCalibrate(void) {
    uint32_t tick_start = *((volatile uint32_t*)0x4ba);
    uint32_t tick_end = tick_start;
    loops_per_125ms = 0;
    do {
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
        tick_end = *((volatile uint32_t*)0x4ba);
        loops_per_125ms++;
        if (loops_per_125ms > 1000000UL) {
            loops_per_125ms = 0;
            break;
        }
    } while ((tick_end - tick_start) <= 25UL);

    tIDA =      1UL;    /* 150 ns */
    tSE  =  25000UL;    /*  25 ms */
    tSCE = 100000UL;    /* 100 ms */
    tBP  =     20UL;    /*  20 us */
}

/*-----------------------------------------------------------------------------*/
static void fIndicate(bool en)
{
    if (en) {
        *((volatile uint8_t*)(RV_PADDR_UART1 + 0x10)) &= ~1;    /* pwr led on */
    } else {
        *((volatile uint8_t*)(RV_PADDR_UART1 + 0x10)) |= 1;     /* pwr led off */
    }
}

/*-----------------------------------------------------------------------------*/

static void fWrite(uint32_t addr, uint32_t data) {
    *((volatile uint32_t*)(flash_addr+addr)) = data;
}

static uint32_t fRead(uint32_t addr) {
    return *((volatile uint32_t*)(flash_addr+addr));
}

static void fCmd(uint32_t addr, uint32_t cmd) {
    fWrite(0x5555UL << 2, 0xAAAAAAAAUL);
    fWrite(0x2AAAUL << 2, 0x55555555UL);
    fWrite(addr << 2, cmd);
}

static void fIdent(uint32_t* did, uint32_t* mid) {
    fCmd(0x5555UL, 0x90909090UL);
    fDelay(tIDA);
    *mid = fRead(0x0000UL);
    *did = fRead(0x0004UL);
    fCmd(0x5555UL, 0xF0F0F0F0UL);
}

/*-----------------------------------------------------------------------------*/
void flash_Close(void) {
    /* restore rom page descriptors */
    uint16_t sr = setsr(0x2700);
    raven()->mmu_Map(flash_addr, flash_addr, flash_size, flash_desc);
    raven()->mmu_Flush();
    setsr(sr);
}

/*-----------------------------------------------------------------------------*/
bool flash_Open() {
    uint16_t sr;
    uint16_t mid, did;
    uint32_t lmid, ldid;

    /* calibrate delay loop */
    fCalibrate();

    /* defaults */
    flash_bits = 16;
    flash_addr = RV_PADDR_SIMM3;
    flash_size = 0x20000UL;
    flash_desc = *raven()->mmu_GetPageDescriptor(flash_addr) & 0x000000FFUL;

    /* make flash commands writable */
    sr = setsr(0x2700);
    raven()->mmu_Map(flash_addr, flash_addr, flash_size, (2 << 5) | (1 << 3) | (3 << 0));  /* PRECISE | USED | READ_WRITE */
    raven()->mmu_Flush();

    /* identify device*/
    fIdent(&ldid, &lmid);
    setsr(sr);

    /* sanity check */
    if (((lmid >> 16) != (lmid & 0xffff)) || ((ldid >> 16) != (ldid & 0xffff))) {
        flash_Close();
        return 0;
    }

    /* bit depth */
    mid = lmid & 0xffff;
    did = ldid & 0xffff;
    if ((((mid >> 8) & 0xff) == (mid & 0xff)) && (((did >> 8) & 0xff) == (did & 0xff))) {
        flash_bits = 8;
        mid = mid & 0xff;
        did = did & 0xff;
    }

    /* todo: validate against supported devices */
    printf(" device id......%04x:%04x\n", mid, did);
    return true;
}

/*-----------------------------------------------------------------------------*/

bool flash_Program(void* data, int32_t size) {
    uint16_t sr;
    uint32_t pos, val, *ptr;
    uint32_t align_size = (32UL * 1024);

    /* no interrupts from here on */
    sr = setsr(0x2700);

    /* make entire flash writable */
    flash_size = (size + align_size-1) & ~(align_size-1);
    raven()->mmu_Map(flash_addr, flash_addr, flash_size, (2 << 5) | (1 << 3) | (3 << 0));  /* PRECISE | USED | READ_WRITE */
    raven()->mmu_Flush();

    /* erase */
    fIndicate(false);
    fCmd(0x5555UL, 0x80808080UL);
    fCmd(0x5555UL, 0x10101010UL);
    fDelay(tSCE);
    fIndicate(true);

    /* program */
    ptr = (uint32_t*) data;
    for (pos = 0; pos < size; pos += 4)
    {
        int32_t timeout = tBP;
        val = *ptr++;
        fCmd(0x5555UL, 0xA0A0A0A0);
        fWrite(pos, val);
        fIndicate((pos & 0x4000UL) ? true : false);
        fDelay(tBP);
    }

    /* restart */
    fIndicate(true);
    restart();
}
