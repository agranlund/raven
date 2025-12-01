/*-------------------------------------------------------------------------------
 * rvsnd : raven midiport driver
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

#include "driver.h"
#include "lib/raven.h"

/* -------------------------------------------------------------------- */
/* midi out                                                             */
/* -------------------------------------------------------------------- */

static int32_t cdecl miditx_st(void) {
    volatile uint8_t tsr = *((volatile uint8_t*)(RV_PADDR_MFP2+45));
    return (tsr & 0x80) ? -1L : 0L;
}
static void cdecl miditx_tx(uint32_t c) {
    *((volatile uint8_t*)(RV_PADDR_MFP2+47)) = (uint8_t)(c & 0xff);
}

/* -------------------------------------------------------------------- */
/* midi in                                                              */
/* -------------------------------------------------------------------- */
static int32_t cdecl midirx_st(void) {
    volatile uint8_t rsr = *((volatile uint8_t*)(RV_PADDR_MFP2+43));
    return (rsr & 0x80) ? -1L : 0L;
}
static int32_t cdecl midirx_rx(void) {
    return (int32_t) *((volatile uint8_t*)(RV_PADDR_MFP2+47));
}

/* -------------------------------------------------------------------- */
/* driver                                                               */
/* -------------------------------------------------------------------- */
static const char* dnames[] = {"Raven", 0};
static const rvdev_miditx_t txdev = {
    RVDEV_MIDI_OUT, 0, RV_PADDR_MFP2,
    dnames,
    0, 0,
    miditx_st,
    miditx_tx
};

static const rvdev_midirx_t rxdev = {
    RVDEV_MIDI_IN, 0, RV_PADDR_MFP2,
    dnames,
    0, 0,
    midirx_st,
    midirx_rx
};

int32_t init(void) {
    if (!rvsnd->getcookie("RAVN", 0)) {
        return -1;
    }
    dev_publish(&txdev);
    dev_publish(&rxdev);
    return 0;
}
