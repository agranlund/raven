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
#include "sta_vdi.h"

#define VDI_DEBUG           0
#define VDI_ACCEL           1
#define VDI_ACCEL_VBAR      1
#define VDI_ACCEL_HLINE     1
#define VDI_ACCEL_PLINE     0

#define STA_DISASM_ABS              0x10000UL /* text offset in disassembly */

#define STA_OFFSET_TRAP2            (0x219d0UL - STA_DISASM_ABS)  /* used to find relocated start and everything else */
#define STA_OFFSET_VT_TABLE         (0x1beb2UL - STA_DISASM_ABS)  /* */
#define STA_OFFSET_VD_TABLE         (0x1c616UL - STA_DISASM_ABS)

#define STA_OFFSET_VDI_TABLE        (0x21a20UL - STA_DISASM_ABS)    /* vdi dispatch table */
#define STA_OFFSET_LINEA_PTRS       (0x1c06cUL - STA_DISASM_ABS)    /* neg, pos */

#define STA_OFFSET_VTFUNCS_CUR      (0x31080UL - STA_DISASM_ABS)    /* todo should not use */
#define STA_OFFSET_VDFUNCS_CUR      (0x308daUL - STA_DISASM_ABS)    /* todo should not use */
#define STA_OFFSET_WKS_CUR          (0x2e8e0UL - STA_DISASM_ABS)


/* 0x2e8da is a pointer to the active table in 0x308da, which in turn was 
    copied from the correct static table depending on gfxmode.
    which happens in openwk */

/*-----------------------------------------------------------------------------
 globals
-----------------------------------------------------------------------------*/
static uint32_t sta_text;               /* start of relocated text+data+bss */
static VDIESC* sta_vdiesc;              /* negative linea */
static LINEA* sta_linea;                /* positive linea */

static dev_hdr_t* sta_devhdr;           /* dev header for all modes */
static dev_funcs_t* sta_devfuncs;       /* dev funcs */

static vdi_table_t* sta_vdifuncs;       /* vdi functions for current mode */
static vt_table_t*  sta_vtfuncs;        /*  vt functions for current mode */

/*-----------------------------------------------------------------------------
vdi helpers
-----------------------------------------------------------------------------*/
#define vdi_min(a,b) (((a)<(b)) ? (a) : (b))
#define vdi_max(a,b) (((a)>(b)) ? (a) : (b))
#define vdi_is_screenwk(_wk) (_wk->screen_addr >= VADDR_MEM)

static uint32_t vdi_coord_to_addr(vec_t* coord) {
    uint16_t bits_per_pixel = (nova->planes < 8) ? 1 : ((nova->planes+1) & ~7);
    return (((uint32_t)coord->y) * nova->pitch) + ((coord->x * bits_per_pixel) >> 3);
}

static bool vdi_getrop(stawk_t* wk, uint16_t* rop) {
#if 1
    *rop = BL_ROP_S;
    return (wk->writemode == 0) ? true : false;
#else
    /* todo: verify driver rop implementation */
    switch (wk->writemode) {
        default:
        case 0: { /* replace */
            *rop = BL_ROP_S;
        } break;
        case 1: { /* transparent */
            *rop = BL_ROP_DSo;
        } break;
        case 2: { /* xor */
            *rop = BL_ROP_DSx;
        } break;
        case 3: { /* reverse transparent */
            *rop = BL_ROP_DSno;
        } break;
    }
    return true;
#endif
}

static bool vdi_getfillparams(stawk_t* wk, int16_t* style, int16_t* color, uint16_t* rop) {
    if (!vdi_getrop(wk, rop)) {
        return false;
    }
    switch (wk->vsf_interior) {
        default:    /* undefined -> hollow */
        case 0: {   /* hollow */
            *style = 0;
            *color = wk->vsf_color;
            return true;
        }
        case 1: {   /* solid */
            *style = 0;
            *color = wk->vsf_color;
            return true;
        }
        case 2: {   /* pattern */
            *style = (7 - wk->vsf_style);
            *color = wk->vsf_color;
            return ((*style) >= 0);
        }
        case 3: {   /* hatch */
            return false;            
        }
        case 4: {   /* user defined */
            return false;            
        }
    }
}

static void vdi_prepare_clip(rect_t* clip, rect_t* wkclip) {
    clip->min.x = 0; clip->max.x = nova->max_x;
    clip->min.y = 0; clip->max.y = nova->max_y;
    if (wkclip) {
        clip->min.x = vdi_max(clip->min.x, vdi_min(wkclip->min.x, wkclip->max.x));
        clip->max.x = vdi_min(clip->max.x, vdi_max(wkclip->min.x, wkclip->max.x));
        clip->min.y = vdi_max(clip->min.y, vdi_min(wkclip->min.y, wkclip->max.y));
        clip->max.y = vdi_min(clip->max.y, vdi_max(wkclip->min.y, wkclip->max.y));
    }
}

