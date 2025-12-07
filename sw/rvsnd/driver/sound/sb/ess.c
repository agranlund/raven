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

/* -------------------------------------------------------------------- */
/* ES186X                                                               */
/* -------------------------------------------------------------------- */

static bool mixer_combine_midi_fm = true;   /* FM controls both Wavetable and OPL like a normal soundblaster */

static rvmixctrl_t mixer_ctrls[] = {
    { "Master", 0x0032, 4, RVMIX_XBIOS_MASTER   },  /* master volume        default: 8 */
    { "Voice",  0x0014, 4, RVMIX_XBIOS_PCM      },  /* dac playback volume  default: 8 */
    { "Music",  0x0036, 4, RVMIX_XBIOS_FM       },  /* fm volume            default: 8 */
    { "Mic",    0x001A, 4, RVMIX_XBIOS_MIC      },  /* mic mix volume       default: 0 */
    { "Line",   0x003E, 4, RVMIX_XBIOS_LINE     },  /* line volume          default: 0 */
    { "CD",     0x0038, 4, RVMIX_XBIOS_CD       },  /* aux1 volume          default: 0 */ 
    { "Aux2",   0x003A, 4, RVMIX_XBIOS_AUX      },  /* aux2 volume (midi)   default: 0 */
    { "Beeper", 0x003C, 3, 0                    },  /* pc speaker volume    default: 8 */
    { 0,        0,      0, 0,                   }
};

static void mixer_set(uint16_t idx, uint16_t data) {
    switch (idx) {
        case 0x0014:    /* ess: voice       */
        case 0x001A:    /* ess: mic         */
        case 0x0032:    /* ess: master      */
        case 0x0036:    /* ess: fm          */
        case 0x0038:    /* ess: cd          */
        case 0x003A:    /* ess: aux2        */
        case 0x003E: {  /* ess: line        */
            data = min(data, 15);
            if (mixer_combine_midi_fm && ((idx == 0x36) || (idx == 0x3A))) {
                sb_mixer_outp(0x36, (data | (data << 4)));
                sb_mixer_outp(0x3A, (data | (data << 4)));
            } else {
                sb_mixer_outp(idx, (data | (data << 4)));
            }
            return;
        }
        case 0x003C: {  /* ess: pc speaker  */
            sb_mixer_outp(idx, min(data, 7));
            return;
        }
    }
}

static uint16_t mixer_get(uint16_t idx) {
    switch (idx) {
        case 0x0014:    /* ess: voice       */
        case 0x001A:    /* ess: mic         */
        case 0x0032:    /* ess: master      */
        case 0x0036:    /* ess: fm          */
        case 0x0038:    /* ess: cd          */
        case 0x003A:    /* ess: aux2        */
        case 0x003E: {  /* ess: line        */
            uint8_t d = sb_mixer_inp(idx);
            return (((d & 0xf) + ((d >> 4) & 0xf)) >> 1);
        }
        case 0x003C: {  /* ess: pc speaker  */
            return (sb_mixer_inp(idx) & 3);
        }
    }
    return 0;
}


/* -------------------------------------------------------------------- */
/* init and detect                                                      */
/* -------------------------------------------------------------------- */

void ess_init_mixer(rvdev_mix_t* out, bool midivol_auxb) {
    out->ctrls = mixer_ctrls;
    out->set = mixer_set;
    out->get = mixer_get;

    /* match volume of aux2 with same as fm */
    sb_mixer_outp(0x3A, sb_mixer_inp(0x36));

    /* bind xbios aux to fm */
    mixer_combine_midi_fm = !midivol_auxb;
    if (mixer_combine_midi_fm) {
        mixer_ctrls[6].flags = mixer_ctrls[2].flags;
        #if 1
        memcpy((void*)&mixer_ctrls[6], (void*)&mixer_ctrls[7], sizeof(rvmixctrl_t));
        memset((void*)&mixer_ctrls[7], 0, sizeof(rvmixctrl_t));
        #endif
    }
}


bool ess_detect(ess_info_t* out, sb_info_t* sb) {
    memset((void*)out, 0, sizeof(ess_info_t));
    if (sb && sb->port) {
        uint8_t id[4];
        uint8_t save04 = bus->inp(sb->port + 0x04);
        bus->outp(sb->port + 0x04, 0x40);  delayus(1000);
        id[0] = bus->inp(sb->port + 0x05); delayus(1000);
        id[1] = bus->inp(sb->port + 0x05); delayus(1000);
        id[2] = bus->inp(sb->port + 0x05); delayus(1000);
        id[3] = bus->inp(sb->port + 0x05); delayus(1000);
        if ((id[0] == 0x18) && ((id[1] == 0x68) || (id[1] == 0x69))) {
            out->version = ((((uint16_t)id[0]) << 8) | id[1]);
            return true;
        }
        bus->outp(sb->port + 0x04, save04);
    }
    return false;
}

