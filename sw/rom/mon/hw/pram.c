#include "sys.h"
#include "raven.h"
#include "hw/cpu.h"
#include "hw/flash.h"


// todo: get rid of index checksum.
// each entry is a 32bit atomic write anyway and we can have
// those extra 8bit for something more useful.


#define PRAM_MAGIC      0x50524D01          /* 'PRM' + version */

static uint32_t pram_start;
static uint32_t pram_size;
static uint32_t pram_vars_start;
static uint32_t pram_vars_size;
static uint32_t pram_buff_start;
static uint32_t pram_buff_size;
static uint16_t pram_cache[256] __attribute__((aligned(4)));

static uint32_t pram_encode(uint8_t idx, uint16_t val) {
    uint8_t uid1 = idx;
    uint8_t uid2 = ~idx;
    uint32_t v = val;
    v <<= 8; v |= uid2;
    v <<= 8; v |= uid1;
    return v;
}

static bool pram_decode(uint32_t entry, uint8_t* idx_out, uint16_t* val_out) {
    uint8_t uid1 = entry & 0xff;
    uint8_t uid2 = (~(entry >> 8)) & 0xff;
    if ((uid1 == 0xff) || (uid1 != uid2)) {
        return false;
    }
    if (idx_out) { *idx_out = uid1; }
    if (val_out) { *val_out = (uint16_t)(entry >> 16); }
    return true;
}

void pram_Clear(void) {
    memset(pram_cache, 0, 256 * 2);
    if (pram_start && pram_size) {
        uint32_t magic = PRAM_MAGIC;
        flash_SectorErase((uint32_t*)pram_start, pram_size);
        flash_Write((uint32_t*)pram_start, 4, &magic);
    }
}

static void pram_WriteInternal(uint8_t idx, uint16_t val) {
    if (pram_vars_start && pram_vars_size) {
        uint32_t* ptr = (uint32_t*)pram_vars_start;
        for (int i=0; i<(pram_vars_size>>2); i++) {
            if (ptr[i] == 0xffffffff) {
                uint32_t v = pram_encode(idx, val);
                flash_Write(&ptr[i], 4, &v);
                return;
            }
        }
    }
}

static void pram_Compact(void) {
    if (pram_start && pram_size) {
        uint32_t magic = PRAM_MAGIC;
        flash_SectorErase((uint32_t*)pram_start, pram_size);
        for (short i=1; i<255; i++) {
            if (pram_cache[i] != 0) {
                pram_WriteInternal(i, pram_cache[i]);
            }
        }
        flash_Write((uint32_t*)pram_start, 4, &magic);
    }
}

static void pram_Write(uint8_t idx, uint16_t val) {
    if (pram_vars_start && pram_vars_size) {
        for (int j=0; j<2; j++) {
            uint32_t* ptr = (uint32_t*)pram_vars_start;
            for (int i=0; i<(pram_vars_size>>2); i++) {
                if (ptr[i] == 0xffffffff) {
                    uint32_t v = pram_encode(idx, val);
                    flash_Write(&ptr[i], 4, &v);
                    return;
                }
            }
            pram_Compact();
        }
    }
}

bool pram_Init(void) {
    memset(pram_cache, 0, 256 * 2);
    rvtoc_t* toc = sys_GetToc(RV_TOC__CFG);
    if (toc) {
        pram_start = toc->start;
        pram_size  = toc->size;

        pram_vars_start = pram_start + 4;
        pram_buff_start = pram_start + (pram_size >> 1);

        pram_vars_size  = pram_buff_start - pram_vars_start;
        pram_buff_size  = (pram_start + pram_size) - pram_buff_start;

        uint32_t* ptr = (uint32_t*)pram_start;
        if (ptr) {
            uint32_t magic = PRAM_MAGIC;
            if (ptr[0] != magic) {
                flash_SectorErase(ptr, pram_size);
                flash_Write(ptr, 4, &magic);
            } else {
                ptr = (uint32_t*)pram_vars_start;
                for (int i=0; i<(pram_vars_size>>2); i++) {
                    uint8_t idx; uint16_t val;
                    if (!pram_decode(ptr[i], &idx, &val)) {
                        break;
                    } else {
                        pram_cache[idx] = val;
                    }
                }
            }
            return true;
        }
    }
    return false;
}

uint16_t pram_Get(uint8_t idx) {
    return pram_cache[idx];
}

void pram_Set(uint8_t idx, uint16_t val) {
    pram_Write(idx, val);
    pram_cache[idx] = val;
}
