#include "sys.h"
#include "lib.h"

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
    fmt("\nCPU:  %sR%b\n", cpuNames[cpuSku], cpuRev);

    // identify rom
    uint32 simm[4];
    fmt("ROM:  %l\n", IOL(PADDR_SIMM3, 0));

	// identify gfx
	kgfx = 1;	// assume ET4000
#ifdef LAUNCH_TOS    
	if ((IOB(0x82000000, 0xC0032)) == '6' && (IOB(0x82000000, 0xC0034) == '2')) {
		IOB(0x83000000, 0x56EE) = 0x55;
		if (IOB(0x83000000, 0x56EE) == 0x55) {
			kgfx = 3;	// todo: further detect Mach32 vs Mach64
		}
	}
    fmt("GFX:  %s\n", gfxNames[kgfx]);
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
        fmt("RAM%d: %l\n", i, simm[i]);
    }
    putchar('\n');

    // clear bios bss area
    puts("InitBss");
    extern uint8 _bss_start, _end;
    uint32* kbss = (uint32*)&_bss_start;
    while (kbss < (uint32*)&_end) {
        *kbss++ = 0;
    }

    // init heap
    puts("InitHeap");
    kmem_Init();

    // init hardware
    puts("InitSys");
    sys_Init();

    // init monitor
    puts("InitMon");
    InitMonitor();

    // init vbr
    puts("InitVbr");
    vbr_Init();

    // install motorola support packages
    //puts("InitSP060\n");
    //sp060_Install();

    // init mmu
    puts("InitMmu");
    pmmu_Init(simm);

    // start
    puts("InitPcr");
    SetPCR(3);

    //puts("TestSP060");
    //sp060_Test();

    puts("Start");

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

