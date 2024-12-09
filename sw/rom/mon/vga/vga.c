#include "../sys.h"
#include "../lib.h"
#include "../hw/cpu.h"
#include "vga.h"
#include "x86emu.h"

#define ISA_IOBASE      PADDR_ISA_IO
#define ISA_MEMBASE     PADDR_ISA_RAM

#define VGABIOS_BASE    0xC0000UL
#define VGAMEM_BASE     0xA0000UL

#define X86_EMU_ADDR    0x00800000UL
#define X86_EMU_SIZE    0x00100000UL

static bool vgaBiosInitied = false;

bool vga_RunBios()
{
    struct X86EMU* emu = x86_Get();
    uint8_t* rom_src = (uint8_t*) (ISA_MEMBASE + VGABIOS_BASE);
    uint8_t* rom_dst = (uint8_t*) (emu->mem_base + VGABIOS_BASE);
    if (rom_src[0] != 0x55 || rom_src[1] != 0xaa) {
        printf(" No VGA bios detected\n");
        return false;
    }

    uint32_t rom_size = 512 * (uint32_t)rom_src[2];
    printf(" %dKb VGA Bios at %08x\n", rom_size / 1024, rom_src);
    memcpy(rom_dst, rom_src, rom_size);

    emu->x86.R_AX = 0xff;
    emu->x86.R_DX = 0x80;
    emu->x86.R_IP = (VGABIOS_BASE + 3) & 0xffff;;
    emu->x86.R_CS = (VGABIOS_BASE & 0xf0000) >> 4;;
    emu->x86.R_SS = 0x0000;
    emu->x86.R_SP = 0xfffe;
    emu->x86.R_DS = 0x0040;
    emu->x86.R_ES = 0x0000;

    // code after returning from vgabios call
    x86_PushWord(emu, 0xf4f4);
    x86_PushWord(emu, 0xf4f4);
    x86_PushWord(emu, emu->x86.R_SS);
    x86_PushWord(emu, emu->x86.R_SP + 2);
    X86EMU_exec(emu);

    return true;
}

void vga_SetMode(uint16_t mode) {
    if (vgaBiosInitied) {
        struct X86EMU* emu = x86_Get();
        emu->x86.R_SS = 0x0000;
        emu->x86.R_SP = 0xfffe;
        emu->x86.R_AH = 0x00;
        emu->x86.R_AL = (uint8_t) mode;
        X86EMU_exec_intr(emu, 0x10);
    }
}

void vga_Clear() {
    // todo: make sure mapmask is set for all layers
    for (int i=0; i<(128 * 1024); i++) {
        *((volatile uint8_t*)(ISA_MEMBASE+VGAMEM_BASE+i)) = 0;
    }
}

bool vga_Init()
{
    x86_Init((void*)X86_EMU_ADDR, X86_EMU_SIZE, PADDR_ISA_IO, PADDR_ISA_RAM);

    cpu_CacheOn();
    vgaBiosInitied = vga_RunBios();
    cpu_CacheOff();

    if (!vgaBiosInitied) {
        return false;
    }

#if 0
    // clear vga framebuffer
    vga_Clear();
#endif

    return true;
}


uint8_t vga_ReadPort(uint16_t port) {
    return *((volatile uint8_t*)(PADDR_ISA_IO + port));
}


void vga_WritePort(uint16_t port, uint8_t val) {
    *((volatile uint8_t*)(PADDR_ISA_IO + port)) = val;
}


void vga_WriteReg(uint16_t port, uint8_t idx, uint8_t val) {
   if (port==0x3C0) {
        (void) vga_ReadPort(0x3DA);
        vga_WritePort(port + 0, idx);
        vga_WritePort(port + 0, val);
   }
   else {
        vga_WritePort(port + 0, idx);
        vga_WritePort(port + 1, val);
   }
}


void vga_Test()
{
    // 320x200x256
    vga_SetMode(0x13);
  
    // screen off
    vga_WritePort(0x3c6, 0x00);

    // palette
    vga_WritePort(0x3c8, 0x00);
    for (int i=0; i<64; i++) { vga_WritePort(0x3c9,   i); vga_WritePort(0x3c9,   0); vga_WritePort(0x3c9,   0); }
    for (int i=0; i<64; i++) { vga_WritePort(0x3c9,   0); vga_WritePort(0x3c9,   i); vga_WritePort(0x3c9,   0); }
    for (int i=0; i<64; i++) { vga_WritePort(0x3c9,   0); vga_WritePort(0x3c9,   0); vga_WritePort(0x3c9,   i); }
    for (int i=0; i< 8; i++) { vga_WritePort(0x3c9, 8*i); vga_WritePort(0x3c9, 8*i); vga_WritePort(0x3c9, 8*i); }

    // test image
    volatile uint8_t* vram = (volatile uint8_t*)(PADDR_ISA_RAM + VGAMEM_BASE);
    for (int y=0; y<200; y++) {
        for (int x=0; x<320; x++) {
            *vram++ = y;
        }
    }

    // screen on
    vga_WritePort(0x3c6, 0xff);
}


void vga_Atari()
{
    // 640x480x16
    vga_SetMode(0x12);

    // screen off
    vga_WritePort(0x3c6, 0x00);

    // clear all planes
    vga_WriteReg(0x3c4, 0x02, 0x0f);
    volatile uint8_t* vram = (volatile uint8_t*)(PADDR_ISA_RAM + VGAMEM_BASE);
    for (int i=0; i<(64 * 1024); i++) { vram[i] = 0x00; }

    // fill plane 0 + 1
    vga_WriteReg(0x3c4, 0x02, 0x03);    // map-mask plane 0
    for (int i=0; i<(64 * 1024); i++) { vram[i] = 0xff; }

    // use plane 1
    vga_WriteReg(0x3c4, 0x02, 0x02);    // map-mask plane 1
    vga_WriteReg(0x3ce, 0x04, 0x01);    // read-map plane 1

    // set up atari palette
    vga_WritePort(0x3c8, 1);            // vga 1 = white = atari 0
    vga_WritePort(0x3c9, 0xff);
    vga_WritePort(0x3c9, 0xff);
    vga_WritePort(0x3c9, 0xff);
    vga_WritePort(0x3c8, 3);            // vga 3 = black = atari 1
    vga_WritePort(0x3c9, 0x00);
    vga_WritePort(0x3c9, 0x00);
    vga_WritePort(0x3c9, 0x00);

    // screen on
    vga_WritePort(0x3c6, 0xff);
}
