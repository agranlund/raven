/*-------------------------------------------------------------------------------
 * rvnsd : driver loader
 * (c)2025 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/

#include "sys.h"
#include "driver.h"
#include "midi.h"
#include "lib/ini.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <tos.h>


/*-------------------------------------------------------------------------------
 * device management
 *-----------------------------------------------------------------------------*/

const char* devtype_names[RVDEV_NUMTYPES] = {
    "none      ",
    "raw       ",
    "mix       ",
    "midi_out  ",
    "midi_in   ",
    "audio_out ",
    "audio_in  "
};

typedef struct {
    rvdev_t** devs;
    uint16_t max;
    uint16_t num;
} devlist_t;

void devlist_init(devlist_t* l, uint16_t size) {
    l->num = 0; l->max = 0;
    l->devs = (rvdev_t**)malloc(size * sizeof(rvdev_t*));
    if (l->devs) {
        memset((void*)l->devs, 0, size * sizeof(rvdev_t*));
        l->max = size;
    }
}

bool devlist_add(devlist_t* l, rvdev_t* d) {
    if (l->num >= l->max) {
        uint16_t max = (l->max * 2);
        rvdev_t** devs = (rvdev_t**)realloc((void*)(l->devs), max * sizeof(rvdev_t*));
        if (!devs) {
            return false;
        }
        l->devs = devs;
        l->max = max;
    }
    l->devs[l->num] = d;
    l->num++;
    return true;
}

devlist_t devs;



/*-------------------------------------------------------------------------------
 * driver api
 *-----------------------------------------------------------------------------*/

bool drvapi_publish(rvdev_t* dev) {
    return driver_AddDevice(dev);
}

rvdev_t* drvapi_retrieve(uint32_t type, const char* name) {
    return driver_FindDevice(type, name);
}

void* drvapi_malloc(uint32_t l) {
    void* d = l ? malloc(l) : 0;
    if (d) { memset(d, 0, l); }
    return d;
}

void drvapi_free(void* d) {
    if (d) { free(d); }
}

void* drvapi_realloc(void* d, uint32_t l) {
    if (l == 0) { drvapi_free(d); return 0; }
    if (d == 0) { return drvapi_malloc(l); }
    return realloc(d, l);
}

void drvapi_memset(void* d, uint8_t c, uint32_t l) {
    memset(d, (int)c, (size_t)l);
}

void drvapi_memcpy(void* d, void* s, uint32_t l) {
    memcpy(d, s, (size_t)l);
}

int32_t drvapi_memcmp(const void* d, const void* s, uint32_t l) {
    return (int32_t) memcmp(d, s, (size_t)l);
}

void drvapi_printf(const char* s, ...) {
    va_list args;
    va_start(args, s);
    vprintf(s, args);
    va_end(args); 
}

void drvapi_sprintf(char* d, const char* s, ...) {
    va_list args;
    va_start(args, s);
    vsprintf(d, s, args);
    va_end(args); 
}

int32_t drvapi_strlen(const char* s1) { return strlen(s1); }
char*   drvapi_strcpy(char* s1, const char* s2) { return strcpy(s1, s2); }
char*   drvapi_strncpy(char* s1, const char* s2, int32_t l) { return strncpy(s1, s2, l); }
char*   drvapi_strstr(const char* s1, const char* s2) { return strstr(s1, s2); }
char*   drvapi_strchr(const char* s1, int32_t c) { return strchr(s1, (int)c); }
int32_t drvapi_atoi(const char* s1) { return atol(s1); }


/*-------------------------------------------------------------------------------
 * driver -> rvsnd api
 *-----------------------------------------------------------------------------*/

rvsnd_driver_api_t rvsnd_driver_api =
{
    /* info */
    sys_getcookie,
    0, 0, 0, 0, 0,

    /* isabus */
    0,

    /* inifile */
    ini_GetSection,
    ini_GetStr,
    ini_GetInt,

    /* device */
    drvapi_publish,
    drvapi_retrieve,

    /* system */
    sys_delayus,
    sys_di,
    sys_ei,
    /* debug */
    dprintf,
    /* strings */
    drvapi_printf,
    drvapi_sprintf,
    drvapi_strlen,
    drvapi_strcpy,
    drvapi_strncpy,
    drvapi_strstr,
    drvapi_strchr,
    drvapi_atoi,
    /* memory */
    drvapi_free,
    drvapi_malloc,
    drvapi_realloc,
    drvapi_memset,
    drvapi_memcpy,
    drvapi_memcmp,
};


/*-------------------------------------------------------------------------------
 * driver loader
 *-----------------------------------------------------------------------------*/

typedef struct {
    uint16_t magic; /* 0x601a       */
    uint32_t tlen;  /* size text    */
    uint32_t dlen;  /* size data    */
    uint32_t blen;  /* size bss     */
    uint32_t slen;  /* size symbols */
    uint32_t res0;
    uint32_t res1;
    uint16_t res2;
} prghdr_t;

