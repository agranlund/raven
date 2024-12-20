/*
 * midi.c - MIDI routines
 *
 * Copyright (C) 2001-2019 Martin Doering
 *
 * Authors:
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "acia.h"
#include "iorec.h"
#include "asm.h"
#include "midi.h"
#ifdef MACHINE_RAVEN
#include "mfp.h"
#include "raven.h"
#endif

/*==== MIDI bios functions =========================================*/

LONG bconstat3(void)
{
#if CONF_WITH_MIDI_ACIA
    if (midiiorec.head == midiiorec.tail)
    {
        return 0;   /* iorec empty */
    }
    else
    {
        return -1;  /* not empty => input available */
    }
#elif defined(MACHINE_RAVEN)
    return raven_midi_bconstat();
#else
    return 0;
#endif    
}

LONG bconin3(void)
{
    while(!bconstat3())
        ;

#if CONF_WITH_MIDI_ACIA
    {
        WORD old_sr;
        LONG value;

        /* disable interrupts */
        old_sr = set_sr(0x2700);

        midiiorec.head++;
        if (midiiorec.head >= midiiorec.size)
        {
            midiiorec.head = 0;
        }
        value = *(UBYTE *)(midiiorec.buf+midiiorec.head);

        /* restore interrupts */
        set_sr(old_sr);
        return value;
    }
#elif defined(MACHINE_RAVEN)
    return raven_midi_readb();
#else
    return 0;
#endif
}


/* can we send a byte to the MIDI ACIA ? */
LONG bcostat3(void)
{
#if defined (MACHINE_RAVEN)
    return raven_midi_bcostat();
#elif CONF_WITH_MIDI_ACIA
    if (midi_acia.ctrl & ACIA_TDRE)
    {
        return -1;  /* OK */
    }
    else
    {
        /* Data register not empty */
        return 0;   /* not OK */
    }
#else
    return 0;       /* not OK */
#endif
}

/* send a byte to the MIDI ACIA */
LONG bconout3(WORD dev, WORD c)
{
    while(!bcostat3())
        ;

#if defined(MACHINE_RAVEN)
    raven_midi_writeb((UBYTE)c);
    return 1;
#elif CONF_WITH_MIDI_ACIA
    midi_acia.data = c;
    return 1L;
#else
    return 0L;
#endif
}

/*==== MIDI xbios function =========================================*/

/* cnt = number of bytes to send less one
 *
 * Note: this effectively treats the cnt argument as unsigned, just like
 * Atari TOS does.
 */
void midiws(WORD cnt, const UBYTE *ptr)
{
    do
    {
        bconout3(3, *ptr++);
    } while(cnt--);
}


/*==== midi_init - initialize the MIDI acia ==================*/
/*
 *  Enable receive interrupts, set the clock for 31.25 kbaud
 */
void midi_init(void)
{
#if CONF_WITH_MIDI_ACIA
    /* initialize midi ACIA */
    midi_acia.ctrl = ACIA_RESET;    /* master reset */

    midi_acia.ctrl = ACIA_RIE|      /* enable RxINT */
                     ACIA_RLTID|    /* RTS low, TxINT disabled */
                     ACIA_DIV16|    /* clock/16 */
                     ACIA_D8N1S;    /* 8 bit, 1 stop, no parity */
#endif
}