static bool vdi_clip_blit(rect_t* d, rect_t* s, vec_t* din, rect_t* sin, rect_t* wkclip) {
    /* prepare cliprect */
    rect_t clip;
    vdi_prepare_clip(&clip, wkclip);
    /* prepare output rects, early out on fully outside */
    s->min.x = vdi_min(sin->min.x, sin->max.x); if (s->min.x > clip.max.x) { return false; }
    s->max.x = vdi_max(sin->min.x, sin->max.x); if (s->max.x < clip.min.x) { return false; }
    s->min.y = vdi_min(sin->min.y, sin->max.y); if (s->min.y > clip.max.y) { return false; }
    s->max.y = vdi_max(sin->min.y, sin->max.y); if (s->max.y < clip.min.y) { return false; }
    d->min.x = din->x; if (d->min.x > clip.max.x) { return false; }
    d->min.y = din->y; if (d->min.y > clip.max.y) { return false; }
    d->max.x = din->x + (s->max.x - s->min.x); if (d->max.x < clip.min.x) { return false; }
    d->max.y = din->y + (s->max.y - s->min.y); if (d->max.y < clip.min.y) { return false; }
    /* clip source rect */
    if (s->min.x < clip.min.x) { d->min.x += (clip.min.x - s->min.x); s->min.x = clip.min.x; }
    if (s->min.y < clip.min.y) { d->min.y += (clip.min.y - s->min.y); s->min.y = clip.min.y; }
    if (s->max.x > clip.max.x) { d->max.x -= (s->max.x - clip.max.x); s->max.x = clip.max.x; }
    if (s->max.y > clip.max.y) { d->max.y -= (s->max.y - clip.max.y); s->max.y = clip.max.y; }
    /* clip dest rect */
    if (d->min.x < clip.min.x) { s->min.x += (clip.min.x - d->min.x); d->min.x = clip.min.x; }
    if (d->min.y < clip.min.y) { s->min.y += (clip.min.y - d->min.y); d->min.y = clip.min.y; }
    if (d->max.x > clip.max.x) { s->max.x -= (d->max.x - clip.max.x); d->max.x = clip.max.x; }
    if (d->max.y > clip.max.y) { s->max.y -= (d->max.y - clip.max.y); d->max.y = clip.max.y; }
    /* result clip */
    if (((d->max.x - d->min.x) < 0) || ((d->max.y - d->min.y) < 0)) {
        return false;
    }
    return true;    
}

static bool vdi_clip_rect(rect_t* d, rect_t* din, rect_t* wkclip) {
    /* prepare cliprect */
    rect_t clip;
    vdi_prepare_clip(&clip, wkclip);
    /* sort area min/max and reject fully outside */
    d->min.x = vdi_min(din->min.x, din->max.x); if (d->min.x > clip.max.x) { return false; }
    d->max.x = vdi_max(din->min.x, din->max.x); if (d->max.x < clip.min.x) { return false; }
    d->min.y = vdi_min(din->min.y, din->max.y); if (d->min.y > clip.max.y) { return false; }
    d->max.y = vdi_max(din->min.y, din->max.y); if (d->max.y < clip.min.y) { return false; }
    /* adjust to cliprect */
    d->min.x = vdi_max(d->min.x, clip.min.x);
    d->max.x = vdi_min(d->max.x, clip.max.x);
    d->min.y = vdi_max(d->min.y, clip.min.y);
    d->max.y = vdi_min(d->max.y, clip.max.y);
    return true;
}


/*-----------------------------------------------------------------------------
109 : vro_copyfm
-----------------------------------------------------------------------------*/
vdi_func vro_copyfm_old;
void vro_copyfm_new(int16_t fn, int16_t vh, VDIPB* pb, stawk_t* wk) {
    int16_t vrmode = pb->intin[0];
    MFDB* psrcMFDB = *(MFDB**)&pb->contrl[7];
    MFDB* pdstMFDB = *(MFDB**)&pb->contrl[9];
    if ((psrcMFDB->fd_addr == 0) && (pdstMFDB->fd_addr == 0)) {
        if (card->caps & NV_CAPS_BLIT) {
            rect_t src; rect_t dst;
            if (!vdi_clip_blit(&dst, &src, (vec_t*)&pb->ptsin[4], (rect_t*)&pb->ptsin[0], wk->clipflag ? &wk->cliprect : 0)) {
                return;
            }
            if (card->blit(((vrmode & BL_MASK_ROP) << BL_SHIFT_ROP), &src, &dst.min)) {
                return;
            }
        }
    }
    vro_copyfm_old(fn, vh, pb, wk);
}

