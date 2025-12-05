/*-------------------------------------------------------------------------------
 * rvsnd mixer
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

#include "mixer.h"
#include "drvapi.h"
#include <string.h>
#include <malloc.h>

static mixer_ctr_t* mix_ctrs;
static mixer_dev_t* mix_devs;
static uint16_t num_mixdevs;
static uint16_t num_mixctrl;


/* -------------------------------------------------------------------- */
/* internal                                                             */
/* -------------------------------------------------------------------- */

static mixer_ctr_t* mixer_GetControlById(uint16_t id) {
    uint16_t i;
    for (i=0; i<num_mixctrl; i++) {
        mixer_ctr_t* ctr = &mix_ctrs[i];
        if (ctr->id == id) {
            return ctr;
        }
    }
    return 0;
}

static uint8_t mixer_GetControlValueInternal(mixer_ctr_t* ctr) {
    static const uint8_t masks[9] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };
    uint8_t bits = ctr->ctr->bits;
    uint8_t val = ctr->dev->get(ctr->ctr->id);
    uint8_t mask = 0;
    uint8_t shift = 0;
    if (bits <= 8) {
        mask = masks[bits];
        shift = 8 - bits;
    }
    if (shift) {
        val <<= shift;
        mask <<= shift;
    }
    if ((val & mask) == (ctr->val & mask)) {
        return ctr->val;
    }
    return val;
}

static void mixer_SetControlValueInternal(mixer_ctr_t* ctr, uint8_t data) {
    uint8_t bits = ctr->ctr->bits;
    uint8_t shift = (bits < 8) ? (8 - bits) : 0;
    ctr->val = data;
    if (shift) { data >>= shift; }
    ctr->dev->set(ctr->ctr->id, data);
}

static uint8_t mixer_GetXbiosValue(uint16_t xbiosid) {
    uint16_t i; uint16_t val = 0; uint16_t num = 0;
    for (i=0; i<num_mixctrl; i++) {
        mixer_ctr_t* ctr = &mix_ctrs[i];
        if (ctr->ctr->flags == xbiosid) {
            val += mixer_GetControlValueInternal(ctr);
            num++;
        }
    }
    return num ? (uint8_t)(val / num) : 0;
}

static void mixer_SetXbiosValue(uint16_t xbiosid, uint8_t data) {
    uint16_t i;
    for (i=0; i<num_mixctrl; i++) {
        mixer_ctr_t* ctr = &mix_ctrs[i];
        if (ctr->ctr->flags == xbiosid) {
            mixer_SetControlValueInternal(ctr, data);
        }
    }
}

/* -------------------------------------------------------------------- */
/* public                                                               */
/* -------------------------------------------------------------------- */

uint8_t mixer_GetValue(mixer_ctr_t* ctr) {
    if (ctr) {
        if (ctr->ctr->flags) {
            return mixer_GetXbiosValue(ctr->ctr->flags);
        } else {
            return mixer_GetControlValueInternal(ctr);
        }
    }
    return 0;
}

void mixer_SetValue(mixer_ctr_t* ctr, uint8_t data) {
    if (ctr) {
        if (ctr->ctr->flags) {
            mixer_SetXbiosValue(ctr->ctr->flags, data);
        } else {
            mixer_SetControlValueInternal(ctr, data);
        }
    }
}

uint8_t mixer_GetValueById(uint16_t id) {
    return mixer_GetValue(mixer_GetControlById(id));
}

void mixer_SetValueById(uint16_t id, uint8_t data) {
    mixer_SetValue(mixer_GetControlById(id), data);
}

uint16_t mixer_NumDevs() {
    return num_mixdevs;
}

mixer_dev_t* mixer_GetDev(uint16_t idx) {
    return &mix_devs[idx];
}

mixer_dev_t* mixer_FindDev(const char* name) {
    uint16_t i;
    for (i=0; i<num_mixdevs; i++) {
        mixer_dev_t* dev = &mix_devs[i];
        const char** names = dev->dev->names;
        for (; *names; names++) {
            if (stricmp(name, *names) == 0) {
                return dev;
            }
        }
    }
    return 0;
}

mixer_ctr_t* mixer_FindDevCtr(mixer_dev_t* dev, const char* name) {
    uint16_t i;
    for (i=0; i<dev->ctr_count; i++) {
        mixer_ctr_t* ctr = &dev->ctr_list[i];
        if (stricmp(name, ctr->ctr->name) == 0) {
            return ctr;
        }
    }
    return 0;
}

mixer_ctr_t* mixer_FindCtr(const char* name) {
    /* format = "dev:ctr", or just "ctr" for system controls */
    static char tempstr[128];
    mixer_dev_t* dev;
    char* s2;
    char* s1 = tempstr;
    size_t len = 0;

    if (num_mixdevs == 0) { return 0; }
    if (name) { len = strlen(name); }
    if ((len <= 0) || (len >= 127)) { return 0; }
    strcpy(s1, name);
    s2 = strchr(tempstr, ':');
    if (s2) {
        *s2 = 0; 
        dev = mixer_FindDev(s1);
        s1 = s2 + 1;
    } else {
        dev = &mix_devs[0];
    }
    return dev ? mixer_FindDevCtr(dev, s1) : 0;
}

