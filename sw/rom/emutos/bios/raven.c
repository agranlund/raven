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



// todo: these should just redirect to Raven bootbios api

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



#endif /* MACHINE_RAVEN */
