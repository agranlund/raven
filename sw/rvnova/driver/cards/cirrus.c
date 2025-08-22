/*-------------------------------------------------------------------------------
 * NOVA generic vga driver
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
#include "emulator.h"
#include "raven.h"
#include "vga.h"
#include "x86.h"

#if DRV_INCLUDE_CIRRUS

typedef enum {
    UNKNOWN = 0, GD5401, GD5402, GD5420, GD5422, GD5424, GD5426, GD5428, GD5429, GD5430, GD5432, GD5434,
} chipset_e;

static const char* chipset_strings[] = {
    "unknown", "GD5401", "GD5402", "GD5420", "GD5422", "GD5424", "GD5426", "GD5428", "GD5429", "GD5430", "GD5432", "GD5434"
};

#define cl_support_banks()      (chipset >= GD5402)
#define cl_support_linear()     (chipset >= GD5422)
#define cl_support_16kbanks()   (chipset >= GD5426)
#define cl_support_blitter()    (chipset >= GD5426)
#define cl_support_mmio()       (chipset >= GD5429)

static uint16_t vram = 0;
static chipset_e chipset = 0;

/*-------------------------------------------------------------------------------
 * cirrus vgabios extensions
 *-----------------------------------------------------------------------------*/
static uint16_t vgabios_InquireVgaType(void) {
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0x80;
    return (uint16_t) int86(0x10, &r, &r);
}

static uint16_t vgabios_InquireBiosVersion(void) {
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0x81;
    int86(0x10, &r, &r);
    return (uint16_t)r.h.al;
}

static uint16_t vgabios_InquireDesignRevision(void) {
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0x82;
    int86(0x10, &r, &r);
    return (uint16_t) r.h.al;
}

static uint32_t vgabios_InquireInstalledMemory(void) {
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0x85;
    int86(0x10, &r, &r);
    return (uint32_t) (65536UL * r.h.al);
}

static void vgabios_Force8bit(bool on) {
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0x93; r.h.al = on ? 0x00 : 0x01;
    int86(0x10, &r, &r);
}

static uint32_t vgabios_InquireUserOptions(void) {
    uint32_t v;
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0x9a;
    int86(0x10, &r, &r);
    v = r.x.cx; v <<= 16; v |= r.x.ax;
    return v;
}

static bool vgabios_InquireVideoMode(uint16_t mode) {
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0xa0;
    if (mode < 0x80) {
        r.h.al = (uint8_t)(mode & 0x7f);
        int86(0x10, &r, &r);
    }
    return (r.h.ah & 1) ? true : false;
}

static uint16_t vgabios_InquireMonitorType(void) {
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0xa1;
    int86(0x10, &r, &r);
    return r.x.bx;
}

static void vgabios_SetHighRefreshRate(bool on) {
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0xa3; r.h.al = on ? 0x01 : 0x00;
    int86(0x10, &r, &r);
}

static void vgabios_SetMonitorTypeH(uint8_t type) {
    /*
     * 0 = 31.5      : vga
     * 1 = 31.5/35.5 : 8514
     * 2 = 31.5-35.1 : svga
     * 3 = 31.5-35.5 : extended svga
     * 4 = 31.5-37.8 : multifreq
     * 5 = 31.5-48.0 : extended multifreq
     * 6 = 31.5-56.0 : super multifreq
     * 7 = 31.5-64.0 : extender super multifreq
     */
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0xa2; r.h.al = type;
    int86(0x10, &r, &r);
}

static void vgabios_SetMonitorTypeV(uint8_t max_vres, uint8_t freq640, uint8_t freq800, uint8_t freq1024, uint8_t freq1280) {
    /* 
     * max_vres: 0 = 480, 1 = 600, 2 = 768, 3 = 1024
     * freq640:  0 = 60,  1 = 72
     * freq800:  0 = 56,  1 = 60, 2 = 72
     * freq1024: 0 = 87i, 1 = 60, 2 = 70, 3 = 72, 4 = 76
     * freq1280: 0 = 87i, 1 = 60, 2 = 70
     */
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0xa4;
    r.h.al = (max_vres & 0xf) | ((freq640 & 0xf) << 4);
    r.h.bh = (freq800 & 0xf) | ((freq1024 & 0xf) << 4);
    r.h.ch = ((freq1280 & 0xf) << 4);
    r.h.cl = 0;
    int86(0x10, &r, &r);
}

