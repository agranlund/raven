#include "sys.h"
#include "lib.h"
#include "config.h"
#include "hw/rtc.h"

#define CFG_VERSION_NUM     1
#define CFG_VERSION_ADDR    0x3F

#define CFG_MAX             32
#define CFG_ADDR_MIN        0x30
#define CFG_ADDR_MAX        0x3B

static const cfg_entry_t* cfgs[CFG_MAX];
static uint32_t cfgcnt = 0;
static bool cfgvalid = false;


const cfg_entry_t confs[] = {
    { "st_ram_size",    0,  0, 0x30, 3, 0, 0, 4, 0},
    { "st_ram_cache",   0,  0, 0x31, 2, 0, 0, 3, 0},
    { "tt_ram_cache",   0,  0, 0x31, 2, 2, 0, 3, 0},
 
    { "cpuflags",       0,  0, 0x3A, 6, 0, 0, 0x3F, 0x3F },
    { "boot_enable",    0,  0, 0x3B, 1, 0, 0, 1,    1 },
    { "boot_delay",     0,  0, 0x3B, 4, 4, 0, 15,   0 },
};

bool cfg_Init()
{
    cfg_Add(confs, sizeof(confs) / sizeof(confs[0]));
    cfgvalid = rtc_Valid();
    if (cfgvalid) {
        uint8_t ver = 0;
        rtc_Read(CFG_VERSION_ADDR, (uint8_t*)&ver, 1);
        if (ver != CFG_VERSION_NUM) {
            puts("ResetCfg");
            cfg_Reset();
        }
    }
    return cfgvalid;
}

void cfg_Reset() {
    for (int i=0; i<cfgcnt; i++) {
        uint32_t val = cfgs[i]->def;
        if (cfgs[i]->flags & CFGFLAG_INVERT) {
            val = ~val;
        }
        cfg_SetValue(cfgs[i], val);
    }
    uint8_t ver = CFG_VERSION_NUM;
    rtc_Write(CFG_VERSION_ADDR, (uint8_t*)&ver, 1);
}

static uint8_t cfg_GetRtcAddress(const cfg_entry_t* entry) {
    if (entry && (entry->addr >= CFG_ADDR_MIN) && (entry->addr <= CFG_ADDR_MAX))
        return entry->addr;
    return 0xff;
}

static uint32_t cfg_GetMask(const cfg_entry_t* entry) {
    uint32_t mask = 0;
    for (int i=0; i<entry->bits; i++) {
        mask <<= 1; mask |= 1;
    }
    return mask;
}

void cfg_Add(const cfg_entry_t* cfg, int num)
{
    for (int i=0; i<num; i++) {
        // verify config
        const cfg_entry_t* cfgnew = &cfg[i];
        if (cfg_GetRtcAddress(cfgnew) == 0xff) {
            continue;
        }
        // look for existing entry with same name
        const cfg_entry_t** found = 0;
        for (int j=0; j<cfgcnt && !found; j++) {
            if (strcmp(cfgs[j]->name, cfgnew->name) == 0) {
                found = &cfgs[j];
            }
        }
        // create new or replace found entry
        if (found) {
            *found = cfgnew;
        } else if (cfgcnt < CFG_MAX) {
            cfgs[cfgcnt] = cfgnew;
            cfgcnt++;
        } else {
        }
    }
}

int cfg_Num() {
    return (int)cfgcnt;
}

const cfg_entry_t* cfg_Get(int idx) {
    return ((idx >= 0) && (idx < cfgcnt)) ? cfgs[idx] : 0;
}

const cfg_entry_t* cfg_Find(const char* name)
{
    const cfg_entry_t* found = 0;
    for (int i=0; i<cfgcnt && !found; i++) {
        if (strcmp(cfgs[i]->name, name) == 0) {
            found = cfgs[i];
        }
    }
    return found;
}

uint32_t cfg_GetValue(const cfg_entry_t* entry)
{
    if (entry) {
        if (!cfgvalid) {
            return entry->def;
        } else {
            uint8_t siz = (entry->bits + 7) >> 3;
            if (siz > 0) {
                uint32_t v = 0;
                rtc_Read(cfg_GetRtcAddress(entry), (uint8_t*)&v, siz);
                v >>= (((4-siz) << 3) + entry->shift);
                if (entry->flags & CFGFLAG_INVERT) {
                    v = ~v;
                }
                v &= cfg_GetMask(entry);
                return v;
            }
        }
    }
    return 0;
}

void cfg_SetValue(const cfg_entry_t* entry, uint32_t val)
{
    if (cfgvalid && entry) {

        if (val < 0)
            val = 0;
        if (val > entry->max)
            val = entry->max;
        if (entry->flags & CFGFLAG_INVERT)
            val = ~val;

        uint8_t a = cfg_GetRtcAddress(entry);
        if (a != 0xff) {
            uint8_t siz = (entry->bits + 7) >> 3;
            if (siz > 0) {
                uint32_t v = 0;
                rtc_Read(a, (uint8_t*)&v, siz);
                uint32_t s = ((4-siz) << 3) + entry->shift;
                uint32_t m = cfg_GetMask(entry);
                v &= ~(m << s);
                v |= ((val & m) << s);
                rtc_Write(a, (uint8_t*)&v, siz);
            }
        }
    }
}
