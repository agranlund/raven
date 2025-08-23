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
#include <vdi.h>
#include <linea.h>

#include "emulator.h"
#include "nova.h"

#define VDI_DEBUG   0

/*-----------------------------------------------------------------------------
 NOVA VDI internal device function pointers
-----------------------------------------------------------------------------*/
typedef struct
{
/* 00 */    uint32_t    _unk00;
/* 04 */    uint32_t    _unk04;
/* 08 */    uint32_t    _unk08;
/* 0c */    uint32_t    _unk0c;
/* 10 */    uint32_t    _unk10;
/* 14 */    uint32_t    _unk14;     /* used in 4bpp, sets vga regs */
/* 18 */    uint32_t    _unk18;     /* fillpoly? */
/* 1c */    uint32_t    _unk1c;
/* 20 */    uint32_t    _unk20;
/* 24 */    uint32_t    _unk24;
/* 28 */    uint32_t    _unk28;
/* 2c */    uint32_t    _unk2c;
/* 30 */    uint32_t    _unk30;
/* 34 */    uint32_t    _unk34;
/* 38 */    uint32_t    _unk38;
/* 3c */    uint32_t    _unk3c;
/* 40 */    uint32_t    _unk40;
/* 44 */    uint32_t    _unk44;     /* sta6_v_line */
/* 48 */    uint32_t    _unk48;
/* 4c */    uint32_t    putpixel;   /* linea_putpixel */
/* 50 */    uint32_t    getpixel;   /* linea_getpixel */
/* 54 */    uint32_t    bitblt;     /* linea_bitblt  */
/* 58 */    uint32_t    copyfm;     /* linea_copyfm + vro_copyfm */
/* 5c */    uint32_t    _unk5c;
/* 60 */    uint32_t    _unk60;
/* 64 */    uint32_t    _unk64;
/* 68 */    uint32_t    _unk68;
/* 6c */    uint32_t    _unk6c;     /* sta_22_vst_color, sta_25_vsf_color */
/* 70 */    uint32_t    _unk70;     /* sta_22_vst_color, sta_25_vsf_color, sta_105_v_getpixel */
/* 74 */    uint32_t    _unk74;     /* sta_105_v_getpixel */
/* 78 */    uint32_t    _unk78;
/* 7c */    uint32_t    _unk7c;
/* 80 */    uint16_t    _unk80[6];
} stawk91c_t;

/*-----------------------------------------------------------------------------
 NOVA VDI internal workstation data
-----------------------------------------------------------------------------*/
typedef struct
{
/* 000 */   uint8_t     _unk_000[0x01c - 0x000];
/* 01c */   uint16_t    vsf_color;
/* 01e */   uint16_t    vsf_style;
/* 020 */   uint16_t    vsf_perimeter;
/* 022 */   uint16_t    vsf_interior;
/* 024 */   uint8_t     _unk_024[0x4b0 - 0x024];
/* 4b0 */   uint16_t    writemode;
/* 4b2 */   uint16_t    _unk_4b2;
/* 4b4 */   uint16_t    clipflag;
/* 4b6 */   rect_t      cliprect;
/* 4be */   uint8_t     _unk_4be_updateflag;    /* set to 1 in vsf_perimeter and similar */
/* 4c0 */   uint8_t     _unk_4c0[0x8c2 - 0x4c0];
/* some of the following are being updated from at the start of every */
/* sta_vdi handler when 'is_screen' is not zero */
/* 8c2 */   uint16_t    bytes_per_line;
/* 8c4 */   uint8_t     _unk_8c4[0x90a - 0x8c4];
/* 90a */   uint16_t    changed_flag;
/* 90c */   uint8_t     _unk_90c[0x910 - 0x90c];
/* 910 */   uint32_t    screen_addr;
/* 914 */   uint16_t    screen_width;
/* 916 */   uint16_t    screen_height;
/* 918 */   uint16_t    screen_bpp;
/* 91a */   uint16_t    _unk_91a;
/* 91c */   stawk91c_t* screen_funcs;  
/* 920 */   uint16_t    _unk_920;
} stawk_t;

/*-----------------------------------------------------------------------------
 NOVA VDI function handler
 d0.w = vdi function number
 d1.w = vdi handle
 a0.l = parameter blocks
 a1.l = workstation data (sta_vdi internal)
-----------------------------------------------------------------------------*/
typedef void(*vdi_func)(int16_t fn, int16_t vh, VDIPB* pb, stawk_t* wk);

