/*-------------------------------------------------------------------------------
 * rvsnd
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

#ifndef _RVSND_SYS_H
#define _RVSND_SYS_H

#include <stdbool.h>
#include <stdint.h>
#include "lib/ini.h"

#define ISA_EXCLUDE_LIB
#include "isa/isa.h"


#define DEBUG_RAVEN     1

#ifndef DEBUG
#define DEBUG 0
#endif
#define DEBUG_DEV 2

#define IMPERSONATE_GSXB    1

extern void dprint(const char* s, ...);

#if DEBUG
#define dprintf(x) dprint x
#else
#define dprintf(x)
#endif

extern uint16_t sys_di(void);
extern void     sys_ei(uint16_t);
extern uint16_t sys_ipl(uint16_t sr);
extern int16_t  sys_getpid(void);
extern void     sys_icache_clear(void);

extern bool     sys_getcookie(const char* c, uint32_t* v);
extern void     sys_setcookie(const char* c, uint32_t v);

extern void     sys_delayus(uint32_t us);

#define UNUSED(x)   (void)x

typedef struct {
    ini_t       ini;
    isa_t*      isa;
    uint32_t    cookie_cpu;
    uint32_t    cookie_mch;
    uint32_t    cookie_snd;
} rvsnd_sysvars_t;

extern rvsnd_sysvars_t sys;

#endif /* _RVSND_SYS_H*/
