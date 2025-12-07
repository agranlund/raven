/*-------------------------------------------------------------------------------
 * rvsnd : isa ultrasound driver
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

#ifndef _GUSDRIVER_H_
#define _GUSDRIVER_H_

#include "driver.h"

extern isa_t* bus;


typedef struct
{
    uint32_t total;
    uint32_t size[4];
    uint16_t cfg;
} gusraminfo_t;


typedef struct {
    uint16_t port;
    uint16_t p2xr;  /* 2xx      */
    uint16_t p3xr;  /* 3xx      */
    uint16_t pcod;  /* codec    */

    uint8_t  amd;
    gusraminfo_t ram_gf1;
    gusraminfo_t ram_amd;
} gusinfo_t;

extern gusinfo_t gus;

extern bool     gus_detect(uint16_t port);
extern void     gus_init(void);
extern bool     gus_create_mixer(rvdev_mix_t* out);


#endif /* _GUSDRIVER_H_ */
