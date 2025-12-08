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
/* CT1335 mixer : Soundblaster                                          */
/* -------------------------------------------------------------------- */
static rvmixctrl_t ct1335_mixer_ctrls[] = {
    { "Master", 0x0002, 3, RVMIX_XBIOS_MASTER   },  /* master volume        default: 4 */
    { "PCM",    0x000A, 2, RVMIX_XBIOS_PCM      },  /* dac playback volume  default: 0 */
    { "FM",     0x0006, 3, RVMIX_XBIOS_FM       },  /* midi + fm            default: 4 */
    { "CD",     0x0008, 3, RVMIX_XBIOS_CD       },  /* aux1 volume          default: 0 */
    { 0,        0,      0, 0                    }
};

static void ct1335_mixer_set(uint16_t idx, uint16_t data) {
    (void)data;

    switch (idx) {
        case 0x0002:    /* sb: master       */
        case 0x0006:    /* sb: fm + midi    */
        case 0x0008:    /* sb: aux1         */
            sb_mixer_outp(idx, min(data, 7) << 1);
            return;
        case 0x000A:    /* sb: mic          */
            sb_mixer_outp(idx, min(data, 3));
            return;
        default:
            return;
    }
}

static uint16_t ct1335_mixer_get(uint16_t idx) {
    switch (idx) {
        case 0x0002:    /* sb: master       */
        case 0x0006:    /* sb: fm + midi    */
        case 0x0008:    /* sb: aux1         */
            return ((sb_mixer_inp(idx) >> 1) & 7);
        case 0x000A:    /* sb: mic          */
            return (sb_mixer_inp(idx) & 3);
        default:
            return 0;
    }
}

/* -------------------------------------------------------------------- */
/* CT1345 mixer : Soundblaster Pro                                      */
/* -------------------------------------------------------------------- */
static rvmixctrl_t ct1345_mixer_ctrls[] = {
    { "Master", 0x0022, 3, RVMIX_XBIOS_MASTER   },  /* master volume        default: 4 */
    { "PCM",    0x0004, 3, RVMIX_XBIOS_PCM      },  /* dac playback volume  default: 4 */
    { "FM",     0x0026, 3, RVMIX_XBIOS_FM       },  /* midi + fm            default: 4 */
    { "Mic",    0x000A, 2, RVMIX_XBIOS_MIC      },  /* mic mix volume       default: 0 */
    { "Line",   0x002E, 3, RVMIX_XBIOS_LINE     },  /* line volume          default: 0 */
    { "CD",     0x0028, 3, RVMIX_XBIOS_CD       },  /* aux1 volume          default: 0 */
    { 0,        0,      0, 0                    }
};

void ct1345_mixer_set(uint16_t idx, uint16_t data) {
    switch (idx) {
        case 0x0004:    /* sbpro: voice     */
        case 0x0022:    /* sbpro: master    */
        case 0x0026:    /* sbpro: fm + midi */
        case 0x0028:    /* sbpro: cd        */
        case 0x002E: {  /* sbpro: line      */
            data = min(data, 7) << 1;
            sb_mixer_outp(idx, (data | (data << 4)));
        } break;
        case 0x000A: {  /* sbpro: mic       */
            sb_mixer_outp(idx, min(data, 3));
        } break;
    }
}

uint16_t ct1345_mixer_get(uint16_t idx) {
    switch (idx) {
        case 0x0004:    /* sbpro: voice     */
        case 0x0022:    /* sbpro: master    */
        case 0x0026:    /* sbpro: fm + midi */
        case 0x0028:    /* sbpro: cd        */
        case 0x002E: {  /* sbpro: line      */
            uint8_t d = sb_mixer_inp(idx);
            return ((((d >> 1) & 7) + ((d >> 5) & 7)) >> 1);
        }
        case 0x000A: {  /* sbpro: mic       */
            return (sb_mixer_inp(idx) & 3);
        }
    }
    return 0;
}


/* -------------------------------------------------------------------- */
/* init and detect                                                      */
/* -------------------------------------------------------------------- */

void sb_init_mixer(rvdev_mix_t* out) {
    switch (sb.type) {
        case SBTYPE_SB16: {
            /* pretending to be a sbpro for now */
            out->ctrls  = ct1345_mixer_ctrls;
            out->set    = ct1345_mixer_set;
            out->get    = ct1345_mixer_get;
        } break;
        case SBTYPE_SBPRO: {
            out->ctrls  = ct1345_mixer_ctrls;
            out->set    = ct1345_mixer_set;
            out->get    = ct1345_mixer_get;
        } break;
        default: {
            out->ctrls  = ct1335_mixer_ctrls;
            out->set    = ct1335_mixer_set;
            out->get    = ct1335_mixer_get;
        } break;
    }
}


bool sb_detect(sb_info_t* out, uint16_t port) {
    const uint32_t dsp_init_delay = 3000UL;

    /* reset soundblaster dsp */
    uint8_t save06 = bus->inp(port + 0x06);
    bus->outp(port + 0x06, 0x01);
    delayus(dsp_init_delay);
    bus->outp(port + 0x06, 0x00);
    delayus(dsp_init_delay);

    /* dsp readport should return 0xAA after reset */
    memset((void*)out, 0, sizeof(sb_info_t));
    if (bus->inp(port + 0x0A) != 0xAA) {
        delayus(dsp_init_delay);
        bus->outp(port + 0x06, save06);
        delayus(dsp_init_delay);
        return false;
    }

    /* check dsp version */
    bus->outp(port + 0x0C, 0xE1);
    delayus(dsp_init_delay);
    out->version = bus->inp(port + 0x0A);
    out->version <<= 8;
    delayus(dsp_init_delay);
    out->version |= bus->inp(port + 0x0A);
    delayus(dsp_init_delay);

    /* determine type based on dsp version */
    if (out->version >= 0x0400) {
        out->type = SBTYPE_SB16;
    } else if (out->version >= 0x0300) {
        out->type = SBTYPE_SBPRO;
    } else if (out->version >= 0x0201) {
        out->type = SBTYPE_SB2;
    } else {
        out->type = SBTYPE_SB1;
    }

    /* and set the hardware infos */
    out->port = port;
    out->base = (volatile uint8_t*)((uint32_t)bus->iobase + out->port);
    return true;
}
