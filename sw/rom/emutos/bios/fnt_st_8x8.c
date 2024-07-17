/*
 * fnt_st_8x8.c - 8x8 font for Atari ST encoding
 *
 * Copyright (C) 2001-2021 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "bios.h"
#include "fonthdr.h"

static const UWORD dat_table[] =
{
    0x0018, 0x3c18, 0x183c, 0xffe7, 0x017e, 0x1818, 0xf0f0, 0x05a0,
    0x7c06, 0x7c7c, 0xc67c, 0x7c7c, 0x7c7c, 0x0078, 0x07f0, 0x1104,
    0x0018, 0x6600, 0x1800, 0x3818, 0x0e70, 0x0000, 0x0000, 0x0002,
    0x3c18, 0x3c7e, 0x0c7e, 0x3c7e, 0x3c3c, 0x0000, 0x0600, 0x603c,
    0x3c18, 0x7c3c, 0x787e, 0x7e3e, 0x663c, 0x0666, 0x60c6, 0x663c,
    0x7c3c, 0x7c3c, 0x7e66, 0x66c6, 0x6666, 0x7e1e, 0x4078, 0x1000,
    0x0000, 0x6000, 0x0600, 0x1c00, 0x6018, 0x1860, 0x3800, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000e, 0x1870, 0x0000,
    0x0066, 0x0c18, 0x6630, 0x1800, 0x1866, 0x3066, 0x1860, 0x6618,
    0x0c00, 0x3f18, 0x6630, 0x1830, 0x6666, 0x6618, 0x1c66, 0x1c1e,
    0x0c0c, 0x0c0c, 0x3434, 0x0000, 0x0000, 0x00c6, 0xc600, 0x1bd8,
    0x3434, 0x0200, 0x007f, 0x3034, 0x3466, 0x0c00, 0x7a7e, 0x7ef1,
    0x66f6, 0x0000, 0x0000, 0x0000, 0x0000, 0x6000, 0x0060, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000e, 0x0066,
    0x001c, 0x0000, 0xfe00, 0x0000, 0x3c00, 0x001c, 0x0c00, 0x3e3c,
    0x0018, 0x300c, 0x0018, 0x1800, 0x3838, 0x0000, 0x3838, 0x7800,
    0x003c, 0x241c, 0x3899, 0xffc3, 0x03c3, 0x3c1c, 0xc0c0, 0x05a0,
    0xc606, 0x0606, 0xc6c0, 0xc006, 0xc6c6, 0x0060, 0x0ff8, 0x0b28,
    0x0018, 0x666c, 0x3e66, 0x6c18, 0x1c38, 0x6618, 0x0000, 0x0006,
    0x6638, 0x660c, 0x1c60, 0x6006, 0x6666, 0x1818, 0x0c00, 0x3066,
    0x663c, 0x6666, 0x6c60, 0x6060, 0x6618, 0x066c, 0x60ee, 0x7666,
    0x6666, 0x6666, 0x1866, 0x66c6, 0x6666, 0x0618, 0x6018, 0x3800,
    0xc000, 0x6000, 0x0600, 0x3000, 0x6000, 0x0060, 0x1800, 0x0000,
    0x0000, 0x0000, 0x1800, 0x0000, 0x0000, 0x0018, 0x1818, 0x6018,
    0x3c00, 0x1866, 0x0018, 0x1800, 0x6600, 0x1800, 0x6630, 0x0000,
    0x1800, 0x7866, 0x0018, 0x6618, 0x0000, 0x0018, 0x3a66, 0x3630,
    0x1818, 0x1818, 0x5858, 0x3c3c, 0x1800, 0x00cc, 0xcc18, 0x366c,
    0x5858, 0x3c02, 0x00d8, 0x1858, 0x5800, 0x1810, 0xcac3, 0xc35b,
    0x0066, 0x667c, 0x1e7e, 0x7c1c, 0x1e7e, 0x6e3c, 0x3e7e, 0x6c1c,
    0x3e36, 0x7e66, 0x3e78, 0xd67c, 0x1c3e, 0xfe7e, 0x361b, 0x10f7,
    0x0036, 0xfe00, 0x661e, 0x0000, 0x183c, 0x3c36, 0x1810, 0x7066,
    0x7e18, 0x1818, 0x0e18, 0x1832, 0x6c7c, 0x0000, 0x6c6c, 0x0cfe,
    0x0066, 0x24f6, 0x6fc3, 0xfe99, 0x06d3, 0x3c16, 0xfedf, 0x05a0,
    0xc606, 0x0606, 0xc6c0, 0xc006, 0xc6c6, 0x3c78, 0x1fec, 0x0dd8,
    0x0018, 0x66fe, 0x606c, 0x3818, 0x1818, 0x3c18, 0x0000, 0x000c,
    0x6e18, 0x0618, 0x3c7c, 0x600c, 0x6666, 0x1818, 0x187e, 0x1806,
    0x6e66, 0x6660, 0x6660, 0x6060, 0x6618, 0x0678, 0x60fe, 0x7e66,
    0x6666, 0x6660, 0x1866, 0x66c6, 0x3c66, 0x0c18, 0x3018, 0x6c00,
    0x603c, 0x7c3c, 0x3e3c, 0x7c3e, 0x7c38, 0x1866, 0x18ec, 0x7c3c,
    0x7c3e, 0x7c3e, 0x7e66, 0x66c6, 0x6666, 0x7e18, 0x1818, 0xf218,
    0x6600, 0x0000, 0x3c00, 0x003c, 0x003c, 0x0000, 0x0000, 0x1818,
    0x7e7e, 0xd800, 0x0000, 0x0000, 0x663c, 0x663c, 0x303c, 0x667c,
    0x0000, 0x0000, 0x0000, 0x0666, 0x0000, 0x00d8, 0xd800, 0x6c36,
    0x0000, 0x663c, 0x7ed8, 0x0000, 0x3c00, 0x3038, 0xcabd, 0xbd5f,
    0xe666, 0x760c, 0x060c, 0x060c, 0x0c36, 0x660c, 0x0606, 0x3e0c,
    0x3636, 0x6666, 0x060c, 0xd66c, 0x0c06, 0x6666, 0x363c, 0x3899,
    0x7666, 0x66fe, 0x3038, 0x6c7e, 0x3c66, 0x6678, 0x387c, 0x6066,
    0x007e, 0x0c30, 0x1b18, 0x004c, 0x3838, 0x000f, 0x6c18, 0x3800,
    0x00c3, 0xe783, 0xc1e7, 0xfc3c, 0x8cd3, 0x3c10, 0xd8db, 0x0db0,
    0x0000, 0x7c7c, 0x7c7c, 0x7c00, 0x7c7c, 0x0660, 0x1804, 0x0628,
    0x0018, 0x006c, 0x3c18, 0x7000, 0x1818, 0xff7e, 0x007e, 0x0018,
    0x7618, 0x0c0c, 0x6c06, 0x7c18, 0x3c3e, 0x0000, 0x3000, 0x0c0c,
    0x6a66, 0x7c60, 0x667c, 0x7c6e, 0x7e18, 0x0670, 0x60d6, 0x7e66,
    0x7c66, 0x7c3c, 0x1866, 0x66d6, 0x183c, 0x1818, 0x1818, 0xc600,
    0x3006, 0x6660, 0x6666, 0x3066, 0x6618, 0x186c, 0x18fe, 0x6666,
    0x6666, 0x6660, 0x1866, 0x66c6, 0x3c66, 0x0c30, 0x180c, 0x9e34,
    0x6066, 0x3c3c, 0x063c, 0x3c60, 0x3c66, 0x3c38, 0x3838, 0x3c3c,
    0x601b, 0xde3c, 0x3c3c, 0x6666, 0x6666, 0x6660, 0x7c18, 0x7c30,
    0x3c38, 0x3c66, 0x7c66, 0x3e66, 0x183e, 0x7c36, 0x3618, 0xd81b,
    0x3c3c, 0x6e6e, 0xdbde, 0x1818, 0x6600, 0x0010, 0xcab1, 0xa555,
    0x6666, 0x3c0c, 0x0e0c, 0x660c, 0x0636, 0x660c, 0x0606, 0x660c,
    0x3636, 0x763c, 0x360c, 0xd66c, 0x0c06, 0x6676, 0x1c66, 0x6c99,
    0xdc7c, 0x626c, 0x186c, 0x6c18, 0x667e, 0x66dc, 0x54d6, 0x7e66,
    0x7e18, 0x1818, 0x1b18, 0x7e00, 0x0000, 0x0018, 0x6c30, 0x0c00,
    0x00e7, 0xc383, 0xc1c3, 0xf999, 0xd8db, 0x7e10, 0xdeff, 0x0db0,
    0xc606, 0xc006, 0x0606, 0xc606, 0xc606, 0x7e7e, 0x1804, 0x07d0,
    0x0018, 0x006c, 0x0630, 0xde00, 0x1818, 0x3c18, 0x0000, 0x0030,
    0x6618, 0x1806, 0x7e06, 0x6630, 0x6606, 0x1818, 0x1800, 0x1818,
    0x6e7e, 0x6660, 0x6660, 0x6066, 0x6618, 0x0678, 0x60c6, 0x6e66,
    0x6076, 0x6c06, 0x1866, 0x66fe, 0x3c18, 0x3018, 0x0c18, 0x0000,
    0x003e, 0x6660, 0x667e, 0x3066, 0x6618, 0x1878, 0x18d6, 0x6666,
    0x6666, 0x603c, 0x1866, 0x66d6, 0x1866, 0x1818, 0x1818, 0x0c34,
    0x6666, 0x7e06, 0x3e06, 0x0660, 0x7e7e, 0x7e18, 0x1818, 0x6666,
    0x7c7f, 0xf866, 0x6666, 0x6666, 0x6666, 0x6660, 0x303c, 0x6630,
    0x0618, 0x6666, 0x6676, 0x6666, 0x3030, 0x0c6b, 0x6e18, 0x6c36,
    0x0666, 0x7676, 0xdfd8, 0x3c3c, 0x6600, 0x0010, 0x7ab1, 0xb951,
    0x6666, 0x6e0c, 0x1e0c, 0x660c, 0x0636, 0x6600, 0x0606, 0x660c,
    0x3636, 0x060e, 0x360c, 0xd66c, 0x0c06, 0x6606, 0x0c66, 0xc6ef,
    0xc866, 0x606c, 0x306c, 0x6c18, 0x6666, 0x66cc, 0x54d6, 0x6066,
    0x0018, 0x300c, 0x18d8, 0x0032, 0x0000, 0x18d8, 0x6c7c, 0x7800,
    0x0024, 0x66f6, 0x6f99, 0xf3c3, 0x70c3, 0x1070, 0x181e, 0x1998,
    0xc606, 0xc006, 0x0606, 0xc606, 0xc606, 0x6618, 0x1004, 0x2e10,
    0x0000, 0x00fe, 0x7c66, 0xcc00, 0x1c38, 0x6618, 0x3000, 0x1860,
    0x6618, 0x3066, 0x0c66, 0x6630, 0x660c, 0x1818, 0x0c7e, 0x3000,
    0x6066, 0x6666, 0x6c60, 0x6066, 0x6618, 0x666c, 0x60c6, 0x6666,
    0x606c, 0x6666, 0x1866, 0x3cee, 0x6618, 0x6018, 0x0618, 0x0000,
    0x0066, 0x6660, 0x6660, 0x303e, 0x6618, 0x186c, 0x18c6, 0x6666,
    0x6666, 0x6006, 0x1866, 0x3c7c, 0x3c3e, 0x3018, 0x1818, 0x0062,
    0x3c66, 0x607e, 0x667e, 0x7e3c, 0x6060, 0x6018, 0x1818, 0x7e7e,
    0x60d8, 0xd866, 0x6666, 0x6666, 0x3e66, 0x663c, 0x3018, 0x6630,
    0x7e18, 0x6666, 0x666e, 0x3e3c, 0x6030, 0x0cc3, 0xd618, 0x366c,
    0x7e66, 0x6666, 0xd8d8, 0x6666, 0x6600, 0x0010, 0x0abd, 0xad00,
    0xf6f6, 0x667e, 0x360c, 0x660c, 0x0636, 0x7e00, 0x3e0e, 0x6e3c,
    0x1c7e, 0x7e7e, 0x340c, 0xfeec, 0x0c06, 0x7e06, 0x0c3c, 0x8266,
    0xdc66, 0x606c, 0x666c, 0x6c18, 0x3c66, 0x24ec, 0x38d6, 0x7066,
    0x7e00, 0x0000, 0x18d8, 0x184c, 0x0000, 0x1870, 0x0000, 0x0000,
    0x0024, 0x3c1c, 0x383c, 0xe7e7, 0x20c3, 0x38f0, 0x181b, 0x799e,
    0x7c06, 0x7c7c, 0x067c, 0x7c06, 0x7c7c, 0x3c1e, 0x1e3c, 0x39e0,
    0x0018, 0x006c, 0x1846, 0x7600, 0x0e70, 0x0000, 0x3000, 0x1840,
    0x3c7e, 0x7e3c, 0x0c3c, 0x3c30, 0x3c38, 0x0030, 0x0600, 0x6018,
    0x3e66, 0x7c3c, 0x787e, 0x603e, 0x663c, 0x3c66, 0x7ec6, 0x663c,
    0x6036, 0x663c, 0x183e, 0x18c6, 0x6618, 0x7e1e, 0x0278, 0x00fe,
    0x003e, 0x7c3c, 0x3e3c, 0x3006, 0x663c, 0x1866, 0x3cc6, 0x663c,
    0x7c3e, 0x607c, 0x0e3e, 0x186c, 0x6606, 0x7e0e, 0x1870, 0x007e,
    0x083e, 0x3c3e, 0x3e3e, 0x3e08, 0x3c3c, 0x3c3c, 0x3c3c, 0x6666,
    0x7e7e, 0xdf3c, 0x3c3c, 0x3e3e, 0x063c, 0x3e18, 0x7e18, 0x7c60,
    0x3e3c, 0x3c3e, 0x6666, 0x0000, 0x6630, 0x0c86, 0x9f18, 0x1bd8,
    0x3e3c, 0x3c3c, 0x7e7f, 0x7e7e, 0x6600, 0x0000, 0x0ac3, 0xc300,
    0x0606, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x3000, 0x0000, 0x0c06, 0x0006, 0x0cd8, 0x0000,
    0x767c, 0x606c, 0xfe38, 0x7f18, 0x183c, 0x6678, 0x307c, 0x3e66,
    0x007e, 0x7e7e, 0x1870, 0x1800, 0x0000, 0x0030, 0x0000, 0x0000,
    0x003c, 0x1818, 0x1800, 0x0000, 0x007e, 0x1060, 0x0000, 0x718e,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1754, 0x3800,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x007c, 0x0000, 0x7000, 0x0000, 0x0000,
    0x6006, 0x0000, 0x0000, 0x0000, 0x007c, 0x0000, 0x1800, 0x0000,
    0x3800, 0x0000, 0x0000, 0x0018, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x7c00, 0x0018, 0x0000, 0x6000,
    0x0000, 0x0000, 0x0000, 0x3c3c, 0x3c00, 0x000f, 0x0618, 0x0000,
    0x0000, 0x4040, 0x0000, 0x6666, 0x3c00, 0x0000, 0x0a7e, 0x7e00,
    0x1c1c, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0070, 0x0000,
    0x0060, 0xf848, 0x0000, 0xc010, 0x3c00, 0x0000, 0x6010, 0x0000,
    0x0000, 0x0000, 0x1800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

const Fonthead fnt_st_8x8 = {
    1,  /* WORD font_id */
    9,  /* WORD point */
    "8x8 system font",  /*   char name[32]      */
    0,  /* WORD first_ade */
    255,  /* WORD last_ade */
    6,  /* UWORD top */
    6,  /* UWORD ascent */
    4,  /* UWORD half */
    1,  /* UWORD descent */
    1,  /* UWORD bottom */
    7,  /* UWORD max_char_width */
    8,  /* UWORD max_cell_width */
    1,  /* UWORD left_offset */
    3,  /* UWORD right_offset */
    1,  /* UWORD thicken */
    1,  /* UWORD ul_size */
    0x5555, /* UWORD lighten */
    0x5555, /* UWORD skew */
    F_STDFORM | F_MONOSPACE | F_DEFAULT,  /* UWORD flags        */
    0,                  /*   UBYTE *hor_table   */
    off_8x8_table,       /*   UWORD *off_table   */
    dat_table,           /*   UWORD *dat_table   */
    256,  /* UWORD form_width */
    8,  /* UWORD form_height */
    0,  /* Fonthead * next_font */
    0                   /*   reserved by Atari  */
};