/*-------------------------------------------------------------------------------
 * RVGA Tseng driver
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
#include "emulator.h"
#include "raven.h"
#include "vga.h"
#include "ini.h"

#define support_blitter() (chipset >= ET4000W32)
#define support_linear()  (chipset >= ET4000AX)

#if DRV_INCLUDE_TSENG

typedef enum {
    UNKNOWN = 0, ET4000AX, ET4000W32, ET4000W32I, ET4000W32P
} chipset_e;

static const char* chipset_strings[] = {
    "unknown", "ET4000AX", "ET4000/W32", "ET4000/W32i", "ET4000/W32p"
};

static chipset_e chipset;
static uint16_t vram;
static uint16_t numclks;
static struct {
    uint8_t code;
    uint32_t freq;
} clocks[64];

static bool identify(void) {
    uint8_t old3bf, old3d8;

    chipset = 0; vram = 0;

    /* enbable tseng extensions */
    old3bf = vga_ReadPort(0x3bf);
    old3d8 = vga_ReadPort(0x3d8);
    vga_WritePort(0x3bf, 3);
    vga_WritePort(0x3d8, 0xa0);

    /* look for tseng card */
    if (vga_TestPort(0x3cd, 0x3f)) {
        if (vga_TestReg(0x3d4, 0x33, 0x0f)) {
            chipset = ET4000AX;
            if (vga_TestPort(0x3cb, 0x33)) {
                switch (vga_ReadReg(0x217a, 0xec) >> 4) {
                    case 0: chipset = ET4000W32; break;
                    case 2: chipset = ET4000W32P; break;
                    case 3: chipset = ET4000W32I; break;
                }
            }
        }
    }

    /* look for installed memory */
    if (chipset != 0) {
        uint8_t vconf = vga_ReadReg(0x3d4, 0x37);
        vram = (vconf & 0x08) ? 256 : 64;
        switch (vconf & 0x3) {
            case 2: vram <<= 1; break;
            case 3: vram <<= 2; break;
        }
        if (vconf & 0x80) {
            vram <<= 1;
        }
        dprintf((" vconf %02x\n", vconf));
        return true;
    }

    /* restore and fail */
    vga_WritePort(0x3d8, old3d8);
    vga_WritePort(0x3bf, old3bf);
    return false;
}

static void addclock(uint8_t code, uint32_t val) {
    if (numclks < 64) {
        int i;
        uint32_t f = ((val+249) / 500) * 500;
        for (i=0; i<numclks; i++) {
            if (clocks[i].freq == f) {
                return;
            }
        }
        clocks[numclks].code = code;
        clocks[numclks].freq = f;
        numclks++;
    }
}

static uint8_t getclock(void) {
    uint8_t clk = ((vga_ReadPort(0x3cc) >> 2) & 0x03);   /* bit 1..0  */
    clk |= ((vga_ReadReg(0x3d4, 0x34) << 1) & 0x04);     /* bit 2     */
    if (chipset >= ET4000W32) {
        clk |= ((vga_ReadReg(0x3d4, 0x31) >> 3) & 0x18); /* bit 4..3 */
        clk |= ((vga_ReadReg(0x3d4, 0x34) << 5) & 0x20); /* bit 5 (translation table) */
        clk |= ((vga_ReadReg(0x3c4, 0x07) << 0) & 0x40); /* bit 6 (div2) */
        clk |= ((vga_ReadReg(0x3c4, 0x07) << 7) & 0x80); /* bit 7 (div4) */
    } else {
        uint8_t seq07 = vga_ReadReg(0x3c4, 0x07);
        if ((seq07 & 0x41) == 0x41) {
            clk |= 0x10;                                 /* bit 4..3 */
        } else {
            clk |= (seq07 >> 3) & 0x08;                  /* bit 3    */
        }
    }
    return clk;
}

static void setclock(uint8_t clk) {
    vga_WritePort(0x3c2, (vga_ReadPort(0x3cc) & 0xf3) | ((clk & 0x03) << 2));   /* bit 1..0 */
    vga_ModifyReg(0x3d4, 0x34, 0x02, (clk & 0x04) >> 1);        /* bit 2 */
    if (chipset >= ET4000W32) {
        vga_ModifyReg(0x3d4, 0x31, 0xc0, (clk & 0x18) << 3);    /* bit 4..3 */
        vga_ModifyReg(0x3d4, 0x34, 0x01, (clk & 0x20) >> 5);    /* bit5 (translation table) */
        vga_ModifyReg(0x3c4, 0x07, 0x40, (clk & 0x40) >> 0);    /* bit6 (div2) */
        vga_ModifyReg(0x3c4, 0x07, 0x01, (clk & 0x80) >> 7);    /* bit7 (div4) */
    } else {
        uint8_t seq07 = vga_ReadReg(0x3c4, 0x07);
        seq07 &= 0xbe;
        seq07 |= (clk & 0x10) ? 0x41 : 0x00;                    /* bit 4 */
        seq07 |= ((clk & 0x08) << 3);                           /* bit 3 */
        vga_WriteReg(0x3c4, 0x07, seq07);
    }
}

