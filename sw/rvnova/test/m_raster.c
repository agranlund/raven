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
/*
 * cirrus specific for convenience of testing and iterating
 * for purpose of evaluating driver implementations
 */


/*
 * large rect test:
 *  rect  full = 215 mb/s  100%
 *  rect 8band = 208 mb/s   97%
 *  rect 1band = 170 mb/s   80%
 *
 * large triangle test:
 *  8band       = 190mb/s   88%
 *  8band clean = 150mb/s   70%
 *  1band       = 120mb/s   56%
 * 
 * edge test:
 *  8x8       =  17mb/s      8%
 *  soft      =   4mb/s      2%
 *  
 */


static char result_string[256];

static uint32_t mmio_addr;
#define mmio_16le(a,b)  *((volatile uint16_t*)(mmio_addr+(a))) = (b)
#define mmio_16be(a,b)  *((volatile uint16_t*)(mmio_addr+(a))) = ((((b)&0xff00)>>8) | (((b)<<8)&0xff00))
#define mmio_24le(a,b)  *((volatile uint32_t*)(mmio_addr+(a))) = (b)
#define mmio_24be(a,b)  *((volatile uint32_t*)(mmio_addr+(a))) = (((b)<<24) | (((b)&0xff00UL)<<8) | (((b)>>8)&0xff00UL))
#define mmio_32le(a,b)  *((volatile uint32_t*)(mmio_addr+(a))) = (b)
#define mmio_start()    *((volatile uint16_t*)(mmio_addr+0x40)) = 0x0200
#define mmio_wait()     while(*((volatile uint16_t*)(mmio_addr+0x40)) & 0x0100)


/*------------------------------------------------------------------------------*/
/* triangle test                                                                */
/*------------------------------------------------------------------------------*/
static void trapez_common(void) {
    uint32_t patt_addr = card->bank_size - 2048;
    mmio_16le(0x18, 0x8000 | 0x4000);   /* mono expand, opaque, 8bpp */
    mmio_16le(0x1a, 0x0D00);            /* ROP_S */
    mmio_24be(0x14, patt_addr);         /* solid pattern */
}

static void trapez(int32_t* p, uint16_t c, int16_t band)
{
    int32_t xl = p[0];
    int32_t xr = p[2];
    int32_t y0 = p[1] >> 16;
    int32_t y1 = p[5] >> 16;
    int32_t dxl = (p[4] - p[0]) / (int16_t)(y1 - y0);    /* left slope */
    int32_t dxr = (p[6] - p[2]) / (int16_t)(y1 - y0);    /* right slope */
    uint32_t pitch = nova->max_x + 1;

    mmio_16be(0x00, c);     /* fg color */

    if (band >= 8)
    {
        mmio_16le(0x0a, 0x0700);     /* height_minus_one */
        dxl <<= 3;
        dxr <<= 3;
        for (; y0 <= y1; y0+=8) {
            uint32_t d;
            int16_t x0, x1, w;

            x0 = ((xl >> 16) + 7) & ~7;
            x1 = (xr >> 16) & ~7;
            xl += dxl;
            xr += dxr;

            w = x1 - x0;
            w = ((w >> 8) | (w << 8));
            d = (y0 * pitch) + x0;
            d = (((d)<<24) | (((d)&0xff00UL)<<8) | (((d)>>8)&0xff00UL));
            mmio_wait();
            mmio_24le(0x10, d);
            mmio_16le(0x08, w);
            mmio_start();
        }
        mmio_wait();

        /* temp add cost for future edge cleanup */
        if (band == 9)
        {
            int16_t i;
            uint16_t pat = (card->bank_size - 2048) & 0xffff;

            mmio_16le(0x08, 0x0700);    /* width */
            mmio_16be(0x14, pat);       /* left pattern */
            y0 = p[1] >> 16;
            for (; y0 <= y1; y0+=8) {
                uint32_t d = 0;
#if 0
    /* this gives 10mb/s */
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
#else                
                mmio_wait();
#endif                
                mmio_24le(0x10, d);     /* dest */
                mmio_start();
            }
            mmio_wait();

            mmio_16be(0x14, pat);       /* right pattern */
            y0 = p[1] >> 16;
            for (; y0 <= y1; y0+=8) {
                uint32_t d = 0;
#if 0
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
                cpu_nop(); cpu_nop();
#else                
                mmio_wait();
#endif                
                mmio_24le(0x10, d);     /* dest */
                mmio_start();
            }
            mmio_wait();
            mmio_16be(0x14, pat);       /* solid pattern */
        }
    }
    else
    {
        mmio_16be(0x0a, 0);     /* height_minus_one */

        /*
        mmio_16be(0x08, y1);
        dxl = 0;
        dxr = 0;
        xl = 0;
        xr = y1 << 15;
        */

        for (; y0 <= y1; y0++) {
            uint32_t d;
            int16_t x0, x1, w;

            x0 = xl >> 16;
            x1 = xr >> 16;
            xl += dxl;
            xr += dxr;

            w = x1 - x0;
            w = ((w >> 8) | (w << 8));
            d = (y0 * pitch) + x0;
            d = (((d)<<24) | (((d)&0xff00UL)<<8) | (((d)>>8)&0xff00UL));
#if 0
            {
                int16_t del = (x1 - x0) / 30;
                /*if (del < 2) { del = 2; }*/
                if (del > 40) { del = 40; }
                for (; del>=0; del--) {
                    cpu_nop();
                }
            }
#else            
            mmio_wait();
#endif            
            mmio_24le(0x10, d);
            mmio_16le(0x08, w);
            mmio_start();
        }
        mmio_wait();
    }
}

