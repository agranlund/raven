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


/* todo:
 * I have no idea when certain features came to the lesser cards so most
 * stuff is disabled for anything less than WD90C31 for now.
 */
 
#define wd_support_blitter() (chipset >= WD90C31)
#define wd_support_linear()  (chipset >= WD90C31)
#define wd_support_32bit()   (chipset >= WD90C31)
#define wd_support_banks()   (chipset >= WD90C31)


#if DRV_INCLUDE_WDC

typedef enum {
    UNKNOWN = 0, PVGA1, WD90C00, WD90C10, WD90C11, WD90C30, WD90C31, WD90C33,
} chipset_e;

static const char* chipset_strings[] = {
    "unknown", "PVGA1", "WD90C00", "WDC90C10", "WDC90C11", "WD90C30", "WD90C31", "WD90C33"
};

static chipset_e chipset;
static uint16_t vram;

static bool identify(void) {
    uint16_t rcrtc;
    uint8_t old_3c4_06;
    uint8_t old_3ce_0f;
    uint8_t old_3d4_29;

    /* look for known string in vgabios */
    chipset = 0; vram = 0;
    if (('V' != *((volatile char*)(PADDR_MEM8 + 0xC007dUL + 0x00))) ||
        ('G' != *((volatile char*)(PADDR_MEM8 + 0xC007dUL + 0x01))) ||
        ('A' != *((volatile char*)(PADDR_MEM8 + 0xC007dUL + 0x02))) ||
        ('=' != *((volatile char*)(PADDR_MEM8 + 0xC007dUL + 0x03))))
    {
        return false;
    }

    /* test lock PR0-PR4 */
    old_3ce_0f = vga_ReadReg(0x3ce, 0x0f);
    vga_WriteReg(0x3ce, 0x0f, 0x17 | old_3ce_0f);
    if (vga_TestReg(0x3ce, 0x09, 0x7f)) {
        vga_WriteReg(0x3ce, 0x0f, old_3ce_0f);
        return false;
    }

    /* test unlock PR0-PR4 */
    vga_WriteReg(0x3ce, 0x0f, 0x05);
    if (!vga_TestReg(0x3ce, 0x09, 0x7f)) {
        vga_WriteReg(0x3ce, 0x0f, old_3ce_0f);
        return false;
    }

    /* we have some kind of wdc chip, figure out which one */
    chipset = PVGA1;
    rcrtc = vga_GetBaseReg(4);
    old_3d4_29 = vga_ReadReg(rcrtc, 0x29);
    vga_ModifyReg(rcrtc, 0x29, 0x8f, 0x85);  /* test unlock PR11-PR14 */
    if (vga_TestReg(rcrtc, 0x2b, 0xff)) {
        old_3c4_06 = vga_ReadReg(0x3c4, 0x06);
        vga_WriteReg(0x3c4, 0x06, 0x48);    /* unlock extended sequencer registers */
        if (!vga_TestReg(0x3c4, 0x07, 0xf0)) {
            chipset = WD90C00;
        } else if (!vga_TestReg(0x3c4, 0x10, 0xff)) {
            if (vga_TestReg(rcrtc, 0x31, 0x68)) {
                chipset = 0; /*WD90C22;*/
            } else {
                chipset = 0; /*WD90C20;*/
            }
        } else if (vga_TestReg(0x3c4, 0x14, 0x0f)) {
            uint8_t id36 = vga_ReadReg(rcrtc, 0x36);
            uint8_t id37 = vga_ReadReg(rcrtc, 0x37);
            if ((id36 == 0x32) && (id37 == 0x34)) {
                chipset = 0; /*WD90C24;*/
            } else if ((id36 == 0x32) && (id37 == 0x36)) {
                chipset = 0; /*WD90C26;*/
            } else if ((id36 == 0x33) && (id37 == 0x30)) {
                chipset = WD90C30;
            } else if ((id36 == 0x33) && (id37 == 0x31)) {
                chipset = WD90C31;
            } else if ((id36 == 0x33) && (id37 == 0x33)) {
                chipset = WD90C33;
            } else {
                /*chipset = 0;*/
            }
        } else if (!vga_TestReg(0x3c4, 0x10, 0x04)) {
            chipset = WD90C10;
        } else {
            chipset = WD90C11;
        }
    }

    /* find out how much vram we have */
    vram = 256;
    switch (vga_ReadReg(0x3ce, 0x0b) >> 6) {
        case 2:
            vram = 512;
            break;
        case 3:
            vram = 1024;
            if (chipset == WD90C33) {
                if (vga_ReadReg(rcrtc, 0x3e) & 0x80) {
                    vram = 2048;
                }
            }
            break;
        default:
            break;
    }

    if (chipset && vram) {
        /* unlock crtc registers */
        vga_ModifyReg(rcrtc, 0x11, 0x80, 0x00);
        /* unlock extended sequencer registers */
        if (chipset != WD90C33) {
            vga_WriteReg(0x3c4, 0x06, 0x48 | (old_3c4_06 & 0xef));
        }
        return true;
    }

    /* restore what we modified */
    vga_WriteReg(rcrtc, 0x29, old_3d4_29);
    vga_WriteReg(0x3ce, 0x0f, old_3ce_0f);
    return false;
}


