#include <stdio.h>
#include "pico.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "hardware/clocks.h"
#include "flash.h"

#define GPIO_AD             0
#define GPIO_ALE            19
#define GPIO_DBS0           20
#define GPIO_DBS1           21
#define GPIO_DBS2           22
#define GPIO_DBS3           26
#define GPIO_DRD            27
#define GPIO_DWR            28

#define GPIO_AD_MASK        (0x7FFFF<<GPIO_AD)
#define GPIO_ALE_MASK       (1<<GPIO_ALE)
#define GPIO_DBS0_MASK      (1<<GPIO_DBS0)
#define GPIO_DBS1_MASK      (1<<GPIO_DBS1)
#define GPIO_DBS2_MASK      (1<<GPIO_DBS2)
#define GPIO_DBS3_MASK      (1<<GPIO_DBS3)
#define GPIO_DRD_MASK       (1<<GPIO_DRD)
#define GPIO_DWR_MASK       (1<<GPIO_DWR)

#define GPIO_MASK           (GPIO_AD_MASK | GPIO_ALE_MASK | GPIO_DBS0_MASK | GPIO_DBS1_MASK | GPIO_DBS2_MASK | GPIO_DBS3_MASK | GPIO_DRD_MASK | GPIO_DWR_MASK)

#define DBMASK01            (GPIO_DBS0_MASK | GPIO_DBS1_MASK)
#define DBMASK23            (GPIO_DBS2_MASK | GPIO_DBS3_MASK)

// ------------------------------------------------------------------
// device info
// ------------------------------------------------------------------
struct TId { uint mid; uint did; const char* str; };

struct TId ManufacturerIds[] = {
    {0x0000, 0x0000, "Unknown"},
    {0xFFFF, 0x0000, "Unknown"},
    {0x00BF, 0x0000, "SST"},
    {0xBFBF, 0x0000, "SST"},
};

struct TId DeviceIds[] = {
    {0x0000, 0x0000, "Unknown"},
    {0xFFFF, 0xFFFF, "Unknown"},
    {0xBFBF, 0xB5B5, "SST39SF010A"},          // 8bit
    {0xBFBF, 0xB6B6, "SST39SF020A"},
    {0xBFBF, 0xB7B7, "SST39SF040"},
    {0xBFBF, 0xD5D5, "SST39LF/VF010"},
    {0xBFBF, 0xD6D6, "SST39LF/VF020"},
    {0xBFBF, 0xD7D7, "SST39LF/VF040"},
    {0x00BF, 0x2321, "SST39LF/VF401C"},     // 16bit
    {0x00BF, 0x2322, "SST39LF/VF402C"},
    {0x00BF, 0x233B, "SST39LF/VF801C"},
    {0x00BF, 0x233A, "SST39LF/VF802C"},
    {0x00BF, 0x234B, "SST39VF1601"},
    {0x00BF, 0x234F, "SST39VF1601C"},
    {0x00BF, 0x234A, "SST39VF1602"},
    {0x00BF, 0x234E, "SST39VF1602C"},
    {0x00BF, 0x235B, "SST39VF3201"},
    {0x00BF, 0x235A, "SST39VF3202"},
    {0x00BF, 0x236D, "SST39VF6401B"},
    {0x00BF, 0x236C, "SST39VF6402B"},
};


// ------------------------------------------------------------------
// timing
// ------------------------------------------------------------------
uint tALEH;
uint tALEL;
uint tRC;       // read cycle
uint tWP;       // write pulse
uint tIDA;      // id access and exit
uint tSE;       // sector erase
uint tSCE;      // chip erase
uint tBP;       // program time

uint sector_erase_cmd;
uint sector_erase_siz;

uint GetCyclesForNs(uint ns) {

    float mhz = clock_get_hz(clk_sys) / 1000000;
    float cycles_per_ns = 1000 / mhz;
    return (uint) (cycles_per_ns * ns);
}

uint GetCyclesForMs(uint ns) {
    return ns * (clock_get_hz(clk_sys) / 1000000);
}

static inline void WaitCycles(uint cycles) {
    busy_wait_at_least_cycles(cycles);
}

