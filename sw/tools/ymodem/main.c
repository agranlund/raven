/*-------------------------------------------------------------------------------
 * ramdisk
 * simple ramdisk utility
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
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <tos.h>
#include "ymodem.h"

void flush_cin(void) {
    int max = 1024;
    while((Cconis() == -1) && (max != 0)) {
        max--;
        Crawcin();
    }
}

static LINE pbuf;
unsigned long _StkSize = 4096;
int main(int args, char** argv)
{
    const char* path = "";

    printf("[Ymodem]\n");
    if (args < 2) {
        printf("Target folder: ");
        pbuf.actuallen = 0;
        pbuf.buffer[0] = 0;
        pbuf.maxlen = FILE_PATH_LENGTH - 2;
        if (Cconrs(&pbuf) > 0) {
            path = pbuf.buffer;
        } else {
            path = "";
        }
    } else {
        path = argv[1];
    }
    ymodem_receive(path);

    flush_cin();
	return 0;
}
