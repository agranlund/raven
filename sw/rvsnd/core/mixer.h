/*-------------------------------------------------------------------------------
 * rvsnd midi
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


#ifndef _SYS_MIXER_H_
#define _SYS_MIXER_H_

#include "sys.h"
#include "driver.h"

extern bool     mixer_Init(void);
extern void     mixer_Setup(void);

typedef struct {
    rvdev_mix_t* dev;
    rvmixctrl_t* ctr;
    uint16_t id;
    uint8_t val;
    uint8_t reserved;
} mixer_ctr_t;

typedef struct {
    rvdev_mix_t* dev;
    mixer_ctr_t* ctr_list;
    uint16_t ctr_count;
} mixer_dev_t;

extern uint8_t      mixer_GetValueByName(const char* name);
extern void         mixer_SetValueByName(const char* name, uint8_t data);

extern uint8_t      mixer_GetValueById(uint16_t id);
extern void         mixer_SetValueById(uint16_t id, uint8_t data);

extern uint16_t     mixer_NumDevs(void);
extern mixer_dev_t* mixer_GetDev(uint16_t idx);

extern mixer_dev_t* mixer_FindDev(const char* name);
extern mixer_ctr_t* mixer_FindCtr(const char* name);    /* "dev:ctr" or just "ctr" for system mixer */
extern mixer_ctr_t* mixer_FindDevCtr(mixer_dev_t* dev, const char* name);

#define SYSMIXER_ID_MASTER  0x0000
#define SYSMIXER_ID_PCM     0x0001
#define SYSMIXER_ID_FM      0x0002
#define SYSMIXER_ID_MIC     0x0003
#define SYSMIXER_ID_LINE    0x0004
#define SYSMIXER_ID_CD      0x0005
#define SYSMIXER_ID_TV      0x0006
#define SYSMIXER_ID_AUX     0x0007

#endif /* _SYS_MIXER_H_*/
