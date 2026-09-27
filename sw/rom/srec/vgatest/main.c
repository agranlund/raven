/*
 * vga testbed
 */

#include <lib.h>
#include "../../../lib/raven.h"

static raven_t* rv;
static uint32_t vga_iobase;
static uint32_t vga_membase;

#define ISA_IOBASE      RV_PADDR_ISA_IO16
#define ISA_MEMBASE     RV_PADDR_ISA_RAM16

#define ISA_IOBASE8     RV_PADDR_ISA_IO
#define ISA_MEMBASE8    RV_PADDR_ISA_RAM

#define VGAMEM_BASE     0xA0000UL


static uint8_t pal_ega[16*3] = {
    0x00, 0x00, 0x00,   //  0: black
    0x00, 0x00, 0xaa,   //  1: blue
    0x00, 0xaa, 0x00,   //  2: green
    0x00, 0xaa, 0xaa,   //  3: cyan
    0xaa, 0x00, 0x00,   //  4: red
    0xaa, 0x00, 0xaa,   //  5: magenta
    0xaa, 0x55, 0x00,   //  6: brown
    0xaa, 0xaa, 0xaa,   //  7: light gray
    0x55, 0x55, 0x55,   //  8: dark gray
    0x55, 0x55, 0xff,   //  9: bright blue
    0x55, 0xff, 0x55,   // 10: bright green
    0x55, 0xff, 0xff,   // 11: bright cyan
    0xff, 0x55, 0x55,   // 12: bright red
    0xff, 0x55, 0xff,   // 13: bright magenta
    0xff, 0xff, 0x55,   // 14: bright yellow
    0xff, 0xff, 0xff    // 15: bright white
};

/* ---------------------------------------------------------------------------- */
/* vga helpers                                                                  */
/* ---------------------------------------------------------------------------- */



static uint8_t vga_ReadPort(uint16_t port) {
    return *((volatile uint8_t *)(vga_iobase + port));
}

static void vga_WritePort(uint16_t port, uint8_t val) {
    *((volatile uint8_t *)(vga_iobase + port)) = val;
}

static void vga_WriteReg(uint16_t port, uint8_t idx, uint8_t val)
{
    if (port == 0x3C0) {
        (void)vga_ReadPort(0x3DA);
        vga_WritePort(port + 0, idx);
        vga_WritePort(port + 0, val);
    } else {
        vga_WritePort(port + 0, idx);
        vga_WritePort(port + 1, val);
    }
}

static uint8_t vga_ReadReg(uint16_t port, uint8_t idx)
{
    if (port == 0x3C0) {
        (void)vga_ReadPort(0x3DA);
        vga_WritePort(port + 0, idx);
        return vga_ReadPort(port + 1);
    } else {
        vga_WritePort(port + 0, idx);
        return vga_ReadPort(port + 1);
    }
}

static void vga_dumpregs(void)
{
    uint8_t i;

    printf("SEQ:\n");
    for (i = 0; i <= 0x04; ++i)
        printf("  SEQ%02X = %02X\n", i, vga_ReadReg(0x3C4, i));

    printf("GC:\n");
    for (i = 0; i <= 0x08; ++i)
        printf("  GC%02X  = %02X\n", i, vga_ReadReg(0x3CE, i));

    printf("CRTC:\n");
    for (i = 0; i <= 0x18; ++i)
        printf("  CR%02X  = %02X\n", i, vga_ReadReg(0x3D4, i));

    printf("AC:\n");
    for (i = 0; i <= 0x14; ++i)
        printf("  AC%02X  = %02X\n", i, vga_ReadReg(0x3C0, i));

    printf("MISC = %02X\n", vga_ReadPort(0x3CC));
    printf("DACM = %02X\n", vga_ReadPort(0x3C6));

    // Reset AC flip-flop so subsequent AC access starts predictably.
    (void)vga_ReadPort(0x3DA);	
}

static void vga_Fill(uint8_t color)
{
    uint32_t c8 = (uint32_t)color;
    uint32_t c32 = (c8 << 24) | (c8 << 16) | (c8 << 8) | c8;
    uint32_t* ptr = (uint32_t*) vga_membase;
    for (uint32_t i = 0; i < ((64 * 1024) / 4); i++) {
        *ptr++ = c32;
    }
}

