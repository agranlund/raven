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

#include "emulator.h"
#include "nova.h"

#define VDI_DEBUG   0

/*
 * VDI patching.
 *
 * We cannot simply add our own trap2 handler because the VDI's are
 * caching certain stuff as well as playing foul with the trap2 chain.
 * Furthermore, NVDI replaces the trap and gets rid of STA_VDI from
 * that chain completely, calling its dispatcher directly when needed.
 * 
 * So. when we get the first call from the resident STA_VDI we search
 * it's memory from the stacked return address to find and modify
 * the internal VDI jumptable. This is located right after the trap code.
 *
 */
uint32_t vdi_patch_findtrap2_backward(uint32_t from, uint32_t len) {
    uint32_t addr = from - 2;
    for (; addr > (from - len); addr -= 2) {
        if (*((uint32_t*)(addr+0)) == 0x58425241UL) {       /* 'XBRA' */
            if (*((uint32_t*)(addr+4)) == 0x494D4E45UL) {   /* 'IMNE' */
                return addr;
            }
        }
    }
    return 0;
}

uint32_t vdi_patch_find_vditable(uint32_t addr) {
    uint32_t tableaddr = 0;
    uint32_t trap2addr = vdi_patch_findtrap2_backward(addr, 16 * 1024UL);
    dprintf("vdi_patch: %08lx trap: %08lx\n", addr, trap2addr);
    if (!trap2addr) {
        return 0;
    }
    /* todo: verify trap code to be sure the table is at offset 0x50 from it */
    tableaddr = trap2addr + 0x50;
    return tableaddr;
}

bool vro_cpyfm_new(int16_t handle, int16_t vr_mode, int16_t *pxyarray, MFDB *psrcMFDB, MFDB *pdesMFDB) {
#if VDI_DEBUG
    dprintf("vro_copyfm: %d %d %08lx %08lx %08lx\n", handle, vr_mode, (uint32_t)pxyarray, (uint32_t)psrcMFDB, (uint32_t)pdesMFDB);
    dprintf("%d,%d->%d,%d -> ", pxyarray[0], pxyarray[1], pxyarray[2], pxyarray[3]);
    dprintf("%d,%d->%d,%d\n", pxyarray[4], pxyarray[5], pxyarray[6], pxyarray[7]);
    dprintf("srcMFDB: %08lx, %d, %d\n", (uint32_t)psrcMFDB->fd_addr, psrcMFDB->fd_nplanes, psrcMFDB->fd_stand);
    dprintf("dstMFDB: %08lx, %d, %d\n", (uint32_t)pdesMFDB->fd_addr, pdesMFDB->fd_nplanes, pdesMFDB->fd_stand);
#endif
    /* source and destination are screen */
    if ((psrcMFDB->fd_addr == 0) && (pdesMFDB->fd_addr == 0)) {
        /* blit in device specific format */
        /*if ((psrcMFDB->fd_stand == 0) && (pdesMFDB->fd_stand == 0))*/ {
            /* hardware blit */
            static blcmd_t blcmd;
          
            /* early out if fully outside */
            if ((pxyarray[0] > nova.max_x) ||
                (pxyarray[1] > nova.max_y) ||
                (pxyarray[4] > nova.max_x) ||
                (pxyarray[5] > nova.max_y)
            ) {
                return false;
            }

            /* src clip */
            blcmd.x0 = pxyarray[0];
            blcmd.y0 = pxyarray[1];
            blcmd.width = (pxyarray[2] - pxyarray[0]) + 1;
            blcmd.height = (pxyarray[3] - pxyarray[1]) + 1;
            if ((blcmd.x0 > nova.max_x) || (blcmd.y0 > nova.max_y)) {
                return false;
            }
            if (blcmd.x0 < 0) {
                blcmd.width += blcmd.x0;
                blcmd.x0 = 0;
            }
            if (blcmd.x0 + blcmd.width > nova.max_x) {
                blcmd.width = nova.max_x + 1 - blcmd.x0;
            }
            if (blcmd.y0 < 0){ 
                blcmd.height += blcmd.y0;
                blcmd.y0 = 0;
            }
            if (blcmd.y0 + blcmd.height > nova.max_y) {
                blcmd.height = nova.max_y + 1 - blcmd.y0;
            }

            /* dst clip */
            blcmd.x1 = pxyarray[4] + blcmd.x0 - pxyarray[0];
            blcmd.y1 = pxyarray[5] + blcmd.y0 - pxyarray[1];
            if ((blcmd.x1 > nova.max_x) || (blcmd.y1 > nova.max_y)) {
                return false;
            }
            if (blcmd.x1 < 0) {
                blcmd.width += blcmd.x1;
                blcmd.x0 -= blcmd.x1;
                blcmd.x1 = 0;
            }
            if (blcmd.x1 + blcmd.width > nova.max_x) {
                blcmd.width = nova.max_x + 1 - blcmd.x1;
            }
            if (blcmd.y1 < 0) {
                blcmd.height += blcmd.y1;
                blcmd.y0 -= blcmd.y1;
                blcmd.y1 = 0;
            }
            if (blcmd.y1 + blcmd.height > nova.max_y) {
                blcmd.height = nova.max_y + 1 - blcmd.y1;
            }

            /* blit */
            if (blcmd.width > 0 && blcmd.height > 0) {
                blcmd.flags = 0;
                if (card->blit(&blcmd)) {
                    return true;
                }
            }
        }
    }
    return false;
}