/*-----------------------------------------------------------------------------
114 : vr_recfl
-----------------------------------------------------------------------------*/
vdi_func vr_recfl_old;
void vr_recfl_new(int16_t fn, int16_t vh, VDIPB* pb, stawk_t* wk) {
    if (vdi_is_screenwk(wk)) {
        if (card->caps & NV_CAPS_FILL) {
            int16_t style = 0; int16_t color = 0; uint16_t rop = BL_ROP_S;
            if ((wk->vsf_interior == 0) || vdi_getfillparams(wk, &style, &color, &rop)) {
                rect_t dst;
                if (!vdi_clip_rect(&dst, (rect_t*)&pb->ptsin[0], wk->clipflag ? &wk->cliprect : 0)) {
                    return;
                }
                if (card->blit(BL_FILL|rop|BL_FGCOL(color)|BL_PATTERN(style), &dst, &dst.min)) {
                    return;
                }
            }
        }
    }
    vr_recfl_old(fn, vh, pb, wk);
}

/*-----------------------------------------------------------------------------
11 : v_gdp
-----------------------------------------------------------------------------*/
#if VDI_ACCEL_VBAR    
vdi_func v_gdp_old;
void v_gdp_new(int16_t fn, int16_t vh, VDIPB* pb, stawk_t* wk) {
    if (vdi_is_screenwk(wk) && (card->caps & NV_CAPS_FILL)) {

        /*------------------------------------------
        1 : v_bar
        ------------------------------------------*/           
        if (pb->contrl[5] == 1) {
            rect_t dst; int16_t style, color; uint16_t rop;
            /*dprintf(("wk = %08lx dfun = %08lx\n", (uint32_t)wk, (uint32_t)wk->screen_funcs));*/
            if (vdi_getfillparams(wk, &style, &color, &rop)) {
                if (!vdi_clip_rect(&dst, (rect_t*)&pb->ptsin[0], wk->clipflag ? &wk->cliprect : 0)) {
                    return;
                }
                if ((wk->vsf_interior == 0) || card->blit(BL_FILL|rop|BL_FGCOL(color)|BL_PATTERN(style), &dst, &dst.min)) {
                    if (wk->vsf_perimeter) {
                        rect_t l;
                        l.min.x = dst.min.x;          /* top */
                        l.max.x = dst.max.x;
                        l.min.y = dst.min.y;
                        l.max.y = dst.min.y;
                        card->blit(BL_FILL|rop|BL_FGCOL(color)|BL_PATTERN(0), &l, &l.min);
                        l.min.y = dst.max.y;          /* bottom */
                        l.max.y = dst.max.y;
                        card->blit(BL_FILL|rop|BL_FGCOL(color)|BL_PATTERN(0), &l, &l.min);
                        l.max.x = dst.min.x;          /* left */
                        l.min.y = dst.min.y;
                        card->blit(BL_FILL|rop|BL_FGCOL(color)|BL_PATTERN(0), &l, &l.min);
                        l.min.x = dst.max.x;          /* right */
                        l.max.x = dst.max.x;
                        card->blit(BL_FILL|rop|BL_FGCOL(color)|BL_PATTERN(0), &l, &l.min);
                    }
                    return;
                }
            }
        }
    }

    /* fallback on software version */
    v_gdp_old(fn, vh, pb, wk);
}
#endif

/*-----------------------------------------------------------------------------
vt_clear_rows
  d0.w: first row
  d1.w: last row
  a1.l: linea data
-----------------------------------------------------------------------------*/
typedef void(*vt_clear_rows_fun)(int16_t, int16_t, void*, void*);
vt_clear_rows_fun vt_clear_rows_old;
extern void vt_clear_rows_asm(int16_t, int16_t, void*, void*);
void vt_clear_rows_new(int16_t d0, int16_t d1, void* a0, void* a1) {
    if ((d1 - d0) >= 0) {
        int16_t char_height = *((int16_t*)(((uint32_t)a1) - 0x2e)); /* v_cel_ht (linea -2e) */
        if (char_height > 0) {
            if (card->caps & NV_CAPS_BLIT) {
                rect_t src; rect_t dst;
                src.min.x = 0;
                src.max.x = nova->max_x;
                src.min.y = char_height * d0;
                src.max.y = char_height * (d1 + 1);
                if (!vdi_clip_rect(&dst, &src, 0)) {
                    return;
                }
                if (card->blit(BL_ROP_S|BL_FILL, &dst, &dst.min)) {
                    return;
                }
            }
        }
        vt_clear_rows_old(d0, d1, a0, a1);
    }
}


