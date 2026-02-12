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

#define SB_SUPPORT_PNP      1

isa_t* bus;

sb_info_t sb;       /* soundblaster */
wss_info_t wss;     /* windows soundsystem */
ess_info_t ess;     /* es186x */
opl3sa_info_t sax;  /* opl3sax */

static const uint16_t sb_port_list[]  = { 0x220, 0x230, 0x240, 0x250, 0x260, 0x270, 0x280, 0 };
static const uint16_t wss_port_list[] = { 0x530, 0x604, 0xE80, 0xF40, 0 };
static const uint16_t sax_port_list[] = { 0xf86, 0x370, 0x100, 0 };


/* todo:
 *
 * - settings from inifile
 *   - option to use FM control for wavetable volume like standard soundblaster
 *     (binding AUX2 with FM together)
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
/* mixer device                                                         */
/* -------------------------------------------------------------------- */
static const char* dev_mixer_names[] = { "Soundblaster", "SB", 0 };
static rvdev_mix_t dev_mixer = { RVDEV_MIXER, 0, 0, dev_mixer_names, 0, 0, 0, 0 };




/* -------------------------------------------------------------------- */
/* driver                                                               */
/* -------------------------------------------------------------------- */


static bool detect_pnp(void) {
    #if SB_SUPPORT_PNP
    typedef struct { const char* id; uint16_t type; } sbpnp_t;
    static const sbpnp_t pnpids[] = {
        { "ESS1869", SBTYPE_ESS     },          /* es186x */
        { "ESS1868", SBTYPE_ESS     },          /* ex186x */
        { "YMH0021", SBTYPE_OPL3SA  },          /* opl3-sa2 */
        { "NMX2210", SBTYPE_OPL3SA  },          /* opl3-sa2 */
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
                /* detect standard soundblasters */
                if (sb_detect(&sb, dev->port[0])) {

                    /* get irq number from pnp info */
                    if ((bus->irqmask & (1 << dev->irq[0]))) {
                        sb.irq = dev->irq[0];
                    }

                    /* detect extended clones */
                    switch (pnp->type) {
                        case SBTYPE_ESS: {
                            ess_detect(&ess, &sb);
                        } break;

                        case SBTYPE_OPL3SA: {
                            sax_detect(&sax, &sb, dev->port[4]);
                            wss_detect(&wss, dev->port[1]);
                        } break;

                        default: {
                            return true;
                        }
                    }

                    /* use this card */
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

    bus = rvsnd->isa;
    if (!bus) {
        return -1;
    }

    /* plug and play detect */
    if (use_pnp && (sb.port == 0)) {
        detect_pnp();
    }
    
    /* brute force detect */
    if (sb.port == 0) {

        /* todo: inifile overrides */
        const uint16_t* sbports = sb_port_list;
        const uint16_t* wsports = wss_port_list;
        const uint16_t* saports = sax_port_list;

        for (; (sbports && *sbports); sbports++) {
            if (sb_detect(&sb, *sbports)) {
                /* found a soundblaster compatible, detect extended clones */
                if (sb.type == SBTYPE_SBPRO) {

                    /* es186x */
                    if (ess_detect(&ess, &sb)) {
                        break;
                    }

                    /* opl3sa is special because of buggy soundblaster compatibility */
                    for (; (saports && *saports); saports++) {
                        if (sax_detect(&sax, &sb, *saports)) {
                            break;
                        }
                    }

                    /* windows sound system */
                    for (; (wsports && *wsports); wsports++) {
                        if (wss_detect(&wss, *wsports)) {
                            break;
                        }
                    }
                }
                /* use this card */
                break;
            }
        }
    }

    dprintf(("sb: %04x, ess: %04x, wss: %04x, sax: %04x\n", sb.port, ess.version, wss.port, sax.port));

    /* prepare suitable devices */
    if (sax.port) {
        sax_init_mixer(&dev_mixer);
    } else if (ess.version) {
        ess_init_mixer(&dev_mixer, false);
    } else  if (sb.port) {
        sb_init_mixer(&dev_mixer);
    } else {
        return -1;
    }

    /* publish devices */
    rvsnd->publish(rvdev_base(&dev_mixer));
    return 0;
}
