/*-------------------------------------------------------------------------------
 * FMRADIO for Atari with ISA bus
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
#ifndef _FMRADIO_H_
#define _FMRADIO_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    const char* name;

    bool        (*Detect)(void);
    void        (*SetFrequency)(uint32_t freq);
    void        (*AdjustVolume)(int8_t v);

} fmdriver_t;


#endif /* _FMRADIO_H_*/