// ------------------------------------------------------------------
// Helper functions
// ------------------------------------------------------------------
static inline uint ByteSwap16(uint data)
{
    uint swapped =  ((data & 0x0000ff00) >> 8) |
                    ((data & 0x000000ff) << 8);
    return swapped;
}

static inline uint ByteSwap32(uint data)
{
    uint swapped =  ((data & 0xff000000) >> 24) |
                    ((data & 0x00ff0000) >>  8) |
                    ((data & 0x0000ff00) <<  8) |
                    ((data & 0x000000ff) << 24);
    return swapped;
}

static inline uint GetData01(uint data) { return ((data >> 16) & 0xffff); }
static inline uint GetData23(uint data) { return ((data >>  0) & 0xffff); }

static inline uint GetData(uint data01, uint data23) {
    return ((data01 & 0xffff) << 16) | ((data23 & 0xffff) <<  0);
}

static inline void FlashLatchAddress(uint addr)
{
    gpio_put_masked(GPIO_AD_MASK, addr);
    gpio_put_masked(GPIO_ALE_MASK, 0xffffffff);
    WaitCycles(tALEH);
    gpio_put_masked(GPIO_ALE_MASK, 0x00000000);
    WaitCycles(tALEL);
}

static inline uint FlashRead16(uint dbmask)
{
    gpio_set_dir_in_masked(GPIO_AD_MASK);
    gpio_put_masked(dbmask | GPIO_DRD_MASK, 0x00000000);
    WaitCycles(tRC);
    uint data = gpio_get_all() & 0xffff;
    gpio_put_masked(dbmask | GPIO_DRD_MASK, 0xffffffff);
    gpio_set_dir_out_masked(GPIO_AD_MASK);
    WaitCycles(tRC);
    return data;
}

static inline void FlashWrite16(uint dbmask, uint data)
{
    gpio_put_masked(dbmask | GPIO_DWR_MASK, 0x00000000);
    gpio_put_masked(GPIO_AD_MASK, data);
    WaitCycles(tWP);
    gpio_put_masked(dbmask | GPIO_DWR_MASK, 0xffffffff);
    WaitCycles(tWP);
}

static inline void FlashCommand16(uint dbmask, uint addr, uint cmd)
{
    FlashLatchAddress(0x5555); FlashWrite16(dbmask, 0xAAAA);
    FlashLatchAddress(0x2AAA); FlashWrite16(dbmask, 0x5555);
    FlashLatchAddress(addr); FlashWrite16(dbmask, cmd);
}

static inline void FlashProg16(uint dbmask, uint addr, uint data)
{
    FlashCommand16(dbmask, 0x5555, 0xA0A0);
    FlashLatchAddress(addr);
    gpio_put_masked(GPIO_AD_MASK, data);
    gpio_put_masked(dbmask | GPIO_DWR_MASK, 0x00000000);
    WaitCycles(tWP);
    gpio_put_masked(GPIO_DWR_MASK, 0xffffffff);
    WaitCycles(tBP);
    gpio_put_masked(dbmask, 0xffffffff);
}


// ------------------------------------------------------------------
// 32bit helpers
// ------------------------------------------------------------------

static uint FlashRead32(uint addr)
{
    FlashLatchAddress(addr);
    uint d01 = FlashRead16(DBMASK01);
    uint d23 = FlashRead16(DBMASK23);
    uint data = GetData(d01, d23);
    return data;
}

static void FlashWrite32(uint addr, uint data)
{
    FlashLatchAddress(addr);
    FlashWrite16(DBMASK01, GetData01(data));
    FlashWrite16(DBMASK23, GetData23(data));
}

static void FlashCommand32(uint addr, uint cmd)
{
    FlashCommand16(DBMASK01, addr, GetData01(cmd));
    FlashCommand16(DBMASK23, addr, GetData23(cmd));
}

static void FlashProg32(uint addr, uint data)
{
    FlashProg16(DBMASK01, addr, GetData01(data));
    FlashProg16(DBMASK23, addr, GetData23(data));
}