static uint32_t measureclock(int16_t ticks) {
    while(!(vga_ReadPort(0x3DA) & 0x08));
    timer_start();
    for(; ticks; ticks--) {
        while(vga_ReadPort(0x3DA) & 0x08);
        while(!(vga_ReadPort(0x3DA) & 0x08));
    }
    return timer_stop();
}

static uint8_t findclock(uint32_t freq) {
    uint16_t i;
    uint8_t best_code = 0;
    uint32_t best_err = 0xffffffffUL;
    for (i=0; i<numclks; i++) {
        uint32_t err = (freq > clocks[i].freq) ? (freq - clocks[i].freq) : (clocks[i].freq - freq);
        if (err < best_err) {
            best_code = clocks[i].code;
            best_err = err;
        }
    }
    return best_code;
}

static void initclocks(void) {
    uint16_t i;
    uint8_t oclock;
    uint32_t oticks;
    uint32_t pixc = 850UL * 525;
    vga_ModifyReg(0x3d4, 0x11, 0x80, 0x00);
    vga_screen_on(false);
    oclock = getclock();
    oticks = measureclock(1);
    dprintf((" clocks\n"));
    for (i=0; i<64; i++) {
        uint32_t nticks;
        uint32_t freq;
        setclock(i);
        nticks = measureclock(1);
        freq = (25175UL * oticks) / nticks;
        addclock(i, freq);
        if (chipset >= ET4000W32) {
            addclock((i | 0x40), freq >> 1);
            addclock((i | 0x80), freq >> 2);
        }
    }
    setclock(oclock);
    vga_screen_on(true);
#if DEBUG
    for (i=0; i<numclks; i++) {
        dprintf((" %2d: %02x: %ld\n", i, clocks[i].code, clocks[i].freq));
    }
#endif
}

static void configure_framebuffer(card_t* card, gfxmode_t* mode) {
    if (support_linear()) {
        vga_ModifyReg(0x3ce, 0x06, 0x0c, 0x00);         /* 128kb mode */
        vga_WriteReg(0x3ce, 0x08, 0x0ff);               /* all bits enable */
        vga_WritePort(0x3cd, 0x00);                     /* segment 0 */

        if (chipset >= ET4000W32) {
            /* detect SEGE and A22 wiring on the card */
            /* different hardware vendors wire them up in different ways */
            static int8_t lconf = -1;
            if (lconf < 0) {
                vga_ModifyReg(0x3d4, 0x36, 0x10, 0x00);                                 /* linear off */
                *((volatile uint32_t*)(card->isa_mem + 0xA0000UL)) = 0xdeadbeefUL;      /* write test pattern */
                for (lconf = 0; lconf <= 3; lconf++) {
                    vga_ModifyReg(0x3d4, 0x36, 0x10, 0x00);                             /* linear off */
                    vga_WriteReg(0x3d4, 0x30, lconf);
                    vga_ModifyReg(0x3d4, 0x36, 0x10, 0x10);                             /* linear on */
                    if (*((volatile uint32_t*)(card->isa_mem + card->bank_addr)) == 0xdeadbeefUL) {
                        break;
                    }
                }
                if (lconf < 0) { lconf = 0x01; }
                dprintf((" lconf %02x\n", lconf));
                vga_ModifyReg(0x3d4, 0x36, 0x10, 0x00);                             /* linear off */
                *((volatile uint32_t*)(card->isa_mem + 0xA0000UL)) = 0;             /* clear test pattern */
            }

            /* set linear mode. address must be set first on certain cards */
            vga_WriteReg(0x3d4, 0x30, lconf);           /* linear address */
            vga_ModifyReg(0x3d4, 0x36, 0x18, 0x10);     /* linear enable  */

            /* reset screen offsets */
#if 0            
            vga_WriteReg(0x3d4, 0x0c, 0x00);            /* addr start 15..8 */
            vga_WriteReg(0x3d4, 0x0d, 0x00);            /* addr start  7..0 */
            vga_ModifyReg(0x3d4, 0x33, 0x13, 0x00);     /* bit 0,1,4 = bit 16,17,18 */
            vga_ModifyReg(0x3d4, 0x35, 0x0f, 0x00);     /* another extended display start */
#endif
        } else {
            /* todo: can it be moved somewhere above the 1MB mark? */
            vga_ModifyReg(0x3d4, 0x36, (3<<4), (3<<4)); /* linear adressing enable */
        }
    } else if (vram >= 512) {
        /* single 128kb bank */
        vga_ModifyReg(0x3ce, 0x06, 0x0c, 0x00);
    }

    /* blitter configuration */
    if (support_blitter() && mode) {
    }
}

