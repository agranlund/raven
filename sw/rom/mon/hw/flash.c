#include "sys.h"
#include "raven.h"
#include "hw/cpu.h"
#include "hw/flash.h"

extern uint32_t ksimm[4];

#define FLASH_PADDR     RV_PADDR_SIMM3
#define FLASH_LADDR     0x21000000UL


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
void flashR_ProgramAndReset(uint32_t addr, void* src, uint32_t size)
{
    cpu_CacheOff();

    // erase
    flashR_Indicate(false);
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
    *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0x80808080UL;
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
    *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
    *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0x10101010UL;
    for (int i=0; i<8; i++) {
        uint32_t d = *((volatile uint32_t*)(addr));
        if ((d & 0x80808080UL) != 0x80808080UL) {
            i = 0;
        }
    }

    // program
    flashR_Indicate(true);
    uint32_t* sptr = (uint32_t*)src;
    for (uint32_t i=0; i<size; i+=4) {
        uint32_t val = *sptr++;
        *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xAAAAAAAAUL;
        *((volatile uint32_t*)(addr+(0x2AAAUL<<2))) = 0x55555555UL;
        *((volatile uint32_t*)(addr+(0x5555UL<<2))) = 0xA0A0A0A0UL;
        *((volatile uint32_t*)(addr+i)) = val;
        flashR_Indicate((i & 0x4000UL) ? false : true);

        // wait ~20us or early out when D6 stops toggling
        for (int j=0; j<100; j++) {
            volatile uint32_t d0 = *((volatile uint32_t*)(addr+i)) & 0x00400040UL;
            volatile uint32_t d1 = *((volatile uint32_t*)(addr+i)) & 0x00400040UL;
            volatile uint32_t d2 = *((volatile uint32_t*)(addr+i)) & 0x00400040UL;
            volatile uint32_t d3 = *((volatile uint32_t*)(addr+i)) & 0x00400040UL;
            if (d0 == d1 && d0 == d2 && d0 == d3) {
                break;
            }
        }
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
        : : : );
}



#if 0
//-----------------------------------------------------------------------
// program
//-----------------------------------------------------------------------
void flashR_WriteSettings(uint32_t addr, void* src)
{
}
#endif


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


bool flash_Program(void* data, uint32_t size) {

    // todo: stricter flash identification?
    uint32_t dev = flash_Identify();
    if (dev == 0) {
        printf("Flash identification failed\n");
        return false;
    }

    // verify rom data
    uint32_t version = 0;
    uint32_t* hdr = (uint32_t*)data;
    if ((hdr[0] >= FLASH_PADDR) && (hdr[0] < (FLASH_PADDR + size)) && (hdr[1] >= FLASH_PADDR) && (hdr[1] < (FLASH_PADDR + size))) {
        hdr = (uint32_t*) (hdr[0] - FLASH_PADDR + (uint32_t)data);
        if (hdr[0] == 0x5241564EUL) {
            version = hdr[1];
        }
    }

    if (version == 0) {
        printf("Invalid ROM image\n");
        return false;
    }

    printf("\n");
    printf("Flashing ROM:\n");
    printf(" rom version....%08lx\n", version);
    printf(" rom size.......%ld Kb\n", size / 1024);
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
        flashR_ProgramAndReset(addr, data, size);
        flash_Lock(addr);
    }
    cpu_SetIPL(ipl);
    return false;
}


bool flash_Init() {
    return true;
}
