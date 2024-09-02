#include "sys.h"
#include "lib.h"
#include "config.h"
#include "hw/rtc.h"

#define CFG_MAX         32
#define CFG_ADDR_MIN    0x30
#define CFG_ADDR_MAX    0x3B

static const cfg_entry_t* cfgs[CFG_MAX];
static uint32_t cfgcnt = 0;

bool cfg_Init()
{
    return true;
}

static uint8_t cfg_GetRtcAddress(const cfg_entry_t* entry) {
    if (entry) {
        int addr = CFG_ADDR_MAX - entry->idx;
        if ((addr >= CFG_ADDR_MIN) && (addr <= CFG_ADDR_MAX))
            return (uint8_t)addr;
    }
    return 0;
}

void cfg_Add(const cfg_entry_t* cfg, int num)
{
    for (int i=0; i<num; i++) {
        const cfg_entry_t* cfgnew = &cfg[i];

        // verify config
        if (cfg_GetRtcAddress(cfgnew) == 0) {
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

int cfg_GetValue(const cfg_entry_t* entry)
{
    uint8_t v = 0;
    if (entry) {
        rtc_GetRam(cfg_GetRtcAddress(entry), &v, 1);
        v >>= entry->shift;
        v &= entry->mask;
    }
    return (int)v;
}

void cfg_SetValue(const cfg_entry_t* entry, int val)
{
    if (entry) {

        if (val < 0)
            val = 0;
        if (val > entry->max)
            val = entry->max;

        uint8_t v = 0;
        uint8_t a = cfg_GetRtcAddress(entry);
        if (a != 0) {
            rtc_GetRam(a, &v, 1);
            v &= ~(entry->mask << entry->shift);
            v |= ((((uint8_t)val) & entry->mask) << entry->shift);
            rtc_SetRam(a, &v, 1);
        }
    }
}


