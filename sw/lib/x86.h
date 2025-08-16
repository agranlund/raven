/*-------------------------------------------------------------------------------
 * X86 defines
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
#ifndef _X86_H_
#define _X86_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "raven.h"

/*----------------------------------------------------------------------
 *
 * Wrappers to raven bios calls
 * 
 *--------------------------------------------------------------------*/
static uint16_t x86int(uint16_t no, x86_regs_t* regs_in, x86_regs_t* regs_out) {
    return raven()->int86x((uint32_t)no, regs_in, regs_out, 0);
}

static uint16_t x86intx(uint16_t no, x86_regs_t* regs_in, x86_regs_t* regs_out, x86_sregs_t* sregs) {
    return raven()->int86x((uint32_t)no, regs_in, regs_out, sregs);
}

/*----------------------------------------------------------------------
 *
 * MS-DOS compatible functions for easy porting
 * 
 *--------------------------------------------------------------------*/
#ifndef X86_EXCLUDE_LIB_MSDOS
#define int86 x86int
#define int86x x86intx
#endif

#endif /* _X86_H_ */
