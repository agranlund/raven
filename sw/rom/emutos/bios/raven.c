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
extern void raven_int_50hz(void);

void raven_mfp_init(void)
{
    // mfp2
    MFP *mfp = RAVEN_MFP2_BASE;
    mfp->gpip   = 0x00;
    mfp->aer    = 0xFF;         /* ISA interrupts trigger on low->high */
    mfp->ddr    = 0x00;         /* ISA interrupts are inputs */
    mfp->iera   = 0x00;
    mfp->ierb   = 0x00;
    mfp->ipra   = 0x00;
    mfp->iprb   = 0x00;
    mfp->isra   = 0x00;
    mfp->isrb   = 0x00;
    mfp->imra   = 0x00;
    mfp->imrb   = 0x00;
    mfp->vr     = 0x58;         /* vectors 0x50 to 0x5F, software end of interrupt */
    mfp->tacr   = 0x00;
    mfp->tbcr   = 0x00;
    mfp->tcdcr  = 0x00;
    mfp->tadr   = 0x00;
    mfp->tbdr   = 0x00;
    mfp->tcdr   = 0x00;
    mfp->tddr   = 0x00;
    mfp->scr    = 0x00;
    mfp->ucr    = 0x00;
    mfp->rsr    = 0x00;
    mfp->tsr    = 0x00;
    mfp->udr    = 0x00;

    // todo: midi uart mfp2

    // todo: i2c setup mfp1 

    // i3 = clk
    // i7 = dta

}


void raven_screen_init(void)
{
    sshiftmod = ST_HIGH;

    // interrupt vectors
    VEC_VBL = raven_int_vbl;                    // 0000:VBLANK
    *((ULONG*)0x154) = (ULONG)raven_int_50hz;   // MFP2:TimerC

    // Set MFP2:TimerC to 50hz
    // Base frequency is 2000000hz
    MFP *mfp = RAVEN_MFP2_BASE;
    mfp->tcdr   = 200;                          // count 200     => 10000hz
    mfp->tcdcr  = (mfp->tcdcr & 0x7) | 0x70;    // divide by 200 => 50hz
    mfp->ierb   |= (1 << 5);                    // enable timerC interrupt
    mfp->imrb   |= (1 << 5);                    // enable timerC interrupt
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

#endif /* MACHINE_RAVEN */
