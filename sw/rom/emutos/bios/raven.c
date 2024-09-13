/*
 * raven.c - Raven specific functions
 *
 * Copyright (C) 2013-2024 The EmuTOS development team
 *
 * Authors:
 *  Anders Granlund
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#ifdef MACHINE_RAVEN

#include "emutos.h"
#include "raven.h"
#include "vectors.h"
#include "asm.h"
#include "tosvars.h"
#include "bios.h"
#include "processor.h"
#include "biosext.h"            /* for cache control routines */
#include "gemerror.h"
#include "ikbd.h"               /* for call_mousevec() */
#include "screen.h"
#include "nova.h"
#include "mfp.h"
#include "delay.h"
#include "asm.h"
#include "string.h"
#include "disk.h"
#include "biosmem.h"
#include "bootparams.h"
#include "machine.h"
#include "has.h"
#include "../bdos/bdosstub.h"

#include "raven.h"

extern void raven_int_ikbd(void);
extern void raven_int_vbl(void);

void raven_screen_init(void)
{
    sshiftmod = ST_HIGH;
    VEC_VBL = raven_int_vbl;
}

const UBYTE *raven_physbase(void)
{
#if CONF_WITH_NOVA
    const UBYTE* nova_addr = get_novamembase();
    if (nova_addr)
        return nova_addr;
#endif
    return logbase();
}

void raven_kbd_init(void)
{
    *((ULONG*)0x74) = (ULONG)raven_int_ikbd;
    volatile UBYTE* uart = (volatile UBYTE*)RAVEN_UART1_BASE;
    uart[0x08] |= 0x01;     // RX fifo enabled
    uart[0x04]  = 0x01;     // RHR interrupts enabled
}

// todo: redirect to bootbios api

#define RAVEN_MFP2_BASE ((MFP *)RAVEN_PADDR_MFP2)

LONG raven_ikbd_bcostat(void)
{
    return (*((volatile UBYTE*)RAVEN_UART1_BASE+0x14) & (1 << 5)) ? - 1 : 0;
}

void raven_ikbd_writeb(UBYTE b)
{
    *((volatile UBYTE*)RAVEN_UART1_BASE+0x00) = b;
}

LONG raven_ikbd_bconstat(void)
{
    return (*((volatile UBYTE*)RAVEN_UART1_BASE+0x14) & (1 << 0)) ? - 1 : 0;
}

UBYTE raven_ikbd_readb(void)
{
    return *((volatile UBYTE*)RAVEN_UART1_BASE+0x00);
}

LONG raven_midi_bcostat(void)
{
    return (RAVEN_MFP2_BASE->tsr & 0x80) ? -1 : 0;
}

void raven_midi_writeb(UBYTE b)
{
    RAVEN_MFP2_BASE->udr = (char)b;
}

LONG raven_midi_bconstat(void)
{
    return (RAVEN_MFP2_BASE->rsr & 0x80) ? -1 : 0;
}

UBYTE raven_midi_readb(void)
{
    return RAVEN_MFP2_BASE->udr;
}



#define RAVEN_BIOS_BASEPTR      0x40000000UL
#define RAVEN_BIOS_RTCREAD      0x40
#define RAVEN_BIOS_RTCWRITE     0x44

#define RAVEN_RTC_EMUL_START    14
#define RAVEN_RTC_REAL_START    8

typedef void(*raven_nvram_func)(ULONG,UBYTE*,ULONG);

static inline UBYTE int2bcd(UWORD a) { return (a % 10) + ((a / 10) << 4); }
static inline UWORD bcd2int(UBYTE a) { return (a & 15) + ((a >> 4) * 10); }

static inline UBYTE nvram_read_raw(int index)
{
    UBYTE value = 0;
    ULONG rv = *((ULONG*)RAVEN_BIOS_BASEPTR);
    WORD old_sr = set_sr(0x2700);
    raven_nvram_func f = *((raven_nvram_func*)(rv + RAVEN_BIOS_RTCREAD));
    f(index, &value, 1);
    set_sr(old_sr);
    return value;
}

static inline void nvram_write_raw(int index, UBYTE value)
{
    ULONG rv = *((ULONG*)RAVEN_BIOS_BASEPTR);
    WORD old_sr = set_sr(0x2700);
    raven_nvram_func f = *((raven_nvram_func*)(rv + RAVEN_BIOS_RTCWRITE));
    f(index, &value, 1);
    set_sr(old_sr);
}

UBYTE raven_nvram_readb(int index)
{
    switch (index)
    {
        case  0: return bcd2int(nvram_read_raw(0) & 0x7f);      // seconds
        case  1: return 0x00;
        case  2: return bcd2int(nvram_read_raw(1) & 0x7f);      // minutes
        case  3: return 0x00;
        case  4: return bcd2int(nvram_read_raw(2) & 0x3f);      // hours
        case  5: return 0x00;
        case  6: return 0x00;
        case  7: return bcd2int(nvram_read_raw(4) & 0x3f);      // date
        case  8: return bcd2int(nvram_read_raw(5) & 0x1f);      // month
        case  9: return bcd2int(nvram_read_raw(6) & 0xff);      // year
        case 10: return 0x00;
        case 11: return 0x00;
        case 12: return 0x00;
        case 13: return 0x80;
        default: return nvram_read_raw(index - RAVEN_RTC_EMUL_START + RAVEN_RTC_REAL_START);
    }
}

void raven_nvram_writeb(int index, UBYTE value)
{
    switch (index)
    {
        case  0: nvram_write_raw(0, (nvram_read_raw(0) & 0x80) | (int2bcd(value) & 0x7f)); break;   // seconds
        case  1: break;
        case  2: nvram_write_raw(1, int2bcd(value) & 0x7f); break;                                  // minutes
        case  3: break;
        case  4: nvram_write_raw(2, int2bcd(value) & 0x3f); break;                                  // hours
        case  5: break;
        case  6: break;
        case  7: nvram_write_raw(4, int2bcd(value) & 0x3f); break;                                  // date
        case  8: nvram_write_raw(5, int2bcd(value) & 0x1f); break;                                  // month
        case  9: nvram_write_raw(6, int2bcd(value) & 0xff); break;                                  // year
        case 10: break;
        case 11: nvram_write_raw(0, (nvram_read_raw(0) & 0x7f) | (value & 0x80)); break;
        case 12: break;
        case 13: break;
        default: return nvram_write_raw(index - RAVEN_RTC_EMUL_START + RAVEN_RTC_REAL_START, value);
    }
}

void raven_nvram_detect(void)
{
    has_nvram = 1;
}

#endif /* MACHINE_RAVEN */
