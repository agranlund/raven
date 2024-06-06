#include "sys.h"

extern void vec_berr();

extern uint32 GetPCR();
extern int test_ym_write(int size);

extern void sp060_Install(void);
extern void sp060_Test(void);
extern uint8 kgfx;

const char* cpuNames[] = {
    "M68XC060",
    "M68EC060",
    "M68LC060",
    "M68060"
};

const char* gfxNames[] = {
	"",
	"ET4000AX",
	"ET4000/W32",
	"ATI Mach32",
	"ATI Mach64"
};

int Init()
{
    // clear bios bss area
    extern uint8 _bss_start, _end;
    uint32* kbss = (uint32*)&_bss_start;
    while (kbss < (uint32*)&_end) {
        *kbss++ = 0;
    }

    // identify cpu
    uint32 cpuRev = 0;
    uint32 cpuSku = DetectCPU(&cpuRev, 0);
    uart_printString("CPU:  ");
    uart_printString(cpuNames[cpuSku]);
    uart_printHex(3, "R", cpuRev, "\n");

    // identify rom
    uint32 simm[4];
    simm[3] = IOL(PADDR_SIMM3, 0);
    uart_printHex(32, "ROM:  ", simm[3], "\n");

	// identify gfx
	kgfx = 1;	// assume ET4000
	if ((IOB(0x82000000, 0xC0032)) == '6' && (IOB(0x82000000, 0xC0034) == '2')) {
		IOB(0x83000000, 0x56EE) = 0x55;
		if (IOB(0x83000000, 0x56EE) == 0x55) {
			kgfx = 3;	// todo: further detect Mach32 vs Mach64
		}
	}
	uart_printString("GFX:  ");
	uart_printString(gfxNames[kgfx]);
	uart_printString("\n");

	// identify ram
    for (int i=0; i<3; i++) {
        for (int j=15; j>=0; j--) {
            *((volatile uint32*)((((i*16)+j)*1024*1024))) = j;
        }
    }
    for (int i=0; i<3; i++) {
        simm[i] = 0;
        for (int j=0; j<16; j++) {
            if ( *((volatile uint32*)((((i*16)+j)*1024*1024))) == j) {
                simm[i] = (j+1) * 1024 * 1024;
            } else {
                j = 16;
            }
        }
        uart_printHex(4, "RAM", i, ": ");
        uart_printHex(32, 0, simm[i], "\n");
    }

    // init heap
    kmem_Init();

    // init hardware
    sys_Init();

    // init monitor
    InitMonitor();

    // init vbr
    vbr_Init();

    // install motorola support packages
    //sp060_Install();

    // init mmu
    pmmu_Init(simm);
/*
    {
        uint32* dst = (uint32*)8;
        uint32* src = (uint32*)0x00E00008;
        for (uint32 i=8; i<256; i++) {
            *dst++ = *src++;
        }
    }
*/
    SetCACR(0x00000000);        // tos will enable cache later
    SetPCR(3);

//    test_ym_write(1);

/*
    uart_printString("before sp060_Test\n");
    sp060_Test();
    uart_printString("after sp060_Test\n");
*/

    // clear TOS variables and launch
	for (int i=0x400; i<0x700; i+=4) {
		IOL(0, i) = 0;
	}
    Call(0xe00000);

    // launch monitor
    StartMonitor();

/*
    uart_printString("\n\n");
    while(1) {
//        uart_printString("> ");

        if ((IOB(PADDR_UART2, UART_LSR) & (1 << 0)) != 0) {
            uint8 c = IOB(PADDR_UART2, UART_RHR);
            uart_printChar(c);
        }
    }
*/
    return 0;
}


