/*-------------------------------------------------------------------------------
 * Bios logo image converter
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
#include <stdbool.h>
#include <string.h>

/* ----------------------------------------------------------------- */
int abs(int a) { return a < 0 ? -a : a; }
int min(int a, int b) { return (a < b) ? a : b; }
int max(int a, int b) { return (a > b) ? a : b; }
uint16_t bswap16(uint16_t val) { return ((val >> 8) | (val << 8)); }

/* ----------------------------------------------------------------- */
typedef struct
{
    char signature[2];
    uint32_t filesize;
    uint32_t reserved;
    uint32_t offset;
    int32_t  size;
    int32_t  width;
    int32_t  height;
    int16_t  planes;
    int16_t  bitcount;
    int32_t  compression;
    int32_t  imagesize;
    int32_t  xpixelsperm;
    int32_t  ypixelsperm;
    int32_t  colorsused;
    int32_t  colorsimportant;

    bool     yflip;
    uint8_t* colortable;
    uint8_t* rasterdata;
} __attribute__((packed)) bmp_t;

void bmp_flipy(bmp_t* bmp) {
    static uint8_t lx[1024];
    int lines = abs(bmp->height);
    for (int i=0; i<(lines/2); i++) {
        uint8_t* l0 = &(bmp->rasterdata[bmp->width * i]);
        uint8_t* l1 = &(bmp->rasterdata[bmp->width * (lines-1-i)]);
        if (l0 != l1) {
            memcpy(lx, l0, bmp->width);
            memcpy(l0, l1, bmp->width);
            memcpy(l1, lx, bmp->width);
        }
    }
}

bool bmp_load(const char* fname, bmp_t* bmp) {
    FILE* f = fopen(fname, "rb");
    if (f) {
        memset(bmp, 0, sizeof(bmp_t));
        fread(bmp, 14 + 40, 1, f);
        uint32_t rastersize = bmp->width * abs(bmp->height);
        printf("load %s (%d x %d)\n", fname, bmp->width, abs(bmp->height));
        bmp->colortable = malloc(bmp->colorsused * 4);
        memset(bmp->colortable, 0, bmp->colorsused * 4);
        fread(bmp->colortable, 1, bmp->colorsused * 4, f);
        bmp->rasterdata = malloc(rastersize);
        memset(bmp->rasterdata, 0, rastersize);
        fseek(f, bmp->offset, SEEK_SET);
        fread(bmp->rasterdata, rastersize, 1, f);
        fclose(f);
        if (bmp->height > 0) {
            bmp->yflip = true;
            bmp_flipy(bmp);
        }
        bmp->height = abs(bmp->height);
        return true;
    }
    printf("failed to load %s\n", fname);
    return false;
}

bool bmp_save(char* fname, bmp_t* bmp) {
    FILE* f = fopen(fname, "wb");
    if (f) {
        printf("save %s\n", fname);
        bmp->offset = 14 + 40 + (bmp->colorsused * 4);
        if (bmp->yflip) {
            bmp_flipy(bmp);
        }
        bmp->height = bmp->yflip ? abs(bmp->height) : -abs(bmp->height);
        fwrite(bmp, 14 + 40, 1, f);
        fwrite(bmp->colortable, bmp->colorsused * 4, 1, f);
        fwrite(bmp->rasterdata, bmp->width * abs(bmp->height), 1, f);
        fclose(f);
        if (bmp->yflip) {
            bmp_flipy(bmp);
        }
        bmp->height = abs(bmp->height);
        return true;
    }
    printf("failed to save %s\n", fname);
    return false;
}

void bmp_close(bmp_t* bmp) {
    if (bmp->rasterdata) {
        free(bmp->rasterdata);
        bmp->rasterdata = 0;
    }
}


/* ----------------------------------------------------------------- */
bool fload(char* fname, int offset, void* buf, int bufsize) {
    FILE* f = fopen(fname, "rb");
    if (f) {
        fseek(f, offset, SEEK_SET);
        fread(buf, bufsize, 1, f);
        fclose(f);
        printf("loaded %s\n", fname);
        return true;
    }
    printf("failed to load %s\n", fname);
    return false;
}

