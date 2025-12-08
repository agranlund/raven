/*-------------------------------------------------------------------------------
 * rvnsd
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

#define INI_IMPL
#define INI_FILE_TOS 1
#include "lib/ini.h"
#include "isa/isa.h"

#include "sys.h"
#include "driver.h"
#include "midi.h"
#include "mixer.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "mint/cookie.h"
#include "mint/osbind.h"
#include "mint/falcon.h"
#include "pubapi.h"

rvsnd_sysvars_t sys;

extern void pubapi_Init(void);

/* ------------------------------------------------------------------- */
#if defined(DEBUG) && DEBUG
#if defined(DEBUG_RAVEN) && DEBUG_RAVEN
#undef DEBUG_DEV
#define DEBUG_DEV 7
#include "lib/raven.h"
static void dputc(char c) {
    while((*((uint8_t*)(RV_PADDR_UART2 + 0x14)) & (1 << 5)) == 0);
    *((char*)(RV_PADDR_UART2 + 0x00)) = c;
}
#else
static void dputc(char c) {
    while(Bcostat(DEBUG_DEV) == 0);
    Bconout(DEBUG_DEV, (int16_t)c);
}
#endif
static char printbuf[256];
void dprintf(const char* s, ...)
{
    va_list args;
    char* buf = printbuf;
    va_start(args, s);
    vsprintf(buf, s, args);
    va_end(args); 
    while(*buf != 0) {
        dputc(*buf++);
    }
    #if (DEBUG_DEV == 2)
    dputc('\r');
    #endif
}
#else
void dprintf(const char* s, ...) { (void)s; }
#endif /* DEBUG*/



/* ------------------------------------------------------------------- */
bool sys_getcookie(const char* c, uint32_t* v) {
    uint32_t rv; int32_t rr = Getcookie(*((long*)c), (long*)&rv);
    if (rr != C_FOUND) { rv = 0; }
    if (v) { *v = rv; }
    return (rr == C_FOUND) ? true : false;    
}

void sys_setcookie(const char* c, uint32_t v) {
    /* find free slot */
    uint32_t id = *((uint32_t*)c);
    int32_t cookies_size = 0;
	int32_t cookies_used = 0;
    int32_t cookies_avail = 0;
	uint32_t* jar = (uint32_t*) *((uint32_t*)0x5a0);
	uint32_t* cok = jar;
	while (1) {
		cookies_used++;
		if (cok[0] == id) {
			cok[1] = v;
			return;
		} else if (cok[0] == 0) {
            cookies_size = cok[1];
			break;
		}
		cok += 2;
	}
    /* grow jar when necessary */
    cookies_avail = cookies_size - cookies_used;
	if (cookies_avail <= 0) {
        uint32_t* newjar;
        int32_t oldsize = (2*4*(cookies_size + 0));
        int32_t newsize = (2*4*(cookies_size + 8));
        cookies_size += 8;
        newjar = (uint32_t*)malloc(newsize);
        memcpy(newjar, jar, oldsize);
        *((uint32_t*)0x5a0) = (uint32_t)newjar;
        jar = newjar;
	}
	/* install cookie */
	jar[(cookies_used<<1)-2] = id;                  /* overwrite end marker */
	jar[(cookies_used<<1)-1] = v;
	jar[(cookies_used<<1)+0] = 0;                   /* write new end marker */
	jar[(cookies_used<<1)+1] = cookies_size;
}

/* ------------------------------------------------------------------- */
void sys_icache_clear(void) {
    if (sys.cookie_cpu >= 40) {
        extern void sys_icache_clear40(void);
        sys_icache_clear40();
    } else if (sys.cookie_cpu >= 20) {
        extern void sys_icache_clear20(void);
        sys_icache_clear20();
    }
}

uint32_t sys_icache_enable(void) {
    if (sys.cookie_cpu >= 40) {
        extern uint32_t sys_icache_enable40(void);
        return sys_icache_enable40();
    } else if (sys.cookie_cpu >= 20) {
        extern uint32_t sys_icache_enable20(void);
        return sys_icache_enable20();
    } return 0;
}

void sys_icache_restore(uint32_t cacr) {
    if (sys.cookie_cpu >= 40) {
        extern void sys_icache_restore40(uint32_t);
        sys_icache_restore40(cacr);
    } else if (sys.cookie_cpu >= 20) {
        extern void sys_icache_restore20(uint32_t);
        sys_icache_restore20(cacr);
    }
}

/* ------------------------------------------------------------------- */
void sys_delayus(uint32_t us) {
    isa_delay(us);
}

/* ------------------------------------------------------------------- */
static void sys_InitIsa(void) {
    uint32_t cacr = sys_icache_enable();
    sys.isa = isa_init();
    isa_delay(1);
    sys_icache_restore(cacr);
}

/* ------------------------------------------------------------------- */
static bool sys_Init(void) {
    uint32_t cookie;

    memset((void*)&sys, 0, sizeof(rvsnd_sysvars_t));
    if (Getcookie(C__CPU, (long*)&cookie) == C_FOUND) { sys.cookie_cpu = cookie; }
    if (Getcookie(C__MCH, (long*)&cookie) == C_FOUND) { sys.cookie_mch = cookie; }
    if (Getcookie(C__SND, (long*)&cookie) == C_FOUND) { sys.cookie_snd = cookie; }

    /* load ini file */
    if (!ini_Load(&sys.ini, "rvsnd.inf")) {
        ini_Load(&sys.ini, "c:\\rvsnd.inf");
    }

    /* calibrate delays */
    cookie = sys_icache_enable();
    sys_delayus(1);
    sys_icache_restore(cookie);

    /* initialize isabus */
    sys_InitIsa();
    return true;
}

