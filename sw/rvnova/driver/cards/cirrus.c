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
static uint8_t mclk_override = 0;

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

static void vgabios_SetMonitorTypeGD543x(uint8_t max_vres, uint8_t freq640, uint8_t freq800, uint8_t freq1024, uint8_t freq1280) {
    /* 
    * max_vres: 0 = 480, 1 = 600, 2 = 768, 3 = 1024
    * freq640:  0 = 60,  1 = 72, 2 = 75
    * freq800:  0 = 56,  1 = 60, 2 = 72, 3 = 75
    * freq1024: 0 = 43i, 1 = 60, 2 = 70, 3 = 72, 4 = 75
    * freq1280: 0 = 43i, 1 = 60, 2 = 72, 3 = 75
    */
    x86_regs_t r; r.h.ah = 0x12; r.h.bl = 0xa4;
    r.h.al = (max_vres & 0xf) | ((freq640 > 0) ? (1 << 4) : 0);
    r.h.bh = (freq800 & 0xf) | ((freq1024 & 0xf) << 4);
    r.h.ch = ((freq1280 & 0xf) << 4);
    int86(0x10, &r, &r);

    if (freq640 > 0) {
        r.h.ah = 0x12;
        r.h.bl = 0xaf;
        r.h.al = (freq640 >> 1) & 1;
        int86(0x10, &r, &r);
    }
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

        /* protected overscan color */
        if ((chipset >= GD5426) && (chipset <= GD5429)) {
            uint8_t overscan_rgb[3] = {0, 0, 0 };
            vga_ModifyReg(0x3c4, 0x12, 0x02, 0x02);
            vga_setcolors(0x02, 1, overscan_rgb);
            vga_ModifyReg(0x3c4, 0x12, 0x82, 0x80);
        }

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
        /* use 2MB linear aperture */
        vga_ModifyReg(0x0ce, 0x0b, 0x21, 0x20);
        /* enable and map to isa:2MB */
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

/* todo: GD5429+ doesn't need pre-rotated patterns for Y offsets */
static void cl_upload_colorexp_patterns(void) {
    int x, y, w;
    for (w = 0; w < 8; w++) {           /* patterns 0-7 */
        uint32_t* vram = (uint32_t*)(card->isa_mem + colorexp_addr + ((w & 7) << 8));
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

static uint32_t cl_get_colorexp_pattern(uint8_t idx, uint16_t xoffs, uint16_t yoffs) {
    return (colorexp_addr + ((idx & 7) << 8) + ((yoffs & 3) << 6) + ((xoffs & 7) << 3));
}

static uint8_t cl_rops[16] = {
    0x00,  /* BL_ROP_0     */
    0x05,  /* BL_ROP_DSa   */
    0x09,  /* BL_ROP_SDna  */
    0x0D,  /* BL_ROP_S     */
    0x50,  /* BL_ROP_DSna  */
    0x06,  /* BL_ROP_D     */
    0x59,  /* BL_ROP_DSx   */
    0x6D,  /* BL_ROP_DSo   */
    0x90,  /* BL_ROP_DSon  */
    0x95,  /* BL_ROP_DSxn  */
    0x0B,  /* BL_ROP_Dn    */
    0xAD,  /* BL_ROP_SDno  */
    0xD0,  /* BL_ROP_Sn    */
    0xD6,  /* BL_ROP_DSno  */
    0xDA,  /* BL_ROP_DSan  */
    0x0E,  /* BL_ROP_1     */
};

static uint8_t cl_get_rop(uint32_t cmd) {
    return cl_rops[((cmd) >> BL_SHIFT_ROP) & BL_MASK_ROP];
}

static uint32_t cl_getpitch(uint32_t width, uint8_t bpp) {
    return (bpp < 8) ? (width >> 3) : ((width * ((bpp + 7) & ~7)) >> 3);
}

static void configure_blitter(mode_t* mode) {
    uint32_t pitch = cl_getpitch(mode->width, mode->bpp);

    /* disable mmio */
    if (chipset >= GD5429) {
        vga_ModifyReg(0x3c4, 0x17, 0x44, 0x00);
    }

    /* default pitch */
    vga_WriteReg(0x3ce, 0x26, (uint8_t)((pitch >> 0) & 0xff)); /* src pitch lo */
    vga_WriteReg(0x3ce, 0x24, (uint8_t)((pitch >> 0) & 0xff)); /* dst pitch lo */
    vga_WriteReg(0x3ce, 0x27, (uint8_t)((pitch >> 8) & 0xff)); /* src pitch hi */
    vga_WriteReg(0x3ce, 0x25, (uint8_t)((pitch >> 8) & 0xff)); /* dst pitch hi */
    vga_WriteReg(0x3ce, 0x30, 0);       /* direction forward */
    vga_WriteReg(0x3ce, 0x32, 0x0D);    /* rop = SRCCOPY */

    /* capabilities */
    card->caps |= NV_CAPS_BLIT | NV_CAPS_FILL | NV_CAPS_DITHER;

    /* only in 8bpp blit for now but not Mode13h */
    if ((mode->bpp != 8) || (mode->width < 640)) {
        card->caps &= ~(NV_CAPS_BLIT | NV_CAPS_FILL);
    }

    /* transparency color and mask */
    if ((chipset >= GD5426) && (chipset <= GD5428)) {
        card->caps |= NV_CAPS_SRCKEY;
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

#if 0    
    /* enable mmio */
    if (cl_support_mmio()) {
        vga_ModifyReg(0x3c4, 0x17, 0x44, 0x04);
        colorexp_restore00 = (((colorexp_restore00 & 0xff) << 8) | (colorexp_restore10 & 0xff));
        colorexp_restore01 = (((colorexp_restore01 & 0xff) << 8) | (colorexp_restore11 & 0xff));
     }
#endif
}

static bool blit(uint32_t cmd, rect_t* src, vec_t* dst) {
    uint32_t srcaddr, dstaddr;
    uint32_t width_minus_one;
    uint32_t height_minus_one;
    uint16_t bytes_per_pixel;
    uint16_t ctrl = 0x3000;

    /* source and destination address */
    bytes_per_pixel = (((nova->planes + 1) & ~7) >> 3);
    height_minus_one = (uint32_t)(src->max.y - src->min.y);
    width_minus_one = (uint32_t)(src->max.x - src->min.x);
    width_minus_one = (uint32_t)((width_minus_one * bytes_per_pixel) + (bytes_per_pixel - 1));
    dstaddr = ((uint32_t)dst->y * nova->pitch) + ((uint32_t)dst->x * bytes_per_pixel);

    if (cmd & BL_FILL) {
        uint8_t pattern = BL_GETPATTERN(cmd);
        srcaddr = cl_get_colorexp_pattern(pattern, dst->x, dst->y);
        /* enable color expansion and set colors */
        vga_WritePortWLE(0x3ce, colorexp_set04);
        vga_WritePortWLE(0x3ce, colorexp_set05);
        vga_WritePortWLE(0x3ce, 0x0000 | BL_GETFGCOL(cmd));
        vga_WritePortWLE(0x3ce, 0x1000 | BL_GETBGCOL(cmd));
        /* forward blit, 8x8 pattern with color expansion enabled */
        ctrl |= (0x0080 | 0x0040 | ((bytes_per_pixel < 2) ? 0x0000 : 0x0010));
    } else {
        srcaddr = ((uint32_t)src->min.y * nova->pitch) + ((uint32_t)src->min.x * bytes_per_pixel);
        if ((srcaddr < dstaddr) && (src->max.y >= dst->y)) {
            uint32_t offs = (height_minus_one * nova->pitch) + width_minus_one;
            srcaddr += offs;
            dstaddr += offs;
            ctrl |= (1 << 0); /* reverse blit */
        }
    }

    if (cmd & BL_TRANSPARENT) {
        ctrl |= (1 << 3);
    }
   
    vga_WritePortWLE(0x3ce, ctrl);
    vga_WritePortWLE(0x3ce, 0x3200 | cl_get_rop(cmd));
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
    vga_WritePortWLE(0x3ce, 0x3100 | 0x02);
    while (vga_ReadPortWLE(0x3ce) & 1);

    /* disable color expansion mode */
    if (cmd & BL_FILL) {
        vga_WritePortWLE(0x3ce, colorexp_restore05);
        vga_WritePortWLE(0x3ce, colorexp_restore04);
        vga_WritePortWLE(0x3ce, colorexp_restore00);
        vga_WritePortWLE(0x3ce, colorexp_restore10);
    }

    return true;
}

#if 0
static bool hlines(uint16_t flg, uint16_t col, int16_t num, int16_t ypos, int16_t* pts) {
    volatile uint16_t* regw = (volatile uint16_t*)(PADDR_IO + 0x3ce);
    uint32_t yaddr = ((uint32_t)ypos) * nova->pitch;
    uint16_t bytes_per_pixel = (((nova->planes + 1) & ~7) >> 3);
    if (bytes_per_pixel != 1) {
        return false;
    }

    /* constant settings */
    *regw = (0x3000 | 0x0080 | 0x0040 | ((bytes_per_pixel < 2) ? 0x0000 : 0x0010));
    *regw = colorexp_set04;
    *regw = colorexp_set05;
    *regw = 0x0000 | (col & 0xff);
    *regw = 0x1000 | (col >> 8);
    if ((flg & 7) == 0) {
        *regw = 0x2c00 | (uint8_t)(colorexp_addr & 0xff);
        *regw = 0x2d00 | (uint8_t)((colorexp_addr >> 8) & 0xff);
        *regw = 0x2e00 | (uint8_t)((colorexp_addr >> 16) & 0xff);
    }

    /* select status register and draw lines */
    *(volatile uint8_t*)regw = 0x31;
    while (num >= 0) {
        int16_t x = (uint16_t) (*pts++);
        uint32_t addr = yaddr + x;
        int16_t width = (*pts++) - x;
        if (flg & 7) {
            uint32_t paddr = cl_get_colorexp_pattern(flg, x, ypos); ypos++;
            while (*regw & 1);
            *regw = 0x2c00 | (uint8_t)(paddr & 0xff);
            *regw = 0x2d00 | (uint8_t)((paddr >> 8) & 0xff);
            *regw = 0x2e00 | (uint8_t)((paddr >> 16) & 0xff);
        } else  {
            while (*regw & 1);
        }
        *regw = 0x2800 | (uint8_t)(addr & 0xff);
        *regw = 0x2900 | (uint8_t)((addr >> 8) & 0xff);
        *regw = 0x2a00 | (uint8_t)((addr >> 16) & 0xff);
        *regw = 0x2000 | (uint8_t)(width & 0xff);
        *regw = 0x2100 | (uint8_t)(width >> 8);
        *regw = 0x2200 | 0;    /* dest height */
        *regw = 0x2300 | 0;
        *regw = 0x3100 | 0x02; /* start blitter */
        yaddr += nova->pitch;
        num--;
    }
    /* wait for blitter */
    while (*regw & 1);

    /* disable color expansion mode */
    *regw = colorexp_restore05;
    *regw = colorexp_restore04;
    *regw = colorexp_restore00;
    *regw = colorexp_restore10;
    return true;
}
#endif

static void setbank(uint16_t num) {
    vga_WritePortWLE(0x3ce, 0x0900 | num);
}

/* todo:
    currently, this function assumes a 5426+ card
    GD5426/28/29 supports CR1B:bits 0,2,3 (address bits 16, 17, 18)
    GD542x       supports CR1B:bits 0,2   (address bits 16, 17    )
*/
static void setaddr(uint32_t addr) {
    uint16_t crtc = vga_GetBaseReg(4);
    uint32_t hiaddr = addr >> 18;
    hiaddr += (hiaddr & 6);
    vga_ModifyReg(crtc, 0x1B, 0x0D, hiaddr);
    vga_WritePortWLE(crtc, 0x0D00 | ((addr >>  2) & 0xff));
    vga_WritePortWLE(crtc, 0x0C00 | ((addr >> 10) & 0xff));
}


static uint16_t find_vclk(uint32_t target) {
    uint16_t p, d, n;
    uint16_t best_val = 0;
    uint32_t best_err = 0xffffffffUL;
    uint32_t ref = 14318; /* standard 14.31818 clock */
    for (p = 0; p <= 1; p++) {
        for (d = 0; d <= 63; d++) {
            for (n = 0; n <= 127; n++) {
                uint32_t cur = (ref * n) / (d * (p + 1));
                uint32_t err = (cur >= target) ? (cur - target) : (target - cur);
                if (err < best_err) {
                    best_err = err;
                    best_val = (d << 9) | (p << 8) | n;
                }
            }
        }
    }
    return best_val;
}

static void setcustom(modeline_t* ml) {
    /* find best vclk3 values from frequency */
    uint16_t vclk = find_vclk(ml->pclk);

    dprintf(("vclk %ld, N:%02x, D:%02x\n", ml->pclk, (uint8_t)(vclk&0xff), (uint8_t)(vclk>>8)));

    /* unlock extended sequencer registers */
    vga_WriteReg(0x3c4, 0x06, 0x12);

    /* program vclk3 */
    vga_ModifyReg(0x3c4, 0x0e, 0x7f, vclk & 0xff);  /* N   */
    vga_ModifyReg(0x3c4, 0x1e, 0x3f, vclk >> 8);    /* D,P */

    /* select vclk3 */
    vga_WritePort(0x3c2,  vga_ReadPort(0x3cc) | 0x0c);

    /* apply modeline to standard vga registers */
    vga_modeline(ml);
}

static bool setmode(mode_t* mode) {
    if (vga_setmode(mode->code)) {

        /* custom 1280x720 mode */
        if ((mode->width == 1280) && (mode->height == 720)) {
            setcustom(&modeline_720p_cvtrb);
        }
        configure_framebuffer();
        if (cl_support_blitter()) {
            configure_blitter(mode);
        }


        if ((chipset >= GD5424) && (mclk_override > 0)) {
            uint8_t mclk = vga_ReadReg(0x3c4, 0x1f) & 0x3f;
            if (mclk != mclk_override) {
                vga_ModifyReg(0x3c4, 0x1f, 0x3f, mclk_override);
            }
        }

#if DEBUG
        {
            uint8_t dram = vga_ReadReg(0x3c4, 0x0f);    /* dram control register */
            uint8_t tune = vga_ReadReg(0x3c4, 0x16);    /* performance tuning register */
            uint8_t mclk = vga_ReadReg(0x3c4, 0x1f);    /* mclk register */
            int16_t freq = (int16_t)(mclk & 0x3f) * 179;
            dprintf((" cfg:  %02x %02x %02x\n", dram, tune, mclk));
            dprintf((" dram: "));
            switch (dram & 3) {
                case 0: dprintf(("50 mhz, ")); break;
                case 1: dprintf(("44 mhz, ")); break;
                case 2: dprintf(("41 mhz, ")); break;
                case 3: dprintf(("37 mhz, ")); break;
            }
            switch ((dram >> 3) & 3) {
                case 0: dprintf((" 8bit, ")); break;
                case 1: dprintf(("16bit, ")); break;
                case 2: dprintf(("32bit, ")); break;
                case 3: dprintf(("32bit, ")); break;
            }
            switch ((dram >> 2) & 1) {
                case 0: dprintf(("extended RAS\n")); break;
                case 1: dprintf(("standard RAS\n")); break;
            }
            dprintf((" fifo: %s, threshold: %d\n", dram & (1 << 5) ? "16/20" : "8", tune & 7));
            dprintf((" fpm:  %s\n", dram & (1 << 6) ? "off" : "on"));
            dprintf((" mclk: %02x : ", mclk & 0x3f));
            dprintf(("%d.%d mhz\n", freq / 100, ((freq - ((freq / 100) * 100)) + 4) / 10));
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
        card->setaddr = setaddr;    /* todo: should be for all cards, but see note in setaddr() */
        /*card->hlines = hlines;*/
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


    /* mclk override */
    /* todo when we have user settings for it */
    mclk_override = 0;
#if 0    
    if ((chipset >= GD5424) && (chipset <= GD5434)) {
        /* xfree86 mclk overrides */
        /* 0x1c : 50mhz : slow_dram, usually bios default */
        /* 0x1f : 55mhz :  med_dram */
        /* 0x22 : 60mhz : fast_dram, xfree86 reprograms this by default for GD5434-RevE */
        uint8_t mclk = vga_ReadReg(0x3c4, 0x1f) & 0x3f;
        if (chipset >= GD5434) {
            if (mclk < 0x22) { mclk_override = 0x22; }
        } else if (chipset >= GD5426) {
            if (mclk < 0x1f) { mclk_override = 0x1f; }
        }
    }
#endif

    if (chipset >= GD5430) {
        vgabios_SetMonitorTypeGD543x(
            3,      /* max 1024 vertical resolution */
            1,      /* 72hz @  640 x  480 */
            2,      /* 72hz @  800 x  600 */
            3,      /* 70hz @ 1024 x  768 */
            1       /* 60hz @ 1280 x 1024 */
        );

        /* todo: temporary workaround for bugged speedstar64 bios */
        /*vga_enable_fastclear(false);*/

        /* speedstar64 only accept the vesa variants */
        addmode( 800, 600,  4, 0, 0x102);   /* 4 bpp */
        addmode(1024, 768,  4, 0, 0x104);
        addmode(1280,1024,  4, 0, 0x106);
        addmode( 640, 480,  8, 0, 0x101);   /*  8bpp */
        addmode( 800, 600,  8, 0, 0x103);
        addmode(1024, 768,  8, 0, 0x105);
        addmode(1280, 720,  8, 0, 0x105);   /* (custom) */
        addmode(1280,1024,  8, 0, 0x107);
        addmode( 640, 480, 16, 0, 0x111);   /* 16bpp */
        addmode( 800, 600, 16, 0, 0x114);
        addmode(1024, 768, 16, 0, 0x117);
        addmode(1280,1024, 16, 0, 0x11A);
        addmode( 640, 480, 24, 0, 0x112);   /* 24bpp */
        addmode( 800, 600, 24, 0, 0x115);
        addmode(1024, 768, 24, 0, 0x118);

    } else {

        vgabios_SetMonitorTypeH(
            7                       /* 31.5khz - 64.0 khz*/
        );
        vgabios_SetMonitorTypeV(
            3,                      /* max 1024 vertical resolution */
            1,                      /* max 72hz @  640 x  480 */
            2,                      /* max 72hz @  800 x  600 */
            3,                      /* max 70hz @ 1024 x  768 */
            1                       /* max 60hz @ 1280 x 1024 */
        );
        vgabios_SetHighRefreshRate(true);

        /* add bios videmodes*/
        addmode_validated(addmode,  800, 600,  4, 0, 0x58); /* 4bpp */
        addmode_validated(addmode, 1024, 768,  4, 0, 0x5d);
        addmode_validated(addmode, 1024, 768,  4, 0, 0x5d);
        addmode_validated(addmode, 1280,1024,  4, 0, 0x6c);
        addmode_validated(addmode,  640, 400,  8, 0, 0x5e); /* 8bpp*/
        addmode_validated(addmode,  640, 480,  8, 0, 0x5f);
        addmode_validated(addmode,  800, 600,  8, 0, 0x5c);
        addmode_validated(addmode, 1024, 768,  8, 0, 0x60);
        addmode_validated(addmode, 1280, 720,  8, 0, 0x60); /* (custom) */
        addmode_validated(addmode, 1280,1024,  8, 0, 0x6d);
        addmode_validated(addmode,  320, 200, 16, 0, 0x6f); /* 16bpp */
        addmode_validated(addmode,  640, 480, 16, 0, 0x64);
        addmode_validated(addmode,  800, 600, 16, 0, 0x65);
        addmode_validated(addmode, 1024, 768, 16, 0, 0x74);
        addmode_validated(addmode, 1280,1024, 16, 0, 0x75);
        addmode_validated(addmode,  320, 200, 24, 0, 0x70); /* 24bpp */
        addmode_validated(addmode,  640, 480, 24, 0, 0x71);
    }

    /* configure linear or banked operation */
    configure_framebuffer();
    return true;
}

driver_t drv_cirrus = { "cirrus", init };

#endif /* DRV_INCLUDE_CIRRUS */
