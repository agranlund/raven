/*-------------------------------------------------------------------------------
 * rvsctrl
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mint/cookie.h>
#include <mint/osbind.h>

#include "../core/pubapi.h"

static rvsnd_t* rvsnd = 0;


#define RVSND_DEVTYPE_CHIP          1
#define RVSND_DEVTYPE_MIXER         2
#define RVSND_DEVTYPE_MIDI_OUT      3
#define RVSND_DEVTYPE_MIDI_IN       4
#define RVSND_DEVTYPE_AUDIO_OUT     5
#define RVSND_DEVTYPE_AUDIO_IN      6

#define RVSND_DEVTYPE_MIN   RVSND_DEVTYPE_CHIP
#define RVSND_DEVTYPE_MAX   RVSND_DEVTYPE_AUDIO_IN

static const char* devtypes[] = {
    "<none>",
    "chip",
    "mixer",
    "midi-out",
    "midi-in",
    "audio-out",
    "audio-in"
};

/* ------------------------------------------------------------------- */
int main_devs(int args, char** argv) {
    uint16_t i, j;
    uint16_t first = RVSND_DEVTYPE_MIN;
    uint16_t last = RVSND_DEVTYPE_MAX;

    if (args > 0) {
        for (i=first; i<=last; i++) {
            if (strcmp(argv[0], devtypes[i]) == 0) {
                first = i; last = i; break;
            }
        }
        if (first != last) {
            i = atoi(argv[0]);
            if ((i >= first) && (i <= last)) {
                first = i; last = i;
            }
        }
    }

    for (i=first; i<=last; i++) {
        uint16_t num = rvsnd->GetNumDevices(i);
        if (num) {
            printf(" %s:\n", devtypes[i]);
            for (j=0; j<num; j++) {
                rvsnd_devinfo_t dev;
                if (rvsnd->GetDeviceInfo(&dev, i, j) && dev.id) {
                    printf(" %c %04x: %s\n",
                        (dev.flags & RVSND_DEVFLAG_ACTIVE) ? '>' : ' ',
                        dev.id,
                        dev.name);
                } 
            }
            printf("\n");
        }
    }
    return 0;
}

/* ------------------------------------------------------------------- */
int main_select(int args, char** argv) {
    if (args && argv[0]) {
        char* str = argv[0];
        size_t len = strlen(str);
        if (len > 0) {
            long val;
            uint16_t type, idx;
            if (str[0] == '$') { str += 1; }
            if ((len > 1) && (str[1] == 'x')) { str += 2; }

            val = strtol(str, NULL, 16);
            type = (uint16_t)((val >> 8) & 0xff);
            idx = (uint16_t)(val & 0xff);

            if ((type >= RVSND_DEVTYPE_MIN) && (type <= RVSND_DEVTYPE_MAX)) {
                rvsnd->SetDefaultDevice(type, idx);
                main_devs(1, (char**)(&devtypes[type]));
            }
        }
    }
    return 0;
}

/* ------------------------------------------------------------------- */
int main_mixer(int args, char** argv) {
    if (args == 2) {
        rvsnd->SetMixerValueByName(argv[0], atoi(argv[1]));
    }
    
    if (args >= 0) {
        uint16_t i, j;
        uint16_t numdevs = rvsnd->GetNumDevices(RVSND_DEVTYPE_MIXER);
        printf("dev  ctr  xb vol id\n");
        for (i=0; i<numdevs; i++) {
            uint16_t controls;
            rvsnd_devinfo_t devinfo;
            rvsnd->GetDeviceInfo(&devinfo, RVSND_DEVTYPE_MIXER, i);
            controls = rvsnd->GetNumMixerControls(i);
            /*printf("%04x %s\n", devinfo.id, devinfo.name);*/
            for (j=0; j<controls; j++) {
                uint16_t val = 0;
                rvsnd_mixinfo_t mixinfo;
                rvsnd->GetMixerControlInfo(&mixinfo, i, j);
                val = rvsnd->GetMixerValueById(mixinfo.id);
                printf("%04x:%04x ", devinfo.id, mixinfo.id);
                if (mixinfo.flg == 0) { printf(".. "); }
                else { printf("%2d ", mixinfo.flg); }
                printf("%3d %s:%s\n", val, devinfo.name, mixinfo.name);
            } printf("\n");
        }
    }
}


/* ------------------------------------------------------------------- */
static rvsnd_t* rvsnd_Get(void) {
    if (Getcookie(C_RSND, (long*)&rvsnd) != C_FOUND) { rvsnd = 0; }
    if ((rvsnd == 0) || (rvsnd->magic != C_RSND)) { rvsnd = 0; }
    return rvsnd;
}


int info(void) {
    printf("usage: rvsctrl [options]\n");
    printf("  devs   {type}\n");
    printf("  mix    {id}\n");
    printf("  select [id]\n");
    return 0;
}

int super_main(int args, char** argv) {
    printf("\n");
    rvsnd = rvsnd_Get();
    if (!rvsnd) {
        printf("error: rvsnd not installed\n");
        return -1;
    }

    if (args < 2) {
        return info();
    }

    if (stricmp(argv[1], "devs") == 0) {
        return main_devs(args-2, &argv[2]);
    }
    if (stricmp(argv[1], "select") == 0) {
        return main_select(args-2, &argv[2]);
    }
    if (stricmp(argv[1], "mix") == 0) {
        return main_mixer(args-2, &argv[2]);
    }

    return 0;
}


/* ------------------------------------------------------------------- */
#if defined(__PUREC__)
unsigned long _StkSize = 4096;
#endif
extern unsigned long _PgmSize;
static int _super_args;
static char** _super_argv;
long super_trampoline() { return super_main(_super_args, _super_argv); }
int main(int _user_args, char** _user_argv) {
    _super_args = _user_args; _super_argv = _user_argv;
    if (Supexec(super_trampoline) == 0) {
        Ptermres(_PgmSize, 0);
    }
    return 0;
}
