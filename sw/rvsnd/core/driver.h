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

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "drvapi.h"

extern void driver_Init(void);
extern bool driver_Load(const char* fname);
extern bool driver_AddDevice(rvdev_t* dev);
extern rvdev_t* driver_FindDevice(uint32_t type, const char* name);

extern uint16_t driver_NumDevs(void);
extern rvdev_t** driver_GetDevs(void);

#endif /* _DRIVER_H_*/
