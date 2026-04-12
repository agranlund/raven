#include "sys.h"
#include "lib.h"
#include "config.h"
#include "hw/pram.h"

#define CFG_MAX 1024
static const cfg_entry_t* cfgs[CFG_MAX] __attribute__((aligned(4)));
static uint32_t cfgcnt __attribute__((aligned(4)));

const cfg_entry_t confs[] = {
    { "com1_baud",      0x01, 0b1111111111111111, 115200, 2400, 1000000, 100 },

    { "ikbd_speed",     0x04, 0b0000000000001111, 5, 1, 8, 0 },

    // these should be moved to atari specific code
    { "st_ram_size",    0x40, 0b0000000000000111, 0, 0, 4, 0 },
    { "st_ram_cache",   0x40, 0b0000000000110000, 0, 0, 3, 0 },
    { "tt_ram_cache",   0x40, 0b0000000011000000, 0, 0, 3, 0 },
};

/*
const cfg_entry_t confs[] = {
    { "st_ram_size",    0,  0, 0x30, 3, 0, 0, 4, 0},
    { "st_ram_cache",   0,  0, 0x31, 2, 0, 0, 3, 0},
    { "tt_ram_cache",   0,  0, 0x31, 2, 2, 0, 3, 0},
    { "ikbd_baud",      0,  0, 0x32, 4, 0, 0, 7, 0 },
    { "cpuflags",       0,  0, 0x3A, 6, 0, 0, 0x3F, 0x3F },
    { "boot_enable",    0,  0, 0x3B, 1, 0, 0, 1,    1 },
    { "boot_delay",     0,  0, 0x3B, 4, 4, 0, 15,   0 },
};
*/

bool cfg_Init() {
    cfg_Add(confs, sizeof(confs) / sizeof(confs[0]));
    return true;
}

void cfg_Reset() {
    for (int i=0; i<cfgcnt; i++) {
        cfg_SetValue(cfgs[i], cfgs[i]->def);
    }
}

void cfg_Add(const cfg_entry_t* cfg, int num) {
    for (short i=0; i<num; i++) {
        const cfg_entry_t** found = 0;
        const cfg_entry_t* cfgnew = &cfg[i];
        for (short j=0; j<cfgcnt && !found; j++) {
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
        }
    }
}

int cfg_Num() {
    return (int)cfgcnt;
}

static uint16_t mask_offset(uint16_t mask) {
    for (uint16_t i = 0; i < 16; i++) {
        if (mask & (1 << i)) { return i; }
    }
    return 16;
}

/*
static uint16_t mask_size(uint16_t mask) {
    uint16_t size = 0;
    for (uint16_t i = mask_offset(mask); i < 16; i++, size++) {
        if ((mask & (1 << i)) == 0) { break; }
    } return size;
}
*/

const cfg_entry_t* cfg_Get(int idx) {
    return ((idx >= 0) && (idx < cfgcnt)) ? cfgs[idx] : 0;
}

const cfg_entry_t* cfg_Find(const char* name) {
    for (short i=0; i<cfgcnt; i++) {
        if (strcmp(cfgs[i]->name, name) == 0) {
            return cfgs[i];
        }
    }
    return 0;
}

uint32_t cfg_GetValue(const cfg_entry_t* entry) {
    uint32_t v32 = 0;
    if (entry) {
        uint8_t addr = entry->addr & 0xff;
        uint16_t v16 = pram_Get(addr);
        v32 = (uint32_t)((v16 & entry->mask) >> mask_offset(entry->mask));
        if (entry->div) { v32 *= entry->div; }
        if (v32 == 0) { v32 = entry->def; }
    }
    return v32;
}

void cfg_SetValue(const cfg_entry_t* entry, uint32_t val) {
    if (entry) {
        if (val < entry->min) { val = entry->min; }
        if (entry->max && (val > entry->max)) { val = entry->max; }
        if (val == entry->def) { val = 0; }
        if (entry->div) { val /= entry->div; }
        uint8_t addr = (entry->addr & 0xff);
        uint16_t v16 = (uint16_t)val;
        v16 = (v16 << mask_offset(entry->mask)) & entry->mask;
        pram_Set(addr, v16 | (pram_Get(addr) & ~entry->mask));
    }
}