/* ------------------------------------------------------------------- */
extern void sys_InstallBios(void);


void load_drivers(void) {
    ini_t ini;
    dprintf("load drivers...\n");
    if (ini_GetSection(&ini, &sys.ini, "drivers")) {
        const char* name = ini.data;
        while (name) {
            /* get next name or wildcard search from inifile */
            name = ini_NextEntry(&ini, name);
            if (name && *name) {
                int16_t result;
                _DTA *dtaold, dta;
                dtaold = Fgetdta();
                Fsetdta(&dta);
                result = Fsfirst(name, 0);
                while (result >= 0) {
                    /* ignore folders or volume labels */
                    if ((dta.dta_attribute >= 0) && (dta.dta_attribute != 0x10)) {
                        /* create full filename*/
                        char fullname[256]; char* s;
                        strcpy(fullname, name);
                        s = strrchr(fullname, '\\');
                        if (!s) { s = strrchr(fullname, '/'); }
                        if (s) { strcpy(s+1, dta.dta_name); }
                        /* load driver */
                        driver_Load(fullname);
                    }
                    result = Fsnext();
                }
                Fsetdta(dtaold);
                name += strlen(name);
            } else {
                break;
            }
        }
    }
}

/* ------------------------------------------------------------------- */
long super_main(int args, char** argv) {
    UNUSED(args); UNUSED(argv);

    dprintf("init...\n");

    /* system setup */
    sys_Init();
    driver_Init();
    midi_Init();
    mixer_Init();

    /* temp load drivers */
    /* todo: driver load is not happy with copyback enabled */
    load_drivers();

    /* prepare mixer */
    mixer_Setup();

    /* inject bios overrides */
    dprintf("install xbios...\n");
    sys_InstallBios();

    /* init public api */
    dprintf("finalize...\n");
    pubapi_Init();

    /* apply settings from inifile */
    {
        ini_t ini;
        const char* name;

        /* mixer mapping*/
        if (ini_GetSection(&ini, &sys.ini, "mixer-map")) {
            const char* skey; const char* sval;
            const char* where = ini.data;
            while (where) {
                where = ini_NextKeyValue(&ini, where, &skey, &sval);
                if (where && skey && sval) {
                    mixer_ctr_t* ctr1 = mixer_FindCtr(skey);
                    mixer_ctr_t* ctr2 = mixer_FindCtr(sval);
                    if (ctr1 && ctr1->ctr && (ctr1->ctr != ctr2->ctr)) {
                        /* get new target xbios value */
                        uint8_t xb = 0;
                        if (ctr2 && ctr2->ctr) {
                            xb = ((ctr2->id & 0xff00) == 0) ? ctr2->ctr->id : ctr2->ctr->flags;
                        }
                        /* apply */
                        if ((ctr1->id & 0xff00) == 0) {
                            /* target is also a system control */
                            if (xb) {
                                ctr1->ctr->id = xb;
                            }
                        } else {
                            /* target is a normal control */
                            ctr1->ctr->flags = xb;
                        }
                    }
                }
            }
        }

        /* mixer volumes */
        if (ini_GetSection(&ini, &sys.ini, "mixer-vol")) {
            const char* skey; const char* sval;
            const char* where = ini.data;
            while (where) {
                where = ini_NextKeyValue(&ini, where, &skey, &sval);
                if (where && skey && sval) {
                    uint16_t val = atoi(sval);
                    val = (val <= 0) ? 0 : (val >= 255) ? 255 : val;
                    mixer_SetValueByName(skey, val);
                }
            }
        }

        /* midi */
        if (ini_GetSection(&ini, &sys.ini, "midi")) {
            name = ini_GetStr(&ini, "in", 0);
            if (name) {
                rvdev_t* dev = driver_FindDevice(RVDEV_MIDI_IN, name);
                if (dev) { midi_SetRxDevice(rvdev_cast(rvdev_midirx_t, dev)); }
            }
            name = ini_GetStr(&ini, "out", 0);
            if (name) {
                rvdev_t* dev = driver_FindDevice(RVDEV_MIDI_OUT, name);
                if (dev) { midi_SetTxDevice(rvdev_cast(rvdev_miditx_t, dev)); }
            }
        }

        /* sound */
    #if 0
        if (ini_GetSection(&ini, &sys.ini, "sound")) {
            name = ini_GetStr(&ini, "in", 0);
            if (name) {
                rvdev_t* dev = driver_FindDevice(RVDEV_AUDIO_IN, name);
                if (dev) { midi_SetRxDevice(rvdev_cast(rvdev_audio_rx_t, dev)); }
            }
            name = ini_GetStr(&ini, "out", 0);
            if (name) {
                rvdev_t* dev = driver_FindDevice(RVDEV_AUDIO_OUT, name);
                if (dev) { midi_SetTxDevice(rvdev_cast(rvdev_audio_tx_t, dev)); }
            }
        }
    #endif
    }


    /* temp */
    sys_setcookie("_SND", 0x00000025UL);
    sys_setcookie("GSXB", 0x00000024UL);

    dprintf("done.\n");
    return 0;
}

/* ------------------------------------------------------------------- */
#if defined(__PUREC__)
unsigned long _StkSize = 4096;
#endif
extern unsigned long _PgmSize;
static int _super_args;
static char** _super_argv;
long super_trampoline() { return super_main(_super_args, _super_argv); }
int main(int _user_args, char** _user_argv) {
    _super_args = _user_args; _super_argv = _user_argv;
    if (Supexec(super_trampoline) == 0) {
        Ptermres(_PgmSize, 0);
    }
    return 0;
}
