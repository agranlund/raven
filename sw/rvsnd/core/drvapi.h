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

#ifndef _DRVAPI_H_
#define _DRVAPI_H_
#include <stdbool.h>
#include <stdint.h>

#include "../../lib/ini.h"

#define ISA_EXCLUDE_LIB
#include "../../isa/isa.h"


typedef enum {
    RVDEV_NONE = 0,
    RVDEV_RAW,
    RVDEV_MIXER,
    RVDEV_MIDI_OUT,
    RVDEV_MIDI_IN,
    RVDEV_AUDIO_OUT,
    RVDEV_AUDIO_IN,
    RVDEV_NUMTYPES,
} rvdev_e;


#define rvdev_cast(t,p)  ((t*)(((uint32_t)(p))))
#define rvdev_base(p)    rvdev_cast(rvdev_t,p)

/* device header */
#define RVDEV_COMMON \
    uint32_t type; \
    uint32_t version; \
    uint32_t addr; \
    const char** names; \
    bool (*start)(void); \
    void (*stop)(void);

/* base */
typedef struct _rvdev_t {
    RVDEV_COMMON
} rvdev_t;

/* raw device */
typedef struct _rvdev_raw_t { RVDEV_COMMON
    void        (*wrb)(uint16_t reg, uint16_t data);
    void        (*wrw)(uint16_t reg, uint16_t data);
    uint16_t    (*rdb)(uint16_t reg);
    uint16_t    (*rdw)(uint16_t reg);
} rvdev_raw_t;

/* midi out device */
typedef struct _rvdev_miditx_t { RVDEV_COMMON
    int32_t     cdecl (*st)(void);          /* bcostat */
    void        cdecl (*tx)(uint32_t);      /* bconout */
} rvdev_miditx_t;

/* midi in device */
typedef struct _rvdev_midirx_t { RVDEV_COMMON
    int32_t     cdecl (*st)(void);          /* bconstat */
    int32_t     cdecl (*rx)(void);          /* bconin   */
} rvdev_midirx_t;


/* driver api */
typedef struct {
    /* info */
    bool        (*getcookie)(const char*, uint32_t*);
    uint32_t    c_mch;
    uint32_t    c_cpu;
    uint32_t    c_mint;
    uint32_t    c_magx;
    uint32_t    machine;
    /* busses */
    isa_t*      isa;
    /* inifile */
    bool        (*ini_getsection)(ini_t*, ini_t*, const char*);
    const char* (*ini_getstr)(ini_t* ini, const char* name, const char* def);
    int32_t     (*ini_getint)(ini_t* ini, const char* name, int32_t def);
    /* devices */
    bool        (*publish)(rvdev_t* dev);
    rvdev_t*    (*retrieve)(uint32_t type, const char* name);
    /* system */
    void        (*delayus)(uint32_t);
    uint16_t    (*irq_disable)(void);
    void        (*irq_enable)(uint16_t);
    /* debug */
    void        (*dprintf)(const char*, ...);
    void        (*printf)(const char*, ...);
    /* strings */
    void        (*sprintf)(char *, const char*, ...);
    int32_t     (*strlen)(const char*);
    char*       (*strcpy)(char*, const char*);
    char*       (*strncpy)(char*, const char*, int32_t);
    char*       (*strstr)(const char*, const char*);
    char*       (*strchr)(const char*, int32_t);
    int32_t     (*atoi)(const char*);
    /* memory*/
    void        (*free)(void*);
    void*       (*malloc)(uint32_t);
    void*       (*realloc)(void*, uint32_t);
    void        (*memset)(void*, uint8_t, uint32_t);
    void        (*memcpy)(void*, void*, uint32_t);
    int32_t     (*memcmp)(const void*, const void*, uint32_t);
} rvsnd_driver_api_t;

#endif /* _DRVAPI_H_*/