static uint32_t test_trapez(int16_t arg) {
    int32_t p[8];
    uint32_t num, time;
    uint32_t cx = (nova->max_x + 1) / 2;
    uint32_t h = (nova->max_y + 1);
    uint32_t w = h;
    if (w > cx) { w = cx; h = cx; }

    w = h = 32;

    p[0] = cx; p[1] = 0;
    p[2] = cx; p[3] = 0;
    p[4] = cx-w; p[5] = h-1;
    p[6] = cx+w; p[7] = h-1;
    for (num=0; num<8; num++) {
        p[num] <<= 16;
    }

    trapez_common();
    time = *((volatile uint32_t*)0x4ba) + 200;
    for (num=0; *((volatile uint32_t*)0x4ba) < time; num++) {
        trapez(p, num&15, arg);
    }
    sprintf(result_string, "%ld / sec (%ld mb/s)\n", num, (num * w * h) / (1024UL * 1024));
    return 0;    
}

static uint32_t test_trapez_clean(int16_t arg) {
    int32_t p[8];
    uint32_t num, time;
    (void)arg;

    /* common setup */
    {
        uint32_t patt_addr = card->bank_size - 2048;
        mmio_16le(0x18, 0x8000 | 0x4000);   /* mono expand, opaque, 8bpp */
        mmio_16le(0x1a, 0x0D00);            /* ROP_S */
        mmio_24be(0x14, patt_addr);         /* solid pattern */

        mmio_16be(0x00, 2);     /* fg color */
        mmio_16be(0x08, 7);     /* width_minus_one */
        mmio_16be(0x0a, 7);     /* height_minus_one */
       
    }

    time = *((volatile uint32_t*)0x4ba) + 200;
    for (num=0; *((volatile uint32_t*)0x4ba) < time; num++) {
        uint32_t dst = 0;
        uint16_t pat_offs = 0;
        mmio_wait();
        mmio_24le(0x10, dst);       /* dst address      */
        mmio_16le(0x14, pat_offs);  /* pattern phase    */
        mmio_start();
    }
    mmio_wait();
    sprintf(result_string, "%ld / sec (%ld mb/s)\n", num, (num * 8 * 8) / (1024UL * 1024));
    return 0;    
}


/*------------------------------------------------------------------------------*/
/* rect / generic fillrate test                                                 */
/*------------------------------------------------------------------------------*/
static void rect_common(void) {
    uint32_t patt_addr = card->bank_size - 2048;
    mmio_16le(0x18, 0x8000 | 0x4000);   /* mono expand, opaque, 8bpp */
    mmio_16le(0x1a, 0x0D00);            /* ROP_S */
    mmio_24be(0x14, patt_addr);         /* solid pattern */
}

static void rect_full(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c) {
    uint32_t pitch = nova->max_x + 1;
    uint32_t dst = (pitch * y0) + x0;
    int16_t w = (x1 - x0);
    int16_t h = (y1 - y0);
    mmio_16be(0x00, c);     /* fg color */
    mmio_16be(0x08, w);     /* width_minus_one */
    mmio_16be(0x0a, h);     /* height_minus_one */
    mmio_24be(0x10, dst);   /* dst address */
    mmio_start();
    mmio_wait();
}