/*-------------------------------------------------------------------------------
 * driver
 *-----------------------------------------------------------------------------*/

static bool identify(void) {
    uint16_t base; uint8_t temp[2];
    chipset = 0; vram = 0;
    temp[0] = vga_ReadReg(0x3c4, 0x06);
    vga_WriteReg(0x3c4, 0x06, 0x00);                /* test lock */
    if (vga_ReadReg(0x3c4, 0x06) == 0x0f) {
        vga_WriteReg(0x3c4, 0x06, 0x12);            /* test unlock */
        if (vga_ReadReg(0x3c4, 0x06) == 0x12) {
            if (vga_TestReg(0x3c4, 0x1e, 0x3f)) {   /* test cirrus register */
                uint16_t type = vgabios_InquireVgaType();
                switch (type) {
                    case 0x10: chipset = GD5401; break;
                    case 0x11: chipset = GD5402; break;
                    case 0x12: chipset = GD5420; break;
                    case 0x13: chipset = GD5422; break;
                    case 0x14: chipset = GD5424; break;
                    case 0x15: chipset = GD5426; break;
                    case 0x16: chipset = GD5420; break; /* rev.1 */
                    case 0x17: chipset = GD5402; break; /* rev.1 */
                    case 0x18: chipset = GD5428; break;
                    case 0x19: chipset = GD5429; break;
                    case 0x30: chipset = GD5432; break;
                    case 0x31: chipset = GD5434; break;
                    case 0x32: chipset = GD5430; break;
                    case 0x33: chipset = GD5434; break; /* rev.1 */
                    default: chipset = 0;
                }
                if (chipset) {
                    vram = (uint16_t) (vgabios_InquireInstalledMemory() / 1024);
                }
            }
        }
    }

    /* leave card unlocked if supported */
    if (chipset && vram) {
        return true;
    }

    /* restore what we modified if this wasn't a supported card */
    vga_WriteReg(0x3c4, 0x06, temp[0]);
    return false;
}

static void configure_framebuffer(void) {
    /* clear offset registers */
    vga_WriteReg(0x3ce, 0x09, 0x00);
    vga_WriteReg(0x3ce, 0x0a, 0x00);
    if (cl_support_linear()) {
        /* enable linear @ isa:2MB */
        vga_ModifyReg(0x3c4, 0x07, 0xf0, 0x20);
    } else if (cl_support_banks()) {
        /* enable banks  */
        vga_ModifyReg(0x3c4, 0x07, 0xf0, 0x00);
        if (cl_support_16kbanks()) {
            /* 16k granularity for 2MB support */
            vga_ModifyReg(0x3ce, 0x0b, 0x21, 0x20);
        } else {
            /* 4k granularity for 1MB support */
            vga_ModifyReg(0x3ce, 0x0b, 0x21, 0x00);
        }
    }
}

static uint32_t colorexp_addr;
static uint16_t colorexp_set04;
static uint16_t colorexp_set05;
static uint16_t colorexp_restore00;
static uint16_t colorexp_restore01;
static uint16_t colorexp_restore04;
static uint16_t colorexp_restore05;
static uint16_t colorexp_restore10;
static uint16_t colorexp_restore11;

void cl_upload_colorexp_patterns(void) {
    int x, y, w;
    for (w = 0; w < 8; w++) {           /* patterns 0-7 */
        uint32_t* vram = (uint32_t*)(PADDR_MEM + colorexp_addr + ((w & 7) << 8));
        uint32_t pat = ~nv_fillpatterns[w];
        for (y = 0; y < 4; y++) {       /* y offset 0-3 */
            for (x = 0; x < 8; x++) {   /* x offset 0-7 */
                uint32_t p0, p1;
                *vram++ = pat; *vram++ = pat;
                p0 = (pat << 1) & 0xfefefefeUL;
                p1 = (pat >> 7) & 0x01010101UL;
                pat = p0 | p1;
            }
            pat = (pat << 8) | (pat >> 24);
        }
    }
}

