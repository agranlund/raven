#include "sys.h"
#include "raven.h"
#include "hw/cpu.h"
#include "hw/flash.h"

extern uint32_t ksimm[4];

#define FLASH_PADDR     RV_PADDR_SIMM3
#define FLASH_LADDR     0x21000000UL
#define HDR_MAGIC       0x5241564EUL
#define CFG_MAGIC       0x434F4E46UL

// working info in ram
static struct {
    uint32_t id;
    uint32_t total_size;            // logical size per simm
    uint32_t sector_size;           // logical size per simm
    uint32_t total_size_phy;        // physical size per chip
    uint32_t sector_size_phy;       // physical size per chip
    uint32_t sector_erase_cmd;      // logical sector erase command
} flash;

// chip database in rom
typedef struct {
    const char*     name;           // human readable name
    const uint32_t  id;             // mid:did
    const uint16_t  width;          // word size
    const uint16_t  size;           // number of words
    const uint16_t  sector_size;    // words per sector
    const uint8_t   sector_erase;   // sector erase command
} flash_chipinfo_t;

const flash_chipinfo_t flash_chipinfos[] = {
    {"Unknown",         0x00000000, 0,    0,    0, 0x00 },
    {"Unknown",         0xFFFFFFFF, 0,    0,    0, 0x00 },

    // SST39 8bit
    {"SST39SF010A",     0x00BF00B5, 1,  128, 4096, 0x30 },
    {"SST39SF020A",     0x00BF00B6, 1,  256, 4096, 0x30 },
    {"SST39SF040",      0x00BF00B7, 1,  512, 4096, 0x30 },
    {"SST39LF010A",     0x00BF00D5, 1,  128, 4096, 0x30 },
    {"SST39LF020A",     0x00BF00D6, 1,  256, 4096, 0x30 },
    {"SST39LF040",      0x00BF00D7, 1,  512, 4096, 0x30 },

    // SST39 16bit
    {"SST39LF401C",     0x00BF2321, 2,  256, 2048, 0x50 },
    {"SST39LF402C",     0x00BF2322, 2,  256, 2048, 0x50 },
    {"SST39LF801C",     0x00BF233B, 2,  512, 2048, 0x50 },
    {"SST39LF802C",     0x00BF233A, 2,  512, 2048, 0x50 },

    {"SST39VF1601C",    0x00BF234F, 2, 1024, 2048, 0x50 },
    {"SST39VF1602C",    0x00BF234E, 2, 1024, 2048, 0x50 },
    {"SST39VF3201C",    0x00BF235F, 2, 2048, 2048, 0x50 },
    {"SST39VF3202C",    0x00BF235E, 2, 2048, 2048, 0x50 },
    {"SST39VF6401B",    0x00BF236D, 2, 4096, 2048, 0x50 },
    {"SST39VF6402B",    0x00BF236C, 2, 4096, 2048, 0x50 },

    {"SST39VF1601",     0x00BF234B, 2, 1024, 2048, 0x30 },
    {"SST39VF1602",     0x00BF234A, 2, 1024, 2048, 0x30 },
    {"SST39VF3201",     0x00BF235B, 2, 2048, 2048, 0x30 },
    {"SST39VF3202",     0x00BF235A, 2, 2048, 2048, 0x30 },

    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
    {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 }, {0, 0, 0, 0, 0, 0 },
};

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
bool flashR_SectorErase(uint32_t addr, uint32_t size)
{
    for (uint32_t i=0; i<size; i+=flash.sector_size) {
        uint32_t base = addr & 0xff000000;
        *((volatile uint32_t*)(base+(0x5555UL<<2))) = 0xAAAAAAAAUL;
        *((volatile uint32_t*)(base+(0x2AAAUL<<2))) = 0x55555555UL;
        *((volatile uint32_t*)(base+(0x5555UL<<2))) = 0x80808080UL;
        *((volatile uint32_t*)(base+(0x5555UL<<2))) = 0xAAAAAAAAUL;
        *((volatile uint32_t*)(base+(0x2AAAUL<<2))) = 0x55555555UL;
        *((volatile uint32_t*)(addr+i)) = flash.sector_erase_cmd;
        flashR_Delay(1000);
        for (int16_t j=0; j<1000; j+=4) {
            uint32_t d = *((volatile uint32_t*)(addr+j));
            if (d != 0xffffffff) { j = 0; }
        }
    }
    return true;
}

