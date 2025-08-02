#include "sys.h"
#include "raven.h"
#include "hw/cpu.h"
#include "hw/flash.h"

extern uint32_t ksimm[4];

#define FLASH_PADDR     RV_PADDR_SIMM3
#define FLASH_LADDR     0x21000000UL
#define HDR_MAGIC       0x5241564EUL
#define CFG_MAGIC       0x434F4E46UL

//-----------------------------------------------------------------------
// ram functions
//-----------------------------------------------------------------------

__attribute__((section(".ramtext")))
void flashR_Id(uint32_t addr, uint32_t* mid, uint32_t* did) 
{
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
    *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0x90909090UL;
    for (int i=0; i<150; i++) { __asm__ __volatile__( "\tnop\n" : : : ); }
    *mid = *((volatile uint32_t*)(addr+0x0000UL));
    *did = *((volatile uint32_t*)(addr+0x0004UL));
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
    *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xF0F0F0F0UL;
}

__attribute__((section(".ramtext")))
void flashR_Indicate(bool en)
{
    if (en) {
        *((volatile uint8_t*)(RV_PADDR_UART1 + 0x10)) &= ~1;    /* pwr led on */
    } else {
        *((volatile uint8_t*)(RV_PADDR_UART1 + 0x10)) |= 1;     /* pwr led off */
    }
}

__attribute__((section(".ramtext")))
void flashR_Delay(uint32_t count) {
    for (uint32_t i=0; i<count; i++) {
        __asm__ __volatile__( "\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n" : : : );
        __asm__ __volatile__( "\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n" : : : );
    }
}

__attribute__((section(".ramtext")))
void flashR_Fatal(void)
{
    while (1) {
        flashR_Indicate(true); flashR_Delay(80000);  flashR_Indicate(false); flashR_Delay(50000);
        flashR_Indicate(true); flashR_Delay(80000);  flashR_Indicate(false); flashR_Delay(50000);
        flashR_Indicate(true); flashR_Delay(80000);  flashR_Indicate(false); flashR_Delay(50000 + 100000);
        flashR_Indicate(true); flashR_Delay(240000); flashR_Indicate(false); flashR_Delay(50000);
        flashR_Indicate(true); flashR_Delay(240000); flashR_Indicate(false); flashR_Delay(50000);
        flashR_Indicate(true); flashR_Delay(240000); flashR_Indicate(false); flashR_Delay(50000 + 100000);
        flashR_Indicate(true); flashR_Delay(80000);  flashR_Indicate(false); flashR_Delay(50000);
        flashR_Indicate(true); flashR_Delay(80000);  flashR_Indicate(false); flashR_Delay(50000);
        flashR_Indicate(true); flashR_Delay(80000);  flashR_Indicate(false); flashR_Delay(50000+ 400000);
    }
}

__attribute__((section(".ramtext")))
bool flashR_WriteSector(uint32_t addr, uint32_t src, uint32_t size)
{
    // erase
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
    *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0x80808080UL;
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
    *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
    *((volatile uint32_t*)(addr))               = 0x50505050UL;
    flashR_Delay(1000);
    for (uint32_t i=0; i<size; i+=4) {
        uint32_t d = *((volatile uint32_t*)(addr+i));
        if (d != 0xffffffff) { i = 0; }
    }

    // program
    for (uint32_t i=0; i<size; i+=4) {
        uint32_t val = *((uint32_t*)(src+i));
        *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
        *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
        *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xA0A0A0A0UL;
        *((volatile uint32_t*)(addr+i)) = val;
        flashR_Delay(10);
        for (short j=0; j<1000; j++) {
            volatile uint32_t d0 = *((volatile uint32_t*)(addr+i));
            volatile uint32_t d1 = *((volatile uint32_t*)(addr+i));
            if ((d0 == d1) && (d0 == val)) {
                break;
            }
        }
    }

    // verify
    bool verified = true;
    for (uint32_t i=0; i<size && verified; i+=4) {
        verified = (*((volatile uint32_t*)(addr+i)) == *((uint32_t*)(src+i))) ? true : false;
    }
    return verified;
}

__attribute__((section(".ramtext")))
void flashR_ProgramAndReset(uint32_t addr, uint32_t src, uint32_t size)
{
    cpu_CacheOff();
    for (int retry = 1; retry >= 0; retry--)
    {
        // erase
        flashR_Indicate(false);
        *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
        *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
        *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0x80808080UL;
        *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
        *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
        *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0x10101010UL;
        flashR_Delay(1000);
        for (short i=0; i<1000; i++) {
            uint32_t d = *((volatile uint32_t*)(addr));
            if (d != 0xffffffff) { i = 0; }
        }

        // program
        flashR_Indicate(true);
        for (uint32_t i=0; i<size; i+=4) {
            uint32_t val = *((uint32_t*)(src+i));
            flashR_Indicate((i & 0x4000UL) ? false : true);
            *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
            *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
            *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xA0A0A0A0UL;
            *((volatile uint32_t*)(addr+i)) = val;
            flashR_Delay(10);
            for (short j=0; j<1000; j++) {
                volatile uint32_t d0 = *((volatile uint32_t*)(addr+i));
                volatile uint32_t d1 = *((volatile uint32_t*)(addr+i));
                if ((d0 == d1) && (d0 == val)) {
                    break;
                }
            }
        }

        // verify
        bool corrupt = false;
        flashR_Indicate(false);
        for (uint32_t i=0; i<size && !corrupt; i++) {
            corrupt = (*((uint8_t*)(addr+i)) != *((uint8_t*)(src+i))) ? true : false;
        }
        retry = corrupt ? retry : 0;
        if (corrupt && (retry == 0)) {
            flashR_Fatal();
        }

        flashR_Delay(10000);
    }

    // reset
    flashR_Indicate(true);
    __asm__ __volatile__(
        "   moveq.l #0,%%d0\n"
        "   movec.l %%d0,%%vbr\n"   /* restore vbr */
        "   movec.l %%d0,%%tc\n"    /* disable mmu */
        "   nop\n"
        "   pflusha\n"
        "   nop\n"
        "   reset\n"
        "   nop\n"
        "   move.l  0x40000004,-(%%sp)\n"   /* reset */
        "   rts\n"
        : : : "d0", "cc" );
}