static uint32_t cl_get_colorexp_pattern(uint16_t idx, uint16_t xoffs, uint16_t yoffs) {
    return (colorexp_addr + ((idx & 7) << 8) + ((yoffs & 3) << 6) + ((xoffs & 7) << 3));
}

static uint32_t cl_getpitch(uint32_t width, uint8_t bpp) {
    return (bpp < 8) ? (width >> 3) : ((width * ((bpp + 7) & ~7)) >> 3);
}

static void configure_blitter(mode_t* mode) {
    uint32_t pitch = cl_getpitch(mode->width, mode->bpp);
    vga_WriteReg(0x3ce, 0x26, (uint8_t)((pitch >> 0) & 0xff)); /* src pitch lo */
    vga_WriteReg(0x3ce, 0x24, (uint8_t)((pitch >> 0) & 0xff)); /* dst pitch lo */
    vga_WriteReg(0x3ce, 0x27, (uint8_t)((pitch >> 8) & 0xff)); /* src pitch hi */
    vga_WriteReg(0x3ce, 0x25, (uint8_t)((pitch >> 8) & 0xff)); /* dst pitch hi */
    vga_WriteReg(0x3ce, 0x30, 0);       /* direction forward */
    vga_WriteReg(0x3ce, 0x32, 0x0D);    /* rop = SRCCOPY */

    /* transparency color and mask */
    if ((chipset >= GD5426) && (chipset <= GD5428)) {
        vga_WriteReg(0x3ce, 0x34, 0x00);
        vga_WriteReg(0x3ce, 0x35, 0x00);
        vga_WriteReg(0x3ce, 0x38, 0x00);
        vga_WriteReg(0x3ce, 0x39, 0x00);
    }

    /* color expansion settings */
    colorexp_restore00 = 0x0000 | vga_ReadReg(0x3ce, 0x00);
    colorexp_restore01 = 0x0100 | vga_ReadReg(0x3ce, 0x01);
    colorexp_restore04 = 0x0400 | vga_ReadReg(0x3ce, 0x04);
    colorexp_restore05 = 0x0500 | vga_ReadReg(0x3ce, 0x05);
    colorexp_restore10 = 0x1000 | vga_ReadReg(0x3ce, 0x10);
    colorexp_restore11 = 0x1100 | vga_ReadReg(0x3ce, 0x11);
    colorexp_set04 = (1<<2) | colorexp_restore04;
    colorexp_set05 = (5<<0) | (colorexp_restore05 & ~7);
    colorexp_addr = card->bank_addr + card->bank_size - 2048;
    cl_upload_colorexp_patterns();

    /* color expansion default background color */
    vga_WritePortWLE(0x3ce, colorexp_set04);
    vga_WritePortWLE(0x3ce, colorexp_set05);
    vga_WritePortWLE(0x3ce, 0x0100);
    vga_WritePortWLE(0x3ce, 0x1100);
    vga_WritePortWLE(0x3ce, colorexp_restore05);
    vga_WritePortWLE(0x3ce, colorexp_restore04);
    vga_WritePortWLE(0x3ce, colorexp_restore01);
    vga_WritePortWLE(0x3ce, colorexp_restore10);
}

