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
    for (i=0; i<64; i++) {
        Fread(fh, 64, dst);
        dst += nova->pitch;
    }
    Fclose(fh);
    return true;
}

typedef struct {
    int16_t x;
    int16_t y;
    int16_t dx;
    int16_t dy;
} spr_t;

#define sprcount 32
static spr_t sprites[sprcount];

static void spawn(uint16_t i) {
    sprites[i].x = rand() & 1023;
    sprites[i].y = 0;
    sprites[i].dx = (rand() & 63) - 32;
    sprites[i].dy = 16+((rand() & 31) << 2);
    while(sprites[i].x >= (nova->max_x - 64)) { sprites[i].x -= (nova->max_x - 64); }
    sprites[i].x <<= 4;
}

#define BLIT_SRCKEY_ONEPASS     0
#define BLIT_DSTKEY_ONEPASS     1
#define BLIT_SRCKEY_TWOPASS     2

#define PRESENT_BLIT            0
#define PRESENT_SWAP            1

void test_sprites(void) {
    rect_t r_screen[2], r_spr, r_hbar, r_vbar;
    uint32_t i, tcount, tblit;
    int screen_idx = 0;
    int mode = BLIT_SRCKEY_ONEPASS;
    int present = PRESENT_SWAP;

    r_screen[0].min.x = 0;
    r_screen[0].min.y = 0;
    r_screen[0].max.x = 639;
    r_screen[0].max.y = 479;

    r_screen[1].min.x = r_screen[0].min.x;
    r_screen[1].min.y = r_screen[0].max.y + 1;
    r_screen[1].max.x = r_screen[0].max.x;
    r_screen[1].max.y = r_screen[1].min.y + r_screen[0].max.y;

    r_hbar.min.x = 0;
    r_hbar.min.y = 0;
    r_hbar.max.x = r_screen[1].max.x - r_screen[1].min.x;
    r_hbar.max.y = (r_screen[1].max.y - r_screen[1].min.y) / 4;

    r_vbar.min.x = 0;
    r_vbar.min.y = 0;
    r_vbar.max.x = 0;
    r_vbar.max.y = r_hbar.max.y;

    r_spr.min.x = r_screen[1].min.x;
    r_spr.min.y = r_screen[1].max.y + 1;
    r_spr.max.x = r_spr.min.x + 63;
    r_spr.max.y = r_spr.min.y + 63;

    loadbmp("sprite.bmp", (uint8_t*)vram_addr(&r_spr.min));
    for (i=0; i<sprcount; i++) {
        spawn(i);
    }

    tcount = 0;
    tblit = 0;
    while (1) {
        vec_t d;
        rect_t* r_fb = &r_screen[(screen_idx+0)&1];
        rect_t* r_bb = &r_screen[(screen_idx+1)&1];
        uint32_t tlast = hz200();

        if (tcount++ >= 5) {
            uint32_t ms = tblit * 5;
            r_hbar.max.x = (((uint32_t)r_screen[0].max.x) * ms) / ((tcount * 167 * 3) / 10);
            tcount = 0;
            tblit = 0;
        }

        if (mode == BLIT_SRCKEY_ONEPASS)
        {
            /* background */
            card->blit(BL_FILL|BL_FGCOL(1)|BL_PATTERN(6)|BL_ROP_S, r_bb, &r_bb->min);
            /* sprites*/
            for (i=0; i<sprcount; i++) {
                d.x = r_bb->min.x + (sprites[i].x >> 4);
                d.y = r_bb->min.y + (sprites[i].y >> 4);
                card->blit(BL_BLIT|BL_TRANSPARENT|BL_ROP_S, &r_spr, &d);
                sprites[i].x += sprites[i].dx;
                sprites[i].y += sprites[i].dy;
                if ((sprites[i].x < 0) || (sprites[i].x >= ((640-64)<<4)) || (sprites[i].y >= ((480-64)<<4))) {
                    spawn(i);
                }
            }
            /* bar */
            d.x = r_bb->min.x;
            d.y = r_bb->min.y + ((r_bb->max.y - r_bb->min.y) / 2) - ((r_bb->max.y - r_bb->min.y) / 8);
            card->blit(BL_FILL|BL_FGCOL(2)|BL_PATTERN(5)|BL_TRANSPARENT|BL_ROP_S, &r_hbar, &d);
        }
        else if (mode == BLIT_DSTKEY_ONEPASS)
        {
            /* clear */
            card->blit(BL_FILL|BL_FGCOL(0)|BL_ROP_S, r_bb, &r_bb->min);
            /* bar */
            d.x = r_bb->min.x;
            d.y = r_bb->min.y + ((r_bb->max.y - r_bb->min.y) / 2) - ((r_bb->max.y - r_bb->min.y) / 8);
            card->blit(BL_FILL|BL_TRANSPARENT|BL_FGCOL(2)|BL_PATTERN(5)|BL_ROP_S, &r_hbar, &d);
            /* sprites */
            for (i=0; i<sprcount; i++) {
                d.x = r_bb->min.x + (sprites[i].x >> 4);
                d.y = r_bb->min.y + (sprites[i].y >> 4);
                card->blit(BL_BLIT|BL_TRANSPARENT|BL_ROP_S, &r_spr, &d);
                sprites[i].x += sprites[i].dx;
                sprites[i].y += sprites[i].dy;
                if ((sprites[i].x < 0) || (sprites[i].x >= ((640-64)<<4)) || (sprites[i].y >= ((480-64)<<4))) {
                    spawn(i);
                }
            }
            /* background */
            card->blit(BL_FILL|BL_TRANSPARENT|BL_FGCOL(1)|BL_PATTERN(6)|BL_ROP_S, r_bb, &r_bb->min);
        }
        else if (mode == BLIT_SRCKEY_TWOPASS)
        {
            /* background */
            card->blit(BL_FILL|BL_FGCOL(1)|BL_PATTERN(6)|BL_ROP_S, r_bb, &r_bb->min);
            /* sprites*/
            for (i=0; i<sprcount; i++) {
                d.x = r_bb->min.x + (sprites[i].x >> 4);
                d.y = r_bb->min.y + (sprites[i].y >> 4);
                card->blit(BL_BLIT|BL_MONO|BL_FGCOL(0x00)|BL_BGCOL(0xff)|BL_ROP_DSa, &r_spr, &d);
                card->blit(BL_BLIT|BL_ROP_DSo, &r_spr, &d);
                sprites[i].x += sprites[i].dx;
                sprites[i].y += sprites[i].dy;
                if ((sprites[i].x < 0) || (sprites[i].x >= ((640-64)<<4)) || (sprites[i].y >= ((480-64)<<4))) {
                    spawn(i);
                }
            }
            /* bar */
            d.x = r_bb->min.x;
            d.y = r_bb->min.y + ((r_bb->max.y - r_bb->min.y) / 2) - ((r_bb->max.y - r_bb->min.y) / 8);
            card->blit(BL_FILL|BL_FGCOL(0xff)|BL_PATTERN(5)|BL_ROP_DSna, &r_hbar, &d);
            card->blit(BL_FILL|BL_FGCOL(2)|BL_PATTERN(5)|BL_ROP_DSo, &r_hbar, &d);
        }

        /* frame lines */
        {
            int16_t step = r_screen[0].max.x / 3;
            d.x = r_bb->min.x + step;
            d.y = r_bb->min.y + ((r_bb->max.y - r_bb->min.y) >> 1) - ((r_bb->max.y - r_bb->min.y) >> 3);
            card->blit(BL_FILL|BL_FGCOL(0xff)|BL_ROP_S, &r_vbar, &d);
            d.x += step;
            card->blit(BL_FILL|BL_FGCOL(0xff)|BL_ROP_S, &r_vbar, &d);
        }

        /* present */
        if (present == PRESENT_SWAP) {
            nova->p_setscreen(vram_addr(&r_bb->min));
            tblit += hz200() - tlast;
            nova->p_vsync();
            screen_idx++;
        } else {
            card->blit(BL_ROP_S, r_bb, &r_fb->min);
            tblit += hz200() - tlast;
        }

        if (Cconis() == -1) {
            int16_t key = (int16_t)(Cconin() & 0xff);
            if (key == 's') {
                present = PRESENT_SWAP;
            } else if (key == 'b') {
                present = PRESENT_BLIT;
            } else if (key == '1') {
                mode = BLIT_SRCKEY_ONEPASS;
            } else if (key == '2') {
                mode = BLIT_DSTKEY_ONEPASS;
            } else if (key == '3') {
                mode = BLIT_SRCKEY_TWOPASS;
            } else {
                break;
            }
        }
    }
    nova->p_setscreen(vram_addr(&r_screen[0].min));
}