static void rect_bands(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c, int16_t band_size) {
    uint32_t pitch = nova->max_x + 1;
    uint32_t dst = (pitch * y0) + x0;
    int16_t w = (x1 - x0);
    int16_t h = (y1 - y0);
    int16_t delay = band_size * 38;
    int16_t fifo = 0;
    mmio_16be(0x00, c);             /* fg color */
    mmio_16be(0x08, w);             /* width_minus_one */
    mmio_16be(0x0a, band_size - 1); /* height_minus_one */
    mmio_24be(0x10, dst);           /* dst address */

    pitch *= band_size;
    h /= band_size;

    for (; h >=0; h--) {
        int16_t i;
        uint32_t d = (((dst)<<24) | (((dst)&0xff00UL)<<8) | (((dst)>>8)&0xff00UL));
#if 0
        for (i=delay; i>=0; i--) {
            cpu_nop();
        }
        mmio_24le(0x10, d);
#elif 0
        mmio_24le(0x10, d);
        mmio_wait();
#else
        mmio_wait();
        mmio_24le(0x10, d);
#endif
        mmio_start();
        dst += pitch;
    }
    mmio_wait();
}

static uint32_t test_rectfill(int16_t band_size) {
    uint32_t num, time;
    uint32_t w = (nova->max_x + 1);
    uint32_t h = (nova->max_y + 1);

    h >>= 1;

    rect_common();
    if (band_size <= 0) {
        time = *((volatile uint32_t*)0x4ba) + 200;
        for (num=0; *((volatile uint32_t*)0x4ba) < time; num++) {
            rect_full(0, 0, w-1, h-1, (num & 15));
        }
    } else {
        time = *((volatile uint32_t*)0x4ba) + 200;
        for (num=0; *((volatile uint32_t*)0x4ba) < time; num++) {
            rect_bands(0, 0, w-1, h-1, (num & 15), band_size);
        }
    }
    sprintf(result_string, "%ld / sec (%ld mb/s)\n", num, (num * w * h) / (1024UL * 1024));
    return 0;
}


/*------------------------------------------------------------------------------*/
/* main menu                                                                    */
/*------------------------------------------------------------------------------*/

typedef struct { const char* name; uint32_t(*func)(int16_t); int16_t arg; } option_t;

static option_t options[] = {
    { "rect - full",        test_rectfill, 0 },
    { "rect - 8band",       test_rectfill, 8 },
    { "rect - 1band",       test_rectfill, 1 },
    { "tri  - 8band clean", test_trapez,   9 },
    { "tri  - 8band",       test_trapez,   8 },
    { "tri  - 1band",       test_trapez,   1 },
    { "tri  - clean",       test_trapez_clean, 8 },
    { 0, 0 }
};

int16_t getkey(void) {
    int16_t key;
    while(Cconis() != -1);
    key = (int16_t)(Cconin() & 0xff);
    while(Cconis() == -1) { Cconin(); }
    printf("\n");
    return key;
}

void restore_card(void) {
    rect_t r;
    r.min.x = 8;
    r.min.y = 8;
    r.max.x = 16;
    r.max.y = 16;
    card->blit(BL_FILL|BL_ROP_1|BL_FGCOL(1), &r, &r.min);
    r.min.x = 0;
    r.min.y = 0;
    r.max.x = nova->max_x;
    r.max.y = nova->max_y;
    card->blit(BL_FILL|BL_ROP_S|BL_FGCOL(0), &r, &r.min);
}

void test_raster(void) {
    int16_t num_options;
    int16_t selected = -1;
    result_string[0] = 0;

    mmio_addr = card->isa_mem + 0xb8000UL;  /* cpu address */

    restore_card();
    while(1) {
        Cconws("\n\033E\n");
        for(num_options=0; (num_options<9) && (options[num_options].func != 0); num_options++) {
            printf(" %d. %s\n", num_options + 1, options[num_options].name);
        }
        printf(" x. exit\n");
        if (*result_string) {
            printf("\nresult (%d) : %s\n", selected, result_string);
            result_string[0] = 0;
        }

        selected = getkey() - '0';
        if ((selected > 0) && (selected <= num_options)) {
            options[selected-1].func(options[selected-1].arg);
            if (result_string) {
            }
            restore_card();

        } else {
            break;
        }
    }
    restore_card();
}
