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
#include "nova.h"

rect_t nv_vramclip;
rect_t nv_scrnclip;

/*-------------------------------------------------------------------------------
 * clipping
 *-----------------------------------------------------------------------------*/
bool nv_clip_point(vec_t* point, rect_t* dclip) {
    if ((point->x < dclip->min.x) || (point->x > dclip->max.x) ||
        (point->y < dclip->min.y) || (point->y > dclip->max.y)
    ) { return false; }
    return true;
}

bool nv_clip_rect(rect_t* rect, rect_t* dclip) {
    /* early out */
    if ((rect->min.x > dclip->max.x) || (rect->max.x < dclip->min.x) ||
        (rect->min.y > dclip->max.y) || (rect->max.y < dclip->min.y)
    ) { return false;
    }
    /* clip it */
    rect->min.x = rect->min.x < dclip->min.x ? dclip->min.x : rect->min.x;
    rect->max.x = rect->max.x > dclip->max.x ? dclip->max.x : rect->max.x;
    rect->min.y = rect->min.y < dclip->min.y ? dclip->min.y : rect->min.y;
    rect->max.y = rect->max.y > dclip->max.y ? dclip->max.y : rect->max.y;
    return true;
}

bool nv_clip_blit(vec_t* dst, rect_t* src, rect_t* dclip, rect_t* sclip) {
    int16_t dst_mx, dst_my;

    /* early out source clip */
    if ((src->min.x > sclip->max.x) || (src->max.x < sclip->min.x) || (src->min.y > sclip->max.y) || (src->max.y < sclip->min.y)) {
        return false;
    }

    /* early out dest clip */
    dst_mx = dst->x + (src->max.x - src->min.x);
    dst_my = dst->y + (src->max.y - src->min.y);
    if ((dst->x > dclip->max.x) || (dst->y > dclip->max.y) || (dst_mx < dclip->min.y) || (dst_my < dclip->min.y)) {
        return false;
    }

    /* source clip */
    if (src->min.x < sclip->min.x) {
        dst->x += (sclip->min.x - src->min.x);
        src->min.x = sclip->min.x;
    }
    if (src->min.y < sclip->min.y) {
        dst->y += (sclip->min.y - src->min.y);
        src->min.y = sclip->min.y;
    }
    if (src->max.x > sclip->max.x) {
        dst_mx -= (src->max.x - sclip->max.x);
        src->max.x = sclip->max.x;
    }
    if (src->max.y > sclip->max.y) {
        dst_my -= (src->max.y - sclip->max.y);
        src->max.y = sclip->max.y;
    }
    /* dest clip */
    if (dst->x < dclip->min.x) {
        src->min.x += (dclip->min.x - dst->x);
        dst->x = dclip->min.x;
    }
    if (dst->y < dclip->min.y) {
        src->min.y += (dclip->min.y - dst->y);
        dst->y = dclip->min.y;
    }
    if (dst_mx > dclip->max.x) {
        src->max.x -= (dst_mx - dclip->max.x);
    }
    if (dst_my > dclip->max.y) {
        src->max.y -= (dst_my - dclip->max.y);
    }
    /* result clip */
    if (((src->max.x - src->min.x) < 0) || ((src->max.y - src->min.y) < 0)) {
        return false;
    }
    return true;
}

bool nv_clip_line(line_t* line, rect_t* dclip) {
    /* todo: */
    return true;
}

/*-------------------------------------------------------------------------------
 * common blitter helpers
 *-----------------------------------------------------------------------------*/
uint32_t nv_fillpatterns[8] = {
    0xffffffffUL, 0xeeffbbffUL, 0xaaffaaffUL, 0xaaddaa77UL,
    0xaa55aa55UL, 0x88552255UL, 0x00550055UL, 0x00440011UL
};
