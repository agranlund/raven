/*-------------------------------------------------------------------------------
 * NOVA driver test
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
#ifndef _RVTEST_H_
#define _RVTEST_H_

#include <tos.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <mint/cookie.h>
#include <mint/osbind.h>

#include "nova.h"
#include "vga.h"
#include "../driver/emulator.h"

extern nova_xcb_t* nova;
extern card_t* card;
extern uint32_t hz200(void);
extern uint32_t vram_addr(vec_t* v);
extern void waitkey_release(void);
extern int16_t waitkey_press(void);
extern int16_t pollkey_press(void);

extern void test_speed(void);
extern void test_sprites(void);
extern void test_scroll(void);
extern void test_raster(void);

#endif /* _RVTEST_H_ */
