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

/* ---------------------------------------------------------------------------- */
/* vga helpers                                                                  */
/* ---------------------------------------------------------------------------- */

uint8_t vga_ReadPort(uint16_t port) {
    return *((volatile uint8_t *)(vga_iobase + port));
}

void vga_WritePort(uint16_t port, uint8_t val) {
    *((volatile uint8_t *)(vga_iobase + port)) = val;
}

void vga_WriteReg(uint16_t port, uint8_t idx, uint8_t val)
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

void vga_Fill(uint8_t color)
{
    uint32_t c8 = (uint32_t)color;
    uint32_t c32 = (c8 << 24) | (c8 << 16) | (c8 << 8) | c8;
    uint32_t* ptr = (uint32_t*) vga_membase;
    for (uint32_t i = 0; i < ((64 * 1024) / 4); i++) {
        *ptr++ = c32;
    }
}

void vga_Clear() {
    vga_Fill(0x00);
}


void vga_SetPal(uint32_t idx, uint32_t num, uint8_t *pal) {
    const uint8_t pshift = 2;
    for (uint32_t i = 0; (i < num) && ((i + idx) < 256); i++) {
        vga_WritePort(0x3c8, (uint8_t)(idx + i));
        vga_WritePort(0x3c9, *pal++ >> pshift);
        vga_WritePort(0x3c9, *pal++ >> pshift);
        vga_WritePort(0x3c9, *pal++ >> pshift);
    }
}


void vga_GetPal(uint32_t idx, uint32_t num, uint8_t *pal) {
    const uint8_t pshift = 2;
    for (uint32_t i = 0; (i < num) && ((i + idx) < 256); i++) {
        vga_WritePort(0x3c8, (uint8_t)(idx + i));
        *pal++ = vga_ReadPort(0x3c9) << pshift;
        *pal++ = vga_ReadPort(0x3c9) << pshift;
        *pal++ = vga_ReadPort(0x3c9) << pshift;
    }
}

void vga_SetColor(uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t c[3] = { r, g, b };
    vga_SetPal(idx, 1, c);
}

uint32_t vga_Init(void) {
    return rv->vga_Init();
}

void vga_SetMode(uint32_t mode) {
    rv->vga_SetMode(mode);
}

uint32_t vga_Addr(void) {
    return rv->vga_Addr();
}


/* ---------------------------------------------------------------------------- */
/* vga helpers                                                                  */
/* ---------------------------------------------------------------------------- */

void main() {
    rv = raven();
    printf("RAVEN:   %x\n", rv);

    vga_Init();
    vga_membase = ISA_MEMBASE8 + VGAMEM_BASE;
    vga_iobase  = ISA_IOBASE8;

    printf("VGA_IO:  %x\n", vga_iobase);
    printf("VGA_MEM: %x\n", vga_membase);

    vga_SetMode(0x13);

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

    // screen on
    vga_WritePort(0x3c6, 0xff);
}
