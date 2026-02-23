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



/*
    circle: 68, 14, 18, 60, 44, 
    60 is some kind of setup, depends on writemode
    44 appears to be general purpose line.
*/


/*
    
91c_8bpp_unk48:                     ; draw hline, with clipping.
    movem.l    {  A6 A5 A4 A3 A2 D7 D6 D5 D4 D3},-(SP)
    movea.l    A0,A6
    move.w     (0x8c8,A6),D0w           ; x0
    move.w     (0x8cc,A6),D1w           ; x1
    move.w     (0x8ca,A6),D6w           ; y0
  ; reject fully outside
    cmp.w      (0x4b8,A6),D6w
    blt.b      LAB_000103e0
    cmp.w      (0x4bc,A6),D6w
    bgt.b      LAB_000103e0
    cmp.w      (0x4ba,A6),D0w
    bgt.b      LAB_000103e0
    cmp.w      (0x4b6,A6),D1w
    blt.b      LAB_000103e0
  ; clip left and right sides
    cmp.w      (0x4b6,A6),D0w
    bgt.b      LAB_000103d4
    move.w     (0x4b6,A6),D0w
LAB_000103d4
    cmp.w      (0x4ba,A6),D1w
    blt.b      LAB_000103de
    move.w     (0x4ba,A6),D1w
LAB_000103de
    bsr.b      91c_8bpp_unk24           ; draw, already clipped?
LAB_000103e0
    movem.l    (SP)+,{  D3 D4 D5 D6 D7 A2 A3 A4 A5 A6}
    rts

91c_8bpp_unk24:                     ; draw hline, already clipped
    bsr.w      91c_8bpp_unk28   ; get draw func
    jmp        (A3)             ; draw the line
    
91c_8bpp_unk28:                     ; setup for hline draw
    movea.l    (0x910,A6),A0        ; returns
    adda.w     D0w,A0               ;   A0 = screen start addr
    move.w     D0w,D2w              ;   A3 = hline draw funcptr for current writemode
    lsr.w      #0x4,D0w
    and.w      #0xf,D2w
    move.w     D2w,D3w
    beq.b      LAB_00010406
    neg.w      D2w
    add.w      #0x10,D2w
    addq.w     #0x1,D0w
LAB_00010406
    subq.w     #0x1,D2w
    move.w     D1w,D4w
    and.w      #0xf,D4w
    lsr.w      #0x4,D1w
    sub.w      D0w,D1w
    move.w     D1w,D5w
    bpl.b      LAB_0001041c
    sub.w      D3w,D4w
    move.w     D4w,D2w
    moveq      #-0x1,D4
LAB_0001041c
    subq.w     #0x1,D5w
    move.w     (0x8ca,A6),D0w
    move.w     D0w,D1w
    movea.l    (0x8d0,A6),A2
    and.w      (0xa,A6),D1w
    move.w     D1w,D6w
    lsl.w      #0x4,D6w
    adda.w     D6w,A2
    mulu.w     (0x8c2,A6),D0                        ; bytes_per_line
    adda.l     D0,A0                                ; a0 = screen addr
    move.w     (0x4b0,A6),D6w                       ; write mode
    lsl.w      #0x2,D6w
    movea.l    (PTR_LAB_00010444,PC,D6w*0x1),A3     ; a3 = line draw function
    rts

PTR_LAB_00010444
    addr       LAB_00010454     ; draw hline writemode 0
    addr       FUN_000104a4     ; draw hline writemode 1
    addr       LAB_0001052a     ; draw hline writemode 2
    addr       LAB_0001048a     ; draw hline writemode 3




*/





/*
    random bits of info:
        TOS VDI doesn't draw the perimeter for v_circle() and v_ellipse()
*/


