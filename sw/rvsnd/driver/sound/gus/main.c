/*-------------------------------------------------------------------------------
 * rvsnd : isa ultrasound driver
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
#include "gus.h"

isa_t* bus;
gusinfo_t gus;

const uint16_t gus_port_list[]  = { 0x220, 0x240, 0x250, 0x260, 0 };


/* -------------------------------------------------------------------- */
/* mixer device                                                         */
/* -------------------------------------------------------------------- */
static const char* dev_mixer_names[] = { "Ultrasound", "gus", 0 };
static rvdev_mix_t dev_mixer = { RVDEV_MIXER, 0, 0, dev_mixer_names, 0, 0, 0, 0 };


/* -------------------------------------------------------------------- */
/* driver                                                               */
/* -------------------------------------------------------------------- */

static bool detect_pnp(void) {
    static const char* pnpids[] = { "GRV0000", 0 };
    const char** pnpid = pnpids;

    for (; (pnpid && *pnpid); pnpid++) {
        isa_dev_t* dev = bus->find_dev(*pnpid, 0);
        if (dev && dev->port[0]) {
            if (gus_detect(dev->port[0])) {
                return true;
            }
        }
    }
    return false;
}

int32_t init(void) {

    bool use_pnp = true;

    bus = rvsnd->isa;
    if (!bus) {
        return -1;
    }

    /* plug and play detect */
    if ((gus.port == 0) && use_pnp) {
        detect_pnp();
    }
    
    /* brute force detect */
    if (gus.port == 0) {
        /* todo: inifile overrides */
        const uint16_t* ports = gus_port_list;
        for (; (ports && *ports); ports++) {
            if (gus_detect(*ports)) {
                break;
            }
        }
    }

    if (gus.port == 0) {
        return -1;
    }

    /* put card in some kind of sensible default */
    gus_init();

    /* create rvsnd mixer */
    if (gus_create_mixer(&dev_mixer)) {
        rvsnd->publish(rvdev_base(&dev_mixer));
    }

    return 0;
}
