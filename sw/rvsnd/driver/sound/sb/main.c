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

#define SB_SUPPORT_PNP          1
#define SB_DEFAULT_PORT_MIN     0x220
#define SB_DEFAULT_PORT_MAX     0x280

isa_t* bus;                 /* isa bus */
volatile uint8_t* sbase;    /* base address */
uint16_t sbport;            /* base port */
uint16_t sbirq;             /* base irq */
uint16_t sbtype;            /* soundblaster variation */
uint16_t wssport;           /* windows soundsystem port */

const uint16_t sb_port_list[]  = { 0x220, 0x230, 0x240, 0x250, 0x260, 0x270, 0x280 };
const uint16_t wss_port_list[] = { 0x530, 0x604, 0xE80, 0xF40 };


/* todo:
 *
 * - settings from inifile
 *   - option to use FM control for wavetable volume like standard soundblaster
 *     (binding AUX2 with FM together)
 *   - autodetect force disable
 *   - pnp force disable
 *   - manual hardware settings
 *     - sb_port, wss_port, sb_irq
 *
 * - hook irq functionality to existing mpu401 driver if found installed,
 *   alternatively, spawn our own mpu401 driver from here.
 * 
 * - drivers in general should be able to provide some kind of
 *   GUI name for main type and subtype.
 *   I want to control it with device name "soundblaster" or "sb"
 *   but also be able to show additional types of info.
 *    - "ESS186X"
 *    - "Gravis Ultrasound", GF1 or AMD, Amount of memory.
 *    - and so on. Perhaps hardcoded three lines?
 *      - id     "gus"  
 *      - name   "Gravis Ultrasound (GF1)"
 *      - info   "4MB memory"
 * 
 *      - id     "sb"
 *      - name   "SoundBlaster 16"
 *      - info   ""
 */


/* -------------------------------------------------------------------- */
/* raw device                                                           */
/* -------------------------------------------------------------------- */

#define PUBLISH_RAWDEV  0

#if PUBLISH_RAWDEV
uint16_t dev_read_byte(uint16_t reg) { return bus->inp(sbport + reg); }
void dev_write_byte(uint16_t reg, uint16_t data) { bus->outp(sbport + reg, data); }
static const char* dnames[] = {"SB", 0 };
static rvdev_raw_t dev = {
    RVDEV_RAW, 0, 0,
    dnames,
    0, 0,
    dev_write_byte, dev_write_byte,
    dev_read_byte, dev_read_byte
};
static void publish_rawdev(void) {
    static const char* names[8];
    uint16_t count = 0;
    if (sbtype == SBTYPE_ESS)        { names[count++] = "ES186X"; }
    else if (sbtype == SBTYPE_SB16)  { names[count++] = "SB16";   }
    if (sbtype >= SBTYPE_SBPRO)      { names[count++] = "SBPRO";  }
    names[count++] = "SB";
    names[count++] = 0;
    dev.names = names;
    rvsnd->publish(rvdev_base(&dev));
}
#endif


/* -------------------------------------------------------------------- */
/* driver                                                               */
/* -------------------------------------------------------------------- */

bool detect_wss(uint16_t port) {
    uint8_t old04,old05,test;

    /* backup old ioport values */
    old04 = bus->inp(port+0x04);
    bus->outp(port+0x04, 0x00);
    old05 = bus->inp(port+0x05);

    /* test write/read to wss mixer register */
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
    } else {
        test = 0;
    }

    /* restore ioport values */
    bus->outp(port+0x04, 0x00);
    bus->outp(port+0x05, old05);
    bus->outp(port+0x04, old04);
    return (test == 0) ? false : true;
}

bool detect_ess(uint16_t port) {
    uint8_t id[4];
    bus->outp(port + 0x04, 0x40);  delayus(1000);
    id[0] = bus->inp(port + 0x05); delayus(1000);
    id[1] = bus->inp(port + 0x05); delayus(1000);
    id[2] = bus->inp(port + 0x05); delayus(1000);
    id[3] = bus->inp(port + 0x05); delayus(1000);
    if ((id[0] == 0x18) && ((id[1] == 0x68) || (id[1] == 0x69))) {
        return true;
    }
    return false;
}

