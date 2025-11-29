/*-------------------------------------------------------------------------------
 * rvsnd : opl isa driver
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

#define OPL_DEFAULT_PORT 0x388

static isa_t* isa = 0;
static uint16_t opl_port = 0;
static uint16_t opl_chip = 0;
static uint16_t opl_delay_idx = 8;      /* opl2 index write delay */
static uint16_t opl_delay_val = 32;     /* opl2 value write delay */

static void opl_write(uint16_t idx, uint8_t val) {
    if (opl_chip != 0) {
        if (idx >= 0x100) {
            /* opl3 second register set */
            isa->outp(opl_port + 2, (idx - 0x100));
            delayus(opl_delay_idx);
            isa->outp(opl_port + 3, val);
            delayus(opl_delay_val);
        } else {
            /* opl2 or opl3 first register set */
            isa->outp(opl_port + 0, idx);
            delayus(opl_delay_idx);
            isa->outp(opl_port + 1, val);
            delayus(opl_delay_val);

        }
    }
}

static uint8_t opl_read(uint16_t idx) {
    (void)idx; return 0xff;
}

/* ------------------------------------------------------------------- */
void dev_write_byte(uint16_t reg, uint16_t data) { opl_write(reg, data); }
void dev_write_word(uint16_t reg, uint16_t data) { opl_write(reg, data); }
uint16_t dev_read_byte(uint16_t reg) { return opl_read(reg); }
uint16_t dev_read_word(uint16_t reg) { return opl_read(reg); }

static const char* dnames[] = {"OPL3", "OPL2", 0};
static rvdev_raw_t dev = {
    RVDEV_RAW, 0, 0,
    dnames,
    0, 0,
    dev_write_byte,
    dev_write_word,
    dev_read_byte,
    dev_read_word
};


/* ------------------------------------------------------------------- */
bool detect(uint16_t port) {
    uint8_t result1, result2;

    /* detect opl version */
    opl_chip = 2;               /* assume opl2 for now */
    opl_port = port;
    opl_write(0x04, 0x60);    /* reset timer1+2 and irq */
    opl_write(0x04, 0x80);
    result1 = isa->inp(opl_port);
    opl_write(0x02, 0xff);    /* set timer1 */
    opl_write(0x04, 0x21);    /* start timer1 */
    delayus(2000);
    result2 = isa->inp(opl_port);
    opl_write(0x04, 0x60);    /* reset timer1+2 and irq */
    opl_write(0x04, 0x80);

    /* now work out if we actually have an opl chip */
    opl_chip = 0;
    if (((result1 & 0xe0) == 0x00) && ((result2 & 0xe0) == 0xc0)) {
        opl_chip = 2;
        if ((result2 & 0x06) == 0x00) {
            opl_chip = 3;
            opl_delay_idx = 4;      /* less delay required on opl3 */
            opl_delay_val = 4;
        }
    }

    return (opl_chip != 0) ? true : false;
}



/* ------------------------------------------------------------------- */
int32_t init(void) {

    /* isa bus required */
    isa = rvsnd->isa;
    if (!rvsnd->isa) {
        return -1;
    }


    /* todo: port from pnp */
    /* todo: port from ini */
    if (!detect(OPL_DEFAULT_PORT)) {
        return -1;
    }

    dev.addr = isa->iobase + opl_port;
    if (opl_chip < 3) {
        dev.names = &dnames[1];
    }

    dev_publish(&dev);
    return 0;
}
