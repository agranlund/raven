#include "sys.h"
#include "lib.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "hw/mfp.h"
#include "hw/ikbd.h"
#include "hw/midi.h"
#include "hw/i2c.h"
#include "hw/rtc.h"
#include "monitor.h"
#include "config.h"
#include "atari.h"

bool mem_Init();

//-----------------------------------------------------------------------
#define KMEMSIZE  512 * 1024


//-----------------------------------------------------------------------
uint8_t  kheap[KMEMSIZE];
uint32_t kheapPtr;
uint32_t ksimm[4];

extern uint8_t __text_end;
extern uint8_t __data_start;
extern uint8_t __data_end;
extern uint8_t __bss_start;
extern uint8_t __bss_end;

//-----------------------------------------------------------------------
const char * const cpuNames[] = {
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
bool sys_Init()
{
    // init and identify cpu
    cpu_Init();
    uint32_t cpuRev = 0;
    uint32_t cpuSku = cpu_Detect(&cpuRev, 0);
    fmt("\nCPU:  %sR%b\n", cpuNames[cpuSku], cpuRev);

    // identify rom
    uint32_t id_simm[4];
    id_simm[3] = IOL(IOL(PADDR_SIMM3, 0), 4);
    fmt("ROM:  %l\n", id_simm[3]);

	// identify ram
    for (int i=0; i<3; i++) {
        for (int j=15; j>=0; j--) {
            uint32_t addr = (((i*16)+j)*1024*1024UL);
            *((volatile uint32_t*)addr) = j;
        }
    }
    for (int i=0; i<3; i++) {
        id_simm[i] = 0;
        for (int j=0; j<16; j++) {
            uint32_t addr = (((i*16)+j)*1024*1024UL);
            if ( *((volatile uint32_t*)addr) == j) {
                id_simm[i] = ((j+1) * 1024) * 1024UL;
            } else {
                j = 16;
            }
        }
        fmt("RAM%d: %l\n", i, id_simm[i]);
    }
    putchar('\n');

    // clear bios bss area & copy data
    puts("InitBss");
    memset(&__bss_start, 0, &__bss_end - &__bss_start);
    memcpy(&__data_start, &__text_end, &__data_end - &__data_start);

    // we can use bss section from this point onward
    ksimm[0] = id_simm[0];
    ksimm[1] = id_simm[1];
    ksimm[2] = id_simm[2];
    ksimm[3] = id_simm[3];

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

    puts("InitI2C");
    i2c_Init();

    puts("InitRtc");
    rtc_Init();

    puts("InitCfg");
    cfg_Init();

    puts("InitVbr");
    vbr_Init();

    puts("InitMonitor");
    mon_Init();

    puts("InitAtari");
    atari_Init();

    puts("StartMonitor");
    mon_Start();

    return 0;
}


//-----------------------------------------------------------------------
//
// kmemory
//
//-----------------------------------------------------------------------
bool mem_Init()
{
    // init heap pointer
    kheapPtr = ((uint32_t)&kheap[0] + KMEMSIZE - 4) & ~16UL;

    return true;
}

uint32_t mem_Alloc(uint32_t size, uint32_t alignment)
{
    if (alignment < 4)
        alignment = 4;

    uint32_t m = ((kheapPtr - size) & ~(alignment - 1));
    if (m >= (uint32_t) &kheap[0])
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
uint32_t strtoi(char* s)
{
    uint32_t v = 0;
    if (*s == '$')
    {
        s++;
        while (*s != 0)
        {
            uint32_t b = (uint32_t) *s; s++;
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
            uint32_t b = (uint32_t) *s; s++;
            if (b >= '0' && b <= '9')   b = b - '0';
            else return 0;
            v *= 10;
            v += b;
        }
    }
    return v;
}
