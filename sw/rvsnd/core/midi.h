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


#ifndef _SYS_MIDI_H_
#define _SYS_MIDI_H_

#include "sys.h"
#include "driver.h"

extern bool     midi_Init(void);
extern void     midi_SetTxDevice(rvdev_miditx_t* dev);
extern void     midi_SetRxDevice(rvdev_midirx_t* dev);

#endif /* _SYS_MIDI_H_*/