static bool setmode(gfxmode_t* mode) {
    if (vga_setmode(mode->code)) {
        /* unlock in case vgabios locked us */
        vga_WritePort(0x3bf, 3);
        vga_WritePort(0x3d8, 0xa0);

        /* todo: make sure vgabios didn't pick interlaced 1024x768 */
        if (mode->code == 0x38) {
            uint8_t misc = vga_ReadPort(0x3cc);
            if ((misc & (1 << 4)) == 0) {
                dprintf(("interlaced\n"));
                /* From ET4000AX Databook */
                vga_WritePort(0x3c2, misc | (1 << 4));
                vga_ModifyReg(0x3d4, 0x11, 0x80, 0x00);
                vga_WriteReg(0x3d4, 0x00, 0xa1);
                vga_WriteReg(0x3d4, 0x02, 0x80);
                vga_WriteReg(0x3d4, 0x03, 0x04);
                vga_WriteReg(0x3d4, 0x04, 0x88);
                vga_WriteReg(0x3d4, 0x05, 0x9e);
                vga_WriteReg(0x3d4, 0x06, 0x26);
                vga_WriteReg(0x3d4, 0x07, 0xfd);
                vga_WriteReg(0x3d4, 0x11, 0x0a);
                vga_WriteReg(0x3d4, 0x15, 0x04);
                vga_WriteReg(0x3d4, 0x16, 0x22);
                vga_WriteReg(0x3d4, 0x35, 0x00);
                vga_ModifyReg(0x3d4, 0x11, 0x80, 0x80);
                setclock(findclock(64000UL));
            }

            if ((mode->width == 1280) && (mode->height == 720)) {
                modeline_t* ml = &modeline_720p_cvtrb;
                vga_modeline(ml);
                setclock(findclock(ml->pclk));
            }
        }

        configure_framebuffer(card, mode);
        return true;
    }
    return false;
}

static void setaddr(uint32_t addr) {
#if 0    
    /* todo... */
    /* 3d4, 33 */
    uint16_t crtc = vga_GetBaseReg(4);
    vga_ModifyReg(0x3ce, 0x0D, 0x18, (addr >> 15));
    vga_WritePortWLE(crtc, 0x0D00 | ((addr >>  2) & 0xff));
    vga_WritePortWLE(crtc, 0x0C00 | ((addr >> 10) & 0xff));
#endif    
}

static bool init(card_t* card, addmode_f addmode) {
    /* detect hardware */
    if (!identify()) {
        return false;
    }

    /* measure clocks */
    initclocks();

    /* supply driver callbacks and card info */
    card->name = chipset_strings[chipset];
    card->vram_size = 1024UL * vram;
    card->setmode = setmode;
    card->setaddr = setaddr;
    if (support_linear()) {
        if (chipset >= ET4000W32) {
            card->bank_addr = 0x400000UL;
        } else {
            card->bank_addr = 0x000000UL;   /* todo: can it be moved? */
        }
        card->bank_size = card->vram_size;
    } else if (vram >= 512) {
        card->bank_size = 1024UL * 128;
    }

    /*
    if (support_blitter()) {
        card->blit = blit;
    }
    */

    /* 4bpp modes */
    addmode( 640, 480,  4, 0, 0x25);
    addmode( 800, 600,  4, 0, 0x29);
    addmode(1024, 768,  4, 0, 0x37);
    
    /* 8bpp modes */
    addmode( 640, 400,  8, 0, 0x2F);
    addmode( 640, 480,  8, 0, 0x2E);
    addmode( 800, 600,  8, 0, 0x30);
    addmode(1024, 768,  8, 0, 0x38);
    addmode(1280, 720,  8, 0, 0x38);    /* custom */

    /* enable 16bit mem access */
    /*
    vga_ModifyReg(0x3d4, 0x36, 0x40, 0x40);
    */

    /* configure linear or banked framebuffer */
    configure_framebuffer(card, 0);

    /* interleave mode */
    if (chipset == ET4000W32I) {
        ini_t ini_root, ini;
        ini_Load(&ini_root, "c:\\rvga.inf");
        ini_GetSection(&ini, &ini_root, "et4000.w32");
        if (ini_GetInt(&ini, "interleave", 0) != 0) {
            vga_ModifyReg(0x3d4, 0x37, 0x01, 0x01); /* membus width */
            vga_ModifyReg(0x3d4, 0x32, 0x83, 0x80); /* interleave enable */
        }
        ini_Unload(&ini_root);
    }
    return true;
}

driver_t drv_tseng = { "tseng", init };

#endif /* DRV_INCLUDE_TSENG */
