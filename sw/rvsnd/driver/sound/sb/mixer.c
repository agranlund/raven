/*-------------------------------------------------------------------------------
 * rvsnd : isa soundblaster mixer
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

/* todo: from inifile */
static bool ess_bind_fm_aux2 = true;


/* -------------------------------------------------------------------- */
/* common hardware access to soundblaster compatible mixer              */
/* -------------------------------------------------------------------- */
static void sbmix_outp(uint8_t reg, uint8_t data) {
    sbase[4] = reg;
    sbase[5] = data;
}
static uint8_t sbmix_inp(uint8_t reg) {
    sbase[4] = reg;
    return sbase[5];
}

/* -------------------------------------------------------------------- */
/* device and controls                                                  */
/* -------------------------------------------------------------------- */
static const char* dnames[] = { "Soundblaster", "SB", 0 };
static rvdev_mix_t dev = { RVDEV_MIXER, 0, 0, dnames, 0, 0, 0, 0 };


/* -------------------------------------------------------------------- */
/* CT1335 : Soundblaster                                                */
/* -------------------------------------------------------------------- */
static rvmixctrl_t ctrls_ct1335[] = {
    { "Master", 0x0002, 3, RVMIX_XBIOS_MASTER   },  /* master volume        default: 4 */
    { "Voice",  0x000A, 2, RVMIX_XBIOS_PCM      },  /* dac playback volume  default: 0 */
    { "FM",     0x0006, 3, RVMIX_XBIOS_FM       },  /* midi + fm            default: 4 */
    { "CD",     0x0008, 3, RVMIX_XBIOS_CD       },  /* aux1 volume          default: 0 */
    { 0,        0,      0, 0                    }
};

static void mixer_set_ct1335(uint16_t idx, uint16_t data) {
    (void)data;

    switch (idx) {
        case 0x0002:    /* sb: master       */
        case 0x0006:    /* sb: fm + midi    */
        case 0x0008:    /* sb: aux1         */
            sbmix_outp(idx, min(data, 7) << 1);
            return;
        case 0x000A:    /* sb: mic          */
            sbmix_outp(idx, min(data, 3));
            return;
        default:
            return;
    }
}

static uint16_t mixer_get_ct1335(uint16_t idx) {
    switch (idx) {
        case 0x0002:    /* sb: master       */
        case 0x0006:    /* sb: fm + midi    */
        case 0x0008:    /* sb: aux1         */
            return ((sbmix_inp(idx) >> 1) & 7);
        case 0x000A:    /* sb: mic          */
            return (sbmix_inp(idx) & 3);
        default:
            return 0;
    }
}

static bool mixer_init_ct1335(void) {
    dev.ctrls = ctrls_ct1335;
    dev.set = mixer_set_ct1335;
    dev.get = mixer_get_ct1335;
    return true;
}


/* -------------------------------------------------------------------- */
/* CT1345 : Soundblaster Pro                                            */
/* -------------------------------------------------------------------- */
static rvmixctrl_t ctrls_ct1345[] = {
    { "Master", 0x0022, 3, RVMIX_XBIOS_MASTER   },  /* master volume        default: 4 */
    { "Voice",  0x0004, 3, RVMIX_XBIOS_PCM      },  /* dac playback volume  default: 4 */
    { "FM",     0x0026, 3, RVMIX_XBIOS_FM       },  /* midi + fm            default: 4 */
    { "Mic",    0x000A, 2, RVMIX_XBIOS_MIC      },  /* mic mix volume       default: 0 */
    { "Line",   0x002E, 3, RVMIX_XBIOS_LINE     },  /* line volume          default: 0 */
    { "CD",     0x0028, 3, RVMIX_XBIOS_CD       },  /* aux1 volume          default: 0 */
    { 0,        0,      0, 0                    }
};

static void mixer_set_ct1345(uint16_t idx, uint16_t data) {
    switch (idx) {
        case 0x0004:    /* sbpro: voice     */
        case 0x0022:    /* sbpro: master    */
        case 0x0026:    /* sbpro: fm + midi */
        case 0x0028:    /* sbpro: cd        */
        case 0x002E: {  /* sbpro: line      */
            data = min(data, 7) << 1;
            sbmix_outp(idx, (data | (data << 4)));
            return;
        } default: {
            mixer_set_ct1335(idx, data);
            return;
        }
    }
}

static uint16_t mixer_get_ct1345(uint16_t idx) {
    switch (idx) {
        case 0x0004:    /* sbpro: voice     */
        case 0x0022:    /* sbpro: master    */
        case 0x0026:    /* sbpro: fm + midi */
        case 0x0028:    /* sbpro: cd        */
        case 0x002E: {  /* sbpro: line      */
            uint8_t d = sbmix_inp(idx);
            return ((((d >> 1) & 7) + ((d >> 5) & 7)) >> 1);
        } default: {
            return mixer_get_ct1335(idx);
        }
    }
}

static bool mixer_init_ct1345(void) {
    dev.ctrls = ctrls_ct1345;
    dev.set = mixer_set_ct1345;
    dev.get = mixer_get_ct1345;
    return true;
}


/* -------------------------------------------------------------------- */
/* CT1745 : Soundblaster 16                                             */
/* -------------------------------------------------------------------- */
static bool mixer_init_ct1745(void) {
    /* todo: currently pretending to be a soundblaster pro */
    dev.ctrls = ctrls_ct1345;
    dev.set = mixer_set_ct1345;
    dev.get = mixer_get_ct1345;
    return true;
}


