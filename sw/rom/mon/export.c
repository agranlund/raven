#include "sys.h"
#include "raven.h"
#include "config.h"
#include "monitor.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "hw/ikbd.h"
#include "hw/midi.h"
#include "hw/i2c.h"
#include "hw/rtc.h"
#include "hw/vga.h"
#include "hw/flash.h"
#include "x86/x86emu.h"

extern struct X86EMU* x86emu;

void b_dbg_GPO(uint32_t num, uint32_t enable) {
    switch (num)
    {
        case 0:     // uart1:powerled
        case 1:     // uart1:TP301
            ikbd_GPO(num, (enable == 0) ? false : true);
            break;
    }
}

uint32_t b_dbg_GPI(uint32_t num) {
    return ikbd_GPI((uint8_t)num) ? 1UL : 0UL;
}

static void b_rtc_Read(uint32_t addr, uint8_t* buf, uint32_t siz) { rtc_Read((uint8_t)addr, buf, (uint8_t)siz); }
static void b_rtc_Write(uint32_t addr, uint8_t* buf, uint32_t siz) { rtc_Write((uint8_t)addr, buf, (uint8_t)siz); }

static uint32_t b_cfg_Read(const char* cfg) { return (uint32_t) cfg_GetValue(cfg_Find(cfg)); }
static void b_cfg_Write(const char* cfg, uint32_t val) { cfg_SetValue(cfg_Find(cfg), val); }

static int32_t b_i2c_Aquire() { return (int32_t) i2c_Aquire(); }
static void b_i2c_Release() { i2c_Release(); }
static void b_i2c_Start() { i2c_Start(); }
static void b_i2c_Stop() { i2c_Stop(); }
static uint32_t b_i2c_Read(uint32_t ack) { return (uint32_t) i2c_Read((uint8_t)ack); }
static uint32_t b_i2c_Write(uint32_t val) { return (uint32_t) i2c_Write((uint8_t)val); }

static uint32_t b_flash_Identify(void) { return flash_Identify(); }
static uint32_t b_flash_Program(void* data, uint32_t size) { return flash_Program(data, size) ? 1 : 0; }

static void b_vga_SetMode(uint32_t mode) { vga_SetMode((uint16_t)mode); }

static uint32_t b_int86x(uint32_t no, x86_regs_t* regs_in, x86_regs_t* regs_out, x86_sregs_t* sregs) {

    /* set sregs */
    if (sregs) {
        x86emu->x86.R_ES = sregs->es;
        x86emu->x86.R_DS = sregs->ds;
        x86emu->x86.R_SS = sregs->ss;
        x86emu->x86.R_CS = sregs->cs;
    } else {
        /* documentation claims suitable defaults would be used so just leave it */
    }

    /* set regs */
    x86emu->x86.R_AX = regs_in->x.ax;
    x86emu->x86.R_BX = regs_in->x.bx;
    x86emu->x86.R_CX = regs_in->x.cx;
    x86emu->x86.R_DX = regs_in->x.dx;
    x86emu->x86.R_SI = regs_in->x.si;
    x86emu->x86.R_DI = regs_in->x.di;
    x86emu->x86.R_FLG = regs_in->x.cflag;

    /* run emulator */
    x86emu->x86.R_SP = 0xfffe;
    x86_Int(x86emu, (uint8_t)no);

    /* update sregs_out. sregs should remain unmodified */
    if (regs_out) {
        regs_out->x.ax = x86emu->x86.R_AX;
        regs_out->x.bx = x86emu->x86.R_BX;
        regs_out->x.cx = x86emu->x86.R_CX;
        regs_out->x.dx = x86emu->x86.R_DX;
        regs_out->x.si = x86emu->x86.R_SI;
        regs_out->x.di = x86emu->x86.R_DI;
        regs_out->x.cflag = x86emu->x86.R_FLG;
    }

    /* return AX*/
    return (uint32_t)x86emu->x86.R_AX;
}

extern uint8_t __toc_start;
extern uint8_t __config_start;

const raven_t ravenBios __attribute__((section(".export"))) =
{
//0x0000
    C_RAVN,                     // magic
    VERSION,                    // rom version
    REV,
    {0,0},
    (rvcfg_t*)&__toc_start,
    (rvtoc_t*)&__config_start,
    sys_Chipset,                  // chipset info
//0x0020
    b_dbg_GPI,
    b_dbg_GPO,
    {0,0,0,0,0,0},
//0x0040
    b_rtc_Read,
    b_rtc_Write,
    {0,0},
    b_cfg_Read,
    b_cfg_Write,
    {0,0},
//0x0060
    b_i2c_Aquire,
    b_i2c_Release,
    b_i2c_Start,
    b_i2c_Stop,
    b_i2c_Read,
    b_i2c_Write,
    {0,0},
//0x0080
    0,
    vga_Init,
    vga_Clear,
    vga_Addr,
    vga_Atari,
    b_vga_SetMode,
    vga_SetPal,
    vga_GetPal,
//0x00A0
    cpu_CacheOn,
    cpu_CacheOff,
    cpu_CacheFlush,
    mmu_Map,
    mmu_Redirect,
    mmu_Invalid,
    mmu_Flush,
    mmu_GetPageDescriptor,
//0x00C0
    mon_Exec,
    b_flash_Identify,
    b_flash_Program,
    {0,0,0},
    &getchar,
    &putchar,
// 0x00D0
    b_int86x,
    {0,0,0,0,0,0,0}
};
