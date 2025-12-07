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
/* OPL3SA, gets own mixer because of buggy soundblaster compatibility   */
/* -------------------------------------------------------------------- */
static rvmixctrl_t mixer_ctrls[] = {
    { "Master", 0x0040, 4, RVMIX_XBIOS_MASTER   },  /* sax: master volume   */
    { "Voice",  0x0004, 3, RVMIX_XBIOS_PCM      },  /* sb:  voice           */
    { "FM",     0x0026, 3, RVMIX_XBIOS_FM       },  /* sb:  midi + fm       */
    { "Mic",    0x0041, 5, RVMIX_XBIOS_MIC      },  /* sax  mic             */
    { "Line",   0x0028, 3, RVMIX_XBIOS_LINE     },  /* sb:  line            */
    { "CD",     0x002E, 3, RVMIX_XBIOS_CD       },  /* sb:  aux1            */
    /* todo: wide, bass, treble */
    { 0,        0,      0, 0                    }
};

static void mixer_outp(uint8_t _r, uint8_t _d)  {
    sax.base[0] = _r;
    sax.base[1] = _d;
}

static uint8_t mixer_inp(uint8_t _r) {
    sax.base[0] = _r;
    return sax.base[1];
}

static void mixer_set(uint16_t idx, uint16_t data) {
    switch (idx) {
        case 0x40: {    /* master */
            uint8_t vol = min(data, 15);
            vol = (vol == 0) ? 0x80 : 15 - vol;
            mixer_outp(0x07, vol);
            mixer_outp(0x08, vol);
        } break;
        case 0x41: {    /* mic */
            uint8_t vol = min(data, 31);
            vol = (vol == 0) ? 0x80 : 31 - vol;
            mixer_outp(0x09, vol);
        } break;
        default: {      /* soundblaster pro */
            ct1345_mixer_set(idx, data);
        } break;
    }
}

static uint16_t mixer_get(uint16_t idx) {
    switch (idx) {
        case 0x40: {    /* master */
            uint16_t l = mixer_inp(0x07);
            uint16_t r = mixer_inp(0x08);
            l = (l & 0x80) ? 0 : (15 - (l & 0x0f));
            r = (r & 0x80) ? 0 : (15 - (r & 0x0f));
            return ((l + r) >> 1);
        }
        case 0x41: {    /* mic */
            uint16_t vol = mixer_inp(0x09);
            return (vol & 0x80) ? 0 : (31 - (vol & 0x1f));
        }
        default: {      /* soundblaster pro */
            return ct1345_mixer_get(idx);
        }
    }
}



/* -------------------------------------------------------------------- */
/* init and detect                                                      */
/* -------------------------------------------------------------------- */

void sax_init_mixer(rvdev_mix_t* out) {
    out->ctrls = mixer_ctrls;
    out->set = mixer_set;
    out->get = mixer_get;
    /* soundblaster compatible master volume is not soundblaster compatible     */
    /* must be set to 1 for other controls to work like a real soundblaster     */
    sb_mixer_outp(0x22, 0x22);
}


bool sax_detect(opl3sa_info_t* out, sb_info_t* sb, uint16_t port) {
    uint8_t old00, old01;
    uint8_t temp1, temp2;

    /* verify soundblaster dsp version against opl3sa setting */
    memset((void*)out, 0, sizeof(opl3sa_info_t));
    old00 = bus->inp(port + 0x00);
    bus->outp(port + 0x00, 0x02);
    temp1 = bus->inp(port + 0x01);
    temp1 = 3 - ((temp1 >> 1) & 3);

    if (temp1 == (sb->version >> 8)) {
        /* test opl3sa volume register */
        bus->outp(port + 0x00, 0x07);
        old01 = bus->inp(port + 0x01);
        bus->outp(port + 0x00, 0x07);
        bus->outp(port + 0x01, 0x80);
        bus->outp(port + 0x00, 0x07);
        temp1 = bus->inp(port + 0x01);
        bus->outp(port + 0x00, 0x07);
        bus->outp(port + 0x01, 0x07);
        bus->outp(port + 0x00, 0x07);
        temp2 = bus->inp(port + 0x01);
        /* restore */
        bus->outp(port + 0x00, 0x07);
        bus->outp(port + 0x01, old01);
        if ((temp1 == 0x80) && (temp2 == 0x07)) {
            /* found */
            out->port = port;
            out->base = (volatile uint8_t*)(bus->iobase + port);
        }
    }
    /* restore and return */
    bus->outp(port + 0x00, old00);
    return (out->port != 0) ? true : false;
}