static uint32_t fillpattern_addr;
static void upload_fillpatterns(uint32_t addr) {
    int i, j, k;
    uint8_t* vram = (uint8_t*)(card->isa_mem + addr);
    fillpattern_addr = addr;
    for (i=0; i<8; i++) {
        for (j=0; j<2; j++) {
            uint32_t pat = nv_fillpatterns[i];
            for (k=0; k<32; k++) {
                *vram++ = (pat & 0x80000000UL) ? 0xff : 0x00;
                pat <<= 1;
            }
        }
    }
}

static uint32_t get_fillpattern(uint16_t idx, int16_t xoffs, int16_t yoffs) {
    return (fillpattern_addr + ((idx & 7) << 6) + ((yoffs & 3) << 3) + ((xoffs & 7) << 0));
}

#define wd_rop(cmd) (((uint16_t)cmd) & 0x0F00)

static bool blit31(uint32_t cmd, rect_t* src, vec_t* dst) {
    uint32_t dstaddr, srcaddr;
    uint32_t width_minus_one;
    uint32_t height_minus_one;
    uint16_t bytes_per_pixel;
    uint16_t ctrl0, ctrl1;

    ctrl0 = ((1 << 8) | (1 << 11)); /* packed pixels, start */
    ctrl1 = 0x1000;
    bytes_per_pixel = (((nova->planes + 1) & ~7) >> 3);
    height_minus_one = (uint32_t)(src->max.y - src->min.y);
    width_minus_one = ((uint32_t)(src->max.x - src->min.x)) * bytes_per_pixel;
    dstaddr = (((uint32_t)dst->y) * nova->pitch) + (dst->x * bytes_per_pixel);

    /* select blitter, read index 0 with no increment */
    vga_WritePortW(0x23c0,  (1 << 12) | (0 << 8) | (1 << 0));

    if (cmd & BL_FILL) {
        uint8_t pattern = BL_GETPATTERN(cmd);
        if (pattern) {
            srcaddr = get_fillpattern(pattern, src->min.x, src->min.y);
            vga_WritePortW(0x23c2, 0x2000 | (uint16_t)(srcaddr & 0xfff));
            vga_WritePortW(0x23c2, 0x3000 | (uint16_t)((srcaddr >> 12) & 0x0ff));
            ctrl1 |= (1 << 4);  /* source format is 8x8 pattern */
            ctrl0 |= ((3 << 2) | (1 << 6));  /* source data is mono and linear */
        } else {
            ctrl0 |= (2 << 2);  /* source data is fixed color */
        }
        vga_WritePortW(0x23c2, 0xA000 | BL_GETFGCOL(cmd));
        vga_WritePortW(0x23c2, 0xB000 | BL_GETBGCOL(cmd));
    } else {
        srcaddr = (((uint32_t)src->min.y) * nova->pitch) + (src->min.x * bytes_per_pixel);
        if (srcaddr < dstaddr) {
            if (src->max.y >= dst->y) {
                uint32_t offset = (height_minus_one * nova->pitch) + width_minus_one;
                srcaddr += offset;
                dstaddr += offset;
                ctrl0 |= (1 << 10);  /* reverse blit */
            }
        }
        vga_WritePortW(0x23c2, 0x2000 | (uint16_t)(srcaddr & 0xfff));
        vga_WritePortW(0x23c2, 0x3000 | (uint16_t)((srcaddr >> 12) & 0x0ff));
        if (cmd & BL_MONO) {
            ctrl0 |= (1 << 2); /* color src -> mono mask -> color expand */
            vga_WritePortW(0x23c2, 0xA000 | BL_GETBGCOL(cmd));
            vga_WritePortW(0x23c2, 0xB000 | BL_GETFGCOL(cmd));
        }
    }

    if (cmd & BL_TRANSPARENT) {
        ctrl1 |= ((1 << 2) | (1 << 0));
    }

    vga_WritePortW(0x23c2, 0x9000 | wd_rop(cmd));
    vga_WritePortW(0x23c2, 0x8000 | (uint16_t)(nova->pitch & 0x7ff));
    vga_WritePortW(0x23c2, 0x6000 | (uint16_t)((width_minus_one + 1) & 0x7ff));
    vga_WritePortW(0x23c2, 0x7000 | (uint16_t)((height_minus_one + 1) & 0x7ff));
    vga_WritePortW(0x23c2, 0x4000 | (uint16_t)(dstaddr & 0xfff));
    vga_WritePortW(0x23c2, 0x5000 | (uint16_t)((dstaddr >> 12) & 0x0ff));

    /* start blitter */
    vga_WritePortW(0x23c2, ctrl1);
    vga_WritePortW(0x23c2, ctrl0);

    /* wait until finished */
    while(vga_ReadPortW(0x23c2) & (1 << 11));
    return true;
}

