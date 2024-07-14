#include "sys.h"

extern void vec_berr();
extern uint32 GetPCR();
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
    // identify cpu
    uint32 cpuRev = 0;
    uint32 cpuSku = DetectCPU(&cpuRev, 0);
    uart_printString("\n");
    uart_printString("CPU:  ");
    uart_printString(cpuNames[cpuSku]);
    uart_printHex(4, "R", cpuRev, "\n");

    // identify rom
    uint32 simm[4];
    simm[3] = IOL(PADDR_SIMM3, 0);
    uart_printHex(32, "ROM:  ", simm[3], "\n");

	// identify gfx
	kgfx = 1;	// assume ET4000
#ifdef LAUNCH_TOS    
	if ((IOB(0x82000000, 0xC0032)) == '6' && (IOB(0x82000000, 0xC0034) == '2')) {
		IOB(0x83000000, 0x56EE) = 0x55;
		if (IOB(0x83000000, 0x56EE) == 0x55) {
			kgfx = 3;	// todo: further detect Mach32 vs Mach64
		}
	}
	uart_printString("GFX:  ");
	uart_printString(gfxNames[kgfx]);
	uart_printString("\n");
#endif

	// identify ram
    for (int i=0; i<3; i++) {
        for (int j=15; j>=0; j--) {
            uint32 addr = (((i*16)+j)*1024*1024UL);
            *((volatile uint32*)addr) = j;
        }
    }
    for (int i=0; i<3; i++) {
        simm[i] = 0;
        for (int j=0; j<16; j++) {
            uint32 addr = (((i*16)+j)*1024*1024UL);
            if ( *((volatile uint32*)addr) == j) {
                simm[i] = ((j+1) * 1024) * 1024UL;
            } else {
                j = 16;
            }
        }
        uart_printHex(4, "RAM", i, ": ");
        uart_printHex(32, 0, simm[i], "\n");
    }


    uart_printString("\n");

    // clear bios bss area
    uart_printString("InitBss\n");
    extern uint8 _bss_start, _end;
    uint32* kbss = (uint32*)&_bss_start;
    while (kbss < (uint32*)&_end) {
        *kbss++ = 0;
    }

    // init heap
    uart_printString("InitHeap\n");
    kmem_Init();

    // init hardware
    uart_printString("InitSys\n");
    sys_Init();

    // init monitor
    uart_printString("InitMon\n");
    InitMonitor();

    // init vbr
    uart_printString("InitVbr\n");
    vbr_Init();

    // install motorola support packages
    //uart_printString("InitSP060\n");
    //sp060_Install();

    // init mmu
    uart_printString("InitMmu\n");
    pmmu_Init(simm);

    // start
    uart_printString("InitPcr\n");
    SetPCR(3);

    //uart_printString("TestSP060\n");
    //sp060_Test();

    uart_printString("Start\n");

#ifdef LAUNCH_TOS
    // clear TOS variables and launch
	for (int i=0x400; i<0x700; i+=4) {
		IOL(0, i) = 0;
	}
    Call(0xe00000);

#else

    // launch monitor
    StartMonitor();

#endif

    return 0;
}