__attribute__((section(".ramtext")))
bool flashR_Write(uint32_t addr, uint32_t size, uint32_t* data)
{
    for (uint32_t i=0; i<size; i+=4) {
        uint32_t val = *data++;
        uint32_t base = addr & 0xff000000;
        *((volatile uint32_t*)(base+(0x5555UL<<2))) = 0xAAAAAAAAUL;
        *((volatile uint32_t*)(base+(0x2AAAUL<<2))) = 0x55555555UL;
        *((volatile uint32_t*)(base+(0x5555UL<<2))) = 0xA0A0A0A0UL;
        *((volatile uint32_t*)(addr+i)) = val;
        flashR_Delay(10);
        for (int16_t j=0; j<1000; j++) {
            volatile uint32_t d0 = *((volatile uint32_t*)(addr+j));
            volatile uint32_t d1 = *((volatile uint32_t*)(addr+j));
            if ((d0 == d1) && (d0 == val)) {
                break;
            }
        }
    }
    return true;
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
        for (short j=0; j<1000; j++) {
            uint32_t d = *((volatile uint32_t*)(addr));
            if (d != 0xffffffff) { j = 0; }
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
static const flash_chipinfo_t* flash_GetDeviceInfo(uint32_t id) {
    for (const flash_chipinfo_t* p = flash_chipinfos; p && p->name; p++) {
        if (p->id == id) {
            return p;
        }
    }
    return &flash_chipinfos[0];
}

uint32_t flash_Id(void) {
    return flash.id;
}

uint32_t flash_Size(void) {
    return flash.total_size;
}

bool flash_Valid(void) {
    return (flash.total_size > 0);
}

const char* flash_Name() {
    return flash_GetDeviceInfo(flash.id)->name;
}

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

static rvtoc_t* get_pram_toc(uint32_t base) {
    rvtoc_t* toc = (rvtoc_t*)(base + 0x400 + 32);
    return (toc->id == 0x5F434647) ? toc : 0;
}

bool flash_Program(void* data, uint32_t size) {

    if (!flash_Valid()) {
        printf("Unknown flash device %08x\n", flash.id);
        return false;
    }

    // verify rom image
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

    // keep pram data
    uint32_t version_support_configspace = 0x20250729UL;
    if ((header_new->version >= version_support_configspace) && (header_old->version >= version_support_configspace)) {
        rvtoc_t* pram_toc_old = get_pram_toc(FLASH_PADDR);
        rvtoc_t* pram_toc_new = get_pram_toc((uint32_t)data);
        if (pram_toc_old && pram_toc_old->size) {
            if (pram_toc_new && pram_toc_new->size) {
                if (pram_toc_old->size <= pram_toc_new->size) {
                    uint32_t* pram_src = (uint32_t*)(pram_toc_old->start);
                    uint32_t* pram_dst = (uint32_t*)(pram_toc_new->start - FLASH_PADDR + (uint32_t)data);
                    uint32_t  pram_siz = pram_toc_old->size;
                    for (uint32_t i=0; i<pram_siz; i+=4) {
                        *pram_dst++ = *pram_src++;
                    }
                }
            }
        }
    }

    // print some useful info
    printf("\n");
    printf(" old version....%08lx\n", header_old->version);
    printf(" new version....%08lx\n", header_new->version);
    printf(" new size.......%ld Kb\n", size / 1024);
    printf(" device id......%04x:%04x\n", (flash.id>>16), (flash.id & 0xffff));
    printf(" device name....%04x:%04x\n", flash_Name());
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


bool flash_SectorErase(uint32_t* ptr, uint32_t size) {
    if (flash_Valid()) {
        uint32_t ipl = cpu_SetIPL(7);
        uint32_t base = flash_Unlock();
        if (base) {
            uint32_t addr = (uint32_t)ptr;
            addr = (addr & 0x00ffffff) | (base & 0xff000000);
            flashR_SectorErase(addr, size);
            flash_Lock(base);
            cpu_SetIPL(ipl);
            return true;
        }
        cpu_SetIPL(ipl);
    }
    return false;
}

bool flash_Write(uint32_t* ptr, uint32_t size, uint32_t* data) {

    if (flash_Valid()) {
        uint32_t ipl = cpu_SetIPL(7);
        uint32_t base = flash_Unlock();
        if (base) {
            uint32_t addr = (uint32_t)ptr;
            addr = (addr & 0x00ffffff) | (base & 0xff000000);
            flashR_Write(addr, size, data);
            flash_Lock(base);
            cpu_SetIPL(ipl);
            return true;
        }
        cpu_SetIPL(ipl);
    }
    return false;
}


static uint32_t flash_Identify() {
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

bool flash_Init() {
    static bool inited = false;
    if (inited) {
        return true;
    }

    flash.id = flash_Identify();
    const flash_chipinfo_t* chip = flash_GetDeviceInfo(flash.id);
    uint16_t num_chips = chip->width ? (4 / chip->width) : 0;

    flash.total_size_phy  = ((uint32_t)chip->size) * chip->width * 1024;
    flash.sector_size_phy = ((uint32_t)chip->sector_size) * chip->width;

    flash.total_size = flash.total_size_phy * num_chips;
    flash.sector_size = flash.sector_size_phy * num_chips;
    flash.sector_erase_cmd  = chip->sector_erase; flash.sector_erase_cmd <<= 8;
    flash.sector_erase_cmd |= chip->sector_erase; flash.sector_erase_cmd <<= 8;
    flash.sector_erase_cmd |= chip->sector_erase; flash.sector_erase_cmd <<= 8;
    flash.sector_erase_cmd |= chip->sector_erase;
/*
    printf("id = %08x\n", flash_Id());
    printf("name = %s\n", flash_Name());
    printf("size = %d\n", flash_Size());
    printf("cmd = %08x\n", flash.sector_erase_cmd);
    printf("width = %d\n", chip->width);
    printf("chips = %d\n", num_chips);
    printf("sec_l = %d\n", flash.sector_size);
    printf("sec_p = %d\n", flash.sector_size_phy);
*/
   return flash.id ? true : false;
}