static bool blit(rect_t* src, vec_t* dst) {
    uint32_t srcaddr, dstaddr;
    uint32_t width_minus_one;
    uint32_t height_minus_one;
    uint16_t bytes_per_pixel;
    uint8_t dir;

    /* we cannot blit planar modes */
    bytes_per_pixel = (((nova.planes + 7) & ~7) >> 3);
    if (bytes_per_pixel == 0) {
        return false;
    }

    /* source and destination address */
    height_minus_one = (uint32_t)(src->max.y - src->min.y);
    width_minus_one = (uint32_t)(src->max.x - src->min.x);
    width_minus_one = (uint32_t)((width_minus_one * bytes_per_pixel) + (bytes_per_pixel - 1));
    srcaddr = ((uint32_t)src->min.y * nova.pitch) + ((uint32_t)src->min.x * bytes_per_pixel);
    dstaddr = ((uint32_t)dst->y * nova.pitch) + ((uint32_t)dst->x * bytes_per_pixel);

    /* blit direction */
    dir = 0;
    if ((src->min.y < dst->y) || ((src->min.y == dst->y) && (src->min.x < dst->x))) {
        if (src->max.y >= dst->y) {
            uint32_t offs = (height_minus_one * nova.pitch) + width_minus_one;
            srcaddr += offs;
            dstaddr += offs;
            dir = 1;
        }
    }

    /* prepare blitter regs */
    vga_WritePortWLE(0x3ce, 0x3000 | dir);
    vga_WritePortWLE(0x3ce, 0x2c00 | (uint8_t)(srcaddr & 0xff)); srcaddr >>= 8;
    vga_WritePortWLE(0x3ce, 0x2d00 | (uint8_t)(srcaddr & 0xff)); srcaddr >>= 8;
    vga_WritePortWLE(0x3ce, 0x2e00 | (uint8_t)(srcaddr & 0xff));
    vga_WritePortWLE(0x3ce, 0x2800 | (uint8_t)(dstaddr & 0xff)); dstaddr >>= 8;
    vga_WritePortWLE(0x3ce, 0x2900 | (uint8_t)(dstaddr & 0xff)); dstaddr >>= 8;
    vga_WritePortWLE(0x3ce, 0x2a00 | (uint8_t)(dstaddr & 0xff));
    vga_WritePortWLE(0x3ce, 0x2000 | (uint8_t)(width_minus_one & 0xff)); width_minus_one >>= 8;
    vga_WritePortWLE(0x3ce, 0x2100 | (uint8_t)(width_minus_one & 0xff));
    vga_WritePortWLE(0x3ce, 0x2200 | (uint8_t)(height_minus_one & 0xff)); height_minus_one >>= 8;
    vga_WritePortWLE(0x3ce, 0x2300 | (uint8_t)(height_minus_one & 0xff));

    /* start blitter and wait until finished  */
    vga_WritePort(0x3ce, 0x31);
    vga_WritePort(0x3cf, (vga_ReadPort(0x3cf) | 0x02));
    while (vga_ReadPort(0x3cf) & 1) {
        cpu_nop();
    }
    return true;
}

