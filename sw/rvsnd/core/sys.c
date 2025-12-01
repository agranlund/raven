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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "mint/cookie.h"
#include "mint/osbind.h"
#include "mint/falcon.h"
#include "pubapi.h"

rvsnd_sysvars_t sys;

extern void pubapi_Init(void);

/* ------------------------------------------------------------------- */
#if defined(DEBUG) && DEBUG
#ifdef DEBUG_RAVEN
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
void dprintf(const char*, ...) { }
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
    ini_Load(&sys.ini, "c:\\rvsnd.ini");

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


/* ------------------------------------------------------------------- */
long super_main(int args, char** argv) {
    UNUSED(args); UNUSED(argv);

    dprintf("init...\n");

    /* system setup */
    sys_Init();
    driver_Init();
    midi_Init();

    /* temp load drivers */
    /* todo: auto load from folders, based on inifile settings */
    dprintf("load drivers...\n");
    driver_Load("chip\\opl.rvs");
    driver_Load("midi\\raven.rvs");
    driver_Load("midi\\mpu401.rvs");
    driver_Load("midi\\opl.rvs");

    /* inject bios overrides */
    dprintf("install xbios...\n");
    sys_InstallBios();

    /* init public api */
    pubapi_Init();

#if 0    
    /* temp */
    {
        uint16_t i;
        uint16_t num = driver_NumDevs();
        rvdev_t** devs = driver_GetDevs();
        dprintf("\nnum devs = %d\n", driver_NumDevs());
        for (i=0; i<num; i++) {
            dprintf(" [%08lx] [%02x] [%s]\n", (uint32_t)devs[i], (uint8_t)devs[i]->type, devs[i]->names[0]);
        }
    }
#endif

#if 0
    /* temp */
    {
        int32_t r;
        dprintf("test xbios start\n");
        r = Locksnd();
        dprintf("test xbios done, r = %d\n", (int)r);
    }
#endif

#if 0
    /* temp */
    {
#if 1
        rvdev_t* dev = driver_FindDevice(RVDEV_MIDI_OUT, "OPL");
#else        
        rvdev_t* dev = driver_FindDevice(RVDEV_MIDI_OUT, "MPU401");
#endif
        midi_SetTxDevice(rvdev_cast(rvdev_miditx_t, dev));
    }
#endif
    {
        rvdev_t* dev = driver_FindDevice(RVDEV_MIDI_OUT, "Raven");
        midi_SetTxDevice(rvdev_cast(rvdev_miditx_t, dev));
    }


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
