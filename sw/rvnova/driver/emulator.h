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
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "nova.h"

#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#define DEBUG           0
#define DEBUGPRINT_UART 1

#if defined(DEBUG) && DEBUG
    #if DEBUGPRINT_UART
        extern void dprintf_uart(char* s, ...);
        #define dprintf dprintf_uart
    #else
        #include <stdio.h>
        #define dprintf printf
    #endif
#else
    static void dprintf(char* s, ...) { }
#endif


extern nova_xcb_t nova;
extern bool nova_prepare(void);

#endif /* _EMULATOR_H_ */