static bool fill(uint16_t col, uint16_t pat, rect_t* dst) {
    uint32_t srcaddr, dstaddr;
    uint32_t width_minus_one;
    uint32_t height_minus_one;
    uint16_t bytes_per_pixel;
    uint16_t blitmode;

    /* we cannot blit planar modes */
    /* temp: disable >8bit fills also, until our vdi hook is capable of passing such color values */
    bytes_per_pixel = (((nova.planes + 7) & ~7) >> 3);
    if (bytes_per_pixel != 1) {
        return false;
    }

    /* destination address */
    height_minus_one = (uint32_t)(dst->max.y - dst->min.y);
    width_minus_one = (uint32_t)(dst->max.x - dst->min.x);
    width_minus_one = (uint32_t)((width_minus_one * bytes_per_pixel) + (bytes_per_pixel - 1));
    dstaddr = ((uint32_t)dst->min.y * nova.pitch) + ((uint32_t)dst->min.x * bytes_per_pixel);

    /* enable color expansion and set foreground color */
    vga_WritePortWLE(0x3ce, colorexp_set04);
    vga_WritePortWLE(0x3ce, colorexp_set05);
    vga_WritePortWLE(0x3ce, 0x0000 | ((col >> 0) & 0xff));
    vga_WritePortWLE(0x3ce, 0x1000 | ((col >> 8) & 0xff));

    /* forward blit, 8x8 pattern with color expansion enabled */
    vga_WritePortWLE(0x3ce, 0x3000 | 0x0080 | 0x0040 | ((bytes_per_pixel < 2) ? 0x0000 : 0x0010));

    /* update common blitter regs */
    srcaddr = cl_get_colorexp_pattern(pat, dst->min.x, dst->min.y);
    vga_WritePortWLE(0x3ce, 0x2c00 | (uint8_t)(srcaddr & 0xff)); srcaddr >>= 8;
    vga_WritePortWLE(0x3ce, 0x2d00 | (uint8_t)(srcaddr & 0xff)); srcaddr >>= 8;
    vga_WritePortWLE(0x3ce, 0x2e00 | (uint8_t)(srcaddr & 0xff));
    vga_WritePortWLE(0x3ce, 0x2800 | (uint8_t)(dstaddr & 0xff)); dstaddr >>= 8;
    vga_WritePortWLE(0x3ce, 0x2900 | (uint8_t)(dstaddr & 0xff)); dstaddr >>= 8;
    vga_WritePortWLE(0x3ce, 0x2a00 | (uint8_t)(dstaddr & 0xff));
    vga_WritePortWLE(0x3ce, 0x2000 | (uint8_t)(width_minus_one & 0xff)); width_minus_one >>= 8;
    vga_WritePortWLE(0x3ce, 0x2100 | (uint8_t)(width_minus_one & 0xff));
    vga_WritePortWLE(0x3ce, 0x2200 | (uint8_t)(height_minus_one & 0xff)); height_minus_one >>= 8;
    vga_WritePortWLE(0x3ce, 0x2300 | (uint8_t)(height_minus_one & 0xff));

    /* start blitter and wait until finished */
    vga_WritePort(0x3ce, 0x31);
    vga_WritePort(0x3cf, (vga_ReadPort(0x3cf) | 0x02));
    while (vga_ReadPort(0x3cf) & 1) {
        cpu_nop();
    }

    /* disable color expansion mode */
    vga_WritePortWLE(0x3ce, colorexp_restore05);
    vga_WritePortWLE(0x3ce, colorexp_restore04);
    vga_WritePortWLE(0x3ce, colorexp_restore00);
    vga_WritePortWLE(0x3ce, colorexp_restore10);
    return true;
}

static void setbank(uint16_t num) {
    vga_WritePortWLE(0x3ce, 0x0900 | num);
}

static bool setmode(mode_t* mode) {
    if (vga_setmode(mode->code)) {
        configure_framebuffer();
        if (cl_support_blitter()) {
            configure_blitter(mode);
        }

#if DEBUG
        {
            uint8_t dram = vga_ReadReg(0x3c4, 0x0f);    /* dram control register */
            uint8_t tune = vga_ReadReg(0x3c4, 0x16);    /* performance tuning register */
            uint8_t mclk = vga_ReadReg(0x3c4, 0x1f);    /* mclk register */
            int16_t freq = (int16_t)(mclk & 0x3f) * 179;
            dprintf(" cfg:  %02x %02x %02x\n", dram, tune, mclk);
            dprintf(" dram: ");
            switch (dram & 3) {
                case 0: dprintf("50 mhz, "); break;
                case 1: dprintf("44 mhz, "); break;
                case 2: dprintf("41 mhz, "); break;
                case 3: dprintf("37 mhz, "); break;
            }
            switch ((dram >> 3) & 3) {
                case 0: dprintf(" 8bit, "); break;
                case 1: dprintf("16bit, "); break;
                case 2: dprintf("32bit, "); break;
                case 3: dprintf("32bit, "); break;
            }
            switch ((dram >> 2) & 1) {
                case 0: dprintf("extended RAS\n"); break;
                case 1: dprintf("standard RAS\n"); break;
            }
            dprintf(" fifo: %s, threshold: %d\n", dram & (1 << 5) ? "16/20" : "8", tune & 7);
            dprintf(" fpm:  %s\n", dram & (1 << 6) ? "off" : "on");
            dprintf(" mclk: %02x : ", mclk & 0x3f);
            dprintf("%d.%d mhz\n", freq / 100, ((freq - ((freq / 100) * 100)) + 4) / 10);
        }
#endif
        return true;
    }
    return false;
}

