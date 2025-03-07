#include "sys.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "hw/mfp.h"
#include "hw/vga.h"
#include "atari.h"
#include "monitor.h"
#include "config.h"

#define DEBUG_MMU               0
#define RESERVED_SIZE			(4UL * (1024 * 1024))
#define ACIA_EMULATION          0
#define ENABLE_CART_TEST        0
#define DELAY_BOOT              0

extern uint32_t ksimm[4];

extern void vecRTE_MFP1I0();
extern void vecRTE_MFP1I1();
extern void vecRTE_MFP1I2();
extern void vecRTE_MFP1I3();
extern void vecRTE_MFP1I4();
extern void vecRTE_MFP1I5();
extern void vecRTE_MFP1I6();
extern void vecRTE_MFP1I7();
extern void vecVBL_MPF2TC();    // vblank emulation
extern void vecKBD_MFP1I4();    // acia emulation

uint32_t vecKBD_Busy;

const char * const gfxNames[] = {
    "(none)",
    "ET4000AX",
    "ET4000/W32",
    "ATI Mach32",
    "ATI Mach64"
};


static void mmu_Map24bit(uint32_t log, uint32_t phys, uint32_t size, uint32_t flags) {
    mmu_Map(log & 0x00ffffff, phys, size, flags);
    mmu_Map(log | 0xff000000, phys, size, flags);
}
static void mmu_Invalid24bit(uint32_t log, uint32_t size) {
    mmu_Invalid(log & 0x00ffffff, size);
    mmu_Invalid(log | 0xff000000, size);
}

/*
static void mmu_Redirect24bit(uint32_t logsrc, uint32_t logdst, uint32_t size) {
    mmu_Redirect(logsrc & 0x00ffffff, logdst, size);
    mmu_Redirect(logsrc | 0xff000000, logdst, size);
}
static void mmu_Ignore24bit(uint32_t log, uint32_t size) {
    mmu_Map24bit(log, 0x21000000, size, PMMU_READWRITE | PMMU_CM_PRECISE);
}
*/


bool atari_InitEMU()
{
    //---------------------------------------------------------------------
    // acia emulation
    //---------------------------------------------------------------------
    vecKBD_Busy = 0;
    volatile uint8_t* acia = (volatile uint8_t*)RV_PADDR_ACIA;
    for (int i=0; i<8; i++) {
        acia[i] = 0;
    }
    acia[ACIA_KEYB_CTRL] = 0x02;    // tx reg empty
    acia[ACIA_MIDI_CTRL] = 0x02;    // tx reg empty

    //---------------------------------------------------------------------
    // vbl emulation - MFP2:TimerC @ 50hz
    //---------------------------------------------------------------------
    volatile uint8_t* mfp2 = (volatile uint8_t*)RV_PADDR_MFP2;
    mfp2[MFP_TCDR]   = 200;                         // count 200     => 10000hz
    mfp2[MFP_TCDCR] &= 0x07;
    mfp2[MFP_TCDCR] |= 0x70;                        // divide by 200 => 50hz
    mfp2[MFP_IERB]  |= (1 << 5);                    // enable timerC interrupt
    mfp2[MFP_IMRB]  |= (1 << 5);                    // enable timerC interrupt
    mfp2[MFP_ISRB] &= ~(1 << 5);                    // clear timerC in service
    return true;
}


bool atari_InitVBR()
{
    vbr_Set(0x10C,  (uint32_t) vecRTE_MFP1I3);        // MFP1I3 - I2C line    - IGNORE  (ST = blitter)
    vbr_Set(0x13C,  (uint32_t) vecRTE_MFP1I7);        // MFP1I7 - I2C line    - IGNORE  (ST = mono detect)

#if ACIA_EMULATION
    vbr_Set(0x74,   (uint32_t) vecRTE);               // IRQ5   - Eiffel      - IGNORE
    vbr_Set(0x118,  (uint32_t) vecKBD_MFP1I4);        // MFP1I4 - Eiffel      - Emulate (ST = acia)
#else
    vbr_Set(0x118,  (uint32_t) vecRTE_MFP1I4);        // MFP1I4 - Eiffel      - IGNORE  (ST = acia)
#endif

    //vbr_Set(0x11C,  (uint32_t) vecRTE_MFP1I5);        // MFP1I5 - Fdd/Hdd     - IGNORE (todo: can use it, connected to IDE)

    vbr_Set(0x154,  (uint32_t) vecVBL_MPF2TC);        // MFP2 TimerC -> IRQ4 VBLANK emulation
    vbr_Apply();
    return true;
}