/*-----------------------------------------------------------------------------
 NOVA VDI function table
-----------------------------------------------------------------------------*/
typedef struct
{
    uint16_t ptsout;
    uint16_t intout;
    vdi_func fun;
} vdi_funcdata_t;

/*-----------------------------------------------------------------------------
 globals
-----------------------------------------------------------------------------*/
#define STAVDI_BSS_OFFSET_NOVAPLANE        (0x00030fbcUL - 0x0002ddb2UL)
#define STAVDI_BSS_OFFSET_VTSCROLLFUNC     (0x000310a4UL - 0x0002ddb2UL)

static vdi_funcdata_t* sta_vdifuncs;
static LINEA* sta_linea;
static uint32_t sta_bss;



/*-----------------------------------------------------------------------------
109 : vro_copyfm
-----------------------------------------------------------------------------*/
vdi_func vro_copyfm_old;
void vro_copyfm_new(int16_t fn, int16_t vh, VDIPB* pb, stawk_t* wk) {
    int16_t vrmode = pb->intin[0];
    MFDB* psrcMFDB = *(MFDB**)&pb->contrl[7];
    MFDB* pdstMFDB = *(MFDB**)&pb->contrl[9];

    if ((psrcMFDB->fd_addr == 0) && (pdstMFDB->fd_addr == 0)) {
        rect_t* src = (rect_t*)&pb->ptsin[0];
        vec_t* dst = (vec_t*)&pb->ptsin[4];

        if (wk->clipflag && !nv_clip_blit(dst, src, &wk->cliprect, &nv_scrnclip)) {
            return;
        }

        if (card->blit(src, dst)) {
            return;
        }
    }
    vro_copyfm_old(fn, vh, pb, wk);
}

/*-----------------------------------------------------------------------------
114 : vr_recfl
-----------------------------------------------------------------------------*/
vdi_func vr_recfl_old;
void vr_recfl_new(int16_t fn, int16_t vh, VDIPB* pb, stawk_t* wk) {

    /* todo: verify better if this is for screen */
    if (card->fill && (vh == 1)) {
        /* only src_copy fills */
        if (wk->writemode == 0) {
            int16_t style;
            int16_t color = wk->vsf_color;
            switch (wk->vsf_interior) {
                case 0: style = 7; color = 0; break;
                case 1: style = 7; break;
                case 2: style = wk->vsf_style; break;
                default: style = 8; break;
            }
            /* only dither patterns */
            if (style < 8) {
                /* clip and blit */
                rect_t* r = (rect_t*)&pb->ptsin[0];
                if (wk->clipflag && !nv_clip_rect(r, &wk->cliprect)) {
                    return;
                }
                if (card->fill(color,  7 - style, r)) {
                    return;
                }
                /* fall through to software blit if 
                driver rejected it for some reason */
            }
        }
    }
    vr_recfl_old(fn, vh, pb, wk);
}

/*-----------------------------------------------------------------------------
11 : v_gdp
-----------------------------------------------------------------------------*/
vdi_func v_gdp_old;
void v_gdp_new(int16_t fn, int16_t vh, VDIPB* pb, stawk_t* wk) {
    if (pb->contrl[5] == 1) {
        /* v_bar, doesn't appear to be used much
            identical to vr_recfl except it can draw an outline if wk->vsf_perimeter is set */
        /*dprintf("vbar: %d,%d,%d : %d,%d - %d,%d (%d,%d)\n", wk->writemode, wk->vsf_interior, wk->vsf_perimeter, pb->ptsin[0], pb->ptsin[1], pb->ptsin[2], pb->ptsin[3], pb->ptsin[2]-pb->ptsin[0], pb->ptsin[3]-pb->ptsin[1]);*/
    }
    v_gdp_old(fn, vh, pb, wk);
}


/*-----------------------------------------------------------------------------
vt_scroll
  d0.w: first row
  d1.w: last row
  a1.l: linea data
-----------------------------------------------------------------------------*/
typedef void(*vt_scroll_func)(int16_t, int16_t, void*, void*);
extern void vt_scroll_asm(int16_t, int16_t, void*, void*);
vt_scroll_func vt_scroll_old;
void vt_scroll(int16_t d0, int16_t d1, void* a0, void* a1) {
    if ((d1 - d0) > 0) {
        int16_t char_height = *((int16_t*)(((uint32_t)a1) - 0x2e)); /* v_cel_ht (linea -2e) */
        if (char_height > 0) {
            rect_t src; vec_t dst;
            src.min.y = char_height * d0;
            src.max.y = char_height * (d1 + 1);
            dst.y = (src.min.y - char_height);
            dst.x = 0;
            src.min.x = 0;
            src.max.x = nova.max_x;
            if (nv_clip_rect(&src, &nv_scrnclip) && card->blit(&src, &dst)) {
                return;
            }
        }
        vt_scroll_old(d0, d1, a0, a1);
    }
}