static void wait33(uint16_t size) {
    vga_WritePortW(0x23ce, 0x0020);
    while ((vga_ReadPortW(0x23ce) & 0x000f) > (8 - size)) {
    }
}

static bool blit33(uint32_t cmd, rect_t* src, vec_t* dst) {
    uint16_t ctrl1, ctrl2;
    int16_t srcx = src->min.x;
    int16_t srcy = src->min.y;
    int16_t dstx = dst->x;
    int16_t dsty = dst->y;
    int16_t height_minus_one = src->max.y - src->min.y;
    int16_t width_minus_one = src->max.x - src->min.x;

    ctrl1 = (1 <<  9);  /* bitblt */
    ctrl2 = (1 << 10) | /* 8bit chunky */
            (3 <<  5);  /* reserved bits, should be 11 */


    /* select DE2, read index 0 with no increment */
    vga_WritePortW(0x23c0,  (1 << 12) | (0 << 8) | (3 << 0));
    wait33(8);
    vga_WritePortW(0x23c2, 0x0000 | 0x00);  /* map base offset */
    vga_WritePortW(0x23c2, 0x1000 | (uint16_t)(nova->pitch & 0xfff));   /* row pitch */
    if (cmd & BL_FILL) {
        uint8_t pattern = BL_GETPATTERN(cmd);
        if (pattern) {
            uint32_t patoffs = get_fillpattern(pattern, src->min.x, src->min.y) - card->bank_addr;
            srcy = patoffs / ((int16_t)nova->pitch);
            srcx = patoffs - (nova->pitch * srcy);
            ctrl1 |= (1 << 2) |  /* source data is pattern */
                     (1 << 3);   /* source format is mono  */
            vga_WritePortW(0x23c2, 0x2000 | BL_GETBGCOL(cmd));
            vga_WritePortW(0x23c2, 0x4000 | BL_GETFGCOL(cmd));
        } else {
            ctrl1 |= (2 << 3);   /* source data is fixed color */
            vga_WritePortW(0x23c2, 0x2000 | BL_GETFGCOL(cmd));
            vga_WritePortW(0x23c2, 0x4000 | BL_GETBGCOL(cmd));
        }
    } else {
        if (srcx < dstx) {
            ctrl1 |= (1 << 8);  /* right -> left */
            srcx += width_minus_one;
            dstx += width_minus_one;
        }
        if (srcy < dsty) {
            ctrl1 |= (1 << 7);  /* bottom -> top */
            srcy += height_minus_one;
            dsty += height_minus_one;
        }
        if (cmd & BL_MONO) {
            ctrl1 |= (1 << 3); /* color src -> mono mask -> color expand */
            vga_WritePortW(0x23c2, 0x2000 | BL_GETFGCOL(cmd));
            vga_WritePortW(0x23c2, 0x4000 | BL_GETBGCOL(cmd));
        }
    }

    if (cmd & BL_TRANSPARENT) {
        ctrl2 |= ((1 << 9) | (1 << 8)); /* positive polarity, enable */
    }

    /* select DE1, read index 0 with no increment */
    vga_WritePortW(0x23c0,  (1 << 12) | (0 << 8) | (1 << 0));
    wait33(4);
    vga_WritePortW(0x23c2, 0x2000 | (uint16_t)(srcx & 0xfff));
    vga_WritePortW(0x23c2, 0x3000 | (uint16_t)(srcy & 0xfff));
    vga_WritePortW(0x23c2, 0x4000 | (uint16_t)(dstx & 0xfff));
    vga_WritePortW(0x23c2, 0x5000 | (uint16_t)(dsty & 0xfff));
    wait33(5);
    vga_WritePortW(0x23c2, 0x6000 | (uint16_t)(width_minus_one & 0xfff));
    vga_WritePortW(0x23c2, 0x7000 | (uint16_t)(height_minus_one & 0xfff));
    vga_WritePortW(0x23c2, 0x8000 | wd_rop(cmd));
    vga_WritePortW(0x23c2, 0x1000 | ctrl2);
    vga_WritePortW(0x23c2, 0x0000 | ctrl1);
    /* wait until finished */
    while(vga_ReadPortW(0x23ce) & 0x0080);
    return true;
}


