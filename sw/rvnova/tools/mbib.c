/*-------------------------------------------------------------------------------
 * dummy bib creator
 * (c)2026 Anders Granlund
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nova.h"

nova_bibres_t bibs[128];
int bibcnt = 0;

typedef struct {
    int w, h, b;
} res_t;

res_t res_vga[] = {
    {  320,  200, 4 },
    {  320,  200, 8 },
    {  640,  480, 1 },
    {  640,  480, 4 },
};

res_t res_svga[] = {
    {   640,  480, 8 },
    {   800,  600, 8 },
    {  1024,  768, 8 },
    {  1280,  720, 8 },
};

int real_bpp(int bpp) {
    if (bpp > 16) {
        return 32;
    } else if (bpp > 8) {
        return 16;
    } else if (bpp > 4) {
        return 8;
    } else if (bpp > 1) {
        return 4;
    }
    return 1;
}

int bpp_to_mode(int bpp) {
    switch (bpp) {
        case 1: return NOVA_MODE_1BPP;
        case 2:
        case 4: return NOVA_MODE_4BPP;
        case 8: return NOVA_MODE_8BPP;
        case 15: return NOVA_MODE_15BPP;
        case 16: return NOVA_MODE_16BPP;
        case 24: return NOVA_MODE_24BPP;
        case 32: return NOVA_MODE_32BPP;
        default: return 0;
    }
}

uint16_t be16(uint16_t le16) {
    return ((le16 >> 8) | (le16 << 8));
}

void makebib(res_t* res) {
    nova_bibres_t* bib = &bibs[bibcnt];

    memset(bib, 0, sizeof(nova_bibres_t));
    sprintf((char*)bib->name, "%dx%dx%d", res->w, res->h, res->b);

    int bpp = real_bpp(res->b);
    uint16_t mode;
    switch (res->b) {
        case  1: mode = NOVA_MODE_1BPP; break;
        case  2:
        case  4: mode = NOVA_MODE_4BPP; break;
        case  8: mode = NOVA_MODE_8BPP; break;
        case 15: mode = NOVA_MODE_15BPP; break;
        case 16: mode = NOVA_MODE_16BPP; break;
        case 24: mode = NOVA_MODE_24BPP; break;
        case 32: mode = NOVA_MODE_32BPP; break;
        default: mode = 0; break;
    }
    bib->mode    = be16(mode);
    bib->pitch   = be16((res->b < 8) ? (res->w / 8) : (res->w * bpp));
    bib->planes  = be16(res->b);
    bib->colors  = be16((res->b < 16) ? (1 << res->b) : 0);
    bib->hcmode  = be16(NOVA_HCMODE_1X1);
    bib->max_x   = be16(res->w - 1);
    bib->max_y   = be16(res->h - 1);
    bib->real_x  = be16(res->w - 1);
    bib->real_y  = be16(res->h - 1);
    bib->freq    = be16(0);
    bib->freq2   = 0;
    bib->low_res = 0;
    bibcnt++;
}

void sortbibs() {
    bool sort_by_planes = true;
    nova_bibres_t tmp;
    nova_bibres_t* b2 = &tmp;
    for (int i=0; i<bibcnt-1; i++) {
        for (int j=0; j<bibcnt-1-i; j++) {
            nova_bibres_t* b0 = &bibs[j+0];
            nova_bibres_t* b1 = &bibs[j+1];
            bool swap = false;
            if (sort_by_planes) {
                swap = be16(b0->planes) > be16(b1->planes);
                if (!swap && (be16(b0->planes) == be16(b1->planes))) {
                    swap = (be16(b0->max_x) > be16(b1->max_x));
                    if (!swap && (be16(b0->max_x) == be16(b1->max_x))) {
                        swap = (be16(b0->max_y) > be16(b1->max_y));
                    }
                }
            } else {
                swap = (be16(b0->max_x) > be16(b1->max_x));
                if (!swap && (be16(b0->max_x) == be16(b1->max_x))) {
                    swap = (be16(b0->max_y) > be16(b1->max_y));
                    if (!swap && (be16(b0->max_y) == be16(b1->max_y))) {
                        swap = (be16(b0->planes) > be16(b1->planes));
                    }
                }
            }
            if (swap) {
                memcpy(b2, b0, sizeof(nova_bibres_t));
                memcpy(b0, b1, sizeof(nova_bibres_t));
                memcpy(b1, b2, sizeof(nova_bibres_t));
            }
        }
    }
}

int writebibs(const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) { return -1; }
    sortbibs();
    printf("%s\n", filename);
    for (int i=0; i<bibcnt; i++) {
        nova_bibres_t* bib = &bibs[i];
        printf(" %s\n", bib->name);
        fwrite((void*)bib, sizeof(nova_bibres_t), 1, f);
    }
    fclose(f);
    return 0;
}

int main() {
    for (int i=0; i<sizeof(res_vga)/sizeof(res_t); i++) {
        makebib(&res_vga[i]);
    }
    writebibs("vga.bib");
    for (int i=0; i<sizeof(res_svga)/sizeof(res_t); i++) {
        makebib(&res_svga[i]);
    }
    writebibs("svga.bib");
    return 0;
}
