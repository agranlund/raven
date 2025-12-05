/*-------------------------------------------------------------------------------
 * rvsnd
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
#include "sys.h"
#include "pubapi.h"
#include "driver.h"
#include "midi.h"
#include "mixer.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>

static rvsnd_t pubapi;

/*------------------------------------------------------------------------------*/
/* helpers                                                                      */
/*------------------------------------------------------------------------------*/
static rvdev_t* GetDeviceByTypeAndIndex(uint32_t type, uint32_t idx) {
    uint16_t i;
    uint16_t numdevs = driver_NumDevs();
    rvdev_t** devs = driver_GetDevs();
    uint16_t found = 0;
    for (i = 0; i < numdevs; i++) {
        rvdev_t* dev = devs[i];
        if (dev && (dev->type == type)) {
            if (idx == found) {
                return dev;
            }
            found++;
        }
    }
    return 0;
}

/*------------------------------------------------------------------------------*/
/* public api : devices                                                         */
/*------------------------------------------------------------------------------*/
int32_t _RVSND_API pubapi_GetNumDevices(uint32_t type) {
    uint16_t i, found;
    uint16_t numdevs = driver_NumDevs();
    rvdev_t** devs = driver_GetDevs();
    for (i = 0, found = 0; i < numdevs; i++) {
        if (devs[i] && (devs[i]->type == type)) {
            found++;
        }
    }
    return (uint32_t)found;
}

int32_t _RVSND_API pubapi_GetDeviceInfo(rvsnd_devinfo_t* out, uint32_t type, uint32_t idx) {
    rvdev_t* dev = GetDeviceByTypeAndIndex(type, idx);
    if (dev) {
        out->name = dev->names[0];
        out->version = dev->version;
        out->id = (uint16_t)(((dev->type & 0xff) << 8) | (idx & 0xff));
        out->flags = 0;

        switch (((uint16_t)dev->type)) {
            case RVDEV_MIDI_IN:
                if (dev == rvdev_base(midi_GetRxDevice())) {
                    out->flags |= RVSND_DEVFLAG_ACTIVE;
                }
                break;
            case RVDEV_MIDI_OUT:
                if (dev == rvdev_base(midi_GetTxDevice())) {
                    out->flags |= RVSND_DEVFLAG_ACTIVE;
                }
                break;
        }

        return 1;
    }
    memset((void*)out, 0, sizeof(rvsnd_devinfo_t));
    return 0;
}

void _RVSND_API pubapi_SetDefaultDevice(uint32_t type, uint32_t idx) {
    rvdev_t* dev = GetDeviceByTypeAndIndex(type, idx);
    if (dev) {
        uint16_t devtype = (uint16_t)dev->type;
        switch(devtype) {
            case RVDEV_MIDI_IN: {
                midi_SetRxDevice(rvdev_cast(rvdev_midirx_t, dev));
            } break;
            case RVDEV_MIDI_OUT: {
                midi_SetTxDevice(rvdev_cast(rvdev_miditx_t, dev));
            } break;
            case RVDEV_AUDIO_IN: {
            } break;
            case RVDEV_AUDIO_OUT: {
            } break;
        }
    }
}

/*------------------------------------------------------------------------------*/
/* public api : mixer                                                           */
/*------------------------------------------------------------------------------*/

int32_t _RVSND_API pubapi_GetNumMixerControls(uint32_t devid) {
    mixer_dev_t* dev = mixer_GetDev((uint16_t)devid);
    return dev ? dev->ctr_count : 0;
}

int32_t _RVSND_API pubapi_GetMixerControlInfo(rvsnd_mixinfo_t* out, uint32_t devid, uint32_t idx) {
    mixer_dev_t* dev = mixer_GetDev(devid);
    if (dev) {
        if (idx < dev->ctr_count) {
            uint16_t bits = dev->ctr_list[idx].ctr->bits;
            out->name = dev->ctr_list[idx].ctr->name;
            out->id = dev->ctr_list[idx].id;
            out->max = bits ? ((1 << dev->ctr_list[idx].ctr->bits) - 1) : 0;
            if (devid == 0) { out->flg = dev->ctr_list[idx].ctr->id; }
            else { out->flg = dev->ctr_list[idx].ctr->flags; }
            return 1;
        }
    }
    memset(out, 0, sizeof(rvsnd_mixinfo_t));
    return 0;
}

int32_t _RVSND_API pubapi_GetMixerValueById(uint32_t ctrid) { return (uint32_t)mixer_GetValueById(ctrid);}
void _RVSND_API pubapi_SetMixerValueById(uint32_t ctrid, uint32_t data) { mixer_SetValueById(ctrid, data); }
int32_t _RVSND_API pubapi_GetMixerValueByName(const char* name) { return (uint32_t)mixer_GetValueByName(name); }
void _RVSND_API pubapi_SetMixerValueByName(const char* name, uint32_t val) { mixer_SetValueByName(name, val); }



/*------------------------------------------------------------------------------*/
/* initialize public api                                                        */
/*------------------------------------------------------------------------------*/
void pubapi_Init(void) {

    /* create cookie api */
    memset((void*)&pubapi, 0, sizeof(rvsnd_t));
    pubapi.magic = C_RSND;
    pubapi.version = V_RSND;
 
    pubapi.GetNumDevices        = pubapi_GetNumDevices;
    pubapi.GetDeviceInfo        = pubapi_GetDeviceInfo;
    pubapi.SetDefaultDevice     = pubapi_SetDefaultDevice;

    pubapi.GetNumMixerControls  = pubapi_GetNumMixerControls;
    pubapi.GetMixerControlInfo  = pubapi_GetMixerControlInfo;
    pubapi.GetMixerValueById    = pubapi_GetMixerValueById;
    pubapi.SetMixerValueById    = pubapi_SetMixerValueById;
    pubapi.GetMixerValueByName  = pubapi_GetMixerValueByName;
    pubapi.SetMixerValueByName  = pubapi_SetMixerValueByName;

    sys_setcookie("RSND", (uint32_t)&pubapi);
}


/*------------------------------------------------------------------------------*/
/* $8F : xbios rvsnd dispatch
/*------------------------------------------------------------------------------*/
int32_t xb_rvsnd(uint16_t* args) {
    (void)args; return 0;
}
