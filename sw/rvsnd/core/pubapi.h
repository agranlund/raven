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

#ifndef _RVSND_PUBAPI_H_
#define _RVSND_PUBAPI_H_

#define C_RSND 0x52534E44UL
#define V_RSND 0x00251130UL

#ifdef __GNUC__
    #define _RVSND_API
    #define _RVSND_INL static inline
#else
    #define _RVSND_API cdecl
    #define _RVSND_INL static
#endif

#define RVSND_DEVTYPE_CHIP          1
#define RVSND_DEVTYPE_MIXER         2
#define RVSND_DEVTYPE_MIDI_OUT      3
#define RVSND_DEVTYPE_MIDI_IN       4
#define RVSND_DEVTYPE_AUDIO_OUT     5
#define RVSND_DEVTYPE_AUDIO_IN      6

#define RVSND_DEVFLAG_ACTIVE        (1<<0)

/* device info */
typedef struct {
    const char* name;
    uint32_t    version;
    uint16_t    id;
    uint16_t    flags;
} rvsnd_devinfo_t;

/* public cookie api */
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t reserved[126];

    int32_t _RVSND_API (*GetNumDevices)(uint32_t type);
    int32_t _RVSND_API (*GetDeviceInfo)(rvsnd_devinfo_t* out, uint32_t type, uint32_t idx);
    void    _RVSND_API (*SetDefaultDevice)(uint32_t type, uint32_t idx);
} rvsnd_t;

#endif /* _RVSND_PUBAPI_H_*/
