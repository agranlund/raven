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

    // start from standard 320x200x4bpp EGA mode 0Dh
    vga_SetMode(0x0d);

    // make planes interleaved
    //  0 = pixels 0..7,  plane 0
    //  1 = pixels 0..7,  plane 1
    //  2 = pixels 0..7,  plane 2
    //  3 = pixels 0..7,  plane 3
    //  4 = pixels 8..15, plane 0
    //  etc..
    vga_WriteReg(0x3c4, 0x02, 0xf);     // plane write mask
    vga_WriteReg(0x3c4, 0x04, 0x08);    // chain-4 enable
    vga_WriteReg(0x3d4, 0x14, (1<<6));  // 32bit addressing

    // assigned 16 color palette
    vga_SetPal(0, 16, pal_ega);

    scr_stlow[0] = 0xffff;
    scr_stlow[1] = 0xffff;
    scr_stlow[4] = 0xffff;

    volatile uint8_t* vram = (volatile uint8_t*)vga_membase;

    // convert from st-low

    // src = planar 16bit interleaved
    // aaaaaaaabbbbbbbb ccccccccdddddddd eeeeeeeeffffffff gggggggghhhhhhhh

    // dst = planar 8bit interleaved
    // aaaaaaaa cccccccc eeeeeeee gggggggg bbbbbbbb dddddddd ffffffff hhhhhhhh

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
}


static void testMode_640x200x4bpp_planar_interleaved() {
    // todo: test optimal st-medium -> vga conversion
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
#elif 1
    testMode_320x200x8bpp_chunky_60hz();
#elif 0
    testMode_320x200x8bpp_chunky_50hz();
#else
    testMode_320x200x4bpp_planar_interleaved();
#endif    
}
