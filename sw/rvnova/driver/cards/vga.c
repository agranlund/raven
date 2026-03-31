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

#define DAC_CACHE 0     /* doesn't work correctly yet 4bpp */


/*-------------------------------------------------------------------------------
 * custom modelines
 *-----------------------------------------------------------------------------*/
modeline_t modeline_720p_cvt = { "720p_60hz_CVT",
    74500UL, 1280, 1344, 1472, 1664, 720, 723, 728, 748, 1, 0 };

modeline_t modeline_720p_cvtrb = { "720p_60hz_CVT-RB",
    64000UL, 1280, 1328, 1360, 1440, 720, 723, 728, 741, 0, 1 };

modeline_t modeline_720p_cea = { "720p_60hz_CEA",
    74250UL, 1280, 1390, 1430, 1650, 720, 725, 730, 750, 0, 0 };

modeline_t modeline_720p_75mhz = { "720p_60hz_75Mhz",
    75000UL, 1280, 1436, 1476, 1664, 720, 723, 728, 750, 0, 0 };


/*-------------------------------------------------------------------------------
 * shared vga functionality
 *-----------------------------------------------------------------------------*/
static bool vga_fastclear = false;
void vga_enable_fastclear(bool on) {
    vga_fastclear = on;
}

void vga_vblank_out(void) {
    while (vga_ReadPort(0x3DA) & 8);
}

void vga_vblank_in(void) {
    while (!(vga_ReadPort(0x3DA) & 8));
}

void vga_vsync(void) {
    vga_vblank_out();
    vga_vblank_in();
}

void vga_delay(uint32_t ms) {
    /* a bit of a kludge and not accurate at all
     * assumes a vertical refresh of 60hz which may not be true
     * but close enough for our intended use */
    uint32_t vsyncs = (ms / 17);
    vsyncs = vsyncs ? vsyncs : 1;
    for (; vsyncs; vsyncs--) {
        vga_vsync();
    }
}

bool vga_screen_on(bool on) {
    bool ret = vga_ReadPort(0x3c6) == 0 ? false : true;
    vga_WritePort(0x3c6, on ? 0xff : 0x00);
    return ret;
}

void vga_clear(void) {
    uint32_t col = 0x00000000UL;
    /* write to all planes */
    uint8_t mask = vga_ReadReg(0x3c4, 0x02);
    vga_WriteReg(0x3c4, 0x02, 0x0f);
    /* clear all banks */
    if (card) {
        uint16_t bank; uint16_t banks = card->bank_count ? card->bank_count : 1;
        for (bank = 0; bank < banks; bank++) {
            uint32_t* dst  = (uint32_t*)(card->isa_mem + card->bank_addr);
            uint32_t  siz  = card->bank_size >> 2;
            card->setbank(bank);
            for (; siz; siz--) {
                *dst++ = col;
            }
        }
    } else {
        uint32_t* dst = (uint32_t*)(PADDR_MEM8 + 0xA0000UL);
        uint32_t  siz = (1024UL * 128) >> 2;
        for (; siz; siz--) {
            *dst++ = col;
        }
    }
    /* restore write mask */
    vga_WriteReg(0x3c4, 0x02, mask);
}

bool vga_setmode(uint16_t code) {
    x86_regs_t r;
    bool result;
    if (code >= 0x100) {
        /* vesa */
        uint16_t clearflag = vga_fastclear ? 0x8000 : 0x0000;
        if (clearflag && !(code & clearflag)) {
            vga_vsync();
            vga_clear();
        }
        vga_vsync();
        r.x.bx = code | clearflag;
        r.x.ax = 0x4f02;    /* setmode */
        dprintf(("setmode %04x %04x\n", r.x.ax, r.x.bx));
        int86(0x10, &r, &r);
        dprintf(("ax = %04x %02x:%02x\n", r.x.ax, r.h.ah, r.h.al));
        result = (r.h.ah == 0);
    } else {
        /* vga or svga */
        uint8_t clearflag = vga_fastclear ? 0x80 : 0x00;
        if (clearflag && !(code & clearflag)) {
            vga_vsync();
            vga_clear();
        }
        vga_vsync();
        r.x.ax = code | clearflag;
        int86(0x10, &r, &r);
        result = true;
    }

    /* delay here because some cards can lock up if accessed too soon after mode change */
    /* the value used here is based on absolutely nothing */
    vga_delay(500);
    vga_vsync();
    return result;
}

