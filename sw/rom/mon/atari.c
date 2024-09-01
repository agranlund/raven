#include "sys.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "hw/mfp.h"
#include "atari.h"
#include "monitor.h"

#define RESERVED_SIZE			(3 * (1024 * 1024))
#define COPYBACK_STRAM          0
#define COPYBACK_TTRAM          0
#define ACIA_EMULATION          0

extern uint8_t kgfx;
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


bool atari_InitEMU()
{
    //---------------------------------------------------------------------
    // acia emulation
    //---------------------------------------------------------------------
    vecKBD_Busy = 0;
    volatile uint8_t* acia = (volatile uint8_t*)PADDR_ACIA;
    for (int i=0; i<8; i++) {
        acia[i] = 0;
    }
    acia[ACIA_KEYB_CTRL] = 0x02;    // tx reg empty
    acia[ACIA_MIDI_CTRL] = 0x02;    // tx reg empty

    //---------------------------------------------------------------------
    // vbl emulation - MFP2:TimerC @ 50hz
    //---------------------------------------------------------------------
    volatile uint8_t* mfp2 = (volatile uint8_t*)PADDR_MFP2;
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
    uint32_t* mmuTable = mmu_Init();

    extern uint32_t _bss_start;
    uint32_t stram_size = ((uint32_t)&_bss_start) & 0xfff00000;
    uint32_t reserved_start = stram_size;
    uint32_t reserved_end = reserved_start + RESERVED_SIZE;

    uint32_t lmem = 0x00000000;
    for (uint32_t i=0; i<3; i++)
    {
        uint32_t pmem = i << 24;
        for (uint32_t j=0; j<(simms[i] >> 20); j++)
        {
			if (pmem < reserved_start || pmem >= reserved_end)
            {
				if (lmem == 0) {
					// system vectors + variables
	                mmu_Map(0x00000000, 0x00000000, 0x00002000, PMMU_READWRITE | PMMU_CM_PRECISE);
	                mmu_Map(0x00002000, 0x00002000, 0x000FE000, PMMU_READWRITE | PMMU_CM_WRITETHROUGH);
				}
				else if (lmem >= 0x01000000) {
					// tt-ram
                    #if COPYBACK_TTRAM
	            	    mmu_Map(lmem, pmem, 0x00100000, PMMU_READWRITE | PMMU_CM_COPYBACK);
                    #else
	            	    mmu_Map(lmem, pmem, 0x00100000, PMMU_READWRITE | PMMU_CM_WRITETHROUGH);
                    #endif

				}
				else {
					// st-ram
                    #if COPYBACK_STRAM
	                    mmu_Map(lmem, pmem, 0x00100000, PMMU_READWRITE | PMMU_CM_COPYBACK);
                    #else
	                    mmu_Map(lmem, pmem, 0x00100000, PMMU_READWRITE | PMMU_CM_WRITETHROUGH);
                    #endif
				}
                lmem += 0x00100000;
            }
            if (lmem == stram_size) {
                lmem = 0x01000000;
            }
            pmem += 0x00100000;
        }
    }

    // internally reserved ram
	for (uint32_t addr = reserved_start; addr < reserved_end; addr += 0x00100000) {
	    mmu_Map(addr, addr, 0x00100000, PMMU_READWRITE | PMMU_CM_WRITETHROUGH);
	}

    // bus error on MB block after tt-ram
    mmu_Invalid(lmem, 0x00100000);

    // bus error on first and last 4k block of the MB beyond st-ram (inside reserved area)
    mmu_Invalid(stram_size, 0x00010000);
    mmu_Invalid(stram_size + 0x000F0000, 0x00010000);

    // peripheral access flags
    mmu_Map(0x40000000, 0x40000000, 0x00100000, PMMU_READONLY  | PMMU_CM_WRITETHROUGH);    // ROM
    mmu_Map(0x20000000, 0x20000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // LOCBUS  (uart)
    mmu_Map(0xA0000000, 0xA0000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS  (ide, mfp2)
    mmu_Map(0xA1000000, 0xA1000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS  (ym, mfp1)
    mmu_Map(0xA2000000, 0xA2000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS  (unused)
    mmu_Map(0xA3000000, 0xA3000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS  (unused)
    mmu_Map(0x80000000, 0x80000000, 0x01000000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA-RAM (generic)
    mmu_Map(0x81000000, 0x81000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA-IO  (generic)
    mmu_Map(0x82000000, 0x82000000, 0x01000000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA-RAM (gfxcard)
    mmu_Map(0x83000000, 0x83000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA-IO  (gfxcard)

    // peripheral memory map
    mmu_Redirect(0xFF000000, 0x00000000, 0x01000000);  // 24bit space
    mmu_Redirect(0x00E00000, 0x40040000, 0x00080000);  // ROM      ($e00000 -> $40040000) size 512Kb at offset 256Kb
    mmu_Redirect(0xFFE00000, 0x03040000, 0x00080000);
    mmu_Redirect(0x00F00000, 0xA0000000, 0x00001000);  // IDE      ($f00000 -> $a0000000)
    mmu_Redirect(0xFFF00000, 0xA0000000, 0x00001000);
    mmu_Redirect(0x00FF8000, 0xA1000000, 0x00001000);  // YM2149   ($ff8800 -> $a1000800)
    mmu_Redirect(0xFFFF8000, 0xA1000000, 0x00001000);

    // hardware MFP1 + emulated ACIA
    // ACIA : ffffc00 goes to ram0 reserved area  (by help of cpld)
    // MFP1 : ffffx00 goes to 0xA1xxxxxx as usual (emu address bits ignored)
    mmu_Map(0xA1000000 + BIOS_EMU_MEM, 0xA1000000 + BIOS_EMU_MEM, 0x00001000, PMMU_READWRITE | PMMU_CM_PRECISE);
    mmu_Redirect(0x00FFF000, 0xA1000000 + BIOS_EMU_MEM, 0x00001000);  // MFP1     ($fffa00 -> $a1000a00)
    mmu_Redirect(0xFFFFF000, 0xA1000000 + BIOS_EMU_MEM, 0x00001000);

    // special case graphics card access to satisfy existing Atari drivers
	if (kgfx == 3) {
		// Mach32
	    mmu_Redirect(0xFE900000, 0x83000000, 0x00100000);      // TT Nova Mach32 reg base : 1024 kb
	    mmu_Redirect(0xFE800000, 0x82000000, 0x00100000);      // TT Nova Mach32 vga base :  128 kb
	    mmu_Redirect(0xFEA00000, 0x82200000, 0x00100000);      // TT Nova Mach32 mem base : 2048 kb ?? target addr ??
	    mmu_Redirect(0xFEB00000, 0x82300000, 0x00100000);      // TT Nova Mach32 mem base : 2048 kb ?? target addr ??

	}
	else if (kgfx == 4) {
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
	    mmu_Redirect(0xFED00000, 0x83000000, 0x00100000);      // TT Nova ET4k reg base    : 1024 kb
	    mmu_Redirect(0xFEC00000, 0x82000000, 0x00100000);      // TT Nova ET4k mem base	: 1024 kb
	    mmu_Redirect(0x00D00000, 0x83000000, 0x00100000);      // ST Nova ET4k reg base    : 1024 kb
	    mmu_Redirect(0x00C00000, 0x82000000, 0x00100000);      // ST Nova ET4k mem base    : 1024 kb
	}

    // hades compatible ISA I/O to take advantage of existing drivers
	mmu_Redirect(0xFFF30000, 0x81000000, 0x00010000);

	// ST emulation space
    mmu_Redirect(0x41000000, stram_size + 0x00100000, 0x00100000);		// ram	1024kb
    mmu_Redirect(0x41E00000, stram_size + 0x00200000, 0x00040000);		// tos	 256kb
    mmu_Redirect(0x41FC0000, stram_size + 0x00200000, 0x00030000);      // tos   192kb
    mmu_Redirect(0x41FA0000, stram_size + 0x00240000, 0x00020000);		// cart  128kb
    mmu_Redirect(0x41FF0000, stram_size + 0x00260000, 0x00010000);		// io	  64kb

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



bool atari_Init()
{
    puts("InitVbr");
    atari_InitVBR();

    puts("InitMmu");
    atari_InitMMU(ksimm);

    puts("InitEmu");
    atari_InitEMU();

    puts("InitTos");
	for (int i=0x400; i<0x700; i+=4) {
		IOL(0, i) = 0;
	}

    puts("Start");
#ifdef LAUNCH_TOS
    cpu_Call(0xe00000);
#else
    mon_Start();
#endif

    return true;
}
