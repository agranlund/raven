/*-------------------------------------------------------------------------------
 * Low level card hackery
 * (c)2025 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  whthout even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rvnova.h"

#define vga_iobase                  0x81000000UL

#define VGA_REG_CRTC_I              0x3D4 
#define VGA_REG_CRTC_W              0x3D5
#define VGA_REG_CRTC_R              0x3D5
#define VGA_REG_GDC_SEG             0x3CD
#define VGA_REG_GDC_I               0x3CE
#define VGA_REG_GDC_W               0x3CF
#define VGA_REG_GDC_R               0x3CF
#define VGA_REG_SEQ_I               0x3C4
#define VGA_REG_SEQ_W               0x3C5
#define VGA_REG_SEQ_R               0x3C5
#define VGA_REG_VIDSUB              0x3C3
#define VGA_REG_MISC                0x3C2
#define VGA_REG_DAC_PEL             0x3C6
#define VGA_REG_DAC_I               0x3C8
#define VGA_REG_DAC_W               0x3C9
#define VGA_REG_DAC_R               0x3C9
#define VGA_REG_ATC_I               0x3C0
#define VGA_REG_ATC_W               0x3C0
#define VGA_REG_ATC_R               0x3C1
#define VGA_REG_STAT1               0x3DA

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

/*-----------------------------------------------------------------------------*/
bool w32i_EnableInterleaveMode(void) {

    /* todo: it would be good if this code first identified that we actually
       have an ET4000/W32i card.
    */

    uint16_t port;
    uint8_t r32, r37;

    /*
    NOTE: The "KEY" must be set in order to write CRTC indices above 18, except indices 33 and
    35, (CRTC 35 is protected by bit 7 of CRTC 11). See Section 5.1.2, Input Status Register Zero for
    definition of "KEY
    */

    /* To set the KEY: write 03 to hercules compatibility registers */
    vga_WritePort(0x3bf, 3);
    /* then set bits 7 and 5 of mode control register to 1,1, e.g.A0 (other bits are don't care) */
    port = (vga_ReadPort(0x3cc) & 1) ? 0x3d8 : 0x3b8;
    vga_WritePort(port, 0xa0);

    /* now enable interleaved memory mode */
    r32 = vga_ReadReg(VGA_REG_CRTC_I, 0x32);            /* ras/cas conf */
    r37 = vga_ReadReg(VGA_REG_CRTC_I, 0x37);            /* vsconf2 */

    /* Databook page: 126
    CRTC:37 video system conf2
    bit0 : display memory data bus width
    */
    vga_WriteReg(VGA_REG_CRTC_I, 0x37, (r37 | 1));

    /* Databook page: 130
     CRTC:32 ras/cas config
     bit7 : memory interleave enable.
     When set to 1 (memory interleave enabled), the chip operates properly only if
     CSW<0> = 0, CSP<0> = 0, and CRTC Indexed Register 37 bit 0 = 1.
    */
    vga_WriteReg(VGA_REG_CRTC_I, 0x32, ((r32 | (1 << 7)) & 0xFC));
    return true;
}

