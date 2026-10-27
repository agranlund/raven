// boot stage 1
// hardware detect and initialization
//
// proceeds to stage 2 when TOS is present in ROM,
// else stops at interactive debug monitor.

#include "sys.h"
#include "lib.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "hw/mfp.h"
#include "hw/ikbd.h"
#include "hw/ym.h"
#include "hw/midi.h"
#include "hw/i2c.h"
#include "hw/rtc.h"
#include "hw/mac.h"
#include "hw/flash.h"
#include "hw/pram.h"
#include "monitor.h"
#include "config.h"

extern void boot_stage2();

//-----------------------------------------------------------------------
#define resmagic0 0x752019f3
#define resmagic1 0x237698aa
#define resmagic2 0x1357bd13
#define resmagic3 0x31415926

static const char * const cpuNames[] = {
    "M68XC060",
    "M68EC060",
    "M68LC060",
    "M68060"
};

//-----------------------------------------------------------------------
//
// Startup
//
//-----------------------------------------------------------------------
bool boot_stage1()
{
    // init and identify cpu
    cpu_Init();
    uint32_t cpuRev = 0;
    uint32_t cpuSku = cpu_Detect(&cpuRev, 0);

	// identify board revision.
	uint8_t rev = (IOB(RV_PADDR_UART1, UART_MSR) & (1 << 5)) ? 0xA1 : 0xA2;

	// identify dsp
	uint16_t dsp = 0;
	if (rev >= 0xA2) {
		// dsp on
		*((volatile uint8_t*)(RV_PADDR_UART1+UART_MCR)) |= 0x02;
		sys_Delay(10);
		// check if cvr valid
		if (*((volatile uint8_t*)(RV_PADDR_DSP+0x04)) != 0xff) {
			dsp = 56303;
		}
		// dsp off
		*((volatile uint8_t*)(RV_PADDR_UART1+UART_MCR)) &= 0xfd;
	}

    // identify rom size
    uint32_t siz_simm[4];
    for (int i=0; i<16; i++) {
        bool romaddr_valid = true;
        siz_simm[3] = ((i+1) * 1024) * 1024UL;
        for (int j=0; j<1024 && romaddr_valid; j+=4) {
            romaddr_valid = (IOL(ROM_ADDR, j) != IOL((ROM_ADDR + siz_simm[3]), j)) ? true : false;
        }
        i = romaddr_valid ? i : 16;
    }

	// identify ram sizes
    for (int i=0; i<3; i++) {
        for (int j=15; j>=0; j--) {
            uint32_t addr = (((i*16)+j)*1024*1024UL);
            *((volatile uint32_t*)addr) = j;
        }
    }
    for (int i=0; i<3; i++) {
        siz_simm[i] = 0;
        for (int j=0; j<16; j++) {
            uint32_t addr = (((i*16)+j)*1024*1024UL);
            if ( *((volatile uint32_t*)addr) == j) {
                siz_simm[i] = ((j+1) * 1024) * 1024UL;
            } else {
                j = 16;
            }
        }
    }

    // determine if this is a warm or cold boot
    // top 16 bytes of stack area is reserved for persistent variables
    bool coldboot = true;
    uint32_t* magics = (uint32_t*)(((uint32_t)&__stack_top)-16);
    if ((magics[0] == resmagic0) && (magics[1] == resmagic1) && (magics[2] == resmagic2) && (magics[3] == resmagic3)) {
        coldboot = false;
    } else {
        magics[0] = resmagic0;
        magics[1] = resmagic1;
        magics[2] = resmagic2;
        magics[3] = resmagic3;
    }

    // clear bios bss area & copy data
    memset(&__bss_start, 0, &__bss_end - &__bss_start);
    memcpy(&__data_start, &__data_load, &__data_end - &__data_start);

    // we can use bss section from this point onward
    ksimm[0] = siz_simm[0];
    ksimm[1] = siz_simm[1];
    ksimm[2] = siz_simm[2];
    ksimm[3] = siz_simm[3];
	krev = rev;

    // init systems
    lib_Init();

    // can use printf and other lib functions from this point onward
    putchar('\n');
	printf("REV:   %02x\n", rev);
    printf("CPU:   %sR%d\n", cpuNames[cpuSku], cpuRev);
	if (rev >= 0xA2) {
		printf("DSP:   ", dsp);
		if (dsp > 0) { printf("DSP%d\n", dsp); }
		else { printf("--------\n"); };
	}
    printf("SIMM0: %08x\n", siz_simm[0]);
    printf("SIMM1: %08x\n", siz_simm[1]);
    printf("SIMM2: %08x\n", siz_simm[2]);
    printf("SIMM3: %08x\n", siz_simm[3]);
    //printf("FLASH: %08x\n", flash_Identify());
    putchar('\n');

    const bool safemode = false;

    initprint(safemode ? "Safemode" : coldboot ? "Coldboot" : "Warmboot");

    initprint("InitFlash");
    flash_Init();
    printf("%s %08x\n", flash_Name(), flash_Id());

    initprint("InitPram");
    pram_Init();

    initprint("InitHeap");
    mem_Init();

    initprint("InitUart");
    uart_Init();

    initprint("InitIkbd");
    ikbd_Init((krev >= 0xA2) ? IKBD_BAUD_125000 : IKBD_BAUD_7812);

    if (!safemode)
    {
        initprint("InitMfp");
        mfp_Init();

		initprint("InitYm");
        ym_Init();

        initprint("InitMidi");
        midi_Init();

        initprint("InitI2C");
        i2c_Init();

        initprint("InitRtc");
        rtc_Init();

        initprint("InitMac");
        mac_Init();

		uint8_t macaddr[6];
		if (mac_Addr(&macaddr)) {
			printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
				macaddr[0], macaddr[1], macaddr[2],
				macaddr[3], macaddr[4], macaddr[5]);
			}

        initprint("InitCfg");
        cfg_Init();

        initprint("InitVbr");
        vbr_Init();

        initprint("InitMsp");
        msp_Init();
    }

    initprint("InitMonitor");
    mon_Init();

    if (!safemode)
    {
        initprint("IkbdConnect");
		if (krev >= 0xA2) {
			ikbd_ConnectEx(IKBD_BAUD_125000, IKBD_BAUD_125000);
			uint8_t rgb[4];
			if (ikbd_ReadSetting(0x2c, &rgb[0], &rgb[1], &rgb[2], &rgb[3]) == 0x2c) {
				rgb[1] = (uint8_t)(((uint16_t)rgb[0] * rgb[1]) >> 8);
				rgb[2] = (uint8_t)(((uint16_t)rgb[0] * rgb[2]) >> 8);
				rgb[3] = (uint8_t)(((uint16_t)rgb[0] * rgb[3]) >> 8);
				ikbd_ARGB(0, 255, 1, &rgb[1]);
			}
		} else {
		    uint8_t baud_safe = coldboot ? ikbd_DefaultBaud() : ikbd_Baud();
            uint8_t baud_ideal = (uint8_t) cfg_GetValue(cfg_Find("ikbd_speed"));
        	ikbd_ConnectEx(baud_safe, baud_ideal);
		}
        ikbd_Info();

        // try boot operating system or fall through
        // to interactive debug monitor
        boot_stage2();
    }

    initprint("StartMonitor");
    mon_Start();

    return 0;
}
