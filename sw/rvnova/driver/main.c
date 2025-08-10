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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <mint/cookie.h>
#include <mint/osbind.h>

#include "emulator.h"
#include "nova.h"

nova_xcb_t nova;
static uint8_t nova_dummy_area[4096];
static uint8_t nova_colormap[16];


/*-----------------------------------------------------------------------------*/
#if defined(DEBUG) && DEBUG && DEBUGPRINT_UART
static char dbgstr[256];
void dprintf_uart(char* s, ...) {
    va_list args;
    char* buf = dbgstr;
    va_start(args, s);
    vsprintf(buf, s, args);
    va_end(args); 
    while(*buf != 0) {
        while((*((uint8_t*)(RV_PADDR_UART2 + 0x14)) & (1 << 5)) == 0);
        *((char*)(RV_PADDR_UART2 + 0x00)) = *buf++;
    }
}
#endif

/*-----------------------------------------------------------------------------*/
static void setcookie(uint32_t cookie_id, uint32_t cookie_value) {
    /* find existing cookie, or a free slot */
    int32_t cookies_size = 0;
	int32_t cookies_used = 0;
    int32_t cookies_avail = 0;
	uint32_t* jar = (uint32_t*) *((uint32_t*)0x5a0);
	uint32_t* c = jar;

    while (1) {
		cookies_used++;
		if (c[0] == cookie_id) {
			c[1] = cookie_id;
			return;
		}
		else if (c[0] == 0) {
            cookies_size = c[1];
			break;
		}
		c += 2;
	}

    /* grow jar when necessary */
    cookies_avail = cookies_size - cookies_used;
	if (cookies_avail <= 0) {
        uint32_t* newjar;
        int32_t oldsize = (2*4*(cookies_size + 0));
        int32_t newsize = (2*4*(cookies_size + 8));
        cookies_size += 8;
        newjar = (uint32_t*)malloc(newsize);
        memcpy(newjar, jar, oldsize);
        *((uint32_t*)0x5a0) = (uint32_t)newjar;
        jar = newjar;
	}

	/* install cookie */
	jar[(cookies_used<<1)-2] = cookie_id;           /* overwrite end marker */
	jar[(cookies_used<<1)-1] = cookie_value;
	jar[(cookies_used<<1)+0] = 0;                   /* write new end marker */
	jar[(cookies_used<<1)+1] = cookies_size;
}

/*-----------------------------------------------------------------------------*/
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

static void update_nova_resname(char* name) {
    strncpy((char*)nova.mode_name, name, 35);
}

static void update_nova_resinfo(uint16_t width, uint16_t height, uint16_t bpp) {
    nova.max_x = width - 1;
    nova.max_y = height - 1;
    nova.planes = bpp;

    nova.mode = bpp_to_mode(nova.planes);
    nova.colors = (1 << nova.planes);
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
    nova.scrn_size = (uint32_t)(nova.max_x+1);
    nova.scrn_size *= (nova.max_y+1);
    if (nova.planes >= 8) {
        /* planar mode: scrn_size is per plane  */
        /* chunky mode: scrn_size is total size */
        nova.scrn_size *= nova.planes;
    }
    nova.scrn_size >>= 3;
    nova.scrn_count = nova.mem_size / nova.scrn_size;
    nova.scrn_count = (nova.scrn_count < 1) ? 1 : nova.scrn_count;
    nova.base = nova.mem_base;
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
    uint16_t sr, w, h, b;

    w = bib->max_x + 1;
    h = bib->max_y + 1;
    b = bib->planes;

    dprintf("nova: p_changeres: %08lx, %ld : %dx%dx%d\n", (uint32_t)bib, offs, w, h, b);

    sr = cpu_di();
    if (nv_setmode(w, h, b)) {

        /* update nova cookie */
        nova.mem_size = card->bank_size * card->num_banks;
        update_nova_resinfo(w, h, b);
        update_nova_resname(bib->name);

        /* sta_vdi needs register access to draw in 16 color planar */
        if (nova.planes == 4) {
            cpu_map(VADDR_IO, PADDR_IO, VSIZE_IO, PMMU_CM_PRECISE | PMMU_READWRITE);
            cpu_flush_atc();
        } else {
            uint32_t offs;
            for (offs = 0; offs < VSIZE_IO; offs += 4096) {
                cpu_map(VADDR_IO + offs, (uint32_t)nova_dummy_area, 4096, PMMU_CM_PRECISE | PMMU_READWRITE);
                cpu_flush_atc();
            }
        }
    }
    cpu_ei(sr);
}