/*
 * VDI patching.
 *
 * We cannot simply add our own trap2 handler because the VDI's are
 * caching certain stuff as well as playing foul with the trap2 chain.
 * Furthermore, NVDI replaces the trap and gets rid of STA_VDI from
 * that chain completely, calling its dispatcher directly when needed.
 * 
 * So. when we get the first call from the resident STA_VDI we search
 * it's memory from the stacked return address to find and modify
 * the internal VDI jumptable. This is located right after the trap code.
 *
 * VT52 scrolling is patched by finding its function pointer in BSS
 * 
 */
uint32_t vdi_patch_findtrap2_backward(uint32_t from, uint32_t len) {
    uint32_t addr = from - 2;
    for (; addr > (from - len); addr -= 2) {
        if (*((uint32_t*)(addr+0)) == 0x58425241UL) {       /* 'XBRA' */
            if (*((uint32_t*)(addr+4)) == 0x494D4E45UL) {   /* 'IMNE' */
                return addr;
            }
        }
    }
    return 0;
}

vdi_funcdata_t* vdi_patch_find_vditable(uint32_t addr) {
    uint32_t tableaddr = 0;
    uint32_t trap2addr = vdi_patch_findtrap2_backward(addr, 16 * 1024UL);
    dprintf("vdi_patch: start at %08lx\n", addr);
    dprintf("vdi_patch: trap2 at %08lx\n", trap2addr);
    if (!trap2addr) {
        return 0;
    }
    tableaddr = trap2addr + 0x50;
    return (vdi_funcdata_t*)tableaddr;
}

uint32_t vdi_patch_find_bss(vdi_funcdata_t* vditable) {
    uint32_t v105func = (uint32_t)vditable[105].fun;
    uint32_t var_planes = *((uint32_t*)(v105func + 0x20));
    uint32_t bss = var_planes - STAVDI_BSS_OFFSET_NOVAPLANE;
    return bss;
}

bool vdi_patch(void* sp) {
    uint16_t sr;
    sta_vdifuncs = vdi_patch_find_vditable(*((uint32_t*)sp));
    if (!sta_vdifuncs) {
        dprintf("vdi_patch: table not found\n");
        return false;
    }
    dprintf("vdi_patch: table at %08lx\n", (uint32_t)sta_vdifuncs);

    sta_bss = vdi_patch_find_bss(sta_vdifuncs);
    if (!sta_bss) {
        dprintf("bdi_patch: bss not found\n");
    }
    dprintf("vdi_patch: bss   at %08lx\n", sta_bss);

    sr = cpu_di();

    /* vro_copyfm */
    vro_copyfm_old = sta_vdifuncs[109].fun;
    sta_vdifuncs[109].fun = vro_copyfm_new;

    /* vr_recfl */
    if (card->fill) {
        vr_recfl_old = sta_vdifuncs[114].fun;
        sta_vdifuncs[114].fun = vr_recfl_new;
    }

#if 0
    /* v_gpd */
    v_gdp_old = sta_vdifuncs[11].fun;
    sta_vdifuncs[11].fun = v_gdp_new;
#endif

    /* vt_scroll */
    vt_scroll_old = *((vt_scroll_func*)(sta_bss + STAVDI_BSS_OFFSET_VTSCROLLFUNC));
    *((vt_scroll_func*)(sta_bss + STAVDI_BSS_OFFSET_VTSCROLLFUNC)) = vt_scroll_asm;

    /* grab pointer to sta_vdi's internal linea structure
     * vro_copyfm beings with "movea.l LINEA.l,A2" ($24 $79 $addr) */
    sta_linea = *(LINEA**)(((uint32_t)vro_copyfm_old) + 0x02);
    dprintf("vdi_patch: linea at %08lx\n", (uint32_t)sta_linea);

    cpu_flush_cache();
    cpu_ei(sr);
    return true;
}