static void configure_framebuffer(mode_t* mode) {

    if (wd_support_linear()) {

        /* todo: which cards and configs support this, does it require 1MB? */
        bool vram32bit = wd_support_32bit();

        /* bank location @ isa:2MB */
        vga_WriteReg(0x3C4, 0x14, 0x02);
        
        /* memory settings */ 
        vga_WriteReg(0x3CE, 0x0b,
            (vram32bit ? 0xC0 : 0x80) |     /* ram size */
            (3 << 4) |                      /* map 1MB */
            (1 << 2) |                      /* 16bit isa memory access */
            (vga_ReadReg(0x3CE, 0x0b) & 0x0f)
        );
        
        /* disable vga adressing */
        vga_WriteReg(vga_GetBaseReg(4), 0x2f, 0x00);

        /* memory interface */
        vga_WriteReg(0x3C4, 0x10,
            0xC0 |                          /* 4 level write buffer */
            (vram32bit ? 0x00 : 0x20) |     /* vram data path width */
            0x00 |                          /* 8 level fifo */
            0x01                            /* display fifo request @ 2 levels*/
        );
    } else if (wd_support_banks()) {

    } else if (vram >= 512) {
        vga_ModifyReg(0x3ce, 0x06, 0x0c, 0x00); /* single 128kb bank */
    }

    /* blitter configuration */
    if (wd_support_blitter() && mode) {
        if (chipset >= WD90C33) {
            card->caps |= NV_CAPS_BLIT | NV_CAPS_FILL | NV_CAPS_DITHER | NV_CAPS_DSTKEY | NV_CAPS_AUTOMASK;
            if (mode->bpp != 8) {
                card->caps &= ~(NV_CAPS_BLIT | NV_CAPS_FILL);
            }
            /* select DE2, read index 0 with no increment */
            vga_WritePortW(0x23c0,  (1 << 12) | (0 << 8) | (3 << 0));
            /* set default blitter registers */
            wait33(4);
            vga_WritePortW(0x23c2, 0x2000 | 0xff);  /* foreground color lo */
            vga_WritePortW(0x23c2, 0x3000 | 0xff);  /* foreground color hi */
            vga_WritePortW(0x23c2, 0x4000 | 0x00);  /* background color lo */
            vga_WritePortW(0x23c2, 0x5000 | 0x00);  /* background color hi */
            wait33(6);
            vga_WritePortW(0x23c2, 0x6000 | 0x00);  /* transparency color lo */
            vga_WritePortW(0x23c2, 0x7000 | 0x00);  /* transparency color hi */
            vga_WritePortW(0x23c2, 0x8000 | 0xff);  /* transparency mask lo */
            vga_WritePortW(0x23c2, 0x9000 | 0xff);  /* transparency mask hi */
            vga_WritePortW(0x23c2, 0xA000 | 0xff);  /* plane mask lo */
            vga_WritePortW(0x23c2, 0xB000 | 0xff);  /* plane mask hi */
            /* select DE1, read index 0 with no increment */
            vga_WritePortW(0x23c0,  (1 << 12) | (0 << 8) | (1 << 0));
            wait33(4);
            vga_WritePortW(0x23c2, 0x9000 | 0);  /* X0 clip */
            vga_WritePortW(0x23c2, 0xB000 | 0);  /* Y0 clip */
            vga_WritePortW(0x23c2, 0xA000 | (uint16_t)((mode->width-1) & 0xfff));  /* X1 clip */
            vga_WritePortW(0x23c2, 0xC000 | (uint16_t)((mode->height-1) & 0xfff));  /* Y1 clip */
        } else {
            card->caps |= NV_CAPS_BLIT | NV_CAPS_FILL | NV_CAPS_DITHER | NV_CAPS_DSTKEY | NV_CAPS_AUTOMASK;
            if (mode->bpp != 8) {
                card->caps &= ~(NV_CAPS_BLIT | NV_CAPS_FILL);
            }
            /* select blitter, read index 0 with no increment */
            vga_WritePortW(0x23c0,  (1 << 12) | (0 << 8) | (1 << 0));
            /* set default blitter registers */
            vga_WritePortW(0x23c2, 0xA000 | 0xff);  /* foreground color */
            vga_WritePortW(0x23c2, 0xB000 | 0x00);  /* background color */
            vga_WritePortW(0x23c2, 0xC000 | 0x00);  /* transparency color */
            vga_WritePortW(0x23c2, 0xD000 | 0xff);  /* transparency mask */
            vga_WritePortW(0x23c2, 0xE000 | 0xff);  /* plane mask */
        }
        /* upload fill patterns to vram */
        upload_fillpatterns(card->bank_addr + card->bank_size - 2048);
    }
}