void mixer_SetValueByName(const char* name, uint8_t data) {
    mixer_SetValue(mixer_FindCtr(name), data);
}

uint8_t mixer_GetValueByName(const char* name) {
    return mixer_GetValue(mixer_FindCtr(name));
}


/* -------------------------------------------------------------------- */
/* sys                                                                  */
/* -------------------------------------------------------------------- */

static rvmixctrl_t sysdctrls[] = {
    { "Master", RVMIX_XBIOS_MASTER, 8, 0 },
    { "PCM",    RVMIX_XBIOS_PCM,    8, 0 },
    { "FM",     RVMIX_XBIOS_FM,     8, 0 },
    { "Mic",    RVMIX_XBIOS_MIC,    8, 0 },
    { "Line",   RVMIX_XBIOS_LINE,   8, 0 },
    { "CD",     RVMIX_XBIOS_CD,     8, 0 },
    { "TV",     RVMIX_XBIOS_TV,     8, 0 },
    { "Aux",    RVMIX_XBIOS_AUX,    8, 0 },
    { 0,        0,                  0, 0 }
};

static void sysdset(uint16_t idx, uint16_t data) { mixer_SetXbiosValue(idx, data); }
static uint16_t sysdget(uint16_t idx) { return mixer_GetXbiosValue(idx); }
static const char* sysdnames[] = { "System", "sys", "xbios", 0 };
static const rvdev_mix_t sysdev = { RVDEV_MIXER, 0, 0, sysdnames, 0, 0, sysdset, sysdget, sysdctrls};

bool mixer_Init(void) {
    /* add system mixer */
    driver_AddDevice(rvdev_base(&sysdev));
    return true;
}

void mixer_debuglist(void) {
    uint16_t i;
    for (i=0; i<num_mixctrl; i++) {
        mixer_ctr_t* ctr = &mix_ctrs[i];
        dprintf("%04x: %s:%s : %d\n", ctr->id, ctr->dev->names[0], ctr->ctr->name, mixer_GetValue(ctr));
    }
}

void mixer_Setup(void) {
    uint16_t i;
    uint16_t devcount = driver_NumDevs();
    rvdev_t** devs = driver_GetDevs();

    /* count all mixer devices and controls */
    num_mixdevs = 0;
    num_mixctrl = 0;
    for (i=0; i<devcount; i++) {
        rvdev_t* dev = devs[i];
        if (dev->type == RVDEV_MIXER) {
            rvdev_mix_t* mixdev = rvdev_cast(rvdev_mix_t, dev);
            rvmixctrl_t* mixctr = mixdev->ctrls;
            num_mixdevs++;
            while (mixctr->id) {
                num_mixctrl++;
                mixctr++;
            }
        }
    }

    /* allocate global device and control list */
    mix_devs = malloc(num_mixdevs * sizeof(mixer_dev_t));
    mix_ctrs = malloc(num_mixctrl * sizeof(mixer_ctr_t));

    /* organize devices and controls */
    num_mixctrl = 0;
    num_mixdevs = 0;
    for (i=0; i<devcount; i++) {
        rvdev_t* dev = devs[i];
        if (dev->type == RVDEV_MIXER) {
            rvdev_mix_t* mixdev = rvdev_cast(rvdev_mix_t, dev);
            rvmixctrl_t* mixctr = mixdev->ctrls;
            uint16_t ctrnum_first = num_mixctrl;

            mix_devs[num_mixdevs].dev = mixdev;
            mix_devs[num_mixdevs].ctr_list = &mix_ctrs[ctrnum_first];
            while (mixctr->id) {
                mix_ctrs[num_mixctrl].dev = mixdev;
                mix_ctrs[num_mixctrl].ctr = mixctr;
                mix_ctrs[num_mixctrl].id = ((num_mixdevs << 8) | ((num_mixctrl - ctrnum_first) & 0xff));
                mix_ctrs[num_mixctrl].val = 0;
                mix_ctrs[num_mixctrl].reserved = 0;
                num_mixctrl++;
                mixctr++;
            }
            mix_devs[num_mixdevs].ctr_count = (num_mixctrl - ctrnum_first);
            num_mixdevs++;
        }
    }

    /* some defaults */
    mixer_SetXbiosValue(RVMIX_XBIOS_MASTER, 196);
    mixer_SetXbiosValue(RVMIX_XBIOS_PCM,    196);
    mixer_SetXbiosValue(RVMIX_XBIOS_FM,     196);
    mixer_SetXbiosValue(RVMIX_XBIOS_MIC,      0);
    mixer_SetXbiosValue(RVMIX_XBIOS_LINE,   196);
    mixer_SetXbiosValue(RVMIX_XBIOS_CD,     196);
    mixer_SetXbiosValue(RVMIX_XBIOS_TV,     196);
    mixer_SetXbiosValue(RVMIX_XBIOS_AUX,    196);
}
