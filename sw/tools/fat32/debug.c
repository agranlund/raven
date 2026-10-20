/*-------------------------------------------------------------------------------
 * FAT32 filsystem driver for TOS
 * (c) 2026 Anders Granlund
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

#include <stdio.h>
#include <string.h>
#include <mint/osbind.h>
#include <mint/sysbind.h>
#include <mint/sysvars.h>
#include <mint/cookie.h>
#include "main.h"

#if DEBUG

static char dbgBuffer[128];

uint32_t nf_old_sp;
uint32_t nf_old_int;
uint32_t nf_id_print;
uint32_t nf_id_break;
const char* nf_name_break = "NF_DEBUGGER";
const char* nf_name_print = "NF_STDERR";

static void dbgPrintNF(char* str) {
    __asm__ __volatile__ (          \
        "                           \
        move.l  %0,-(sp);           \
        move.l  nf_id_print,-(sp);  \
        clr.l   -(sp);              \
        dc.w    0x7301;             \
        lea     12(sp),sp;          \
        "                           \
      : : "p"(str) : "cc" );
}

static void dbgBreakNF(uint32_t id) {
    __asm__ __volatile__ (          \
        "                           \
        move.l  nf_id_break,-(sp);  \
        move.l  %0,-(sp);           \
        dc.w    0x7301;             \
        addq.l  #8,sp;              \
        "                           \
      : : "d"(id) : "cc" );
}

static void dbgPrintDummy(char* str) { (void)str; }
static void dbgBreakDummy(uint32_t id) { (void)id; }

static void(*dbgPrintFunc)(char*) = dbgPrintDummy;
static void(*dbgBreakFunc)(uint32_t) = dbgBreakDummy;

void dbg_Printf(const char* fmt, ...) {
    if (dbgPrintFunc) {
        va_list args;
        va_start(args, fmt);
        vsprintf(dbgBuffer, fmt, args);
        va_end(args);
        dbgPrintFunc(dbgBuffer);
    }
}

void dbg_Init(int16_t mode) {

	dbgPrintFunc = dbgPrintDummy;
	dbgBreakFunc = dbgBreakDummy;

	if (mode == DBG_NATFEATS) {
        nf_id_break = nf_id_print = 0;
		__asm__ __volatile__ ( \
			"   movem.l d0-d7/a0-a6,-(sp);      \
				move.l  sp,nf_old_sp;           \
				move.l  0x10,nf_old_int;        \
				move.l  1f,0x10.w;              \
				move.l  nf_name_print,-(sp);    \
				pea     0;                      \
				dc.w    0x7300;                 \
				addq.l  #8,sp;                  \
				move.l  d0,nf_id_print;         \
				move.l  nf_name_break,-(sp);    \
				pea     0;                      \
				dc.w    0x7300;                 \
				addq.l  #8,sp;                  \
				move.l  d0,nf_id_break;         \
			1:  move.l  nf_old_sp,sp;           \
				move.l  nf_old_int,0x10;        \
				movem.l (sp)+,d0-d7/a0-a6;      \
			" : : : "cc", "memory"
		);
		if (nf_id_print) {
			dbgPrintFunc = dbgPrintNF;
		} 
		if (nf_id_break) {
			dbgBreakFunc = dbgBreakNF;
		} 
    }
}

#endif /* DEBUG */