bool fsave(char* fname, void* buf, int bufsize) {
    FILE* f = fopen(fname, "wb");
    if (f) {
        fwrite(buf, bufsize, 1, f);
        fclose(f);
        printf("saved %s\n", fname);
        return true;
    }
    printf("failed to save %s\n", fname);
    return false;
}


/* ----------------------------------------------------------------- */
uint8_t readplanebyte(uint8_t* src, int plane) {
    uint8_t data = 0;
    for (int x=0; x<8; x++) {
        uint8_t col = *src++;
        data <<= 1;
        data |= (((col & (1<<plane)) >> plane) & 1);
    }
    return data;
}

bool img_generate(char* in, char* out, int planes) {
    bmp_t bmp;
    static uint8_t tempbuf[1024*1024];
    if(!bmp_load(in, &bmp)) {
        return false;
    }
    int numcolors = (1 << planes);
    uint32_t psize[4] = {0,0,0,0};
    
    memset(tempbuf, 0, 64);
    tempbuf[0] = 'P';
    tempbuf[1] = 'I';
    tempbuf[2] = 'M';
    tempbuf[3] = 'G';
    *((uint16_t*)(&tempbuf[4])) = 0;
    *((uint16_t*)(&tempbuf[6])) = bswap16(bmp.width >> 3);
    *((uint16_t*)(&tempbuf[8])) = bswap16(bmp.height);
    *((uint16_t*)(&tempbuf[10])) = bswap16(planes);
    uint8_t* dst = &tempbuf[64];

    for (int p=0; p<planes; p++) {
        psize[p] = 0;
        for (int y=0; y<bmp.height; y++) {
            int x = 0;
            int run = 1;
            uint8_t* pstart = dst;
            while (x < bmp.width) {
                uint8_t pcur, pnew;
                uint8_t* src = &bmp.rasterdata[(y * bmp.width) + x];
                
                pcur = readplanebyte(src, p); src += 8; x += 8;
                if (x >= bmp.width) {
                    *dst++ = 0x01;
                    *dst++ = pcur;
                    break;
                }

                pnew = readplanebyte(src, p); src += 8; x += 8;
                uint8_t* runhdr = dst; dst++;
                *dst++ = pcur;
                int run = 2;
                if (pcur == pnew) {
                    while ((x < bmp.width) && (run < 127)) {
                        pnew = readplanebyte(src, p);
                        if (pnew == pcur) {
                            src += 8;
                            x += 8;
                            run++;
                        } else {
                            break;
                        }
                    }
                    *runhdr = run | 0x80;
                } else {
                    *dst++ = pnew;
                    while ((x < bmp.width) && (run < 127)) {
                        pcur = pnew;
                        pnew = readplanebyte(src, p);
                        if (pnew != pcur) {
                            *dst++ = pnew;
                            src += 8;
                            x += 8;
                            run++;
                        } else {
                            break;
                        }
                    }
                    *runhdr = run;
                }
            }
            int linelen = (dst - pstart);
            psize[p] += linelen;
        }
    }
    printf("psize0: %d\n", psize[0]);
    printf("psize1: %d\n", psize[1]);
    printf("psize2: %d\n", psize[2]);
    printf("psize3: %d\n", psize[3]);
    int totalsize = 64 + psize[0] + psize[1] + psize[2] + psize[3];

    *((uint16_t*)(&tempbuf[16])) = bswap16(psize[0]);
    *((uint16_t*)(&tempbuf[18])) = bswap16(psize[1]);
    *((uint16_t*)(&tempbuf[20])) = bswap16(psize[2]);
    *((uint16_t*)(&tempbuf[22])) = bswap16(psize[3]);
    fsave(out, tempbuf, totalsize);
    return true;
}

int main(int args, char* argv[]) {
    return (img_generate("logo.bmp", "logo.bin", 1) ? 0 : -1);
}
