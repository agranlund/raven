/*-------------------------------------------------------------------------------
 * rvsnd : isa soundblaster driver
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
#include "isa/isa.h"
#include "sb.h"

static void mixer_outp(uint8_t _r, uint8_t _d)  {
    wss.base[4] = _r;
    wss.base[5] = _d;
}

static uint8_t mixer_inp(uint8_t _r) {
    wss.base[4] = _r;
    return wss.base[5];
}


/* -------------------------------------------------------------------- */
/* init and detect                                                      */
/* -------------------------------------------------------------------- */

bool wss_detect(wss_info_t* out, uint16_t port) {
    uint8_t old04,old05,test;

    /* backup old ioport values */
    old04 = bus->inp(port+0x04);
    bus->outp(port+0x04, 0x00);
    old05 = bus->inp(port+0x05);

    /* test write/read to wss mixer register */
    memset((void*)out, 0, sizeof(wss_info_t));
    bus->outp(port+0x04, 0x00);
    bus->outp(port+0x05, bus->inp(port+0x05) | 0x0f);
    bus->outp(port+0x04, 0x00);
    test = bus->inp(port+0x05);
    if ((test & 0x0f) == 0x0f) {
        bus->outp(port+0x04, 0x00);
        bus->outp(port+0x05, bus->inp(port+0x05) & 0xf0);
        bus->outp(port+0x04, 0x00);
        test = bus->inp(port+0x05);
        test = ((test & 0x0f) == 0) ? 1 : 0;
        if ((test & 0x0f) == 0) {
            out->port = port;
            out->base = (volatile uint8_t*)(bus->iobase + port);
        }
    }

    /* restore ioport values */
    bus->outp(port+0x04, 0x00);
    bus->outp(port+0x05, old05);
    bus->outp(port+0x04, old04);
    return (out->port != 0) ? true : false;
}