bool detect_sb(uint16_t port) {
    const uint32_t dsp_init_delay = 3000UL;
    uint8_t version_hi = 0;
    uint8_t version_lo = 0;

    /* reset soundblaster dsp */
    uint8_t save06 = bus->inp(port + 0x06);
    bus->outp(port + 0x06, 0x01);
    delayus(dsp_init_delay);
    bus->outp(port + 0x06, 0x00);
    delayus(dsp_init_delay);

    /* dsp readport should return 0xAA after reset */
    if (bus->inp(port + 0x0A) != 0xAA) {
        delayus(dsp_init_delay);
        bus->outp(port + 0x06, save06);
        delayus(dsp_init_delay);
        sbport = 0;
        sbase = 0;
        sbtype = SBTYPE_NONE;
        return false;
    }

    /* check dsp version */
    bus->outp(port + 0x0C, 0xE1);
    delayus(dsp_init_delay);
    version_hi = bus->inp(port + 0x0A);
    delayus(dsp_init_delay);
    version_lo = bus->inp(port + 0x0A);
    delayus(dsp_init_delay);

    /* use this device */
    if (version_hi >= 4) {
        sbtype = SBTYPE_SB16;
    } else if (version_hi >= 3) {
        sbtype = SBTYPE_SBPRO;
    } else if ((version_hi >= 2) && (version_lo > 0)) {
        sbtype = SBTYPE_SB2;
    } else {
        sbtype = SBTYPE_SB1;
    }

    /* detect ES186X soundblaster clones */
    if (sbtype != SBTYPE_NONE) {
        if (detect_ess(port)) {
            sbtype = SBTYPE_ESS;
        }
    }

    sbport = port;
    sbase = (volatile uint8_t*)((uint32_t)bus->iobase + sbport);
    return true;
}

static bool detect_pnp(void) {
    #if SB_SUPPORT_PNP
    typedef struct { const char* id; uint16_t type; } sbpnp_t;
    static const sbpnp_t pnpids[] = {
        { "ESS1869", SBTYPE_SBPRO   },          /* es186x */
        { "ESS1868", SBTYPE_SBPRO   },          /* ex186x */
        { "YMH0021", SBTYPE_SBPRO   },          /* opl3-sa2 */
        { "NMX2210", SBTYPE_SBPRO   },          /* opl3-sa2 */
        { "PNPB003", SBTYPE_SB16    },          /* soundblaster 16  */
        { "PNPB002", SBTYPE_SBPRO   },          /* soundblaster pro */
        { "PNPB001", SBTYPE_SB2     },          /* soundblaster 2.0 */
        { "PNPB000", SBTYPE_SB1     },          /* soundblaster 1.5 */
        { 0,         0              }
    };

    uint16_t type, idx;
    for (type = 0; pnpids[type].id; type++) {
        const sbpnp_t* pnp = &pnpids[type];
        for (idx = 0; idx < 4; idx++) {
            isa_dev_t* dev = bus->find_dev(pnp->id, idx);
            if (!dev) { break; }
            else if (dev && dev->port[0]) {
                if (detect_sb(dev->port[0])) {
                    sbirq = dev->irq[0];
                    if ((bus->irqmask & (1 << sbirq)) == 0) {
                        sbirq = 0;
                    }
                    return true;
                }
            }
        }
    }
    #endif
    return false;
}

int32_t init(void) {

    bool use_pnp = true;
    bool use_probe = true;

    bus = rvsnd->isa;
    if (!bus) {
        return -1;
    }

    /* todo: try port from inifile */
    sbtype = SBTYPE_NONE;

    /* plug and play detect*/
    if (use_pnp && (sbtype == SBTYPE_NONE)) {
        detect_pnp();
    }
    
    /* brute force detect */
    if (use_probe && (sbtype == SBTYPE_NONE)) {
        uint16_t i;
        for (i = 0; i < sizeof(sb_port_list) / sizeof(sb_port_list[0]); i++) {
            uint16_t port = sb_port_list[i];
            if (detect_sb(port)) {
                break;
            }
        }
    }

    if (sbtype == SBTYPE_NONE) {
        return -1;
    }

    /* detect wss support */
    if (sbtype == SBTYPE_SBPRO) {
        /* todo: from inifile */

        /* brute force detect */
        if (wssport == 0) {
            uint16_t i;
            for (i = 0; i < sizeof(wss_port_list) / sizeof(wss_port_list[0]); i++) {
                uint16_t port = wss_port_list[i];
                if (detect_wss(port)) {
                    wssport = port;
                    break;
                }
            }
        }
    }

    /* initialize mixer and devices */
    mixer_init();

#if PUBLISH_RAWDEV
    publish_rawdev();
#endif

    return 0;
}