//-----------------------------------------------------------------------
// rom function
//-----------------------------------------------------------------------

static uint32_t flash_Unlock() {
    mmuregs_t mmu;
    cpu_GetMMU(&mmu);
    if (mmu.tcr) {
        mmu_Map(FLASH_LADDR, FLASH_PADDR, ksimm[3], PMMU_READWRITE | PMMU_CM_PRECISE);
        mmu_Flush();
        return FLASH_LADDR;
    }
    return FLASH_PADDR;
}


static void flash_Lock(uint32_t addr) {
    if (addr == FLASH_LADDR) {
        mmu_Map(FLASH_LADDR, FLASH_LADDR, ksimm[3], PMMU_READWRITE | PMMU_CM_PRECISE);
        mmu_Flush();
    }
}


uint32_t flash_Identify() {
    uint32_t mid = 0;
    uint32_t did = 0;
    uint32_t ipl = cpu_SetIPL(7);
    uint32_t addr = flash_Unlock();
    if (addr) {
        flashR_Id(addr, &mid, &did);
        flash_Lock(addr);
    }
    cpu_SetIPL(ipl);

    // sanity check return value
    if (((mid >> 16) != (mid & 0xffff)) || ((did >> 16) != (did & 0xffff))) {
        mid = did = 0;
    }

    // bith depth
    if ((((mid >> 8) & 0xff) == (mid & 0xff)) && (((did >> 8) & 0xff) == (did & 0xff))) {
        mid = mid & 0xff;
        did = did & 0xff;
    }

    return ((mid<<16)|(did&0xffff));
}

static rvtoc_t* get_config_toc(uint32_t base) {
    rvtoc_t* toc = (rvtoc_t*)(base + 0x400 + 32);
    return (toc->id == 0x5F434647) ? toc : 0;
}

bool flash_Program(void* data, uint32_t size) {

    // todo: stricter flash identification?
    uint32_t dev = flash_Identify();
    if (dev == 0) {
        printf("Flash identification failed\n");
        return false;
    }

    // verify rom data
    raven_t* header_new = 0;
    raven_t* header_old = raven();
    uint32_t* romdata32 = (uint32_t*)data;
    if ((romdata32[0] >= FLASH_PADDR) && (romdata32[0] < (FLASH_PADDR + size)) && (romdata32[1] >= FLASH_PADDR) && (romdata32[1] < (FLASH_PADDR + size))) {
        header_new = (raven_t*) (romdata32[0] - FLASH_PADDR + (uint32_t)data);
    }

    if (!header_new || (header_new->magic != HDR_MAGIC)) {
        printf("Invalid ROM image\n");
        return false;
    }

    // keep existing user settings
    uint32_t version_support_configspace = 0x20250729UL;
    if ((header_new->version >= version_support_configspace) && (header_old->version >= version_support_configspace))
    {
        rvtoc_t* cfg_old = get_config_toc(FLASH_PADDR);
        rvtoc_t* cfg_new = get_config_toc((uint32_t)data);

        if (cfg_old && cfg_old->size)
        {
            if (cfg_new && cfg_new->size)
            {
                if (cfg_old->size <= cfg_new->size)
                {
                    uint32_t* settings_src = (uint32_t*)(cfg_old->start);
                    uint32_t* settings_dst = (uint32_t*)(cfg_new->start - FLASH_PADDR + (uint32_t)data);
                    uint32_t settings_size = cfg_old->size;
                    for (uint32_t i=0; i<settings_size; i+=4) {
                        *settings_dst++ = *settings_src++;
                    }
                }
            }
        }
    }

    printf("\n");
    printf(" old version....%08lx\n", header_old->version);
    printf(" new version....%08lx\n", header_new->version);
    printf(" new size.......%ld Kb\n", size / 1024);
    printf(" device id......%04x:%04x\n", (dev>>16), (dev & 0xffff));
    printf("\n");
    printf("-----------------------------------------------------\n");
    printf(" WARNING: Do not turn off the computer.\n");
    printf(" The power LED will blink during programming and\n");
    printf(" computer will automatically restart when done..\n");
    printf("-----------------------------------------------------\n");
    printf("\n");

    // program and reset
    uint32_t ipl = cpu_SetIPL(7);
    uint32_t addr = flash_Unlock();
    if (addr) {
        flashR_ProgramAndReset(addr, (uint32_t)data, size);
        flash_Lock(addr);
    }
    cpu_SetIPL(ipl);
    return false;
}


bool flash_Init() {
    return true;
}