static bool setmode(mode_t* mode) {
    if (vga_setmode(mode->code)) {

        /* custom 1280x720 mode */
        if ((mode->width == 1280) && (mode->height == 720)) {

            uint16_t hto = 1664;
            uint16_t hbs = 1280;
            uint16_t hbe = 1280 + 272;
            uint16_t hss = 1280 + 156;
            uint16_t hse = 1280 + 156 + 40;

            uint16_t vto = 750 - 2;
            uint16_t vde = 720 - 1;
            uint16_t vbs = 720;
            uint16_t vss = 723;

            uint8_t ofl = 
                (((vto & (1 << 8)) ? 1 : 0) << 0) |
                (((vde & (1 << 8)) ? 1 : 0) << 1) |
                (((vss & (1 << 8)) ? 1 : 0) << 2) |
                (((vbs & (1 << 8)) ? 1 : 0) << 3) |
                (1 << 4) |
                (((vto & (1 << 9)) ? 1 : 0) << 5) |
                (((vde & (1 << 9)) ? 1 : 0) << 6) |
                (((vss & (1 << 9)) ? 1 : 0) << 7);

            /* PR5: unlock PR0-4 */
            vga_WriteReg(0x3ce, 0x0f, 0x05);

            /* PR3: unlock polarity and crtc */
            vga_WritePort(0x3ce, 0x0d);
            vga_WritePort(0x3cf, vga_ReadPort(0x3cf) & 0x1c);    

            /* vsync+, hsync+ */
            vga_WritePort(0x3c2, vga_ReadPort(0x3cc) & 0x3f);

            /* unlock crtc */
            vga_WriteReg(0x3d4, 0x11, 0x05);

            /* horizontal */
            vga_WriteReg(0x3d4, 0x00, (hto / 8) - 5);               /* htotal           */
            vga_WriteReg(0x3d4, 0x01, (hbs / 8) - 1);               /* hdisp end        */
            vga_WriteReg(0x3d4, 0x02, (hbs / 8));                   /* hblank start     */
            vga_WriteReg(0x3d4, 0x03, ((hbe / 8) & 0x1f) | 0x80);   /* hblank end       */
            vga_WriteReg(0x3d4, 0x04, hss / 8);                     /* hsync start      */
            vga_WriteReg(0x3d4, 0x05, (hse / 8) & 0x1f);            /* hsync end        */

            /* vertical */
            vga_WriteReg(0x3d4, 0x06, vto & 0xff);                  /* vtotal           */
            vga_WriteReg(0x3d4, 0x07, ofl);                         /* overflow         */
            vga_WriteReg(0x3d4, 0x10, vss & 0xff);                  /* vsync start      */
            vga_WriteReg(0x3d4, 0x12, vde & 0xff);                  /* vdisp end        */
            vga_WriteReg(0x3d4, 0x13, hbs / 8);                     /* pitch            */
            vga_WriteReg(0x3d4, 0x15, vbs & 0xff);                  /* vblank start     */
            vga_WriteReg(0x3d4, 0x16, (vto - 1) & 0xff);            /* vblank end       */

            /* todo: hangs */
            #if 0
            /* relock crtc */
            vga_WritePort(0x3d4, 0x11);
            vga_WritePort(0x3d5, vga_ReadPort(0x3d5) | 0x80);

            /* PR3: relock polarity and crtc */
            vga_WritePort(0x3ce, 0x0d);
            vga_WritePort(0x3cf, vga_ReadPort(0x3cf) | 0xe3);

            /* WDC PR5: relock PR0-4 */
            vga_WriteReg(0x3ce, 0x0f, 0x00);
            #endif     
        }

        configure_framebuffer(mode);
        return true;
    }
    return false;
}

