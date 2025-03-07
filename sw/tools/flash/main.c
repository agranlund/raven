/*-------------------------------------------------------------------------------
 * flash
 * (c)2025 Anders Granlund
 *
 * ROM flasher
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

#define ROM_VERSION_REQUIRED 0x20250307UL


char mon_arg[1024];
void (*putchar_old)(int32_t);
int32_t (*getchar_old)(void);

void cdecl putchar_new(int32_t c) {
    putchar((int)c);
}

int32_t cdecl getchar_new(void) {
    return (int32_t)getchar();
}

void* loadRom(char* filename, int32_t* sizeOut) {
    void* data = 0;
    int32_t result = Fopen(filename, 0);

    if (result <= 0) {
        printf("Failed to open '%s'\n", filename);
        return 0;
    } else {
        int16_t fhandle = (result & 0xffff);
        int32_t fsize = Fseek(0, fhandle, 2);
        *sizeOut = fsize;
        Fseek(0, fhandle, 0);
        data = (void*) Malloc(fsize);
        if (data == 0) {
            printf("Out of memory\n");
        } else {
            result = Fread(fhandle, fsize, data);
            if (result != fsize) {
                printf("Failed to load '%s'\n", filename);
                data = 0;
            }
        }
        Fclose(fhandle);
    }
    return data;
}

static int args;
static char** argv;
long supermain()
{
    uint16_t sr;
    void* rom_data;
    int32_t rom_size;

    if (raven()->version < ROM_VERSION_REQUIRED) {
        printf("This program requires ROM %08lx or later\n", ROM_VERSION_REQUIRED);
        return 0;
    }

    if (args != 2) {
        printf("usage: flash.tos <filename>\n");
        return 0;
    }

    rom_data = loadRom(argv[1], &rom_size);
    if (rom_data == 0) { return 0; }

    getchar_old  = *(raven()->mon_fgetchar);
    putchar_old  = *(raven()->mon_fputchar);
    *(raven()->mon_fgetchar)  = getchar_new;
    *(raven()->mon_fputchar)  = putchar_new;

    raven()->rom_Program(rom_data, rom_size);

    Mfree(rom_data);
    *(raven()->mon_fgetchar)  = getchar_old;
    *(raven()->mon_fputchar)  = putchar_old;
    return 0;
}

unsigned long _StkSize = 4096;
int main(int __args, char** __argv)
{
    args = __args; argv = __argv;
    return (int)Supexec(supermain);
}