static void vga_Clear() {
    vga_Fill(0x00);
}



static uint8_t vga_ModeSTpalIndex(uint8_t idx)
{
    // swap bit 1 (plane 1) and bit 2 (plane 2), keep bits 0 and 3
    return (idx & 0x09) | ((idx & 0x02) << 1) | ((idx & 0x04) >> 1);
}

static void vga_SetPalModeST(uint32_t idx, uint32_t num, uint8_t *pal) {
	static const uint8_t vga_ModeSTpalIndexTable[16] = {
		0x0, 0x1, 0x4, 0x5, 0x2, 0x3, 0x6, 0x7,
		0x8, 0x9, 0xc, 0xd, 0xa, 0xb, 0xe, 0xf
	};

	const uint8_t pshift = 2;
    for (uint32_t i = 0; (i < num) && ((i + idx) < 256); i++) {
		uint8_t swappedIdx = vga_ModeSTpalIndexTable[(idx + i) & 15];
        vga_WritePort(0x3c8, swappedIdx);
        vga_WritePort(0x3c9, *pal++ >> pshift);
        vga_WritePort(0x3c9, *pal++ >> pshift);
        vga_WritePort(0x3c9, *pal++ >> pshift);
    }
}


static void vga_SetPal(uint32_t idx, uint32_t num, uint8_t *pal) {
    const uint8_t pshift = 2;
    for (uint32_t i = 0; (i < num) && ((i + idx) < 256); i++) {
        vga_WritePort(0x3c8, (uint8_t)(idx + i));
        vga_WritePort(0x3c9, *pal++ >> pshift);
        vga_WritePort(0x3c9, *pal++ >> pshift);
        vga_WritePort(0x3c9, *pal++ >> pshift);
    }
}

static void vga_GetPal(uint32_t idx, uint32_t num, uint8_t *pal) {
    const uint8_t pshift = 2;
    for (uint32_t i = 0; (i < num) && ((i + idx) < 256); i++) {
        vga_WritePort(0x3c8, (uint8_t)(idx + i));
        *pal++ = vga_ReadPort(0x3c9) << pshift;
        *pal++ = vga_ReadPort(0x3c9) << pshift;
        *pal++ = vga_ReadPort(0x3c9) << pshift;
    }
}

static void vga_SetColor(uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t c[3] = { r, g, b };
    vga_SetPal(idx, 1, c);
}

static uint32_t vga_Init(void) {
    return rv->vga_Init();
}

static void vga_SetMode(uint32_t mode) {
    rv->vga_SetMode(mode);
}

static uint32_t vga_Addr(void) {
    return rv->vga_Addr();
}


/* ---------------------------------------------------------------------------- */
/* tests                                                                        */
/* ---------------------------------------------------------------------------- */

static void mode13h_testpic() {
    // screen off
    vga_WritePort(0x3c6, 0x00);
    // palette
    for (int i = 0; i < 64; i++) { vga_SetColor(i +   0, (i*4), 0, 0); }
    for (int i = 0; i < 64; i++) { vga_SetColor(i +  64, 0, (i*4), 0); }
    for (int i = 0; i < 64; i++) { vga_SetColor(i + 128, 0, 0, (i*4)); }
    for (int i = 0; i <  8; i++) { vga_SetColor(i + 192, (i*32), (i*32), (i*32)); }
    for (int i = 0; i < 56; i++) { vga_SetColor(i + 200, 0, 0, 0); }
    // test image
    volatile uint8_t *vram = (volatile uint8_t *)vga_membase;
    for (int y = 0; y < 200; y++) {
        for (int x = 0; x < 320; x++) {
            *vram++ = y;
        }
    }
    vram = (volatile uint8_t *)vga_membase;
    for (int x = 0; x < 320; x++) {
        vram[x] = 199;
        vram[x + (320*199)] = 199;
    }
    for (int y = 0; y < 200; y++) {
        vram[y * 320] = 199;
        vram[319 + (y*320)] = 199;
    }

    // screen on
    vga_WritePort(0x3c6, 0xff);    
}


static void testMode_320x200x8bpp_chunky() {
    vga_SetMode(0x13);
    mode13h_testpic();
}

