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

#define _this drv_vga
extern driver_t _this;


static const mode_t modes[] = {
    {  320, 200,  8, 0x0013 },
    {  640, 350,  1, 0x0010 },
    {  640, 350,  4, 0x0010 },
    {  640, 480,  1, 0x0012 },
    {  640, 480,  4, 0x0012 },
};

static mode_t* drv_getmode(uint16_t num) {
    if (num < _this.num_modes) {
        return (mode_t*)&modes[num];
    }
    return 0;
}

static bool drv_setmode(uint16_t num) {
    if (num < _this.num_modes) {
        dprintf("drv setmode %d (%dx%dx%d 0x%04x)\n", num, modes[num].width, modes[num].height, modes[num].bpp, modes[num].code);
        return nv_vesa_setmode(modes[num].code);
    }
    return false;
}

static void drv_setbank(uint16_t num) {
}

static bool drv_init(void) {
    dprintf("drv init\n");
    _this.card_name = _this.driver_name;
    _this.bank_addr = 0xA0000UL;
    _this.bank_size = 64 * 1024UL;
    _this.num_banks = 1;
    _this.num_modes = sizeof(modes) / sizeof(mode_t);

    _this.getmode = drv_getmode;
    _this.setmode = drv_setmode;
    _this.setbank = drv_setbank;
    _this.setcolors = nv_vesa_setcolors;
    _this.getcolors = nv_vesa_getcolors;
    _this.vsync = nv_vesa_vsync;
    return true;
}

driver_t _this = { "Standard VGA", 1, drv_init };
