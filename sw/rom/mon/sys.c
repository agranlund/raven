#include "sys.h"
#include "lib.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "hw/mfp.h"
#include "hw/ikbd.h"
#include "hw/midi.h"
#include "monitor.h"
#include "atari.h"

bool mem_Init();

//-----------------------------------------------------------------------
#define KMEMSIZE  512 * 1024


//-----------------------------------------------------------------------
uint8 kheap[KMEMSIZE];
uint32 kheapPtr;
uint32 ksimm[4];
uint8 kgfx;


//-----------------------------------------------------------------------
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



//-----------------------------------------------------------------------
//
// Startup
//
//-----------------------------------------------------------------------
bool sys_Init()
{
    // init and identify cpu
    cpu_Init();
    uint32 cpuRev = 0;
    uint32 cpuSku = cpu_Detect(&cpuRev, 0);
    fmt("\nCPU:  %sR%b\n", cpuNames[cpuSku], cpuRev);

    // identify rom
    uint32 id_simm[4];
    id_simm[3] = IOL(IOL(PADDR_SIMM3, 0), 4);
    fmt("ROM:  %l\n", id_simm[3]);

	// identify gfx
	uint8 id_gfx = 1;	// assume ET4000
#if defined(CONF_ATARI) && defined(LAUNCH_TOS)
	if ((IOB(PADDR_GFX_RAM, 0xC0032)) == '6' && (IOB(PADDR_GFX_RAM, 0xC0034) == '2')) {
		IOB(PADDR_GFX_IO, 0x56EE) = 0x55;
		if (IOB(PADDR_GFX_IO, 0x56EE) == 0x55) {
			id_gfx = 3;	// todo: further detect Mach32 vs Mach64
		}
	}
    fmt("GFX:  %s\n", gfxNames[id_gfx]);
#endif

	// identify ram
    for (int i=0; i<3; i++) {
        for (int j=15; j>=0; j--) {
            uint32 addr = (((i*16)+j)*1024*1024UL);
            *((volatile uint32*)addr) = j;
        }
    }
    for (int i=0; i<3; i++) {
        id_simm[i] = 0;
        for (int j=0; j<16; j++) {
            uint32 addr = (((i*16)+j)*1024*1024UL);
            if ( *((volatile uint32*)addr) == j) {
                id_simm[i] = ((j+1) * 1024) * 1024UL;
            } else {
                j = 16;
            }
        }
        fmt("RAM%d: %l\n", i, id_simm[i]);
    }
    putchar('\n');


    // clear bios bss area
    puts("InitBss");
    extern uint8 _bss_start, _end;
    uint32* kbss = (uint32*)&_bss_start;
    while (kbss < (uint32*)&_end) {
        *kbss++ = 0;
    }

    // we can use bss section from this point onward
    ksimm[0] = id_simm[0];
    ksimm[1] = id_simm[1];
    ksimm[2] = id_simm[2];
    ksimm[3] = id_simm[3];
    kgfx     = id_gfx;


    // init systems
    puts("InitHeap");
    mem_Init();

    puts("InitUart");
    uart_Init();

    puts("InitIkbd");
    ikbd_Init();

    puts("InitMfp");
    mfp_Init();

    puts("InitMidi");
    midi_Init();

    puts("InitVbr");
    vbr_Init();

    puts("InitMonitor");
    mon_Init();

#ifdef CONF_ATARI
    puts("InitAtari");
    atari_Init();
#else
    puts("StartMonitor");
    mon_Start();
#endif

    return 0;
}


//-----------------------------------------------------------------------
//
// kmemory
//
//-----------------------------------------------------------------------
bool mem_Init()
{
    kheapPtr = ((uint32)&kheap[0] + KMEMSIZE - 4) & ~16UL;
    return true;
}

uint32 mem_Alloc(uint32 size, uint32 alignment)
{
    if (alignment < 4)
        alignment = 4;

    uint32 m = ((kheapPtr - size) & ~(alignment - 1));
    if (m >= (uint32) &kheap[0])
    {
        kheapPtr = m;
        return kheapPtr;
    }
    puts("ERROR: mem_Alloc()");
    return 0;
}


//-----------------------------------------------------------------------
//
// misc helpers
//
//-----------------------------------------------------------------------
uint32 strtoi(char* s)
{
    uint32 v = 0;
    if (*s == '$')
    {
        s++;
        while (*s != 0)
        {
            uint32 b = (uint32) *s; s++;
            if (b >= '0' && b <= '9')       b = b - '0';
            else if (b >= 'a' && b <= 'f')  b = 10 + b - 'a';
            else return 0;
            v <<= 4;
            v |= b;
        }
    }
    else
    {
        while (*s != 0)
        {
            uint32 b = (uint32) *s; s++;
            if (b >= '0' && b <= '9')   b = b - '0';
            else return 0;
            v *= 10;
            v += b;
        }
    }
    return v;
}
