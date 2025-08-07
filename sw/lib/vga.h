/*-------------------------------------------------------------------------------
 * VGA defines
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
#ifndef _VGA_H_
#define _VGA_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern uint32_t vga_iobase;

#define VGA_REG_ATC         0x3C0
#define VGA_REG_MISC        0x3C2
#define VGA_REG_VIDSUB      0x3C3
#define VGA_REG_SEQ         0x3C4
#define VGA_REG_DAC_PEL     0x3C6
#define VGA_REG_DAC         0x3C8
#define VGA_REG_FEATURE     0x3CA
#define VGA_REG_GDC_SEG     0x3CD
#define VGA_REG_GDC         0x3CE
#define VGA_REG_CRTC        0x3D4
#define VGA_REG_STAT1       0x3DA

static uint8_t vga_ReadPort(uint16_t port) {
    return *((volatile uint8_t*)(vga_iobase + port));
}

static void vga_WritePort(uint16_t port, uint8_t val) {
    *((volatile uint8_t* )(vga_iobase + port)) = val;
}

static uint8_t vga_ReadReg(uint16_t port, uint8_t idx) {
    if (port==0x3C0) {
        vga_ReadPort(0x3DA);
    }
    vga_WritePort(port, idx);
    return vga_ReadPort(port+1);
}

static void vga_WriteReg(uint16_t port, uint8_t idx, uint8_t val) {
   if (port==0x3C0) {
      vga_ReadPort(0x3DA);
      vga_WritePort(port, idx);
      vga_WritePort(port, val);
   }
   else {
      vga_WritePort(port, idx);
      vga_WritePort(port+1, val);
   }
}

#endif /* _VGA_H_ */
