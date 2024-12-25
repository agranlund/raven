#include "sys.h"
#include "lib.h"
#include "hw/cpu.h"
#include "hw/vga.h"
#include "x86/x86emu.h"

#define ISA_IOBASE RV_PADDR_ISA_IO16
#define ISA_MEMBASE RV_PADDR_ISA_RAM16

#define ISA_IOBASE8 RV_PADDR_ISA_IO
#define ISA_MEMBASE8 RV_PADDR_ISA_RAM

#define VGABIOS_BASE 0xC0000UL
#define VGAMEM_BASE 0xA0000UL

#define X86_EMU_ADDR 0x00800000UL
#define X86_EMU_SIZE 0x00100000UL

static bool vgaBiosInitied = false;
static struct X86EMU x86emu_static;
struct X86EMU *x86emu = 0;

uint32_t vga_Addr() {
    return RV_PADDR_ISA_RAM + VGAMEM_BASE;
}


uint8_t vga_ReadPort(uint16_t port)
{
    return *((volatile uint8_t *)(ISA_IOBASE8 + port));
}


void vga_WritePort(uint16_t port, uint8_t val)
{
    *((volatile uint8_t *)(ISA_IOBASE8 + port)) = val;
}


void vga_WriteReg(uint16_t port, uint8_t idx, uint8_t val)
{
    if (port == 0x3C0)
    {
        (void)vga_ReadPort(0x3DA);
        vga_WritePort(port + 0, idx);
        vga_WritePort(port + 0, val);
    }
    else
    {
        vga_WritePort(port + 0, idx);
        vga_WritePort(port + 1, val);
    }
}


void vga_WaitVbl()
{
    do { nop(); } while (vga_ReadPort(0x3DA) & 8);
    do { nop(); } while (!(vga_ReadPort(0x3DA) & 8));
}


bool vga_RunBios()
{
    uint8_t *rom_src = (uint8_t *)(ISA_MEMBASE8 + VGABIOS_BASE);
    uint8_t *rom_dst = (uint8_t *)(x86emu->mem_base + VGABIOS_BASE);
    if (rom_src[0] != 0x55 || rom_src[1] != 0xaa)
    {
        printf(" No VGA bios detected\n");
        return false;
    }

    uint32_t rom_size = 512 * (uint32_t)rom_src[2];
    printf(" %dKb VGA Bios at %08x\n", rom_size / 1024, rom_src);
    memcpy(rom_dst, rom_src, rom_size);

    x86emu->x86.R_AX = 0xff;
    x86emu->x86.R_DX = 0x80;
    x86emu->x86.R_IP = x86_Offset(VGABIOS_BASE + 3);
    x86emu->x86.R_CS = x86_Segment(VGABIOS_BASE + 3);
    x86emu->x86.R_SS = 0x0000;
    x86emu->x86.R_SP = 0xfffe;
    x86emu->x86.R_DS = 0x0040;
    x86emu->x86.R_ES = 0x0000;

    // code after returning from vgabios call
    x86_PushWord(x86emu, 0xf4f4);
    x86_PushWord(x86emu, 0xf4f4);
    x86_PushWord(x86emu, x86emu->x86.R_SS);
    x86_PushWord(x86emu, x86emu->x86.R_SP + 2);
    x86_Run(x86emu);
    return true;
}


void vga_SetMode(uint16_t mode)
{
    if (vgaBiosInitied)
    {
        x86emu->x86.R_SS = 0x0000;
        x86emu->x86.R_SP = 0xfffe;
        x86emu->x86.R_AH = 0x00;
        x86emu->x86.R_AL = (uint8_t)mode;
        x86_Int(x86emu, 0x10);
    }
}


void vga_Fill(uint8_t color)
{
    uint32_t c8 = (uint32_t)color;
    uint32_t c32 = (c8 << 24) | (c8 << 16) | (c8 << 8) | c8;
    uint32_t* ptr = (uint32_t*) (ISA_MEMBASE + VGAMEM_BASE);
    for (uint32_t i = 0; i < ((64 * 1024) / 4); i++) {
        *ptr++ = c32;
    }
}


void vga_Clear() {
    vga_Fill(0x00);
}


void vga_SetPal(uint32_t idx, uint32_t num, uint8_t *pal)
{
    const uint8_t pshift = 2;
    for (uint32_t i = 0; (i < num) && ((i + idx) < 256); i++)
    {
        vga_WritePort(0x3c8, (uint8_t)(idx + i));
        vga_WritePort(0x3c9, *pal++ >> pshift);
        vga_WritePort(0x3c9, *pal++ >> pshift);
        vga_WritePort(0x3c9, *pal++ >> pshift);
    }
}


void vga_GetPal(uint32_t idx, uint32_t num, uint8_t *pal)
{
    const uint8_t pshift = 2;
    for (uint32_t i = 0; (i < num) && ((i + idx) < 256); i++)
    {
        vga_WritePort(0x3c8, (uint8_t)(idx + i));
        *pal++ = vga_ReadPort(0x3c9) << pshift;
        *pal++ = vga_ReadPort(0x3c9) << pshift;
        *pal++ = vga_ReadPort(0x3c9) << pshift;
    }
}


void vga_SetColor(uint8_t idx, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t c[3] = { r, g, b };
    vga_SetPal(idx, 1, c);
}


uint32_t vga_Init()
{
    x86emu = &x86emu_static;
    x86_Create(x86emu, (void *)X86_EMU_ADDR, X86_EMU_SIZE, ISA_IOBASE, ISA_MEMBASE);

    cpu_CacheOn();
    vgaBiosInitied = vga_RunBios();
    cpu_CacheOff();

    if (!vgaBiosInitied)
    {
        x86emu = 0;
        return 0;
    }

#if 0
    // clear vga framebuffer
    vga_Clear();
#endif

    return 1;
}


void vga_Test()
{
    // 320x200x256
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
    volatile uint8_t *vram = (volatile uint8_t *)(ISA_MEMBASE + VGAMEM_BASE);
    for (int y = 0; y < 200; y++) {
        for (int x = 0; x < 320; x++) {
            *vram++ = y;
        }
    }

    // screen on
    vga_WritePort(0x3c6, 0xff);
}


void vga_Mode13h()
{
    vga_SetMode(0x13);
    vga_Clear();
}


void vga_Atari()
{
    // 640x480x16
    vga_SetMode(0x12);

    // screen off
    vga_WritePort(0x3c6, 0x00);

    // atari mono palette
    for (int i=0; i<256; i++) {
        vga_SetColor(i, 0x00, 0x00, 0x00);
    }
    vga_SetColor(1, 0xff, 0xff, 0xff);

    // clear all planes
    vga_WriteReg(0x3c4, 0x02, 0x0f);
    vga_Fill(0x00);

    // fill plane 0
    vga_WriteReg(0x3c4, 0x02, 0x01); // map-mask plane 0
    vga_Fill(0xff);

    // use plane 1
    vga_WriteReg(0x3c4, 0x02, 0x02); // map-mask plane 1
    vga_WriteReg(0x3ce, 0x04, 0x01); // read-map plane 1

    // screen on
    vga_WritePort(0x3c6, 0xff);
}
