/*-------------------------------------------------------------------------------
 * STA_VDI internal structures
 * (c)2026 Anders Granlund
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
#include <stdint.h>

/*-----------------------------------------------------------------------------
 NOVA VT52 function ptrs per mode in DATA section
 There are seven of them in the order: 1, 4, 8, 15, 16, 24, 32 bpp
-----------------------------------------------------------------------------*/
typedef struct {
/* 00 */    uint32_t    mouse_save;
/* 04 */    uint32_t    mouse_restore;
/* 08 */    uint32_t    _unk08;
/* 0c */    uint32_t    invert_char;
/* 10 */    uint32_t    clear_row_from_cursor;
/* 14 */    uint32_t    clear_rows;                 /* void clear_rows(int16_t first, int16_t last) */
/* 18 */    uint32_t    copy_rows;                  /* */
/* 1c */    uint32_t    _unk1c;
/* 20 */    uint32_t    _unk20;
/* 24 */    uint32_t    _unk24;
/* 28 */    uint32_t    _unk28;                     /* pointer to some table */
/* 2c */    uint32_t    _unk2c;
/* 30 */    uint32_t    _unk30;
/* 34 */    uint32_t    _unk34;
/* 38 */    uint16_t    _null38;                    /* appears to be padding */
} vt_static_table_t;

/*-----------------------------------------------------------------------------
 NOVA VT52 active device funcs and data
 Filled in from vt_data_t for the active device, and other things.
-----------------------------------------------------------------------------*/
typedef struct {                                /* Ghidra label */
/* 00 */    uint32_t    bg_colors;              /* ModeSpecificFunc10   */
/* 04 */    uint32_t    mouse_save;             /* ModeSpecificFunc0    */
/* 08 */    uint32_t    mouse_restore;          /* ModeSpecificFunc1    */
/* 0c */    uint32_t    _unk0c;                 /* ModeSpecificFunc8    */
/* 10 */    uint32_t    _unk10;                 /* ModeSpecificFunc9    */
/* 14 */    uint32_t    invert_char;            /* ModeSpecificFunc3    */
/* 18 */    uint32_t    clear_rows;             /* ModeSpecificFunc5    */
/* 1c */    uint32_t    clear_row_from_cursor;  /* ModeSpecificFunc4    */
/* 20 */    uint32_t    _unk20;                 /* ModeSpecificFunc2    */
/* 24 */    uint32_t    copy_rows;              /* ModeSpecificFunc6    */
/* 28 */    uint32_t    _unk28;                 /* ModeSpecificFunc7    */
/* 2c */    uint32_t    _unk2c;                 /* DAT_000310ac         */
/* 30 */    uint32_t    _unk30;                 /* ModeSpecificFunc11   */
/* 34 */    uint32_t    _unk34;                 /* ModeSpecificFunc12   */
/* 38 */    uint32_t    _unk38;                 /* ModeSpecificFunc13   */
/* 40 */    uint16_t    _unk40;                 /* ?? mouse save flag   */
/* 42 */    uint32_t    _unk42;                 /* ?? mouse save data   */
} vt_table_t;