static void* driver_Create(const char* fname) {
    prghdr_t hdr;
    int32_t fresult;
    uint32_t prgsize;
    uint32_t fsize;
    int16_t fp;
    void* prgmem = 0;
    void* relmem = 0;

    /* open file */
    fresult = Fopen(fname, 0);
    if (fresult < 0) {
        goto fail;
    }

    /* retrieve filesize */
    fp = (int16_t)fresult;
    fsize = Fseek(0, fp, 2);
    if (fsize <= sizeof(prghdr_t)) {
        goto fail;
    }

    /* load header */
    Fseek(0, fp, 0);
    if (Fread(fp, sizeof(prghdr_t), (void*)&hdr) < 0) {
        goto fail;
    }

    /* verify signature */
    if (hdr.magic != 0x601a) {
        goto fail;
    }

    /* allocate memory for text+data+bss */
    prgsize = hdr.tlen + hdr.dlen + hdr.blen;
    prgmem = malloc(prgsize);
    memset(prgmem, 0, prgsize);

    /* load text + data */
    Fseek(sizeof(prghdr_t), fp, 0);
    if (Fread(fp, prgsize - hdr.blen, prgmem) < 0) {
        goto fail;
    }

    /* load relocation table */
    if (hdr.res2 == 0) {
        uint32_t relsize = fsize - Fseek(sizeof(prghdr_t) + hdr.tlen + hdr.dlen + hdr.slen, fp, 0);
        if (relsize > 0) {
            uint32_t reld;
            uint8_t* relo;
            uint8_t* relp;
            relmem = malloc(relsize);
            if (!relmem) {
                goto fail;
            }
            if (Fread(fp, relsize, relmem) < 0) {
                goto fail;
            }
            /* perform fixup */
            relp = (uint8_t*)relmem;
            relo = (uint8_t*)prgmem;
            reld = *((uint32_t*)relp);
            relp += 4;
            while (reld) {
                if (reld == 1) {
                    relo += 0xfe;
                } else {
                    relo += reld;
                    *((uint32_t*)relo) += (uint32_t)prgmem;
                }
                reld = *relp++;
            }
            /* get rid of relocation table */
            free(relmem);
        }
    }

    goto done;

fail:
    if (prgmem) {
        free(prgmem);
        prgmem = 0;
    }
    if (relmem) {
        free(relmem);
        relmem = 0;
    }

done:
    if (fp) {
        Fclose(fp);
    }
    return prgmem;
}

bool driver_Load(const char* fname) {
    /* load driver */
    int32_t result = 0;
    void* driver = driver_Create(fname);
    if (driver) {
        /* prepare ini section, driver takes ownership */
        ini_t* inisection = malloc(sizeof(ini_t));
        if (inisection) {
            ini_GetSection(inisection, &sys.ini, fname);
            /* call driver init */
            sys_icache_clear();
            result = ((uint32_t cdecl (*)(
                    rvsnd_driver_api_t*,        /* arg1 = driver api pointer */
                    ini_t*                      /* arg2 = inifile section for driver */
                    ))driver)(
                        &rvsnd_driver_api,
                        inisection
                    );

            if (result == 0) {
                return true;
            }
            free(inisection);
        }
        free(driver);
    }
    return false;
}

void driver_Init(void) {
    devlist_init(&devs, 32);
    rvsnd_driver_api.isa = sys.isa;
    rvsnd_driver_api.c_cpu = sys.cookie_cpu;
    rvsnd_driver_api.c_mch = sys.cookie_mch;
}

static rvdev_t** driver_GetDeviceEntry(uint32_t type, const char* name) {
    uint16_t i;
    for (i=0; i<devs.num; i++) {
        rvdev_t* dev = devs.devs[i];
        if (dev && (dev->type == type)) {
            const char** names = dev->names;
            for (; *names; names++) {
                if (stricmp(name, *names) == 0) {
                    return &devs.devs[i];
                }
            }
        }
    }
    return 0;    
}

bool driver_AddDevice(rvdev_t* dev) {

    if (dev && (dev->type > RVDEV_NONE) && (dev->type < RVDEV_NUMTYPES)) {
        /* replace existing */
        rvdev_t** devpp_old = driver_GetDeviceEntry(dev->type, dev->names[0]);
        if (devpp_old) {
            dprintf("driver_chg [%08lx][%08lx] %02x [%s] [%s]\n", (uint32_t)(*devpp_old), (uint32_t)dev, dev->type, devtype_names[dev->type], dev->names[0]);
            *devpp_old = dev;
            return true;
        }
        /* add new */
        dprintf("driver_add [%08lx][%08lx] %02x [%s] [%s]\n", 0UL, (uint32_t)dev, dev->type, devtype_names[dev->type], dev->names[0]);
        return driver_AddDevice(dev);
    }
    return false;
}

rvdev_t* driver_FindDevice(uint32_t type, const char* name) {
    rvdev_t** devpp = driver_GetDeviceEntry(type, name);
    return (devpp && *devpp) ? *devpp : 0;
}

uint16_t driver_NumDevs(void) {
    return devs.num;
}

rvdev_t** driver_GetDevs(void) {
    return devs.devs;
}
