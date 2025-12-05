/*-------------------------------------------------------------------------------
 * rvsnd midi
 * (c)2025 Anders Granlund
 *-------------------------------------------------------------------------------
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
 *-----------------------------------------------------------------------------*/

#include "midi.h"

/* -------------------------------------------------------------------- */
/* null driver                                                          */
/* -------------------------------------------------------------------- */
static const char* mididev_null_names[] = {"Null", 0};
static int32_t cdecl mididev_nulltx_st(void)       { return -1L; }
static void    cdecl mididev_nulltx_tx(uint32_t d) { UNUSED(d); }
static int32_t cdecl mididev_nullrx_st(void)       { return 0L; }
static int32_t cdecl mididev_nullrx_rx(void)       { return 0; }
static rvdev_miditx_t mididev_nulltx = { RVDEV_MIDI_OUT, 0, 0, mididev_null_names, 0, 0, mididev_nulltx_st, mididev_nulltx_tx };
static rvdev_midirx_t mididev_nullrx = { RVDEV_MIDI_IN,  0, 0, mididev_null_names, 0, 0, mididev_nullrx_st, mididev_nullrx_rx };

/* -------------------------------------------------------------------- */
/* tos driver                                                           */
/* -------------------------------------------------------------------- */
static const char* mididev_tos_names[] = {"Tos", 0};
extern void     cdecl (*cur_bconout3)(uint32_t d);
extern int32_t  cdecl (*cur_bconin3)(void);
extern int32_t  cdecl (*cur_bcostat3)(void);
extern int32_t  cdecl (*cur_bconstat3)(void);
extern void     cdecl (*tos_bconout3)(uint32_t d);
extern int32_t  cdecl (*tos_bconin3)(void);
extern int32_t  cdecl (*tos_bcostat3)(void);
extern int32_t  cdecl (*tos_bconstat3)(void);
static rvdev_miditx_t mididev_tostx = { RVDEV_MIDI_OUT, 0, 0, mididev_tos_names, 0, 0, 0, 0 };
static rvdev_midirx_t mididev_tosrx = { RVDEV_MIDI_IN,  0, 0, mididev_tos_names, 0, 0, 0, 0 };

/* -------------------------------------------------------------------- */
/*                                                                      */
/* -------------------------------------------------------------------- */

static rvdev_miditx_t* miditx = 0;
static rvdev_midirx_t* midirx = 0;

static bool midi_SendWithTimeout(rvdev_miditx_t* dev, uint8_t data) {
    uint32_t start = *((volatile uint32_t*)0x4ba);
    uint32_t timeout = (2000 / 5);
    while (dev->st() == 0) {
        uint32_t now = *((volatile uint32_t*)0x4ba);
        if (now < start) { start = now; }
        if ((now - start) > timeout) { return false; }
    }
    dev->tx(data);
    return true;
}

static void midi_AllNotesOff(rvdev_miditx_t* dev) {
    uint8_t ch;
    for (ch=0; ch<16; ch++) {
        #if 0
        /* omni mode off */
        if (!midi_SendWithTimeout(dev, 0xb0|ch)) return;
        if (!midi_SendWithTimeout(dev, 0x7c)) return;
        if (!midi_SendWithTimeout(dev, 0x00)) return;
        #endif        
        /* all notes off */
        if (!midi_SendWithTimeout(dev, 0xb0|ch)) return;
        if (!midi_SendWithTimeout(dev, 0x7b)) return;
        if (!midi_SendWithTimeout(dev, 0x00)) return;
        #if 1
        /* all sound off */
        if (!midi_SendWithTimeout(dev, 0xb0|ch)) return;
        if (!midi_SendWithTimeout(dev, 0x78)) return;
        if (!midi_SendWithTimeout(dev, 0x00)) return;
        #endif        
    }
}

void midi_SetTxDevice(rvdev_miditx_t* dev) {
    dev = dev ? dev : &mididev_nulltx;
    if (dev != miditx) {
        uint16_t sr;
        if (miditx) {
            midi_AllNotesOff(miditx);
            if (miditx->stop) {
                miditx->stop();
            }
        }
        sr = sys_di();
        miditx = dev;
        cur_bcostat3 = miditx->st;
        cur_bconout3 = miditx->tx;
        sys_ei(sr);
        if (miditx->start) {
            miditx->start();
        }
    }
}

void midi_SetRxDevice(rvdev_midirx_t* dev) {
    dev = dev ? dev : &mididev_nullrx;
    if (dev != midirx) {
        uint16_t sr;
        if (midirx && midirx->stop) {
            midirx->stop();
        }
        sr = sys_di();
        midirx = dev;
        cur_bconstat3 = midirx->st;
        cur_bconin3 = midirx->rx;
        sys_ei(sr);
        if (midirx->start) {
            midirx->start();
        }
    }
}

rvdev_miditx_t* midi_GetTxDevice(void) {
    return miditx;
}

rvdev_midirx_t* midi_GetRxDevice(void) {
    return midirx;
}


/* -------------------------------------------------------------------- */
/* sys                                                                  */
/* -------------------------------------------------------------------- */

bool midi_Init(void) {

    /* null driver should always exist */
    driver_AddDevice(rvdev_base(&mididev_nulltx));
    driver_AddDevice(rvdev_base(&mididev_nullrx));
    midi_SetTxDevice(&mididev_nulltx);
    midi_SetRxDevice(&mididev_nullrx);
   
    /* standard tos device */
#if 0    
    mididev_tostx.st = tos_bcostat3;
    mididev_tostx.tx = tos_bconout3;
    mididev_tosrx.st = tos_bconstat3;
    mididev_tosrx.rx = tos_bconin3;
    if (1) {
        driver_AddDevice(rvdev_base(&mididev_tostx));
        driver_AddDevice(rvdev_base(&mididev_tosrx));
        midi_SetTxDevice(&mididev_tostx);
        midi_SetRxDevice(&mididev_tosrx);
    }
#endif        

    return true;
}
