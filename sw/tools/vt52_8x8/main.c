/*-------------------------------------------------------------------------------
 * vt52_8x8
 * (c)2025 Anders Granlund
 *
 * Use 8x8 font in terminal
 * Install as desktop application that runs when gem starts
 *
 *-------------------------------------------------------------------------------
 *
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
 *
 *-----------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <linea.h>
#include "raven.h"
#include "sysutil.h"

extern LINEA *Linea;
extern VDIESC *Vdiesc;
extern FONTS *Fonts;

long supermain(int args, char** argv)
{
    FONT_HDR* font;
    linea_init();
    font = Fonts->font[1];
    Vdiesc->v_cel_ht = font->frm_hgt;
    Vdiesc->v_cel_wr = Linea->v_lin_wr * font->frm_hgt;
    Vdiesc->v_cel_mx = (Vdiesc->v_rez_hz / font->wcel_wdt) - 1;
    Vdiesc->v_cel_my = (Vdiesc->v_rez_vt / font->frm_hgt) - 1;
    Vdiesc->v_fnt_wd = font->frm_wdt;
    Vdiesc->v_fnt_st = font->ade_lo;
    Vdiesc->v_fnt_nd = font->ade_hi;
    Vdiesc->v_fnt_ad = font->fnt_dta;
    Vdiesc->v_off_ad = font->ch_ofst;    
    return 0;
}

int main(int args, char** argv) {
    return Supmain(args, argv, supermain);
}
