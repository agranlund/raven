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
static const char* mididev_null_names[] = {"None", 0};
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

void midi_SetTxDevice(rvdev_miditx_t* dev) {
    dev = dev ? dev : &mididev_nulltx;
    if (dev != miditx) {
        uint16_t sr;
        if (miditx) {
            /* todo: send noteoff */
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

    /* load drivers */

    /* select driver */

    return true;
}
