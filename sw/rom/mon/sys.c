#include "sys.h"
#include "lib.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "hw/mfp.h"
#include "hw/ikbd.h"
#include "hw/midi.h"
#include "hw/i2c.h"
#include "hw/rtc.h"
#include "hw/flash.h"
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
extern uint8_t __data_load;
extern uint8_t __data_start;
extern uint8_t __data_end;
extern uint8_t __bss_start;
extern uint8_t __bss_end;
extern uint8_t __stack_top;

#define resmagic0   0x752019f3
#define resmagic1   0x237698aa
#define resmagic2   0x1357bd13
#define resmagic3   0x31415926


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

    // identify rom size
    uint32_t siz_simm[4];
    for (int i=0; i<16; i++) {
        bool romaddr_valid = true;
        siz_simm[3] = ((i+1) * 1024) * 1024UL;
        for (int j=0; j<1024 && romaddr_valid; j+=4) {
            romaddr_valid = (IOL(RV_PADDR_SIMM3, j) != IOL((RV_PADDR_SIMM3 + siz_simm[3]), j)) ? true : false;
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
    uint8_t ikbdbaud = coldboot ? IKBD_BAUD_7812 : ikbd_Baud();

    // clear bios bss area & copy data
    memset(&__bss_start, 0, &__bss_end - &__bss_start);
    memcpy(&__data_start, &__data_load, &__data_end - &__data_start);

    // we can use bss section from this point onward
    ksimm[0] = siz_simm[0];
    ksimm[1] = siz_simm[1];
    ksimm[2] = siz_simm[2];
    ksimm[3] = siz_simm[3];

    // init systems
    lib_Init();

    // can use printf and other lib functions from this point onward
    putchar('\n');
    printf("CPU:   %sR%d\n", cpuNames[cpuSku], cpuRev);
    printf("SIMM0: %08x\n", siz_simm[0]);
    printf("SIMM1: %08x\n", siz_simm[1]);
    printf("SIMM2: %08x\n", siz_simm[2]);
    printf("SIMM3: %08x\n", siz_simm[3]);
    //printf("FLASH: %08x\n", flash_Identify());
    putchar('\n');

    const bool safemode = false;

    initprint(safemode ? "Safemode" : coldboot ? "Coldboot" : "Warmboot");

    initprint("InitHeap");
    mem_Init();

    initprint("InitUart");
    uart_Init();

    initprint("InitIkbd");
    ikbd_Init();

    if (!safemode)
    {
        initprint("InitMfp");
        mfp_Init();

        initprint("InitMidi");
        midi_Init();

        initprint("InitI2C");
        i2c_Init();

        initprint("InitRtc");
        rtc_Init();

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
        uint8_t ikbdcfgbaud = 7 & ((uint8_t) cfg_GetValue(cfg_Find("ikbd_baud")));
        ikbd_ConnectEx(ikbdbaud, ikbdcfgbaud);
        ikbd_Info();

        initprint("InitAtari");
        atari_Init();
    }

    initprint("StartMonitor");
    mon_Start();

    return 0;
}


//-----------------------------------------------------------------------
uint32_t sys_Chipset(void) {
    return (uint32_t)REV;
}

//-----------------------------------------------------------------------
rvtoc_t* sys_GetToc(uint32_t id) {
    extern uint8_t __toc_start;
    rvtoc_t* toc = (rvtoc_t*)&__toc_start;
    while(toc->id) {
        if ((toc->id == id) || (id == 0)) {
            return toc;
        }
        toc++;
    }
    return 0;
}

//-----------------------------------------------------------------------
rvcfg_t* sys_GetCfg(uint32_t id) {
    extern uint8_t __config_start;
    rvcfg_t* cfg = (rvcfg_t*)&__config_start;
    while(cfg) {
        if ((cfg->id == id) || (id == 0)) {
            return cfg;
        }
        cfg = (rvcfg_t*)(cfg->size + (uint32_t)cfg);
    }
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
void sys_Delay(uint32_t c) {
    for (uint32_t i = 0; i < (c * 1000); i++) {
        nop();
    }
}

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
