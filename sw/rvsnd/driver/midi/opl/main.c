/*-------------------------------------------------------------------------------
 * rvsnd : opl softsynth midi output driver
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
#include "../../../core/midimsg.h"


/* -------------------------------------------------------------------- */
/* softsynth interface                                                  */
/* -------------------------------------------------------------------- */
extern bool OPLWIN_MIDI_init(int16_t opltype);
extern void OPLWIN_MIDI_write(uint32_t packet);

static rvdev_raw_t* opl = 0;
void opl_writereg(uint32_t reg, uint8_t data) {
    opl->wrb((uint16_t)reg, data);
}


/* -------------------------------------------------------------------- */
/* driver                                                               */
/* -------------------------------------------------------------------- */
static int32_t cdecl miditx_st(void) {
    return -1L;
}

static void cdecl miditx_tx(uint32_t c) {
    static midiparser_t parser;
    if (midi_parse(&parser, c)) {
        if (parser.msg.msg.type != MIDIMSG_SYSEX) {
            OPLWIN_MIDI_write(parser.msg.packet);
        }
    }
}

static const char* dnames[] = {"OPL", 0};
static const rvdev_miditx_t dev = {
    RVDEV_MIDI_OUT, 0, 0,
    dnames,
    0, 0,
    miditx_st,
    miditx_tx
};

int32_t init(void) {
    rvdev_t* opldev = 0;
    int16_t opltype = 3;

    /* request opl hardware driver */
    opldev = rvsnd->retrieve(RVDEV_RAW, "OPL3");
    opltype = 3;
    if (!opldev) {
        opldev = rvsnd->retrieve(RVDEV_RAW, "OPL2");
        opltype = 2;
        if (!opldev) {
            return -1;
        }
    }

    /* initialize softsynth */
    opl = rvdev_cast(rvdev_raw_t, opldev);
    if (!OPLWIN_MIDI_init(opltype)) {
        return -1;
    }

    /* publish midi driver */
    dev_publish(&dev);
    return 0;
}