/* -------------------------------------------------------------------- */
/* ES186X                                                               */
/* -------------------------------------------------------------------- */
static rvmixctrl_t ctrls_ess[] = {
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

static void mixer_set_ess(uint16_t idx, uint16_t data) {
    switch (idx) {
        case 0x0014:    /* ess: voice       */
        case 0x001A:    /* ess: mic         */
        case 0x0032:    /* ess: master      */
        case 0x0036:    /* ess: fm          */
        case 0x0038:    /* ess: cd          */
        case 0x003A:    /* ess: aux2        */
        case 0x003E: {  /* ess: line        */
            data = min(data, 15);
            if (ess_bind_fm_aux2 && ((idx == 0x36) || (idx == 0x3A))) {
                sbmix_outp(0x36, (data | (data << 4)));
                sbmix_outp(0x3A, (data | (data << 4)));
            } else {
                sbmix_outp(idx, (data | (data << 4)));
            }
            return;
        }
        case 0x003C: {  /* ess: pc speaker  */
            sbmix_outp(idx, min(data, 7));
            return;
        }
        default: {
            mixer_set_ct1345(idx, data);
            return;
        }
    }
}

static uint16_t mixer_get_ess(uint16_t idx) {
    switch (idx) {
        case 0x0014:    /* ess: voice       */
        case 0x001A:    /* ess: mic         */
        case 0x0032:    /* ess: master      */
        case 0x0036:    /* ess: fm          */
        case 0x0038:    /* ess: cd          */
        case 0x003A:    /* ess: aux2        */
        case 0x003E: {  /* ess: line        */
            uint8_t d = sbmix_inp(idx);
            return (((d & 0xf) + ((d >> 4) & 0xf)) >> 1);
        }
        case 0x003C: {    /* ess: pc speaker  */
            return (sbmix_inp(idx) & 3);
        }
        default: {
            return mixer_get_ct1345(idx);
        }
    }
}

static bool mixer_init_ess(void) {
    dev.ctrls = ctrls_ess;
    dev.set = mixer_set_ess;
    dev.get = mixer_get_ess;

    /* match volume of aux2 with same as fm */
    sbmix_outp(0x3A, sbmix_inp(0x36));

    /* bind xbios aux to fm */
    if (ess_bind_fm_aux2) {
        dev.ctrls[6].flags = dev.ctrls[2].flags;
        #if 1
        memcpy((void*)&dev.ctrls[6], (void*)&dev.ctrls[7], sizeof(rvmixctrl_t));
        memset((void*)&dev.ctrls[7], 0, sizeof(rvmixctrl_t));
        #endif
    }

    return true;
}

/* -------------------------------------------------------------------- */
/* WSS, only used for Aux2                                              */
/* -------------------------------------------------------------------- */
static rvmixctrl_t ctrls_wss[] = {
    /* soundblaster pro */
    { "Master", 0x0022, 3, RVMIX_XBIOS_MASTER   },  /* master volume        default: 4 */
    { "Voice",  0x0004, 3, RVMIX_XBIOS_PCM      },  /* dac playback volume  default: 4 */
    { "FM",     0x0026, 3, RVMIX_XBIOS_FM       },  /* midi + fm            default: 4 */
    { "Mic",    0x000A, 2, RVMIX_XBIOS_MIC      },  /* mic mix volume       default: 0 */
    { "Line",   0x002E, 3, RVMIX_XBIOS_LINE     },  /* line volume          default: 0 */
    { "CD",     0x0028, 3, RVMIX_XBIOS_CD       },  /* aux1 volume          default: 0 */
    /* windows soundsystem */
    { "Aux2",   0x0034, 6, RVMIX_XBIOS_AUX      },  /* aux2 volume          default: 0 */
    { 0,        0,      0, 0                    }
};

static void mixer_set_wss(uint16_t idx, uint16_t data) {
    if (idx >= 0x30) {
        if (data == 0) { data = 0x80; }
        else { data = 63 - min(data, 63); }
        bus->outp(wssport + 0x04, (idx & 0x0f) + 0);
        bus->outp(wssport + 0x05, data);
        bus->outp(wssport + 0x04, (idx & 0x0f) +1);
        bus->outp(wssport + 0x05, data);
    } else {
        mixer_set_ct1345(idx, data);
    }
}

static uint16_t mixer_get_wss(uint16_t idx) {
    if (idx >= 0x30) {
        uint16_t l, r;
        bus->outp(wssport + 0x04, (idx & 0x0f) + 0);
        l = bus->inp(wssport + 0x05);
        l = (l & 0x80) ? 0 : (63 - (l & 0x3f));
        bus->outp(wssport + 0x04, (idx & 0x0f) + 1);
        r = bus->inp(wssport + 0x05);
        r = (r & 0x80) ? 0 : (63 - (r & 0x3f));
        return (((l << 6) + r) >> 1);
    } else {
        return mixer_get_ct1345(idx);
    }
}

static bool mixer_init_wss(void) {
    dev.ctrls = ctrls_wss;
    dev.set = mixer_set_wss;
    dev.get = mixer_get_wss;
    return true;
}


/* -------------------------------------------------------------------- */
/* common initialization                                                */
/* -------------------------------------------------------------------- */
bool mixer_init(void) {
    bool result = false;

    /* reset soundblaster compatible mixer */
    sbmix_outp(0x00, 0x00);

    /* initialize device and controls */
    switch (sbtype) {
        case SBTYPE_ESS:
            result = mixer_init_ess();
            break;
        case SBTYPE_SB16:
            result = mixer_init_ct1745();
            break;
        case SBTYPE_SBPRO:
            result = wssport ? mixer_init_wss() : mixer_init_ct1345();
            break;
        default:
            result = mixer_init_ct1335();
            break;
    }

    /* publish mixer device */
    if (result) {
        rvsnd->publish(rvdev_base(&dev));
    }

    return result;
}