/*-----------------------------------------------------------------------------
 NOVA VDI internal device functions for a particular screen mode
 wks struct in bss holds a pointer the the active one.
-----------------------------------------------------------------------------*/
typedef struct {                /* 8bpp default */
/* 00 */    uint32_t    _unk00; /* rts */
/* 04 */    uint32_t    _unk04;
/* 08 */    uint32_t    _unk08; /* rts */
/* 0c */    uint32_t    _unk0c;
/* 10 */    uint32_t    _unk10;
/* 14 */    uint32_t    _unk14; /* rts */       /* used in 4bpp */
/* 18 */    uint32_t    _unk18; /* rts */       /* used in 4bpp */
/* 1c */    uint32_t    _unk1c; /* rts */
/* 20 */    uint32_t    _unk20; /* rts */
/* 24 */    uint32_t    hline_noclip;           /* called by pieslice */
/* 28 */    uint32_t    hline_setup;            /* calc screen address and writemode funcptr */
/* 2c */    uint32_t    _unk2c;                 /* vr_recfl */      /* <---- verify */
/* 30 */    uint32_t    _unk30;
/* 34 */    uint32_t    _unk34;
/* 38 */    uint32_t    _unk38; /* rts */
/* 3c */    uint32_t    line_special;           /* jumps to 44 after setting A0=(8c8) and D0 = 2 (becomes 0 in 44) */
/* 40 */    uint32_t    _unk40; /* rts */
/* 44 */    uint32_t    line;                   /* general purpose line, called by pieslice, fillarea, etc */
/* 48 */    uint32_t    hline;                  /* linea_hline      */  /* called by vdi_circle */
/* 4c */    uint32_t    putpixel;               /* linea_putpixel   */
/* 50 */    uint32_t    getpixel;               /* linea_getpixel   */
/* 54 */    uint32_t    bitblt; /* rts */       /* linea_bitblt     */
/* 58 */    uint32_t    copyfm;                 /* linea_copyfm + vro_copyfm */
/* 5c */    uint32_t    _unk5c;
/* 60 */    uint32_t    _unk60;
/* 64 */    uint32_t    _unk64;
/* 68 */    uint32_t    _unk68;
/* 6c */    uint32_t    _unk6c;                 /* looks like a get color */        /* sta_22_vst_color, sta_25_vsf_color */
/* 70 */    uint32_t    _unk70;                 /* looks like a get color */        /* sta_22_vst_color, sta_25_vsf_color, sta_105_v_getpixel */
/* 74 */    uint32_t    _unk74;                 /* vid_getpixel, same as 0x4c */    /* sta_105_v_getpixel */
/* 78 */    uint32_t    _unk78;
/* 7c */    uint32_t    _unk7c;
/* 80 */    uint32_t    _unk80;
/* 84 */    uint32_t    _unk84[3];  /* nulldata */
} dev_funcs_t;

/*-----------------------------------------------------------------------------
 Master table of device/mode structs
-----------------------------------------------------------------------------*/
typedef struct {
    uint16_t        num_1bpp;
    dev_funcs_t*    tab_1bpp[2];    /* 0, 0         */
    uint16_t        num_4bpp;
    dev_funcs_t*    tab_4bpp[1];    /* 0            */
    uint16_t        num_8bpp;
    dev_funcs_t*    tab_8bpp[2];    /* 0, 0         */
    uint16_t        num_15bpp;
    dev_funcs_t*    tab_15bpp[1];   /* 0            */
    uint16_t        num_16bpp;
    dev_funcs_t*    tab_16bpp[1];   /* 0            */
    uint16_t        num_24bpp;
    dev_funcs_t*    tab_24bpp[3];   /* 0, 0, 2      */
    uint16_t        num_32bpp;
    dev_funcs_t*    tab_32bpp[4];   /* 0, 0, 2, 3   */
} dev_hdr_t;

/*-----------------------------------------------------------------------------
 NOVA VDI workstation data
-----------------------------------------------------------------------------*/
typedef struct {
/* 000 */   uint8_t         _unk_000[0x01c - 0x000];
/* 01c */   uint16_t        vsf_color;
/* 01e */   uint16_t        vsf_style;
/* 020 */   uint16_t        vsf_perimeter;
/* 022 */   uint16_t        vsf_interior;
/* 024 */   uint8_t         _unk_024[0x4b0 - 0x024];
/* 4b0 */   uint16_t        writemode;
/* 4b2 */   uint16_t        _unk_4b2;
/* 4b4 */   uint16_t        clipflag;
/* 4b6 */   rect_t          cliprect;
/* 4be */   uint8_t         _unk_4be_updateflag;    /* set to 1 in vsf_perimeter and similar */
/* 4c0 */   uint8_t         _unk_4c0[0x8c2 - 0x4c0];        /* 0x402 / 1026 */
/* some of the following are being updated from at the start of every */
/* sta_vdi handler when 'is_screen' is not zero */
/* 8c2 */   uint16_t        bytes_per_line;
/* 8c4 */   uint8_t         _unk_8c4[0x90a - 0x8c4];        /* 0x46 / 70 */
/* 90a */   uint16_t        changed_flag;
/* 90c */   uint8_t         _unk_90c[0x910 - 0x90c];        /* 0x4 / 4 */
/* 910 */   uint32_t        screen_addr;
/* 914 */   uint16_t        screen_width;
/* 916 */   uint16_t        screen_height;
/* 918 */   uint16_t        screen_bpp;
/* 91a */   uint16_t        _unk_91a;
/* 91c */   dev_funcs_t*    screen_funcs;   /* internal device funcs for current screenmode */
/* 920 */   uint16_t        _unk_920;
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
} vdi_table_t;

