/*-------------------------------------------------------------------------------
 * mon
 * (c)2024 Anders Granlund
 *
 * Use ROM monitor from TOS
 *
 *-------------------------------------------------------------------------------
 *
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
 *
 *-----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <mint/osbind.h>
#include "raven.h"
#include "sysutil.h"

char mon_arg[1024];
void (*putchar_old)(int32_t);
int32_t (*getchar_old)(void);

void cdecl putchar_new(int32_t c) {
    putchar((int)c);
}

int32_t cdecl getchar_new(void) {
    return (int32_t)getchar();
}

bool checkRomVersion(void) {
    if ((raven()->version & 0x00ffffffUL) < 0x00241226UL) {
        printf("ROM version is too old\n");
        return false;
    }
    return true;
}

long supermain(int args, char** argv)
{
    int i; char* arg_ptr = mon_arg;

    if (!checkRomVersion()) {
        return -1;
    }

    getchar_old  = *(raven()->mon_fgetchar);
    putchar_old  = *(raven()->mon_fputchar);
    *(raven()->mon_fgetchar)  = getchar_new;
    *(raven()->mon_fputchar)  = putchar_new;

    for (i = 1; i<args; i++)
    {
        int32_t len = strlen(argv[i]);
        strcpy(arg_ptr, argv[i]);
        arg_ptr += len;
        if (i < (args-1)) {
            *arg_ptr++ = ' ';
        }
    }
    *arg_ptr++ = 0;
    raven()->mon_Exec(mon_arg);

    *(raven()->mon_fgetchar)  = getchar_old;
    *(raven()->mon_fputchar)  = putchar_old;
    return 0;
}

int main(int args, char** argv) {
    return Supmain(args, argv, supermain);
}