static bool addmode_validated(addmode_f addmode, uint16_t w, uint16_t h, uint8_t b, uint8_t f, uint16_t c) {
    if (vgabios_InquireVideoMode(c)) {
        addmode(w, h, b, f, c);
        return true;
    }
    return false;
}

static bool init(card_t* card, addmode_f addmode) {

    /* identify cirrus card */
    if (!identify()) {
        return false;
    }

    /* provide card info and callbacks */
    card->name = chipset_strings[chipset];
    card->vram_size = 1024UL * vram;
    card->setmode = setmode;
    if (cl_support_blitter()) {
        card->blit = blit;
        card->fill = fill;
    }
    if (cl_support_linear()) {
        card->bank_addr = 0x200000UL;
        card->bank_size = card->vram_size;
    } else {
        card->bank_addr = 0xA0000UL;
        card->bank_size = 1024UL * 64;
        if (cl_support_banks()) {
            card->setbank = setbank;
            card->bank_count = (card->vram_size / card->bank_size);
            if (cl_support_16kbanks()) {
                card->bank_step = 1024UL * 16;
            } else {
                card->bank_step = 1024UL * 4;
            }
        }
    }

    /* tell bios about monitor limits, assume a modern lcd */
    vgabios_SetMonitorTypeH(
        7                       /* 31.5khz - 64.0 khz*/
    );
    vgabios_SetMonitorTypeV(
        3,                      /* max 1024 vertical resolution */
        1,                      /* max 72hz @  640 x  480 */
        2,                      /* max 72hz @  800 x  600 */
        3,                      /* max 70hz @ 1024 x  768 */
        2                       /* max 70hz @ 1280 x 1024 */
    );

    /* let it choose high refresh rates */
    vgabios_SetHighRefreshRate(true);

    /* validate and add videmodes */
    addmode_validated(addmode,  800, 600,  4, 0, 0x58);    /* 4bpp */
    addmode_validated(addmode, 1024, 768,  4, 0, 0x5d);
    addmode_validated(addmode, 1024, 768,  4, 0, 0x5d);
    addmode_validated(addmode, 1280,1024,  4, 0, 0x6c);
    addmode_validated(addmode,  640, 400,  8, 0, 0x5e);    /* 8bpp*/
    addmode_validated(addmode,  640, 480,  8, 0, 0x5f);
    addmode_validated(addmode,  800, 600,  8, 0, 0x5c);
    addmode_validated(addmode, 1024, 768,  8, 0, 0x60);
    addmode_validated(addmode, 1280,1024,  8, 0, 0x6d);
    if (chipset >= GD5430) {
    addmode_validated(addmode, 1600,1200,  8, 0, 0x78);
    }
    addmode_validated(addmode,  320, 200, 16, 0, 0x6f);    /* 16bpp */
    addmode_validated(addmode,  640, 480, 16, 0, 0x64);
    addmode_validated(addmode,  800, 600, 16, 0, 0x65);
    addmode_validated(addmode, 1024, 768, 16, 0, 0x74);
    addmode_validated(addmode, 1280,1024, 16, 0, 0x75);
    addmode_validated(addmode,  320, 200, 24, 0, 0x70);    /* 24bpp */
    addmode_validated(addmode,  640, 480, 24, 0, 0x71);
    if (chipset >= GD5430) {
        addmode_validated(addmode,  800, 600, 32, 0, 0x72);
        addmode_validated(addmode, 1024, 768, 32, 0, 0x73);
        addmode_validated(addmode, 1152, 870, 32, 0, 0x79);
        addmode_validated(addmode, 1280,1024, 24, 0, 0x77);
    }

    /* configure linear or banked operation */
    configure_framebuffer();
    return true;
}

driver_t drv_cirrus = { "cirrus", init };

#endif /* DRV_INCLUDE_CIRRUS */
