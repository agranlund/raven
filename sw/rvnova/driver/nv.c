/*-------------------------------------------------------------------------------
 * NOVA generic vga driver
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
#include "emulator.h"
#include "raven.h"
#include "vga.h"

/*-------------------------------------------------------------------------------
  standard vga and vesa functionality
-------------------------------------------------------------------------------*/
bool nv_vesa_setmode(uint16_t mode) {
    /* todo: vesa support */
    /* todo: x86 api directly instead of this... */
    raven()->vga_SetMode((uint32_t)mode);
    return true;
}

void nv_vesa_vsync(void) {
    do { cpu_nop(); } while (vga_ReadPort(0x3DA) & 8);
    do { cpu_nop(); } while (!(vga_ReadPort(0x3DA) & 8));
}

void nv_vesa_setcolors(uint16_t index, uint16_t count, uint8_t* colors) {
    while (count) {
        vga_WritePort(0x3c8, index++);
        vga_WritePort(0x3c9, (*colors++)>>2);
        vga_WritePort(0x3c9, (*colors++)>>2);
        vga_WritePort(0x3c9, (*colors++)>>2);
        count--;
    }
}

void nv_vesa_getcolors(uint16_t index, uint16_t count, uint8_t* colors) {
    while (count) {
        vga_WritePort(0x3c8, index++);
        *colors++ = vga_ReadPort(0x3c9);
        *colors++ = vga_ReadPort(0x3c9);
        *colors++ = vga_ReadPort(0x3c9);
        count--;
    }
}


/*-------------------------------------------------------------------------------
  driver core
-------------------------------------------------------------------------------*/

bool nv_setmode(uint16_t width, uint16_t height, uint16_t bpp) {

    typedef struct 
    {
        uint16_t w;
        uint16_t h;
        uint16_t bpp;
        uint16_t code;
    } vgamode_t;

    static const vgamode_t vgamodes[] = {
        {  320, 200,  8, 0x0013 },
        {  640, 350,  1, 0x0010 },
        {  640, 350,  4, 0x0010 },
        {  640, 480,  1, 0x0012 },
        {  640, 480,  4, 0x0012 },
    };

    int i;
    vgamode_t* best = 0;
    for (i = 0; i < sizeof(vgamodes) / sizeof(vgamode_t); i++) {
        if ((vgamodes[i].w == width) && (vgamodes[i].h == height) && (vgamodes[i].bpp == bpp)) {
            best = (vgamode_t*)&vgamodes[i];
        }
    }

    if (best) {
        nv_vesa_vsync();
        nv_vesa_setmode(best->code);
        return true;
    }
    return false;
}

void nv_init_vram(uint32_t phys, uint32_t size, uint32_t count) {
    /* todo: map logical vram, size = bank size, count = bank count */
    /* linear vram is treated as a single very large bank */
}

bool nv_init(void) {
    /* todo: card detection for svga features */
    return true;
}
