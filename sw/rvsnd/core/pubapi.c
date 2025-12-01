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
#include <malloc.h>
#include <string.h>

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
/* public api                                                                   */
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
/* initialize public api                                                        */
/*------------------------------------------------------------------------------*/
void pubapi_Init(void) {

    /* create cookie api */
    memset((void*)&pubapi, 0, sizeof(rvsnd_t));
    pubapi.magic = C_RSND;
    pubapi.version = V_RSND;
 
    pubapi.GetNumDevices = pubapi_GetNumDevices;
    pubapi.GetDeviceInfo = pubapi_GetDeviceInfo;
    pubapi.SetDefaultDevice = pubapi_SetDefaultDevice;

    sys_setcookie("RSND", (uint32_t)&pubapi);
}


/*------------------------------------------------------------------------------*/
/* $8F : xbios rvsnd dispatch
/*------------------------------------------------------------------------------*/
int32_t xb_rvsnd(uint16_t* args) {
    (void)args; return 0;
}