static void testMode_320x200x8bpp_chunky_60hz() {
    vga_SetMode(0x13);
    // vsync-, hsync-
    vga_WritePort(0x3c2, vga_ReadPort(0x3cc) | 0xc0);
    // unlock regs 0-7
    vga_WritePort(0x3d4, 0x11);
    vga_WritePort(0x3d5, vga_ReadPort(0x3d5) & 0x7f);
    vga_WriteReg(0x3d4, 0x06, 0x0b);    // vertical total
    vga_WriteReg(0x3d4, 0x07, 0x3e);    // overflow
    vga_WriteReg(0x3d4, 0x10, 0xbc);    // vertical retrace start
    vga_WriteReg(0x3d4, 0x11, 0x8c);    // vertical retrace end (lock 0-7)
    vga_WriteReg(0x3d4, 0x12, 0x8f);    // vertical display enable end
    vga_WriteReg(0x3d4, 0x15, 0x90);    // vertical blank start
    vga_WriteReg(0x3d4, 0x16, 0x04);    // vertical blank end
    mode13h_testpic();
}

static void testMode_320x200x8bpp_chunky_50hz() {
}


static void testMode_320x200x4bpp_planar_interleaved() {
    static uint16_t scr_stlow[320*200/2];
    static uint16_t scr_stlow_shadow[320*200/2];

	memset(scr_stlow, 0, 320*200/2);
	memset(scr_stlow_shadow, 0, 320*200/2);

    // make planes interleaved
    //  0 = pixels 0..7,  plane 0
    //  1 = pixels 0..7,  plane 1
    //  2 = pixels 0..7,  plane 2
    //  3 = pixels 0..7,  plane 3
    //  4 = pixels 8..15, plane 0
    //  etc..

#if 1
	// start from standard 320x200x8bpp mode 13h
    vga_SetMode(0x13);

	static const uint16_t vga_regs_13h_to_stlow[] =
	{
		// sequencer: halve the dot clock (inside synchronous reset)
		0x3c4, (0x00 << 8) | 0x01,      // sync reset
		0x3c4, (0x01 << 8) | 0x09,      // 8-dot chars, dot clock / 2
		0x3c4, (0x00 << 8) | 0x03,      // end reset
		0x3c4, (0x02 << 8) | 0x0f,      // plane write mask
		0x3c4, (0x04 << 8) | 0x08,      // chain-4 enable

		// CRTC: horizontal timing for 40 char clocks
		0x3d4, (0x11 << 8) | 0x0e,      // unlock CR00-CR07
		0x3d4, (0x00 << 8) | 0x2d,
		0x3d4, (0x01 << 8) | 0x27,
		0x3d4, (0x02 << 8) | 0x28,
		0x3d4, (0x03 << 8) | 0x90,
		0x3d4, (0x04 << 8) | 0x2b,
		0x3d4, (0x05 << 8) | 0x80,
		0x3d4, (0x11 << 8) | 0x8e,      // relock

		0x3d4, (0x13 << 8) | 0x14,      // offset: 160 bytes/line
		0x3d4, (0x14 << 8) | 0x40,      // 32bit addressing
		0x3d4, (0x17 << 8) | 0xe3,      // byte mode

		0x3ce, (0x05 << 8) | 0x00,      // planar shift mode

		// attribute controller: PCS off, PAS bit kept set in the index
		0x3c0, ((0x10 | 0x20) << 8) | 0x01,
	};

    for (uint32_t i = 0; i < sizeof(vga_regs_13h_to_stlow) / sizeof(uint16_t); i += 2) {
		uint16_t p = vga_regs_13h_to_stlow[i+0];
        uint16_t v = vga_regs_13h_to_stlow[i+1];
        vga_WriteReg(p, (uint8_t)(v >> 8), (uint8_t)v);
    }	


#else
	// start from standard 320x200x4bpp EGA mode 0Dh
    vga_SetMode(0x0d);
    vga_WriteReg(0x3c4, 0x02, 0xf);     // plane write mask
    vga_WriteReg(0x3c4, 0x04, 0x08);    // chain-4 enable
    vga_WriteReg(0x3d4, 0x14, (1<<6));  // 32bit addressing
#endif



    volatile uint8_t* vram = (volatile uint8_t*)vga_membase;

    scr_stlow[0] = 0xffff;
    scr_stlow[2] = 0xffff;
    scr_stlow[4] = 0xffff;
	scr_stlow[80] = 0xffff;

#if 1

	// assigned 16 color palette
	vga_SetPalModeST(0, 16, pal_ega);

	__asm__ volatile
	(
		"move.w	#4000-1, %%d4\n\t"
		"move.l	#0xFF00FF00, %%d5\n\t"
		"move.l	#0x00FF00FF, %%d6\n\t"
		"1:\n\t"
		"move.l	(%0)+,%%d0\n\t"		/* d0 = p0h, p0l, p1h, p1l */
		"move.l	(%0)+,%%d1\n\t"		/* d1 = p2h, p2l, p3h, p3l */
#if 1 /* skip unchanged */
		"move.l (%2)+,%%d2\n\t"
		"move.l (%2)+,%%d3\n\t"
		"eor.l	%%d0,%%d2\n\t"
		"eor.l	%%d1,%%d3\n\t"
		"or.l	%%d2,%%d3\n\t"
		"bne.b	2f\n\t"
		"addq.l	#8,%1\n\t"
		"dbra	%%d4,1b\n\t"
		"2:\n\t"
		"move.l	%%d0,-8(%2)\n\t"
		"move.l %%d1,-4(%2)\n\t"
#endif
		"move.l	%%d0,%%d2\n\t"	/* d2 = p0h, p0l, p1h, p1l */
		"and.l 	%%d5,%%d0\n\t"	/* d0 = p0h,   0, p1h,   0 */
		"move.l	%%d1,%%d3\n\t"	/* d3 = p2h, p2l, p3h, p3l */
		"and.l	%%d5,%%d1\n\t"	/* d3 = p2h,   0, p3h,   0 */
		"lsr.l	#8,%%d1\n\t"	/* d1 =   0, p2h,   0, p3h */
		"or.l	%%d1,%%d0\n\t"	/* d0 = p0h, p2h, p1h, p3h */
		"move.l	%%d0,(%1)+\n\t"
		"and.l	%%d6,%%d2\n\t"	/* d2 =   0, p0l,   0, p1l */
		"and.l	%%d6,%%d3\n\t"	/* d3 =   0, p2l,   0, p3l */
		"lsl.l	#8,%%d2\n\t"	/* d2 = p0l,   0, p1l,   0 */
		"or.l	%%d3,%%d2\n\t"	/* d2 = p0l, p2l, p1l, p3l */
		"move.l %%d2,(%1)+\n\t"
		"dbra	%%d4,1b\n\t"
		:
		: "a"(scr_stlow), "a"(vram), "a"(scr_stlow_shadow)
		: "d0", "d1", "d2", "d3", "d4", "d5", "d6", "cc", "memory"
	);


#else	
	// assigned 16 color palette
    vga_SetPal(0, 16, pal_ega);

    int16_t len = ((320*200)/16)-1;
	__asm__ volatile
	(
        "1:\n\t"
        "   move.b  (%0)+,%%d0\n\t"     /* p0 _hi */
        "   move.b  (%0)+,%%d1\n\t"     /* p0_lo */
        "   lsl.l   #8,%%d0\n\t"
        "   lsl.l   #8,%%d1\n\t"
        "   move.b  (%0)+,%%d0\n\t"     /* p1_hi */
        "   move.b  (%0)+,%%d1\n\t"     /* p1_lo */
        "   lsl.l   #8,%%d0\n\t"
        "   lsl.l   #8,%%d1\n\t"
        "   move.b  (%0)+,%%d0\n\t"     /* p2_hi */
        "   move.b  (%0)+,%%d1\n\t"     /* p2_lo */
        "   lsl.l   #8,%%d0\n\t"
        "   lsl.l   #8,%%d1\n\t"
        "   move.b  (%0)+,%%d0\n\t"     /* p3_hi */
        "   move.b  (%0)+,%%d1\n\t"     /* p3_lo */
        "   move.l  %%d0,(%1)+\n\t"     /* write p0..3 hi */
        "   move.l  %%d1,(%1)+\n\t"     /* write p0..3 lo */
        "   dbra.w  %2,1b\n\r"
        :
	: "a"(scr_stlow), "a"(vram), "d"(len)
	: "d0", "d1", "cc", "memory"
	);
#endif

}


