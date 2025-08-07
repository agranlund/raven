/*-------------------------------------------------------------------------------
 * NOVA generic vga driver
 * (c)2025 Anders Granlund
 * 
 * To be used as replacement device driver for NOVA 1.37 compatible VDI
 *  ET4000T0_V3.00 : TT/ET4000 Nova Driver by Idek V3.00
 *  https://silicon-heaven.org/atari/nova/TT030/ET4000/
 * 
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
#include <string.h>
#include <stdarg.h>
#include <mint/cookie.h>
#include <mint/osbind.h>

#include "emulator.h"
#include "nova.h"
#include "raven.h"
#include "vga.h"
/*#include "x86emu.h"*/

nova_xcb_t nova;
uint8_t nova_xcb_safety_area[128];

uint32_t vga_iobase;
uint32_t vga_membase;
static uint8_t atcColorIndexShadow[16];


static uint16_t bpp_to_mode(uint16_t bpp) {
    switch (bpp) {
        case 1: return NOVA_MODE_1BPP;
        case 2:
        case 4: return NOVA_MODE_4BPP;
        case 8: return NOVA_MODE_8BPP;
        case 15: return NOVA_MODE_15BPP;
        case 16: return NOVA_MODE_16BPP;
        case 24: return NOVA_MODE_24BPP;
        case 32: return NOVA_MODE_32BPP;
        default: return 0;
    }
}

static uint16_t mode_to_bpp(uint16_t mode) {
    static const uint16_t mode_to_bpp_table[] = { 4, 1, 8, 15, 16, 24, 32 };
    return (mode <= NOVA_MODE_32BPP) ? mode_to_bpp_table[mode] : 0;
}

static void update_current_resinfo(uint16_t w, uint16_t h, uint16_t bpp) {
    nova.mode = bpp_to_mode(bpp);
    nova.planes = bpp;
    nova.colors = (1 << bpp);
    nova.max_x = (w - 1);
    nova.max_y = (h - 1);
    nova.real_min_x = 0;
    nova.real_max_x = nova.max_x;
    nova.real_min_y = 0;
    nova.real_max_y = nova.max_y;
    nova.v_top = 0;
    nova.v_bottom = nova.max_y;
    nova.v_left = 0;
    nova.v_right = nova.max_x;
    if (nova.planes >= 8) {
        /* chunky mode: total bytes per line */
        nova.pitch = (uint16_t)(((uint32_t)(nova.max_x + 1) * nova.planes) >> 3);
    } else {
        /* planar mode: bytes per line per plane */
        nova.pitch = (uint16_t)((nova.max_x + 1) >> 3);
    }
    nova.scrn_count = 1;
    nova.scrn_size = (uint32_t)(nova.max_x+1);
    nova.scrn_size *= (nova.max_y+1);
    if (nova.planes >= 8) {
        /* planar mode: scrn_size is per plane  */
        /* chunky mode: scrn_size is total size */
        nova.scrn_size *= nova.planes;
    }
    nova.scrn_size >>= 3;        
}



/*-------------------------------------------------------------------------------

 NOVA inward facing API

*-----------------------------------------------------------------------------*/

/* temp... until we have card detect and subdrivers */
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

static void nop(void) 0x4E71;

void drv_vsync(void) {
    do { nop(); } while (vga_ReadPort(0x3DA) & 8);
    do { nop(); } while (!(vga_ReadPort(0x3DA) & 8));
}

bool drv_setmode(uint16_t w, uint16_t h, uint16_t bpp) {
    int i;
    vgamode_t* best = 0;
    /* todo: this should obviously pick the best matching one */
    /*       instead of failing when there is no exact match  */
    for (i = 0; i < sizeof(vgamodes) / sizeof(vgamode_t); i++) {
        if ((vgamodes[i].w == w) && (vgamodes[i].h == h) && (vgamodes[i].bpp == bpp)) {
            best = &vgamodes[i];
        }
    }

    if (best) {
        uint16_t sr;
        dprintf("drv_setmode: %d %d %d %04x\n", best->w, best->h, best->bpp, best->code);

        sr = cpu_di();
        drv_vsync();
        raven()->vga_SetMode((uint32_t)best->code);
        cpu_ei(sr);

        return true;
    }
    return false;
}


bool drv_init(void) {

    /* bus setup */
    vga_iobase  = 0x81000000UL;
    vga_membase = 0x82000000UL;

    /* default ega color table */
    atcColorIndexShadow[ 0] = 0x00;
    atcColorIndexShadow[ 1] = 0x01;
    atcColorIndexShadow[ 2] = 0x02;
    atcColorIndexShadow[ 3] = 0x03;
    atcColorIndexShadow[ 4] = 0x04;
    atcColorIndexShadow[ 5] = 0x05;
    atcColorIndexShadow[ 6] = 0x14;
    atcColorIndexShadow[ 7] = 0x07;
    atcColorIndexShadow[ 8] = 0x38;
    atcColorIndexShadow[ 9] = 0x39;
    atcColorIndexShadow[10] = 0x3A;
    atcColorIndexShadow[11] = 0x3B;
    atcColorIndexShadow[12] = 0x3C;
    atcColorIndexShadow[13] = 0x3D;
    atcColorIndexShadow[14] = 0x3E;
    atcColorIndexShadow[15] = 0x3F;

    /* todo: card detect and individual svga subdrivers */

    return true;
}


