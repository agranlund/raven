/*-------------------------------------------------------------------------------
 * NOVA driver test
 * (c)2025 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  whthout even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/
#include "rvtest.h"

static bool loadbmp(char* filename, uint8_t* dst) {
    uint32_t i, offs;
    int16_t w,h;
    static char hdr[54];
    int16_t fh = (int16_t)Fopen(filename, 0);
    if (fh < 0) {
        return false;
    }
    Fread(fh, 54, (void*)hdr);
    offs  = hdr[13]; offs <<= 8;
    offs |= hdr[12]; offs <<= 8;
    offs |= hdr[11]; offs <<= 8;
    offs |= hdr[10];
    printf("offs = %ld\n", offs);
    Fseek(offs, fh, SEEK_SET);

    /* source: 640 x 3276 */
    w = 640;
    h = card->bank_size / nova->pitch;
    if (h > 3276) { h = 3276; }
    for (i=0; i<h; i++) {
        Fread(fh, w, dst);
        dst += nova->pitch;
    }
    Fclose(fh);
    return true;
}

void delay(int16_t num) {
    for (; num >= 0; num--) {
        nova->p_vsync();
    }
}

void test_scroll(void) {
    int16_t maxy;
    int16_t y = 0;
    int16_t step = 1;
    int16_t h = card->bank_size / nova->pitch;
    h = (h > 3276) ? 3276 : h;
    maxy = h - 480;

    if (!loadbmp("scroll.bmp", (uint8_t*)nova->base)) {
        int s = 1;
        int x = 1;
        int y; uint8_t* ptr = (uint8_t*)nova->base;
        for (y=0; y<h; y++) {
            ptr[x] = (y&255);
            ptr += nova->pitch;
            if ((x == 0) || (x == nova->max_x)) {
                s = -s;
            }
            x += s;
        }
    }

    while (1) {
        uint32_t addr = (uint32_t)nova->base + (((uint32_t)nova->pitch) * y);
        nova->p_setscreen((void*)addr);
        nova->p_vsync();
        if (y == 0) {
            delay(120);
            step = 1;
        } else if (y == maxy) {
            delay(120);
            step = -1;
        }
        y += step;

        if (Cconis() == -1) {
            Cconin();
            break;
        }
    }
}