uint16_t vga_modeline(modeline_t* ml) {
    uint16_t ret;
    uint8_t  ofl;
    uint16_t hto = (ml->htotal / 8) - 5;
    uint16_t hde = (ml->hdisp / 8) - 1;
    uint16_t hbs = (ml->hdisp / 8) - 1;
    uint16_t hbe = (ml->htotal / 8);
    uint16_t hss = (ml->hsyncstart / 8);
    uint16_t hse = (ml->hsyncend / 8);

    uint16_t vto = ml->vtotal - 2;
    uint16_t vss = ml->vsyncstart;
    uint16_t vde = ml->vdisp - 1;
    uint16_t vbs = ml->vdisp;
    uint16_t vbe = ml->vtotal - 1;

#if 1
    /* wdc is getting stuck blanks. */
    /* but this appears to work on both wdc and cirrus */
    hbe = 0;
    vbe = 0;
#endif

    /* vga overflow bits */
    ofl = 
        (((vto & (1 << 8)) ? 1 : 0) << 0) |
        (((vde & (1 << 8)) ? 1 : 0) << 1) |
        (((vss & (1 << 8)) ? 1 : 0) << 2) |
        (((vbs & (1 << 8)) ? 1 : 0) << 3) |
        (1 << 4) |
        (((vto & (1 << 9)) ? 1 : 0) << 5) |
        (((vde & (1 << 9)) ? 1 : 0) << 6) |
        (((vss & (1 << 9)) ? 1 : 0) << 7) ;

    /* svga overflow bits */
    ret = 
        (((vto & (1 << 10)) ? 1 : 0) <<  0) |
        (((vde & (1 << 10)) ? 1 : 0) <<  1) |
        (((vss & (1 << 10)) ? 1 : 0) <<  2) |
        (((vbs & (1 << 10)) ? 1 : 0) <<  3) |
        (((vbe & (1 <<  8)) ? 1 : 0) <<  5) |
        (((vbe & (1 <<  9)) ? 1 : 0) <<  6) |
        (((hto & (1 << 10)) ? 1 : 0) <<  8) |
        (((hbe & (1 <<  6)) ? 1 : 0) <<  9) |
        (((hbe & (1 <<  7)) ? 1 : 0) << 10) ;

    /* sync polarity */
    vga_WritePort(0x3c2,
        (vga_ReadPort(0x3cc) & 0x3f)
        | (ml->vpolarity << 7)
        | (ml->hpolarity << 6)
    );

    /* unlock crtc */
    vga_ModifyReg(0x3d4, 0x11, 0x8f, (ml->vsyncend & 0x0f));        /* vsync end                */

    /* horizontal */
    vga_WriteReg(0x3d4, 0x00, hto & 0xff);                          /* htotal                   */
    vga_WriteReg(0x3d4, 0x01, hde & 0xff);                          /* hdisp end                */
    vga_WriteReg(0x3d4, 0x02, hbs & 0xff);                          /* hblank start             */
    vga_WriteReg(0x3d4, 0x03, ((hbe & 0x1f) | 0x80));               /* hblank end               */
    vga_WriteReg(0x3d4, 0x04, hss & 0xff);                          /* hsync start              */
    vga_WriteReg(0x3d4, 0x05, ((hbe<<2)&0x80) | (hse & 0x1f));      /* hblank end + hsync end   */

    /* vertical */
    vga_WriteReg(0x3d4, 0x06, vto & 0xff);                          /* vtotal                   */
    vga_WriteReg(0x3d4, 0x07, ofl & 0xff);                          /* overflow                 */
    vga_ModifyReg(0x3d4, 0x09, 0x20, (vbs >> 4));                   /* vblank start overflow    */
    vga_WriteReg(0x3d4, 0x10, vss & 0xff);                          /* vsync start              */
    vga_WriteReg(0x3d4, 0x12, vde & 0xff);                          /* vdisp end                */
    vga_WriteReg(0x3d4, 0x13, (ml->hdisp / 8)  & 0xff);             /* pitch                    */
    vga_WriteReg(0x3d4, 0x15, vbs & 0xff);                          /* vblank start             */
    vga_WriteReg(0x3d4, 0x16, vbe & 0xff);                          /* vblank end               */

    /* return svga overflow */
    return ret;
}


#if DAC_CACHE
typedef struct {
    uint8_t ar,ag,ab,aa;
    uint8_t br,bg,bb,ba;
} daccache_t;
static daccache_t daccache[256];
#endif
static uint16_t dacscale = 256;

void vga_setcolors(uint16_t index, uint16_t count, uint8_t* colors) {
    for (; (count != 0) && (index < 256); count--, index++) {
#if DAC_CACHE
        /* software value */
        uint8_t r = *colors++;
        uint8_t g = *colors++;
        uint8_t b = *colors++;

        daccache[index].ar = r;
        daccache[index].ag = g;
        daccache[index].ab = b;

        /* hardware value */
        r = ((dacscale * r) >> 10);
        g = ((dacscale * g) >> 10);
        b = ((dacscale * b) >> 10);

        /* update hardware dac if changed */
        if ((r != daccache[index].br) || (g != daccache[index].bg) || (b != daccache[index].bb)) {
            daccache[index].br = r;
            daccache[index].bg = g;
            daccache[index].bb = b;
            vga_WritePort(0x3c8, index);
            vga_WritePort(0x3c9, r);
            vga_WritePort(0x3c9, g);
            vga_WritePort(0x3c9, b);
        }
#else
        vga_WritePort(0x3c8, index);
        vga_WritePort(0x3c9, ((dacscale * (*colors++)) >> 10));
        vga_WritePort(0x3c9, ((dacscale * (*colors++)) >> 10));
        vga_WritePort(0x3c9, ((dacscale * (*colors++)) >> 10));
#endif        
    }
}