/*-------------------------------------------------------------------------------

 NOVA outward facing API exposed in cookie

*-----------------------------------------------------------------------------*/
void nova_p_changeres(nova_bibres_t* bib, uint32_t offs) {
    /* bib points directly to the requested resolution data */
    /* sta_vdi respects the final info updated in nova cookie so it's */
    /* probably safe to view this call as a polite request with */
    /* possibility of refusing or altering parameters per device capabilities */
    /* I have no idea what offs is supposed to do, */
    /* perhaps screen memory offset but I have only seen this argument be 0 */
    dprintf("nova: p_changeres: %08lx, %ld\n", (uint32_t)bib, offs);
    dprintf(" %d %d %d [%s]\n", bib->max_x, bib->max_y, bib->planes, bib->name);
    if (drv_setmode(bib->max_x+1, bib->max_y+1, bib->planes)) {
        update_current_resinfo(bib->max_x+1, bib->max_y+1, bib->planes);
        sprintf(nova.mode_name, bib->name);
    }
}

void nova_p_setcolor(uint16_t index, uint8_t* colors) {
    /* sta_vdi calls it for all colors, several times with different values on startup */
    /* double check the atc and other registers to figure out why. */
    /* perhaps it writes all four of the available 16-color palettes */

    uint16_t sr = cpu_di();
    uint8_t dacIndex = (nova.planes < 8) ? atcColorIndexShadow[index&0xf] : index;
    dprintf("col %d (%02x): %02x %02x %02x\n", index, dacIndex, colors[0], colors[1], colors[2]);

    if (nova.planes == 1) {
        int idx = index;
        int end = (index == 0) ? 1 : 255;
        for (; idx < end; idx++) {
            vga_WritePort(0x3c8, idx);
            vga_WritePort(0x3c9, colors[0]>>2);
            vga_WritePort(0x3c9, colors[1]>>2);
            vga_WritePort(0x3c9, colors[2]>>2);
        }
    }
    else {
        vga_WritePort(0x3c8, dacIndex);
        vga_WritePort(0x3c9, colors[0]>>2);
        vga_WritePort(0x3c9, colors[1]>>2);
        vga_WritePort(0x3c9, colors[2]>>2);
    }
    cpu_ei(sr);
}

void nova_p_changevirt(uint16_t x, uint16_t y) {
    /* this is called on mouse movement and appears to track the mouse position */
    /* presumably so the driver can perform scrolling over a larger virtual screen */
    /* but could perhaps also be useful for hardware cursor */
    /* test nova_col.acc */
    /*dprintf("nova: p_changevirt: %d, %d\n", x, y);*/
}

void nova_p_instxbios(uint16_t on) {
    /* this gets called with 0 when sta_vdi.prg starts */
    /* likely to stop the redirection stuff that the orignal emulator.prg */
    /* for et4000 was doing */
    dprintf("nova: p_instxbios: %d\n", on);
}

void nova_p_screen_on(uint16_t on) {
    /* have no seen this called from sta_vdi */
    /* likely useed by sta_vdi's screensaver and possibly */
    /* useful for some usercode */
    /* test nova_col.acc */
    dprintf("nova: p_screen_on: %d\n", on);
}

void nova_p_changepos(nova_bibres_t* bib, uint16_t dir, uint16_t offs) {
    /* have not seen this called from sta_vdi and have no idea what it's supposed to do */
    /* best guess is that the vme tool might be using it */
    /* test nova_col.acc */
    dprintf("nova: p_changepos: %08lx, %d, %d\n", (uint32_t)bib, dir, offs);
}

void nova_p_setscreen(void* addr) {
    /* called on sta_vdi start and no confusion here */
    dprintf("nova: p_setscreen: %08lx\n", (uint32_t)addr);
}

void nova_nova_p_vsync(void) {
    /* have no seen this called from sta_vdi, but certainly */
    /* useful and used by user-code, SDL and so on */
    /* test nova_col.acc */
    dprintf("nova: p_vsync\n");
    drv_vsync();
}



/*-------------------------------------------------------------------------------

 NOVA driver initialisation

*-----------------------------------------------------------------------------*/

bool nova_prepare(void) {
    uint32_t cookie;

    /* driver init */
    if (!drv_init()) {
        return false;
    }

    /* general setup */
    memset((void*)&nova, 0, sizeof(nova_xcb_t));
    if (Getcookie(C__CPU, (long*)&cookie) == C_FOUND) {
        nova.cpu = (uint16_t)(cookie & 0xffff);
    }
    nova.version = NOVA_VERSION;
    nova.hcmode = NOVA_HCMODE_1X1;
    nova.blank_time = 0;
    nova.mouse_speed = 1;

    /* api setup */
    nova.p_changeres = nova_p_changeres;
    nova.p_setcolor = nova_p_setcolor;
    nova.p_changevirt = nova_p_changevirt;
    nova.p_instxbios = nova_p_instxbios;
    nova.p_screen_on = nova_p_screen_on;
    nova.p_changepos = nova_p_changepos;
    nova.p_setscreen = nova_p_setscreen;
    nova.p_vsync = nova_nova_p_vsync;

    /* card setup */
    nova.reg_base = vga_iobase + 0x8000UL;
    nova.mem_base = vga_membase + 0xA0000UL;
    nova.mem_size = (64 * 1024UL);

    /* default resolution setup */
    nova.resolution = 1;
    nova.old_resolution = 0;
    nova.base = nova.mem_base;
    nova.scrn_count = 1;
    update_current_resinfo(640, 480, 2);
    sprintf(nova.mode_name, "boot");

    return true;
}
