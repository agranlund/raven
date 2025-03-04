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
#include "flash.h"

char mon_arg[1024];

uint32_t getRomVersion(void* rom) {
    uint32_t* rptr = *((uint32_t**)rom);
    if (rptr[0] == 0x5241564EUL) {
        return rptr[1];
    }
    return 0UL;
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
            } else if (getRomVersion(data) == 0UL) {
                printf("%s is not a valid rom image\n", filename);
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

    if (args != 2) {
        printf("usage: flash.tos <filename>\n");
        return 0;
    }

    rom_data = loadRom(argv[1], &rom_size);
    if (rom_data == 0) { return 0; }

    printf("\n");
    printf(" rom version....%08lx\n", getRomVersion((void*)rom_data));
    printf(" rom size.......%ld Kb\n", rom_size / 1024);
    if (!flash_Open()) {
        printf("Flash identification failed\n");
        Mfree(rom_data);
        return 0;
    }
    printf("\n");
    printf(" WARNING: Do not turn off the computer.\n");
    printf(" The power LED will blink continously during programming and\n");
    printf(" the computer will restart automatically when completed.\n");
    printf("\n");

    flash_Program(rom_data, rom_size);  /* restarts when done */

    flash_Close();
    Mfree(rom_data);
    return 0;
}

unsigned long _StkSize = 4096;
int main(int __args, char** __argv)
{
    args = __args; argv = __argv;
    return (int)Supexec(supermain);
}