// ------------------------------------------------------------------
// Info
// ------------------------------------------------------------------
const char* flash_ManufacturerString(uint mid)
{
    int idx = 0;
    for (int i=0; i<sizeof(ManufacturerIds) / sizeof(ManufacturerIds[0]); i++) {
        if (mid == ManufacturerIds[i].mid) {
            idx = i;
            break;
        }
    }
    return ManufacturerIds[idx].str;
}

const char* flash_DeviceString(uint mid, uint did)
{
    int idx = 0;
    for (int i=0; i<sizeof(DeviceIds) / sizeof(DeviceIds[0]); i++) {
        if ((mid == DeviceIds[i].mid) && (did == DeviceIds[i].did)) {
            idx = i;
            break;
        }
    }
    return DeviceIds[idx].str;
}


bool flash_Identify(uint* mid, uint* did)
{
    *mid = 0;
    *did = 0;

    FlashCommand32(0x5555, 0x90909090);
    WaitCycles(tIDA);
    uint fmid = FlashRead32(0x0000);
    uint fdid = FlashRead32(0x0001);
    FlashCommand32(0x5555, 0xF0F0F0F0);
    WaitCycles(tIDA);

    uint fmid0 = (fmid & 0xffff0000) >> 16;
    uint fmid1 = (fmid & 0x0000ffff) >>  0;
    uint fdid0 = (fdid & 0xffff0000) >> 16;
    uint fdid1 = (fdid & 0x0000ffff) >>  0;

    if (fmid0 == fmid1 && fdid0 == fdid1 && fmid0 != 0xffff && fdid0 != 0xffff)
    {
        *mid = fmid0;
        *did = fdid0;
    }
    return false;
}


// ------------------------------------------------------------------
// Read
// ------------------------------------------------------------------
uint flash_Read(uint addr)
{
    return FlashRead32(addr >> 2);
}

// ------------------------------------------------------------------
// Write
// ------------------------------------------------------------------
void flash_Write(uint addr, uint data)
{
    FlashProg32(addr >> 2, data);
}


// ------------------------------------------------------------------
// Erase
// ------------------------------------------------------------------
void flash_Erase()
{
    FlashCommand32(0x5555, 0x80808080);
    FlashCommand32(0x5555, 0x10101010);
    WaitCycles(tSCE);
}

void flash_EraseSector(uint sector)
{
    uint addr = sector * sector_erase_siz;
    FlashCommand32(0x5555, 0x80808080);
    FlashCommand32(addr, sector_erase_cmd);
    WaitCycles(tSE);
}

uint flash_GetSector(uint addr)
{
    return addr / sector_erase_siz;
}



// ------------------------------------------------------------------
// setup
// ------------------------------------------------------------------
bool flash_Init()
{
    gpio_init_mask(GPIO_MASK);
    gpio_set_dir_out_masked(GPIO_MASK);
    gpio_put_masked(GPIO_DBS0_MASK |
                    GPIO_DBS1_MASK |
                    GPIO_DBS2_MASK |
                    GPIO_DBS3_MASK |
                    GPIO_DWR_MASK |
                    GPIO_DRD_MASK, 0xffffffff);
    gpio_put_masked(GPIO_AD_MASK | GPIO_ALE_MASK, 0x00000000);

    tALEH = GetCyclesForNs(90);
    tALEL = GetCyclesForNs(10);

    tRC  = GetCyclesForNs(70);
    tWP  = GetCyclesForNs(40);
    tIDA = GetCyclesForNs(150);
    tSE  = GetCyclesForMs(25);
    tSCE = GetCyclesForMs(100);
    tBP  = GetCyclesForNs(20000);

    // assume 8bit SST39xF0x0 (ex; SST39SF040)
    sector_erase_siz = 4 * (4 * 1024);
    sector_erase_cmd = 0x30303030;

    // identify chip
    uint mid = 0; uint did = 0;
    flash_Identify(&mid, &did);

    // 16bit SST39xFxx0xC (ex; SST39LF401C)
    if ((did & 0xff00) == 0x2300) {
        sector_erase_cmd = 0x50505050;
        sector_erase_siz = 2 * (4 * 1024);
    }

    return true;
}
