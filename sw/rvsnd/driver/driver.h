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

#ifndef _DRIVER_DRIVER_H_
#define _DRIVER_DRIVER_H_
#include <stdbool.h>
#include <stdint.h>

#define INI_NO_FILE
#include "lib/ini.h"

#define ISA_EXCLUDE_LIB
#include "isa/isa.h"

#include "rvsnd/core/drvapi.h"

extern rvsnd_driver_api_t*  rvsnd;      /* driver.s */
extern ini_t*               rvini;      /* driver.s */

#define dprintf             rvsnd->dprintf
#define printf              rvsnd->printf

#define sprintf             rvsnd->sprintf
#define strlen(a)           rvsnd->strlen(a)
#define strcpy(a,b)         rvsnd->strcpy(a,b)
#define strncpy(a,b,c)      rvsnd->strncpy(a,b,c)
#define strstr(a,b)         rvsnd->strstr(a,b)
#define strchr(a,b)         rvsnd->strchr(a,b)
#define atoi(a)             rvsnd->atoi(a)

#define free(a)             rvsnd->free(a)
#define malloc(a)           rvsnd->malloc(a)
#define realloc(a,b)        rvnsd->realloc(a)
#define memset(a,b,c)       rvsnd->memset(a,b,c)
#define memcpy(a,b,c)       rvsnd->memcpy(a,b,c)
#define memcmp(a,b,c)       rvsnd->memcmp(a,b,c)

#define delayus(a)          rvsnd->delayus(a)

static bool ini_GetSection(ini_t* out, ini_t* ini, const char* name) { return rvsnd->ini_getsection(out, ini, name); }
static const char* ini_GetStr(ini_t* ini, const char* name, const char* def) { return rvsnd->ini_getstr(ini, name, def); }
static int32_t ini_GetInt(ini_t* ini, const char* name, int32_t def) { return rvsnd->ini_getint(ini, name, def); }

#define dev_publish(d)          rvsnd->publish(rvdev_base(d))

#endif /* _DRIVER_DRIVER_H_*/