/*-----------------------------------------------------------------------------
vt_copy_rows
  d0.w: first row
  d1.w: last row
  a1.l: linea data
-----------------------------------------------------------------------------*/
typedef void(*vt_copy_rows_fun)(int16_t, int16_t, void*, void*);
vt_copy_rows_fun vt_copy_rows_old;
extern void vt_copy_rows_asm(int16_t, int16_t, void*, void*);
void vt_copy_rows_new(int16_t d0, int16_t d1, void* a0, void* a1) {
    if ((d1 - d0) > 0) {
        int16_t char_height = *((int16_t*)(((uint32_t)a1) - 0x2e)); /* v_cel_ht (linea -2e) */
        if (char_height > 0) {
            if (card->caps & NV_CAPS_BLIT) {
                rect_t src; rect_t dst;
                src.min.x = 0;
                src.max.x = nova->max_x;
                src.min.y = char_height * d0;
                src.max.y = char_height * (d1 + 1);
                dst.min.x = 0;
                dst.min.y = (src.min.y - char_height);
                if (!vdi_clip_blit(&dst, &src, &dst.min, &src, 0)) {
                    return;
                }
                if (card->blit(BL_ROP_S, &src, &dst.min)) {
                    return;
                }
            }
        }
        vt_copy_rows_old(d0, d1, a0, a1);
    }
}

/*-----------------------------------------------------------------------------
vd_hline_noclip
-----------------------------------------------------------------------------*/
extern uint32_t vd_hline_noclip_old;
extern void vd_hline_noclip_asm(void);
int16_t vd_hline_noclip_new(int16_t x0, int16_t x1, stawk_t* wk) {
    int16_t style, color; uint16_t rop; rect_t r;
    r.min.x = x0;
    r.max.x = x1;
    r.min.y = *((uint16_t*)(0x8ca+(uint32_t)wk));
    r.max.y = r.min.y;
    if (vdi_getfillparams(wk, &style, &color, &rop)) {
        if (wk->vsf_interior == 0) { color = 0; }
        if (card->blit(BL_FILL|rop|BL_FGCOL(color)|BL_PATTERN(style), &r, &r.min)) {
            return true;
        }
    }
    return false;
}

/*-----------------------------------------------------------------------------
vd_hline
-----------------------------------------------------------------------------*/
extern uint32_t vd_hline_old;
extern void vd_hline_asm(void);
int16_t vd_hline_new(int16_t d0, uint16_t* a0, stawk_t* wk) {
    return false;
}

/*-----------------------------------------------------------------------------
vd_line
-----------------------------------------------------------------------------*/
extern uint32_t vd_line_old;
extern void vd_line_asm(void);
int16_t vd_line_new(int16_t d0, uint16_t* a0, stawk_t* wk) {
#if 0
    int16_t i;
    vec_t* v = (vec_t*)a0;
    if (d0 == 2) {
        dprintf(("%d: ", d0));
        for (i=0; i<d0; i++) {
            dprintf(("[%d,%d] ", v[i].x, v[i].y));
        }
        dprintf(("\n"));
    }
#elif 1
    rect_t* r = (rect_t*)a0;
    if (r && ((r->min.y == r->max.y) || (r->min.x == r->max.x))) {
        int16_t style, color; uint16_t rop; rect_t dst;
        if (!vdi_clip_rect(&dst, r, 0)) {
            /*dprintf(("clip out\n"));*/
            return false;
        }
        if (vdi_getfillparams(wk, &style, &color, &rop)) {
            if (card->blit(BL_FILL|rop|BL_FGCOL(color)|BL_PATTERN(style), &dst, &dst.min)) {
                dprintf(("blit %d,%d -> %d,%d\n", dst.min.x, dst.min.y, dst.max.x, dst.max.y));
                return true;
            }
        }
    }
#else
#endif
    return false;
}


/*-----------------------------------------------------------------------------
 * VDI patching.
 *  - look at return address in the call we get from sta_vdi.prg
 *  - search memory backwards to identify the trap2 handler
 *  - from here we can work out the relocated addresses of
 *    all things we are interested in
 *  - patch vdi dispatcher jumptable
 *  - patch mode-specific VT52 function tables
 *  - patch mode-specific gfx function tables
-----------------------------------------------------------------------------*/