typedef void(*vdi_func)(uint32_t, uint32_t, void*, void*);
vdi_func vdi_copyfm_dispatch_old;
void vdi_copyfm_dispatch_new(uint32_t d0, uint32_t d1, void* a0, void* a1) {
    /*
     * d0.w = vdi function number
     * d1.w = vdi handle
     * a0.l = parameter blocks
     * a1.l = workstation data (sta_vdi internal)
    */
    VDIPB* pb = (VDIPB*)a0;
#if VDI_DEBUG
    dprintf("vro_copyfm: %d %d %08lx %08lx\n", (uint16_t)d0, (uint16_t)d1, (uint32_t)a0, (uint32_t)a1);
#endif    
    if (!vro_cpyfm_new(
        pb->contrl[6],
        pb->intin[0],
        (int16_t*)&pb->ptsin[0],
        *(MFDB**)&pb->contrl[7],
        *(MFDB**)&pb->contrl[9]
    )) {
        vdi_copyfm_dispatch_old(d0, d1, a0, a1);
    }
}

bool vdi_patch(void* sp) {
    uint16_t sr;
    vdi_func* fun;
    uint32_t vditable = vdi_patch_find_vditable(*((uint32_t*)sp));
    if (!vditable) {
        dprintf("vdi_patch: table not found\n");
        return false;
    }
    dprintf("vdi_patch: table at %08lx\n", vditable);

    sr = cpu_di();
    fun = (vdi_func*)(vditable + 4 + (8 * 109));
    vdi_copyfm_dispatch_old = *fun;
    *fun = vdi_copyfm_dispatch_new;
    cpu_flush_cache();
    cpu_ei(sr);
    return true;
}



/* 
 * sta_vdi has another initial trap2 handler that moves the unusal one back
 * to the top in case someone else installs a higher prio trap2 handler..
 * NVDI has the same nasty behavior. very bizarre.
 * fVDI will runtime patches that out of NVDI and we could do something similar
 * here in order to override certain STA_VDI functionality -- but it's probably
 * just a lot easier to patch the VDI function table in memory (vdi_trap2_top_table)
 */