/*
 todo:  check if vdi functions calls into stawk91c_t
        assuming these are the device-dependent functions (used by both vdi and linea)
        the linea dispatcher is definitely calling into these functions.

        with luck, we can patch a shared hline function and gain
        automatic acceleration for vdi circles, polygons and so on.

    Ghidra:
        workstation_pointers, in vicinity of vdi_dispatcher.
        pointer to list of stawk_t pointers

    the plan:
        - hook at a lower level than vdi.
        - patching const tables in data section instead
          of the bss tables for active device.
 */


 /*
    STA_VDI.PRG for T0 and T8 are binary identical apart from the info string

    sta0_v_openwk               0x1bc70
        init_91c_ptrtable()
        init_vt_ptrtable()

    sta100_v_openvwk            0x22f80
        init_91c_ptrtable()

    init_91c_ptrtable           0x1be3a
    init_vt_ptrtable            0x1bd62         

    VT function table in text starts at 0x1beb2
        4bpp
        1bpp
        8bpp
        15bpp
        16bpp
        24bpp
        32bpp


    91c function table headers in text starts at 0x1c616
    {
        uint16_t num;
        uint32_t tableptr[num];
    } devf_header_t;

    1bpp:   2
            t91c_1bpp_0
            t91c_1bpp_0
    4bpp:   1
            t91c_4bpp_0
    8bpp:   2
            t91c_8bpp_0
            t91c_8bpp_0
    15bpp:  1
            t91c_15bpp_0
    16bpp:  1
            t91c_16bpp_0
    24bpp:  3
            t91c_24bpp_0
            t91c_24bpp_0
            t91c_24bpp_2
    32bpp:  3
            t91c_32bpp_0
            t91c_32bpp_0
            t91c_32bpp_2
            t91c_32bpp_3

    dev_funcs_t tables are located just above the header table in the order
            t91c_4bpp_0     0x1c080
            t91c_1bpp_0     0x1c110
            t91c_8bpp_0     0x1c1a0
            t91c_15bpp_0    0x1c230
            t91c_16bpp_0    0x1c2c0
            t91c_24bpp_0    0x1c350
            t91c_24bpp_2    0x1c3e0
            t91c_32bpp_0    0x1c470
            t91c_32bpp_2    0x1c500
            t91c_32bpp_3    0x1c590

    just above that is interesting stuff too
            LINEA_NEG_DATA      0x1c06c
            LINEA_DATA          0x1c070
            FUN_00273f4         0x1c074
            0                   0x1c078               
            0                   0x1c08c


*/



/*-----------------------------------------------------------------------------
 NOVA VDI internal device functions for a particular screen mode
 wks struct in bss holds a pointer the the active ones.
 table in text holds these:
    


    sta0_v_openwk               0x1bc70
        init_91c_ptrtable()
        init_vt_ptrtable()

    sta100_v_openvwk            0x22f80
        init_91c_ptrtable()

    init_91c_ptrtable           0x1be3a
    init_vt_ptrtable            0x1bd62         

    VT function table in text starts at 0x1beb2 (t0, t8 same)
        4bpp
        1bpp
        8bpp
        15bpp
        16bpp
        24bpp
        32bpp


    91c function table headers in text starts at    0x1c616 (t0, t8 same)
    {
        uint16_t num;
        uint32_t tableptr[num];
    } devf_header_t;

    1bpp:   2
            t91c_1bpp_0
            t91c_1bpp_0
    4bpp:   1
            t91c_4bpp_0
    8bpp:   2
            t91c_8bpp_0
            t91c_8bpp_0
    15bpp:  1
            t91c_15bpp_0
    16bpp:  1
            t91c_16bpp_0
    24bpp:  3
            t91c_24bpp_0
            t91c_24bpp_0
            t91c_24bpp_2
    32bpp:  3
            t91c_32bpp_0
            t91c_32bpp_0
            t91c_32bpp_2
            t91c_32bpp_3

    dev_funcs_t tables are located just above the header table in the order
            t91c_4bpp_0     0x1c080
            t91c_1bpp_0     0x1c110
            t91c_8bpp_0     0x1c1a0
            t91c_15bpp_0    0x1c230
            t91c_16bpp_0    0x1c2c0
            t91c_24bpp_0    0x1c350
            t91c_24bpp_2    0x1c3e0
            t91c_32bpp_0    0x1c470
            t91c_32bpp_2    0x1c500
            t91c_32bpp_3    0x1c590

    just above that is interesting stuff too
            LINEA_NEG_DATA      0x1c06c
            LINEA_DATA          0x1c070
            FUN_00273f4         0x1c074
            0                   0x1c078               
            0                   0x1c08c









-----------------------------------------------------------------------------*/