void nova_p_setcolor(uint16_t index, uint8_t* colors) {
    uint16_t sr;
    uint8_t dacIndex = (nova.planes < 8) ? nova_colormap[index&0xf] : index;
    /*dprintf("col %d (%02x): %02x %02x %02x\n", index, dacIndex, colors[0], colors[1], colors[2]);*/

    sr = cpu_di();
    if (nova.planes == 1) {
        /* a bit of a temp hack here */
        /* not sure which additional indices we need to set for color 1 so */
        /* we're setting all of them at the moment */
        int idx = index;
        int end = (index == 0) ? 1 : 255;
        for (; idx < end; idx++) {
            card->setcolors(idx, 1, colors);
        }
    }
    else {
        card->setcolors(dacIndex, 1, colors);
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

void nova_p_vsync(void) {
    /* have no seen this called from sta_vdi, but certainly */
    /* useful and used by user-code, SDL and so on */
    /* test nova_col.acc */
    dprintf("nova: p_vsync\n");
    card->vsync();
}



/*-------------------------------------------------------------------------------

 driver initialisation

*-----------------------------------------------------------------------------*/

static bool setbootres(void) {
    int16_t bootdrive;
    int32_t result = 0;
    char* fname = (char*)nova_dummy_area;
    nova_bibres_t* bibres = (nova_bibres_t*)nova_dummy_area;

    strcpy(fname, "C:\\AUTO\\EMULATOR.BIB");
    bootdrive = *((volatile int16_t*)0x446UL) + (int16_t)'A';
    if ((bootdrive >= 'A') && (bootdrive <= 'Z')) {
        fname[0] = (char)bootdrive;
        result = Fopen(fname, 0);
        if (result >= 0) {
            int16_t fp = (int16_t)result;
            int32_t fs = Fseek(0, fp, 2);
            result = -1;
            Fseek(0, fp, 0);
            if (fs >= sizeof(nova_bibres_t)) {
                Fread(fp, fs, (void*)bibres);
                result = 1;
            }
            Fclose(fp);
        }
    }

    if (result >= 0) {
        uint8_t* pal = nova_dummy_area;
        pal[0] = 0xFF; pal[1] = 0xFF; pal[2] = 0xFF;
        pal[3] = 0x00; pal[4] = 0x00; pal[5] = 0x00;
        dprintf("bootres from emulator.bib\n");
        nova_p_changeres(bibres, 0);
        nova_p_setcolor(0, &pal[0]);
        nova_p_setcolor(1, &pal[3]);
        return true;
    }

    return false;
}

long supermain(void) {
    uint32_t cookie;

    /* driver init */
    if (!nv_init()) {
        return -1;
    }

    /* default ega color table */
    nova_colormap[ 0] = 0x00;
    nova_colormap[ 1] = 0x01;
    nova_colormap[ 2] = 0x02;
    nova_colormap[ 3] = 0x03;
    nova_colormap[ 4] = 0x04;
    nova_colormap[ 5] = 0x05;
    nova_colormap[ 6] = 0x14;
    nova_colormap[ 7] = 0x07;
    nova_colormap[ 8] = 0x38;
    nova_colormap[ 9] = 0x39;
    nova_colormap[10] = 0x3A;
    nova_colormap[11] = 0x3B;
    nova_colormap[12] = 0x3C;
    nova_colormap[13] = 0x3D;
    nova_colormap[14] = 0x3E;
    nova_colormap[15] = 0x3F;

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
    nova.p_vsync = nova_p_vsync;

    /* card setup */
    nova.reg_base = (void*) (VADDR_IO + 0x8000UL);
    nova.mem_base = (void*) (VADDR_MEM);
    nova.mem_size = card->bank_size * card->num_banks;

    /* install cookie */
    setcookie(C_NOVA, (uint32_t)&nova);

    /* apply boot resolution */
    nova.old_resolution = 0;
    nova.resolution = 1;
    if (!setbootres()) {
        update_nova_resinfo(640, 480, 2);
        update_nova_resname("boot");
    }
    return 0;
}

unsigned long _StkSize = 4096;
int main()
{
    int ret = (int) Supexec(supermain);
    if (ret == 0) {
        Ptermres(_PgmSize, 0);
    }
    return ret;
}