static void testMode_640x200x4bpp_planar_interleaved() {
    // todo: test optimal st-medium -> vga conversion
}


static void testMode_1280x720x8bpp_60hz() {

    // WDC 1024x768
    // assume card is physically jumpered for 70hz in this mode
    vga_SetMode(0x60);

    // WDC unlock WD90C30/31
    if (0) {
        vga_WritePort(0x3c4, 0x06);
        vga_WritePort(0x3c5, 0x48 | (vga_ReadPort(0x3c5) & 0xef));
    }
    
    // WDC PR5: unlock PR0-4
    vga_WriteReg(0x3ce, 0x0f, 0x05);

    // WDC PR3: unlock polarity and crtc
    vga_WritePort(0x3ce, 0x0d);
    vga_WritePort(0x3cf, vga_ReadPort(0x3cf) & 0x1c);    

    int16_t hto = 1664;
    int16_t hbs = 1280;
    int16_t hbe = 1280 + 272;
    int16_t hss = 1280 + 112;
    int16_t hse = 1280 + 112 + 40;

    int16_t vto = 750 - 2;
    int16_t vde = 720 - 1;
    int16_t vbs = 720;
    int16_t vss = 723;

    uint8_t ofl = 
        (((vto & (1 << 8)) ? 1 : 0) << 0) |
        (((vde & (1 << 8)) ? 1 : 0) << 1) |
        (((vss & (1 << 8)) ? 1 : 0) << 2) |
        (((vbs & (1 << 8)) ? 1 : 0) << 3) |
        (1 << 4) |
        (((vto & (1 << 9)) ? 1 : 0) << 5) |
        (((vde & (1 << 9)) ? 1 : 0) << 6) |
        (((vss & (1 << 9)) ? 1 : 0) << 7);


    // vsync+, hsync+
    vga_WritePort(0x3c2, vga_ReadPort(0x3cc) & 0x3f);

    // unlock crtc
    vga_WriteReg(0x3d4, 0x11, 0x05);

    // horizontal
    vga_WriteReg(0x3d4, 0x00, hto / 8);                     // htotal
    vga_WriteReg(0x3d4, 0x01, (hbs / 8) - 1);               // hdisp end
    vga_WriteReg(0x3d4, 0x02, (hbs / 8));                   // hblank start
    vga_WriteReg(0x3d4, 0x03, ((hbe / 8) & 0x1f) | 0x80);   // hblank end
    vga_WriteReg(0x3d4, 0x04, hss / 8);                     // hsync start
    vga_WriteReg(0x3d4, 0x05, (hse / 8) & 0x1f);            // hsync end

    // vertical
    vga_WriteReg(0x3d4, 0x06, vto & 0xff);                  // vtotal
    vga_WriteReg(0x3d4, 0x07, ofl);                         // overflow
    vga_WriteReg(0x3d4, 0x10, vss & 0xff);                  // vsync start
    vga_WriteReg(0x3d4, 0x12, vde & 0xff);                  // vdisp end
    vga_WriteReg(0x3d4, 0x15, vbs & 0xff);                  // vblank start
    vga_WriteReg(0x3d4, 0x16, (vto - 1) & 0xff);            // vblank end

    // relock crtc
    vga_WritePort(0x3d4, 0x11);
    vga_WritePort(0x3d5, vga_ReadPort(0x3d5) | 0x80);

    // WDC PR3: relock polarity and crtc
    vga_WritePort(0x3ce, 0x0d);
    vga_WritePort(0x3cf, vga_ReadPort(0x3cf) | 0xe3);

    // WDC PR5: relock PR0-4
    vga_WriteReg(0x3ce, 0x0f, 0x00);
}


void main() {
    rv = raven();
    printf("RAVEN:   %x\n", rv);

    vga_Init();
    vga_membase = ISA_MEMBASE8 + VGAMEM_BASE;
    vga_iobase  = ISA_IOBASE8;

    printf("VGA_IO:  %x\n", vga_iobase);
    printf("VGA_MEM: %x\n", vga_membase);

#if 0
    testMode_320x200x8bpp_chunky();
#elif 0
    testMode_320x200x8bpp_chunky_60hz();
#elif 0
    testMode_320x200x8bpp_chunky_50hz();
#elif 0
    testMode_1280x720x8bpp_60hz();
#else
    testMode_320x200x4bpp_planar_interleaved();
#endif    
}