bool vdi_patch(void* sp) {
    uint16_t sr;
    uint32_t trap2addr = 0;

    /* search backward for start of trap2 handler */
    uint32_t attack_pos = *((uint32_t*)sp);
    uint32_t attack_end = attack_pos - (1024UL * 16);

    dprintf(("patch\n"));
#if !VDI_ACCEL
    dprintf((" disabled\n"));
    return false;
#endif
    dprintf((" attack:    %08lx\n", attack_pos));

    for (; (attack_pos > attack_end) && !trap2addr; attack_pos -=2 ) {
        if (*((uint32_t*)(attack_pos+0)) == 0x58425241UL) {       /* 'XBRA' */
            if (*((uint32_t*)(attack_pos+4)) == 0x494D4E45UL) {   /* 'IMNE' */
                trap2addr = attack_pos;
            }
        }
    }
    dprintf((" trap2:     %08lx\n", ((uint32_t)trap2addr)));
    if (!trap2addr) {
        return false;
    }

    /* from here we can work out relocated addresses of all interesting things */
    sta_text = trap2addr - STA_OFFSET_TRAP2;
    dprintf((" text:      %08lx\n", sta_text));

    /* and patch them */
    sr = cpu_di();

    /*--------------------------------------------------------------------------
        MISC
    --------------------------------------------------------------------------*/
    sta_vdiesc = (VDIESC*) *((uint32_t*)(sta_text + STA_OFFSET_LINEA_PTRS + 0));
    sta_linea  = (LINEA*) *((uint32_t*)(sta_text + STA_OFFSET_LINEA_PTRS + 4));
    dprintf((" linea:     %08lx\n", (uint32_t)sta_linea));
    dprintf((" vdiesc:    %08lx\n", (uint32_t)sta_vdiesc));

    /*--------------------------------------------------------------------------
        VT52
        - todo: patch const table with all modes rather than active copy in ram?
          if so, we would need to patch earlier than now, before openwk
    --------------------------------------------------------------------------*/
    sta_vtfuncs = (vt_table_t*)(sta_text + STA_OFFSET_VTFUNCS_CUR);
    dprintf((" vtfuncs:   %08lx\n", (uint32_t)sta_vtfuncs));

    /* vt_fill_rows */
    vt_clear_rows_old = (vt_clear_rows_fun)(sta_vtfuncs->clear_rows);
    sta_vtfuncs->clear_rows = (uint32_t)vt_clear_rows_asm;

    /* vt_copy_rows */
    vt_copy_rows_old = (vt_copy_rows_fun)(sta_vtfuncs->copy_rows);
    sta_vtfuncs->copy_rows = (uint32_t)vt_copy_rows_asm;

    /*--------------------------------------------------------------------------
        GFX
        - todo: patch const table with all modes rather than active copy in ram?
        if so, we would need to patch earlier than now, before openwk
    --------------------------------------------------------------------------*/
    sta_devfuncs = (dev_funcs_t*)(sta_text + STA_OFFSET_VDFUNCS_CUR);

#if VDI_ACCEL_HLINE
    /*  This method is not really worth it.
        Setup overhead on each line ends up stealing most of the gain
        unless drawing longer lines.
        Circles, polygons and so on should really collect all hlines
        in a first pass and then blit them all in a second step.

        At the moment, these functions will reject <128 pixel lines
        so we don't _loose_ performance by using the blitter
    */
    vd_hline_noclip_old = sta_devfuncs->hline_noclip;
    sta_devfuncs->hline_noclip = (uint32_t)vd_hline_noclip_asm;
    vd_hline_old = sta_devfuncs->hline;
    sta_devfuncs->hline = (uint32_t)vd_hline_asm;
#endif

#if VDI_ACCEL_PLINE
    vd_line_old = sta_devfuncs->line;
    sta_devfuncs->line = (uint32_t)vd_line_asm;
#endif

    /*--------------------------------------------------------------------------
        VDI
    --------------------------------------------------------------------------*/
    sta_vdifuncs = (vdi_table_t*)(sta_text + STA_OFFSET_VDI_TABLE);
    dprintf((" vditab:    %08lx\n", (uint32_t)sta_vdifuncs));

    /* vro_copyfm */
    vro_copyfm_old = sta_vdifuncs[109].fun;
    sta_vdifuncs[109].fun = vro_copyfm_new;

    /* vr_recfl */
    vr_recfl_old = sta_vdifuncs[114].fun;
    sta_vdifuncs[114].fun = vr_recfl_new;

    /* v_gpd */
#if VDI_ACCEL_VBAR    
    v_gdp_old = sta_vdifuncs[11].fun;
    sta_vdifuncs[11].fun = v_gdp_new;
#endif


    cpu_flush_cache();
    cpu_ei(sr);
    return true;
}