static void setaddr(uint32_t addr) {
    uint16_t crtc = vga_GetBaseReg(4);
    vga_ModifyReg(0x3ce, 0x0D, 0x18, (addr >> 15));
    vga_WritePortWLE(crtc, 0x0D00 | ((addr >>  2) & 0xff));
    vga_WritePortWLE(crtc, 0x0C00 | ((addr >> 10) & 0xff));
}

static void setbank(uint16_t num) {
    vga_WritePortWLE(0x3ce, 0x0900 | (num &0xff));  /* rdbank */
    vga_WritePortWLE(0x3ce, 0x0a00 | (num &0xff));  /* wrbank */
}

static bool init(card_t* card, addmode_f addmode) {

    /* detect hardware */
    if (!identify()) {
        return false;
    }

    /* supply driver callbacks and card info */
    card->name = chipset_strings[chipset];
    card->vram_size = 1024UL * vram;
    card->setmode = setmode;
    card->setaddr = setaddr;
    if (wd_support_linear()) {
        card->bank_addr = 0x200000UL;
        card->bank_size = card->vram_size;
    } else if (wd_support_banks()) {
        card->setbank = setbank;
        card->bank_size = 1024UL * 64;
        card->bank_step = 1024UL * 4;
        card->bank_count = card->vram_size / card->bank_size;
    } else if (vram >= 512) {
        card->bank_size = 1024UL * 128;
    }

    if (chipset >= WD90C33) {
        card->blit = blit33;
    } else if (chipset >= WD90C31) {
        card->blit = blit31;
    }


    /* 4bpp modes */
    addmode( 800, 600,  4, 0, 0x58);
    addmode(1024, 768,  4, 0, 0x5D);
    addmode(1280, 960,  4, 0, 0x6C);
    if (chipset >= WD90C31) {
        addmode(1280,1024,  4, 0, 0x64);
    }
    /* 8bpp modes */
    addmode( 640, 400,  8, 0, 0x5E);
    addmode( 640, 480,  8, 0, 0x5F);
    if ((chipset >= WD90C30) || (chipset == WD90C11)) {
        addmode( 800, 600,  8, 0, 0x5C);
    }
    if (chipset >= WD90C30) {
        addmode(1024, 768,  8, 0, 0x60);
        addmode(1280, 720,  8, 0, 0x60);    /* tweaked from 1024x768 */
    }
    /* todo: 16bpp modes */
    /* todo: 24bpp modes */

    /* 
     * PR31: system interface control
     *  7 : enable read/write offset
     *  6 : enable turbo for blanked lines
     *  5 : enable turbo for text
     *  4 : cpu rdy release 1
     *  3 : cpu rdy release 0
     *  2 : enable write buffer
     *  1 : enable 16bit IO (ATC)
     *  0 : enable 16bit IO (CRTC, SEQ, GDC)
     */
    if (chipset >= WD90C10) {
        vga_WriteReg(0x3C4, 0x11,
            (1 << 6) |  /* enable turbo for blanked lines */
            (3 << 3) |  /* cpu rdy release */
            (1 << 2) |  /* enable write buffer */
            (1 << 1) |  /* enable 16bit io (atc) */
            (1 << 0)    /* enable 16bit io (crtc, seq, gdc) */
        );
    }

    /* 
     * configure zero-waitstate for:
     *  a) write buffer is ready + memory address decoded + MWR asserted
     *  b) IO write is occuring
     */
    if (chipset >= WD90C10) {
        vga_ModifyReg(0x3c4, 0x13, 0xC0, 0xC0);
    }

    /* configure linear or banked framebuffer */
    configure_framebuffer(0);

    return true;
}

driver_t drv_wdc = { "wdc", init };

#endif /* DRV_INCLUDE_WDC */
