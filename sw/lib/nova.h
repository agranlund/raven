/*-------------------------------------------------------------------------------
 * NOVA defines
 * Anders Granlund, Patrice Mandin
 *-------------------------------------------------------------------------------
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
 *-----------------------------------------------------------------------------*/
#ifndef _NOVA_H_
#define _NOVA_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef C_NOVA
#define C_NOVA 0x4E4F5641UL
#endif

#ifndef NOVA_VERSION
#define NOVA_VERSION 0x312E3337UL        /* '1.37' */
#endif

typedef struct {
/* 00 */	uint8_t		maccel;
/* 01 */	uint8_t		guikey;
/* 02 */	uint8_t		u02;
/* 03 */	uint8_t		u03;
/* 04 */	uint8_t		resid;
/* 05 */	uint8_t		output;
/* 06 */	uint8_t		u06;
/* 07 */	uint8_t		gdos;
/* 08 */	char		gdosfile[48];
} nova_menuinf_t;

typedef struct {
/* 00 */	char		name[33];           /* video mode name */
/* 21 */	char		dummy1;
/* 22 */	uint16_t	mode;               /* video mode type */
/* 24 */	uint16_t	pitch;              /* bpp<8: words/plane, per line*/
/* 26 */	uint16_t	planes;             /* bits per pixel */
/* 28 */	uint16_t	colors;             /* number of colors, unused */
/* 2A */	uint16_t	hcmode;             /* hardcopy mode */
/* 2C */	uint16_t	max_x;              /* virtual resolution - 1 */
/* 2E */    uint16_t    max_y;
/* 30 */	uint16_t	real_x;             /* actual resolution - 1 */
/* 32 */    uint16_t    real_y;
/* 34 */	uint16_t	freq;               /* pixel clock 1 */
/* 36 */	uint8_t		freq2;              /* pixel clock 2 */
/* 37 */	uint8_t		low_res;            /* half pixel clock */
/* 38 */	uint8_t		r_3c2;              /* vga regs */
/* 39 */	uint8_t		r_3d4[25];          /* vga regs */
/* 52 */	uint8_t		extended[3];
/* 55 */	uint8_t		dummy2;
} nova_bibres_t;

typedef struct {
/* 00 */	uint16_t	    num;                /* number of resolutions in file */
/* 02 */	nova_bibres_t*	res;                /* resolution data */
} nova_bib_t;

typedef struct {
/* 00 */    uint32_t    version;            /*  0x312E3337 = '1.37' */
/* 04 */    uint8_t     resolution;         /* resolution number */
/* 05 */    uint8_t     blank_time;         /* screensaver time */
/* 06 */    uint8_t     mouse_speed;        /* mouse acceleration */
/* 07 */    uint8_t     old_resolution;
/* 08 */    void        (*p_changeres)(nova_bibres_t* bib, uint32_t offs);
/* 0C */    uint16_t    mode;               /* video mode type, see  defines below */
/* 0E */    uint16_t    pitch;              /* bpp<8: bytes per plane, per line */
/* 10 */    uint16_t    planes;             /* bits per pixel */
/* 12 */    uint16_t    colors;             /* number of colours, unused */
/* 14 */    uint16_t    hcmode;             /* hardcopy mode */
/* 16 */    uint16_t    max_x;              /* resolution width - 1 */
/* 18 */    uint16_t    max_y;              /* resolution height - 1*/
/* 1A */    uint16_t    real_min_x;
/* 1C */    uint16_t    real_max_x;
/* 1E */    uint16_t    real_min_y;
/* 20 */    uint16_t    real_max_y;
/* 22 */    uint16_t    v_top;
/* 24 */    uint16_t    v_bottom;
/* 26 */    uint16_t    v_left;
/* 28 */    uint16_t    v_right;
/* 2A */    void        (*p_setcolor)(uint16_t index, uint8_t* colors);
/* 2E */    void        (*p_changevirt)(uint16_t x, uint16_t y);
/* 32 */    void        (*p_instxbios)(uint16_t on);
/* 36 */    void        (*p_screen_on)(uint16_t on);
/* 3A */    void        (*p_changepos)(nova_bibres_t* bib, uint16_t dir, uint16_t offs);
/* 3E */    void        (*p_setscreen)(void* addr);
/* 42 */    void*       base;               /* address of screen #0 in video ram */
/* 46 */    void*       mem_base;           /* video ram base */
/* 4A */    uint16_t    scrn_count;         /* number of possible screens in video ram */
/* 4C */    uint32_t    scrn_size;          /* size of one screen */
/* 50 */    void*       reg_base;           /* io register base */
/* 54 */    void        (*p_vsync)(void);
/* 58 */    uint8_t     mode_name[36];      /* video mode name */
/* 7C */    uint32_t    mem_size;           /* total size of video memory */

/* 80 */    uint8_t     unknown_80;         /* checked by sta_vdi */
/* 81 */    uint8_t     unknown_81;         /* checked by sta_vdi */
/* 82 */    uint8_t     unknown_82[6];
/* 88 */    uint16_t    cpu;
/* 8A */    uint8_t     unknown8A[16];
} nova_xcb_t;

#define NOVA_MODE_4BPP      0
#define NOVA_MODE_1BPP      1
#define NOVA_MODE_8BPP      2
#define NOVA_MODE_15BPP     3
#define NOVA_MODE_16BPP     4
#define NOVA_MODE_24BPP     5
#define NOVA_MODE_32BPP     6

#define NOVA_HCMODE_1X1     0
#define NOVA_HCMODE_2X2     1
#define NOVA_HCMODE_4X4     2

#endif /* _NOVA_H_ */