/* dispatcher:  4217c, ghidra:  2198c
        0002198c 48 e7 60 e0     movem.l    {  A2 A1 A0 D2 D1},-(SP)
        00021990 20 41           movea.l    D1,A0
        00021992 22 50           movea.l    (A0),A1
        00021994 32 29 00 0c     move.w     (0xc,A1),D1w
        00021998 30 11           move.w     (A1),D0w
        0002199a b0 7c 00 83     cmp.w      #0x83,D0w
        0002199e 62 18           bhi.b      LAB_000219b8
        000219a0 45 fb 06 7e     lea        (0x21a20,PC,D0w*0x8),A2
        000219a4 33 5a 00 04     move.w     (A2)+,(0x4,A1)=>vdi_trap2_top_table_dw0
        000219a8 33 5a 00 08     move.w     (A2)+,(0x8,A1)=>vdi_trap2_top_table_dw1
        000219ac 24 52           movea.l    (A2),A2=>vdi_trap2_top_table_fun                 = 0002221e
        000219ae 43 fa 08 70     lea        (0x870,PC)=>workstation_pointers,A1
        000219b2 22 71 14 00     movea.l    (0x0,A1,D1w*0x4)=>workstation_pointers,A1
        000219b6 4e 92           jsr        (A2)
                             LAB_000219b8
        000219b8 4c df 07 06     movem.l    (SP=>local_14)+,{  D1 D2 A0 A1 A2}
        000219bc 4e 75           rts
*/

/* trap2: 421cc, ghidra:  219dc
        000219d0 58 42 52 41     ds         "XBRA"
        000219d4 49 4d 4e 45     ds         "IMNE"
vec_trap2top_old
        000219d8 00 00 00 00     long       0h
vec_trap2top_new
        000219dc b0 7c 00 73     cmp.w      #0x73,D0w
        000219e0 66 36           bne.b      LAB_00021a18
        000219e2 48 e7 60 e0     movem.l    {  A2 A1 A0 D2 D1},-(SP)
        000219e6 20 41           movea.l    D1,A0                                            get address of parameter blocks
        000219e8 22 50           movea.l    (A0),A1                                          A1 = contrl arrays
        000219ea 32 29 00 0c     move.w     (0xc,A1),D1w                                     D1 = ctrl[6] = handle
        000219ee 30 11           move.w     (A1),D0w                                         D0 = ctrl[0] = opcode
        000219f0 b0 7c 00 83     cmp.w      #0x83,D0w                                        skip if vdi func > 131
        000219f4 62 1c           bhi.b      LAB_00021a12
        000219f6 45 fb 06 28     lea        (0x21a20,PC,D0w*0x8),A2
        000219fa 33 5a 00 04     move.w     (A2)+,(0x4,A1)=>vdi_trap2_top_table_dw0          set ctrl[2] = ptsout count
        000219fe 33 5a 00 08     move.w     (A2)+,(0x8,A1)=>vdi_trap2_top_table_dw1          set ctrl[4] = intout array
        00021a02 24 52           movea.l    (A2),A2=>vdi_trap2_top_table_fun                 A2 = func pointer
        00021a04 43 fa 08 1a     lea        (0x81a,PC)=>workstation_pointers,A1              A1 = workstation data pointers
        00021a08 c2 7c 01 ff     and.w      #0x1ff,D1w
        00021a0c 22 71 14 00     movea.l    (0x0,A1,D1w*0x4)=>workstation_pointers,A1        A1 = ptr to workstation data for
        00021a10 4e 92           jsr        (A2)                                             jump to opcode handler
LAB_00021a12
        00021a12 4c df 07 06     movem.l    (SP)+,{  D1 D2 A0 A1 A2}
        00021a16 4e 73           rte
LAB_00021a18
        00021a18 2f 3a ff be     move.l     (-0x42,PC)=>vec_trap2top_old,-(SP)
        00021a1c 4e 75           rts
        00021a1e 00              db         0h
        00021a1f 00              db         0h

        ; VDI functions table
vdi_trap2_top_table_dw0
        00021a20 00 00           dw         0h                      ; ptsout
vdi_trap2_top_table_dw1
        00021a22 00 00           dw         0h                      ; intout
vdi_trap2_top_table_fun
        00021a24 00 02 22 1e     addr       FUN_0002220a::rts       ; funcptr
        .....
        00021a28 00 06           dw         6h
        00021a2a 00 2d           dw         2Dh
        00021a2c 00 02 2d c4     addr       FUN_00022dc4
        .....
*/

/* vro_copyfm = 107, 107*8 = 872 = 0x368
    0x21a20 + 0x368 = 21d88
        00021d88 00 00           dw         0h
        00021d8a 00 00           dw         0h
        00021d8c 00 02 1e 9c     addr       DAT_00021e9c
*/

