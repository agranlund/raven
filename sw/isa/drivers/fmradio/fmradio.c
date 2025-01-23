/*-------------------------------------------------------------------------------
 * FMRADIO for Atari with ISA bus
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
unsigned long _StkSize = 4096;
extern unsigned long _PgmSize;

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mint/cookie.h"
#include "mint/osbind.h"
#include "fmradio.h"

extern fmdriver_t drv_aims;

static fmdriver_t* drivers[] = {
    &drv_aims,
    0
};


void info(void) {
    printf(
        "\n"
        "usage: fmradio.prg [arg 1]\n"
        "\n"
        "[arg 1] = Any of the following :\n"
        "          i.   [on] or [off] to switch on or off fm radion\n"
        "          ii.  Frequency value [88-108] Mhz\n"
        "          iii. [-] or [+] to decrease or increase volume\n"
        "\n"
    );
}

/* ------------------------------------------------------------------- */
long super_main(int args, char** argv) {

    fmdriver_t* drv = drivers[0];

    if (args > 1)
    {
        char* arg = argv[1];

        if (stricmp(arg, "on") == 0)
        {
            drv->Start();
        }
        else if (stricmp(arg, "off") == 0)
        {
            drv->Stop();
        }
        else if (arg[0] == '+')
        {
            drv->AdjustVolume(16);
        }
        else if (arg[0] == '-')
        {
            drv->AdjustVolume(-16);
        }
        else if (arg[0] >= '1' && arg[0] <= '9')
        {
            uint32_t freq = 0;
            char* delim = strchr(arg, '.');
            if (delim) {
                freq = ((delim[1] - '0') * 256) / 10;
            }
            *delim = 0;
            freq += (((uint32_t)atoi(argv[1])) << 8);
            drv->SetFrequency(freq);
        }
    }
    else
    {
        info();
    }
    return 0;
}


/* ------------------------------------------------------------------- */
static int _super_args;
static char** _super_argv;
long super_trampoline() {
    return super_main(_super_args, _super_argv);
}
int main(int _user_args, char** _user_argv) {
    _super_args = _user_args; _super_argv = _user_argv;
    Supexec(super_trampoline);
    return 0;
}

