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

static uint32_t test_lines(uint16_t flg) {
#if 1
    return 0;
#else
    static int16_t pts[1024*2];
    uint32_t i, tm;
    uint32_t w = 640;
    uint32_t h = w >> 1;
    uint32_t size = ((w>>1)*h);

    for (i=0; i<w; i+=2) {
        pts[i+0] = (w>>1) - (i>>1);
        pts[i+1] = (w>>1) + (i>>1);
    }

    tm = (200UL * 4) + hz200();
    for (i = 0; hz200() < tm; i++) {
        card->hlines(flg, i&255UL, h, 0, pts);
    }
    return (((i >> 2) * size) / 1024);
#endif
}

static uint32_t test_cpufill(void) {
    uint32_t i, j, tm;
    uint32_t* dst;
    uint32_t c[2] = {0x01010101UL, 0x02020202UL };
    uint32_t size = ((uint32_t)nova->max_x + 1) * (nova->max_y + 1);

    tm = (200UL * 4) + hz200();
    for (i = 0; hz200() < tm; i++) {
        uint32_t col = c[i&1];
        dst = (uint32_t*)nova->mem_base;
        for (j=0; j<size; j+=16) {
            *dst++ = col;
            *dst++ = col;
            *dst++ = col;
            *dst++ = col;
        }
    }
    return (((i >> 2) * size) / 1024);
}

static uint32_t test_fillrate(void) {
    uint32_t i, tm;
    rect_t s;

    s.min.x = 0;
    s.min.y = 0;
    s.max.x = nova->max_x;
    s.max.y = nova->max_y;

    tm = (200UL * 4) + hz200();
    for (i = 0; hz200() < tm; i++) {
        card->blit(BL_FILL|BL_FGCOL(i&15)|BL_BGCOL(15-(i&15))|BL_PATTERN(i&7)|BL_ROP_S, &s, &s.min);
    }
    card->blit(BL_FILL|BL_FGCOL(2)|BL_BGCOL(0)|BL_PATTERN(4)|BL_ROP_S, &s, &s.min);
    return (((i >> 2) * nova->max_x * nova->max_y) / 1024);
}

static uint32_t test_blitrate(void) {
    uint32_t i, tm;
    rect_t s;
    vec_t d;

    s.min.x = 0;
    s.min.y = 1;
    s.max.x = nova->max_x;
    s.max.y = nova->max_y;
    
    d.x = 0;
    d.y = 0;
    tm = (200UL * 4) + hz200();
    for (i = 0; hz200() < tm; i++) {
        card->blit(BL_ROP_S, &s, &d);
    }
    return (((i >> 2) * nova->max_x * nova->max_y) / 1024UL);
}


void test_speed(void) {
    uint32_t r_swfill, r_hwfill, r_hwblit, r_hwply0, r_hwply1;

    r_swfill = test_cpufill();
    r_hwfill = test_fillrate();
    r_hwply0 = test_lines(0);
    r_hwply1 = test_lines(4);
    r_hwblit = test_blitrate();

    printf("swfill: %3ld Mb/s (%4ld Kb/30hz)\n", r_swfill / 1024, r_swfill / 30);
    printf("hwfill: %3ld Mb/s (%4ld Kb/30hz)\n", r_hwfill / 1024, r_hwfill / 30);
    printf("hwblit: %3ld Mb/s (%4ld Kb/30hz)\n", r_hwblit / 1024, r_hwblit / 30);
    printf("hwply0: %3ld Mb/s (%4ld Kb/30hz)\n", r_hwply0 / 1024, r_hwply0 / 30);
    printf("hwply1: %3ld Mb/s (%4ld Kb/30hz)\n", r_hwply1 / 1024, r_hwply1 / 30);
    printf("\nPress any key to exit.\n");
    while(Cconis() != -1);
    Cconin();
}