bool atari_InitMMU(uint32_t* simms)
{
    // direct unmapped pages to an ignored address and set up bus-errors manually
    uint32_t* mmuTable = mmu_Init(0x21000000 | PMMU_READWRITE | PMMU_CM_PRECISE);

    // reserved area
    extern uint32_t __bss_start;
    uint32_t reserved_start = ((uint32_t)&__bss_start) & 0xfff00000;
    uint32_t reserved_end = reserved_start + RESERVED_SIZE;

    // todo: get from battery backed config
    uint32_t stram_size = cfg_GetValue(cfg_Find("st_ram_size")) * 1024 * 1024UL;
    uint32_t stram_cache = (cfg_GetValue(cfg_Find("st_ram_cache")) & 0x03) << 5;
    uint32_t ttram_cache = (cfg_GetValue(cfg_Find("tt_ram_cache")) & 0x03) << 5;

    // identify gfx
    uint8_t id_gfx = 1; // assume ET4000
    if ((IOB(RV_PADDR_ISA_RAM16, 0xC0032)) == '6' && (IOB(RV_PADDR_ISA_RAM16, 0xC0034) == '2')) {
        IOB(RV_PADDR_ISA_IO16, 0x56EE) = 0x55;
        if (IOB(RV_PADDR_ISA_IO16, 0x56EE) == 0x55) {
            id_gfx = 3; // todo: further detect Mach32 vs Mach64
        }
    }
    //fmt("GFX:  %s\n", gfxNames[id_gfx]);

    // auto select st-ram size based on total available ram
    if (stram_size == 0) {
        uint32_t total_mb = (simms[0] + simms[1] + simms[2]) / (1024 * 1024UL);
        if (total_mb >= 48) {             // 48mb+ : 4mb
            stram_size = 4 * 1024 * 1024UL;
        } else if (total_mb >= 32) {      // 32mb+ : 3mb
            stram_size = 3 * 1024 * 1024UL;
        } else if (total_mb >= 16) {      // 16mb+ : 2mb
            stram_size = 2 * 1024 * 1024UL;
        } else {                            //  8mb+ : 1mb
            stram_size = 1 * 1024 * 1024UL;
        }
    }

    // clamp to maximum allowed stram
    uint32_t stram_max = reserved_start - 0x00100000;
    stram_size = (stram_size < stram_max) ? stram_size : stram_max;

    #if DEBUG_MMU
    fmt("rv_phys: %l -> %l\n", reserved_start, reserved_end-1);
    fmt("st_phys: %l -> %l\n", 0, stram_size-1);
    #endif

    // map available physical memory
    uint32_t lmem = 0x00000000;
    for (uint32_t i=0; i<3; i++)
    {
        uint32_t pmem = i << 24;
        for (uint32_t j=0; j<(simms[i] >> 20); j++)
        {
			if (pmem < reserved_start || pmem >= reserved_end)
            {
                #if DEBUG_MMU
                fmt("map %l -> %l\n", lmem, pmem);
                #endif

				if (lmem == 0) {
					// system vectors + variables
	                mmu_Map24bit(0x00000000, 0x00000000, 0x00002000, PMMU_READWRITE | PMMU_CM_PRECISE);
                    // st-ram
	                mmu_Map24bit(0x00002000, 0x00002000, 0x000FE000, PMMU_READWRITE | stram_cache);
				}
				else if (lmem >= 0x01000000) {
					// tt-ram
                    mmu_Map(lmem, pmem, 0x00100000, PMMU_READWRITE | ttram_cache);
				}
				else {
					// st-ram
                    mmu_Map24bit(lmem, pmem, 0x00100000, PMMU_READWRITE | stram_cache);
				}
                lmem += 0x00100000;
            }
            if (lmem == stram_size) {
                lmem = 0x01000000;
            }
            pmem += 0x00100000;
        }
    }

    // bus error on MB block after tt-ram
    mmu_Invalid(lmem, 0x00100000);

    // bus error on MB block after st-ram
    mmu_Invalid24bit(stram_size, 0x00100000);

    // map internally reserved ram
	for (uint32_t addr = reserved_start; addr < reserved_end; addr += 0x00100000) {
	    mmu_Map(addr, addr, 0x00100000, PMMU_READWRITE | PMMU_CM_WRITETHROUGH);
	}

    // peripheral access flags
    mmu_Map(0x40000000, 0x40000000,   simms[3], PMMU_READONLY  | PMMU_CM_WRITETHROUGH);    // ROM
    mmu_Map(0x20000000, 0x20000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // LOCBUS  (uart)
    mmu_Map(0xA0000000, 0xA0000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS  (ide, mfp2)
    mmu_Map(0xA1000000, 0xA1000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS  (ym, mfp1)
    mmu_Map(0xA2000000, 0xA2000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS  (unused)
    mmu_Map(0xA3000000, 0xA3000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS  (unused)
    mmu_Map(0x80000000, 0x80000000, 0x00400000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA-RAM (generic)
    mmu_Map(0x81000000, 0x81000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA-IO  (generic)
    mmu_Map(0x82000000, 0x82000000, 0x00400000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA-RAM (gfxcard)
    mmu_Map(0x83000000, 0x83000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA-IO  (gfxcard)

    // peripheral memory map

    // 0xE00000 : 512k system rom : 512kb at physical rom offset 256kb. repeated to fill entire 1MB
    mmu_Map24bit(0x00E00000, 0x40040000, 0x00080000, PMMU_READONLY  | PMMU_CM_WRITETHROUGH);
    mmu_Map24bit(0x00E80000, 0x40040000, 0x00080000, PMMU_READONLY  | PMMU_CM_WRITETHROUGH);

    // 0xF00000 : reserved io space
    mmu_Map24bit(0x00F00000, 0xA0000000, 0x00001000, PMMU_READWRITE | PMMU_CM_PRECISE);  // IDE ($f00000 -> $a0000000)

#if ENABLE_CART_TEST
    // 0xFA0000 : cart : 128kb at physical rom offset 768kb
    mmu_Map24bit(0x00FA0000, 0x400C0000, 0x00020000, PMMU_READONLY  | PMMU_CM_WRITETHROUGH);
#endif
    // 0xFC0000 : 192k system rom

    // 0xFF0000 : reserved io space

    // 0xFF8000 : standard io space, todo: add bus-errors
    mmu_Map24bit(0x00FF8000, 0xA1000000, 0x00001000, PMMU_READWRITE | PMMU_CM_PRECISE);                 // YM2149   ($ff8800 -> $a1000800)
    // ACIA : ffffc00 goes to ram0 reserved area  (by help of cpld)                                     // ACIA     ($fffc00 -> BIOS_EMU_MEM)
    // MFP1 : ffffx00 goes to 0xA1xxxxxx as usual (emu address bits ignored)
    mmu_Map24bit(0x00FFF000, 0xA1000000 + RV_ACIAEMU_BASE, 0x00001000, PMMU_READWRITE | PMMU_CM_PRECISE);  // MFP1     ($fffa00 -> $a1000a00)


    // special case graphics card access to satisfy existing Atari drivers
	if (id_gfx == 3) {
		// Mach32
	    mmu_Redirect(0xFE900000, 0x83000000, 0x00100000);      // TT Nova Mach32 reg base : 1024 kb
	    mmu_Redirect(0xFE800000, 0x82000000, 0x00100000);      // TT Nova Mach32 vga base :  128 kb
	    mmu_Redirect(0xFEA00000, 0x82200000, 0x00100000);      // TT Nova Mach32 mem base : 2048 kb ?? target addr ??
	    mmu_Redirect(0xFEB00000, 0x82300000, 0x00100000);      // TT Nova Mach32 mem base : 2048 kb ?? target addr ??

	}
	else if (id_gfx == 4) {
		// Mach64
	    mmu_Redirect(0xFEC00000, 0x83000000, 0x00080000);      // TT Nova Mach32 reg base :  512 kb
	    mmu_Redirect(0xFEC80000, 0x82000000, 0x00080000);      // TT Nova Mach32 vga base :  512 kb
	    mmu_Redirect(0xFE800000, 0x82000000, 0x00100000);      // TT Nova Mach32 mem base : 4096 kb ?? target addr ??
	    mmu_Redirect(0xFE900000, 0x82100000, 0x00100000);      // TT Nova Mach32 mem base : 4096 kb ?? target addr ??
	    mmu_Redirect(0xFEA00000, 0x82200000, 0x00100000);      // TT Nova Mach32 mem base : 4096 kb ?? target addr ??
	    mmu_Redirect(0xFEB00000, 0x82300000, 0x00100000);      // TT Nova Mach32 mem base : 4096 kb ?? target addr ??

	}
	else {
		// ET4000
	    mmu_Invalid(0xFE800000, 0x00100000);
	    mmu_Invalid(0xFE900000, 0x00100000);
	    mmu_Redirect(0xFED00000, 0x83000000, 0x00100000);      // TT Nova ET4k reg base : 1024 kb
	    mmu_Redirect(0xFEC00000, 0x82000000, 0x00100000);      // TT Nova ET4k mem base	: 1024 kb
	    mmu_Redirect(0x00D00000, 0x83000000, 0x00100000);      // ST Nova ET4k reg base : 1024 kb
	    mmu_Redirect(0x00C00000, 0x82000000, 0x00100000);      // ST Nova ET4k mem base : 1024 kb
	}

    // hades compatible ISA I/O to take advantage of existing drivers
	mmu_Redirect(0xFFF30000, 0x81000000, 0x00010000);

	// ST emulation space
    mmu_Redirect(0x41000000, reserved_start + 0x00100000, 0x00100000);		// ram	1024kb
    mmu_Redirect(0x41E00000, reserved_start + 0x00200000, 0x00040000);		// tos	 256kb
    mmu_Redirect(0x41FC0000, reserved_start + 0x00200000, 0x00030000);      // tos   192kb
    mmu_Redirect(0x41FA0000, reserved_start + 0x00240000, 0x00020000);		// cart  128kb
    mmu_Redirect(0x41FF0000, reserved_start + 0x00260000, 0x00010000);		// io	  64kb
    // x86 emulation space
    mmu_Redirect(0x42000000, reserved_start + 0x00300000, 0x000a0000);		// 640kb ram    : 00000
    mmu_Redirect(0x420a0000, reserved_start + 0x003a0000, 0x00020000);		// 128kb video  : A0000
    //mmu_Redirect(0x420a0000, 0x80000000     + 0x000a0000, 0x00020000);		// 128kb video  : A0000
    mmu_Redirect(0x420c0000, reserved_start + 0x003c0000, 0x00030000);		// 160kb ebios  : C0000
    //mmu_Redirect(0x420c0000, 0x80000000     + 0x000c0000, 0x00030000);		// 160kb ebios  : C0000
    mmu_Redirect(0x420f0000, reserved_start + 0x003f0000, 0x00010000);		// 160kb sbios  : F0000

/*
    for (uint32_t i = 0; i < 4 * 1024 * 1024UL; i += 64 * 1024) {
        mmu_Redirect(0x42400000UL + i, 0x820a0000 + i, 64 * 1024UL);
    }
*/

    mmuregs_t mmu;
    mmu.urp = mmuTable;
    mmu.srp = mmuTable;
    mmu.tcr = 0x8210;
    mmu.itt0 = 0;
    mmu.itt1 = 0;
    mmu.dtt0 = 0;
    mmu.dtt1 = 0;
    cpu_SetMMU(&mmu);
    return true;
}


bool atari_InitScreen()
{
    if (!vga_Init())
        return 0;

    vga_Atari();
    return true;
}

bool atari_Init()
{
    initprint("InitVbr");
    atari_InitVBR();

    initprint("InitMmu");
    atari_InitMMU(ksimm);

    initprint("InitEmu");
    atari_InitEMU();

    initprint("InitTos");
	for (int i=0x400; i<0x700; i+=4) {
		IOL(0, i) = 0;
	}

    if (!atari_DetectTos()) {
        puts("No TOS detected");
        return true;
    }

    if (cfg_GetValue(cfg_Find("boot_enable")) == 0) {
        puts("TOS boot disabled");
        return true;
    }

    uint32_t bootdelay = cfg_GetValue(cfg_Find("boot_delay"));
    if (bootdelay > 0) {
        puts("\nHit any key to cancel EmuTOS auto-boot...");
        while (bootdelay > 0)
        {
            fmt("%d...\n", bootdelay);
            for (volatile int i = 0x80000; i; i--) {
                if (uart_recvChar() != -1) {
                    return true;
                }
            }
            bootdelay--;
        }
    }

    initprint("InitVga");
    atari_InitScreen();

    puts("Starting TOS");
    cpu_Call(0xe00000);
    return false;
}


bool atari_DetectTos()
{
    extern const uint32_t __etos_signature;

    return __etos_signature == 0x45544f53;  // "ETOS"
}