void vga_getcolors(uint16_t index, uint16_t count, uint8_t* colors) {
    for (; (count != 0) && (index < 256); count--, index++) {
#if DAC_CACHE
        *colors++ = daccache[index].ar;
        *colors++ = daccache[index].ag;
        *colors++ = daccache[index].ab;
#else
        uint16_t r,g,b;
        vga_WritePort(0x3c8, index);
        r = vga_ReadPort(0x3c9);
        g = vga_ReadPort(0x3c9);
        b = vga_ReadPort(0x3c9);
        *colors++ = (uint8_t) ((r << 10) / dacscale);
        *colors++ = (uint8_t) ((g << 10) / dacscale);
        *colors++ = (uint8_t) ((b << 10) / dacscale);
#endif        
    }
}

void vga_updatecolorcache(bool force) {
#if DAC_CACHE
    uint16_t index,r,g,b;
    for(index=0; index<256; index++) {
        vga_WritePort(0x3c8, index);
        r = vga_ReadPort(0x3c9);
        g = vga_ReadPort(0x3c9);
        b = vga_ReadPort(0x3c9);

        if (index < 16) {
            dprintf(("A %d : %02x %02x %02x : %02x %02x %02x\n", index, r, g, b, daccache[index].br, daccache[index].bg, daccache[index].bb ));
        }

        if (force || (r != daccache[index].br)) { daccache[index].br = r; daccache[index].ar = (uint8_t) ((r << 10) / dacscale); }
        if (force || (g != daccache[index].bg)) { daccache[index].bg = g; daccache[index].ag = (uint8_t) ((g << 10) / dacscale); }
        if (force || (b != daccache[index].bb)) { daccache[index].bb = b; daccache[index].ab = (uint8_t) ((b << 10) / dacscale); }

        if (index < 16) {
            dprintf(("B %d : %02x %02x %02x : %02x %02x %02x\n", index, r, g, b, daccache[index].br, daccache[index].bg, daccache[index].bb ));
        }
    }
#endif    
}

void vga_setcolorscale(uint16_t scale) {
#if DAC_CACHE
    if (scale != dacscale) {
        uint16_t index;
        dacscale = scale;
        for (index=0; index<256; index++) {
            vga_setcolors(index, 1, &daccache[index].ar);
        }
    }
#else
    if (scale != dacscale) {
        uint8_t c[4];
        uint16_t index;
        uint16_t oldscale = dacscale;
        for (index=0; index<256; index++) {
            dacscale = oldscale;
            vga_getcolors(index, 1, c);
            dacscale = scale;
            vga_setcolors(index, 1, c);
        }
    }
#endif    
}


void vga_setaddr(uint32_t addr) {
    uint16_t crtc = vga_GetBaseReg(4);
    vga_WriteReg(crtc, 0x0D, (addr >>  2) & 0xff);
    vga_WriteReg(crtc, 0x0C, (addr >> 10) & 0xff);
}

/*-------------------------------------------------------------------------------
 * generic base vga driver
 *-----------------------------------------------------------------------------*/

static void setbank(uint16_t num) {
}

static bool setmode(gfxmode_t* mode) {
    return vga_setmode(mode->code);
}

static bool init(card_t* card, ini_t* settings, addmode_f addmode) {

    card->name = "unknown";
    card->bank_addr = 0xA0000UL;
    card->vram_size = 1024UL * 256;
    card->bank_size = 1024UL * 64;
    card->bank_step = 1024UL * 4;
    card->bank_count = 1;

    card->isa_mem = raven()->vga_Addr() & 0xfff00000UL;
    card->isa_io  = PADDR_IO16;

    card->setmode = setmode;
    card->setbank = setbank;
    card->setaddr = vga_setaddr;
    card->setcolors = vga_setcolors;
    card->getcolors = vga_getcolors;
    card->vsync = vga_vsync;
    card->clear = vga_clear;

    addmode(640, 350, 1, 0, 0x10);
    addmode(640, 480, 1, 0, 0x12);
    addmode(640, 350, 4, 0, 0x10);
    addmode(640, 480, 4, 0, 0x12);
    addmode(320, 200, 8, 0, 0x13);

    vga_updatecolorcache(true);
    vga_setcolorscale(ini_GetInt(settings, "dacscale", 256));

    return true;
}

driver_t drv_vga = { "vga", init };
